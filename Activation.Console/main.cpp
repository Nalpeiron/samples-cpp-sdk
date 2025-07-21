#include "Zentitle.Licensing.Client.CPP/Commons/PlainLogger.hpp"
#include "Zentitle.Licensing.Client.CPP/Commons/DynamicLibraryLoader.hpp"
#include "Zentitle.Licensing.Client.CPP/Commons/OSMacros.hpp"
#include "Zentitle.Licensing.Client.CPP/Api/Models/ActivationCodeCredentialsModel.hpp"
#include "Zentitle.Licensing.Client.CPP/Api/LicenseManager/Activation.hpp"
#include "ActivationConfig.hpp"
#include "ActivationActions.hpp"
#include "LicenseStorage.hpp"
#include "CoreLibraryManagerConfigProvider.hpp"
#include <random>
#include <array>

void handleExceptions(const std::function<void()>& func)
{
	using namespace ZentitleLicensingClient;

	try
	{
		func();
	}
	catch (const CommandExecutorException& e)
	{
		std::cerr << "CommandExecutorException: " << e.what() << std::endl;
	}
	catch (const HttpRequestException& e)
	{
		std::cerr << "HttpRequestException: " << e.what() << std::endl;
	}
	catch (const DataValidationException& e)
	{
		std::cerr << "DataValidationException: " << e.what() << std::endl;
	}
	catch (const StateException& e)
	{
		std::cerr << "StateException: " << e.what() << std::endl;
	}
	catch (const SDKException& e)
	{
		std::cerr << "SDKException: " << e.what() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "An error occurred: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "An unknown error occurred." << std::endl;
	}
}

unsigned long long randomize()
{
	std::mt19937_64 gen(std::random_device{}());
	return gen();
}

int main()
{
	SDKLogger::init("./Activation.Console.log");

	std::string currentPath = std::filesystem::current_path().string();

#ifdef ZEN_WIN64
#ifdef _DEBUG
	currentPath += "\\debug";
#endif
	std::string configPath = currentPath + "\\appsettings.json";
#else
	std::string configPath = currentPath + "/appsettings.json";
#endif

	LOG(LEVEL_INFO, "Application started.");
	LOG(LEVEL_INFO, "Loading configuration from: " + configPath);

	ActivationConsole::ActivationConfig config = ActivationConsole::LoadConfiguration(configPath);
	std::string seatId = "";

	if (config.UseCoreLibrary)
	{
		std::cout << "- Using Zentitle2Core C++ library for device fingerprint, secure license storage and offline activation operations" << std::endl;

		helpers::DynamicLibraryLoader::native_handle_type coreLibrary = nullptr;
		std::unique_ptr<helpers::DynamicLibraryLoader> loader;

		if (!config.CoreLibPath.empty())
		{
			loader = std::make_unique<helpers::DynamicLibraryLoader>(config.CoreLibPath, false);
		}
		else
		{
			std::cerr << "No path to the core library provided. Please provide a valid path in the configuration file." << std::endl;
			return EXIT_FAILURE;
		}

		const auto loadedFunction = loader->get_function<bool(char*, int*, int)>("generateDeviceFingerprint");

		const static unsigned int DeviceFingerprintMaxLength = 128;
		std::array<char, DeviceFingerprintMaxLength> generatedDeviceFingerprint = {};
		int length = 0;

		if (loadedFunction == nullptr)
		{
			LOG(LEVEL_ERROR, "Failed to load core library.");
			return EXIT_FAILURE;
		}

		const static int FINGERPRINT_OPTION_DEFAULT = 0;
		bool retValue = loadedFunction(generatedDeviceFingerprint.data(), &length, FINGERPRINT_OPTION_DEFAULT);

		if (!retValue)
		{
			LOG(LEVEL_ERROR, "Failed to generate fingerprint.");
			return EXIT_FAILURE;
		}

		seatId = std::string(generatedDeviceFingerprint.data());
		LOG(LEVEL_INFO, "Generated fingerprint: " + seatId);

		loader.reset();
	}
	else
	{
		std::cout << "- Zentitle2Core C++ library usage is disabled in 'appsettings.json', the won't be loaded and offline activation won't work";
		seatId = std::to_string(randomize());
		LOG(LEVEL_INFO, "Generated random seatId: " + seatId);
	}

	// Setup Activation Options
	ZentitleLicensingClient::ActivationOptions options;

	ZentitleLicensingClient::OnlineActivationOptions onlineOptions;

	onlineOptions.licensingApiUrl = config.ApiUrl;
	onlineOptions.productId = config.ProductId;
	onlineOptions.seatId = seatId;
	onlineOptions.tenantId = config.TenantId;

	ZentitleLicensingClient::OfflineActivationOptions offlineOptions;
	offlineOptions.tenantRsaKeyModulus = config.TenantRsaKeyModulus;

	options.onlineActivationOptionsOpt = onlineOptions;
	options.offlineActivationOptionsOpt = offlineOptions;

	// Get the core library path from the configuration
	std::filesystem::path coreLibraryPath = config.CoreLibPath;
	std::string libraryDirectory = coreLibraryPath.parent_path().string() + "/";
	std::string libraryName = coreLibraryPath.filename().string();

	options.setActivationStorage(LicenseStorage::Initialize(config.UseCoreLibrary, libraryDirectory, libraryName));


	// Create Activation Instance
	CoreLibraryManagerConfigProvider configProvider(libraryDirectory, libraryName);
	std::shared_ptr<Activation> activation = Activation::create(options, configProvider);

	std::cout << "Activation instance created." << std::endl;
	handleExceptions([&]() {
		ActivationActions::Initialize(*activation);
		});

	std::cout << "Activation initialized." << std::endl;

	// Main application loop
	handleExceptions([&]() {
		bool quit = false;

		while (!quit)
		{
			auto state = activation->getState();
			auto mode = activation->getActivationInfo().activationMode;

			std::cout << "\nCurrent activation state: " << activation->getStateAsString() << std::endl;
			std::cout << "Activation mode: "
				<< (mode == ActivationMode::Online ? "Online" : "Offline") << std::endl;

			std::cout << "Available actions:" << std::endl;

			auto actionMapping = std::find_if(ActivationActions::AvailableActions.begin(),
				ActivationActions::AvailableActions.end(),
				[state](const ActivationActions::ActivationActionMapping& mapping)
				{
					return mapping.state == state;
				});

			std::vector<std::function<void(Activation&, const std::string&)>> filteredActions;
			std::vector<std::string> filteredActionNames;

			if (actionMapping != ActivationActions::AvailableActions.end())
			{
				for (size_t i = 0; i < actionMapping->actionNames.size(); ++i)
				{
					const auto& name = actionMapping->actionNames[i];

					if (mode != ActivationMode::Online &&
						(name == "Checkout advanced feature" ||
							name == "Return element-pool feature" ||
							name == "Track usage of a bool feature" ||
							name == "Deactivate license" ||
							name == "Refresh activation lease" ||
							name == "Pull activation state from the server"))
					{
						continue;
					}

					filteredActions.push_back(actionMapping->actions[i]);
					filteredActionNames.push_back(name);
					std::cout << filteredActionNames.size() << ". " << name << std::endl;
				}
			}
			else
			{
				std::cout << "No specific actions available for current state." << std::endl;
			}

			std::cout << "0. Quit" << std::endl;

			std::string selectedActionStr;
			std::cout << "Enter your choice: ";
			std::getline(std::cin, selectedActionStr);

			if (selectedActionStr == "0" || selectedActionStr == "quit" || selectedActionStr == "q")
			{
				quit = true;
				continue;
			}

			try
			{
				size_t selectedAction = std::stoi(selectedActionStr) - 1;

				if (selectedAction < filteredActions.size())
				{
					std::cout << "\nExecuting: " << filteredActionNames[selectedAction] << std::endl;
					filteredActions[selectedAction](*activation, config.ApiUrl);
				}
				else
				{
					std::cout << "Invalid selection. Please try again." << std::endl;
				}
			}
			catch (const std::exception& ex)
			{
				std::cout << "Error processing selection: " << ex.what() << std::endl;
			}
		}
		});

	return EXIT_SUCCESS;
}