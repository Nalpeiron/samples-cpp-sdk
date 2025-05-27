#pragma once 

#include "json.hpp"
#include <fstream>
#include <string>
#include <iostream>

namespace ActivationConsole
{

	namespace constant_strings
	{
		const std::string configLicesing = "Licensing";
		const std::string configApiUrl = "ApiUrl";
		const std::string configSeatName = "SeatName";
		const std::string configTenantId = "TenantId";
		const std::string configProductId = "ProductId";
		const std::string configUseCoreLibrary = "UseCoreLibrary";
		const std::string configCoreLibPath = "CoreLibPath";
		const std::string tenantRsaKeyModulus = "TenantRsaKeyModulus";
	}

	struct ActivationConfig
	{
		std::string ApiUrl;
		std::string TenantId;
		std::string ProductId;
		bool UseCoreLibrary{ true };
		std::string CoreLibPath;
		std::string TenantRsaKeyModulus;
	};

	ActivationConfig LoadConfiguration(const std::string& filePath)
	{
		std::ifstream configFile(filePath);
		if (!configFile.is_open())
		{
			std::cerr << "Failed to open configuration file." << std::endl;
			exit(EXIT_FAILURE);
		}

		nlohmann::json configJson;
		configFile >> configJson;

		ActivationConfig config;
		config.ApiUrl = configJson[constant_strings::configLicesing][constant_strings::configApiUrl].get<std::string>();
		config.TenantId = configJson[constant_strings::configLicesing][constant_strings::configTenantId].get<std::string>();
		config.ProductId = configJson[constant_strings::configLicesing][constant_strings::configProductId].get<std::string>();
		config.UseCoreLibrary = configJson[constant_strings::configUseCoreLibrary].get<bool>();
		config.CoreLibPath = configJson[constant_strings::configCoreLibPath].get<std::string>();
		config.TenantRsaKeyModulus = configJson[constant_strings::configLicesing][constant_strings::tenantRsaKeyModulus].get<std::string>();

		if (config.UseCoreLibrary && config.CoreLibPath.empty())
		{
			std::cerr << "CoreLibPath is required when UseCoreLibrary is true." << std::endl;
			exit(EXIT_FAILURE);
		}

		if (config.ApiUrl.empty() || config.TenantId.empty() || config.ProductId.empty() || config.TenantRsaKeyModulus.empty())
		{
			std::cerr << "Missing required configuration fields." << std::endl;
			std::cerr << "Please check your configuration file." << std::endl;
			exit(EXIT_FAILURE);
		}

		return config;
	}
}
