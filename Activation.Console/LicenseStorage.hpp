#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include <memory>

#include "IActivationStorage.hpp"
#include "SecureActivationStorage.hpp"
#include "SecureStorage.hpp" 
#include "CoreLibraryManagerConfigProvider.hpp"

using namespace ZentitleLicensingClient;

enum PredefinedFolder
{
	USER_DATA = 0,  ///< Data specific to the user.
	PUBLIC_DATA = 1 ///< Data shared between users.
};



class LicenseStorage : public Persistence::Storage::IActivationStorage
{
public:
	static constexpr const char* AppDirectory = "Z2_OnlineActivation_Console";

	enum DeletionPrompt
	{
		Ask, Skip
	};

	static std::shared_ptr<Persistence::Storage::IActivationStorage> Initialize(bool useCoreLibrary, const std::string libraryPath = "", const std::string libraryName = "", const DeletionPrompt deletionPrompt = DeletionPrompt::Skip)
	{
		std::shared_ptr<Persistence::Storage::IActivationStorage> storage = nullptr;

		// Determine storage type
		if (useCoreLibrary)
		{
			CoreLibraryManagerConfigProvider configProvider(libraryPath, libraryName);
			SecureActivationStorage::setLibraryConfig(configProvider);

			storage = std::make_shared<SecureActivationStorage>(SecureActivationStorage::withAppDirectory(
				SecureStorage::getSystemFolder(PredefinedFolder::USER_DATA),
				AppDirectory,
				"license.encrypted"
			)
			);
		}
		else
		{
			throw std::runtime_error("Core library is not enabled");
		}

		// Log storage details
		std::cout << "- Using SecureActivationStorage storage with file: " << storage->storageId() << "\n";

		// Load data from storage
		auto resultFuture = storage->load();
		auto data = resultFuture.get();

		if (data.isEmpty())
		{
			return storage;
		}

		if (deletionPrompt == DeletionPrompt::Skip)
		{
			std::cout << "Deleting already persisted activation data...\n";
			storage->clear();
		}
		else
		{
			// Confirm deletion of existing activation data
			bool deleteExistingFiles = ConfirmPrompt("Do you want to delete existing activation data in the persistent storage?");
			if (deleteExistingFiles)
			{
				std::cout << "Deleting already persisted activation data...\n";
				storage->clear();
			}
		}

		return storage;
	}

private:
	// Utility to prompt confirmation
	static bool ConfirmPrompt(const std::string& message)
	{
		std::string input;
		while (true)
		{
			std::cout << message << " (yes/no): ";
			std::cin >> input;
			if (input == "yes" || input == "no")
			{
				return input == "yes";
			}
			std::cout << "Invalid input. Please type 'yes' or 'no'.\n";
		}
	}
};
