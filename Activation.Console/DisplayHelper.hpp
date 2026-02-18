#pragma once 

#include <iostream>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>

#include "Activation.hpp"
#include "ActivationStateModel.hpp"
#include "ActivationStateHelpers.hpp"

#include "ActivationFeature.hpp"
#include "ActivationAttribute.hpp"

#include "ActiveFeatureSet.hpp"
#include "PersistentData.hpp"

#pragma warning(disable : 4996)

namespace DisplayHelper
{
	using namespace ZentitleLicensingClient;

	void ShowFeaturesTable(std::vector<ActivationFeature> features, const std::optional<std::string>& keyToHighlight = std::nullopt);
	void ShowAttributesTable(const std::vector<ActivationAttribute>& attributes);

	void SetConsoleColor(int colorCode)
	{
#ifdef _WIN32
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, colorCode);
#else
		(void)colorCode; // No-op on non-Windows
#endif
	}

	void WriteError(const std::string& message)
	{
		SetConsoleColor(4); // Red
		std::cerr << "Error: " << message << std::endl;
		SetConsoleColor(7); // Reset
	}

	void WriteSuccess(const std::string& message)
	{
		SetConsoleColor(2); // Green
		std::cout << "Success: " << message << std::endl;
		SetConsoleColor(7); // Reset
	}

	void WriteWarning(const std::string& message)
	{
		SetConsoleColor(6); // Yellow
		std::cout << "Warning: " << message << std::endl;
		SetConsoleColor(7); // Reset
	}

	std::string timeToString(std::time_t time)
	{
		if (time == 0)
			return "N/A";
		std::stringstream ss;
		ss << std::put_time(std::localtime(&time), "%F %T");
		return ss.str();
	}

	inline std::string licenseTypeToString(LicenseType licenseType)
	{
		switch (licenseType)
		{
		case LicenseType::Subscription:
			return "Subscription";
		case LicenseType::Perpetual:
			return "Perpetual";
		default:
			return "Unknown";
		}
	}

	inline std::string licenseStartTypeToString(LicenseStartType licenseStartType)
	{
		switch (licenseStartType)
		{
		case LicenseStartType::LicenseActivation:
			return "LicenseActivation";
		case LicenseStartType::LicenseEntitlementCreation:
			return "LicenseEntitlementCreation";
		case LicenseStartType::LicenseCustom:
			return "LicenseCustom";
		case LicenseStartType::LicenseManualActivation:
			return "LicenseManualActivation";
		default:
			return "Unknown";
		}
	}

	inline std::string intervalTypeToString(IntervalType intervalType)
	{
		switch (intervalType)
		{
		case IntervalType::None:
			return "None";
		case IntervalType::Day:
			return "Day";
		case IntervalType::Week:
			return "Week";
		case IntervalType::Month:
			return "Month";
		case IntervalType::Year:
			return "Year";
		case IntervalType::Hour:
			return "Hour";
		case IntervalType::Minute:
			return "Minute";
		default:
			return "Unknown";
		}
	}

	inline std::string planModelToString(const PlanModel& plan)
	{
		return "PlanModel: { name: " + plan.name + ", licenseType: " + licenseTypeToString(plan.licenseType) + ", licenseStartType: " + licenseStartTypeToString(plan.licenseStartType) + ", licenseDuration: " + intervalTypeToString(plan.licenseDuration.type);
	}

	inline std::string intervalToString(const Interval& interval)
	{
		return "type: " + intervalTypeToString(interval.type) + ", count: " + (interval.count.has_value() ? std::to_string(interval.count.value()) : "null");
	}


	void ShowEntitlementInfoPanel(const ActivationEntitlementModel& activationEntitlementData)
	{
		std::cout << "Entitlement Info:" << std::endl;
		std::cout << "    Customer Name: " << (activationEntitlementData.customerName.has_value() ? *activationEntitlementData.customerName : "N/A") << std::endl;
		std::cout << "    Customer Account Ref ID: " << (activationEntitlementData.customerAccountRefId.has_value() ? *activationEntitlementData.customerAccountRefId : "N/A") << std::endl;
		std::cout << "    Order Ref ID: " << (activationEntitlementData.orderRefId.has_value() ? *activationEntitlementData.orderRefId : "N/A") << std::endl;
		std::cout << "    Offering Name: " << activationEntitlementData.offeringName << std::endl;
		std::cout << "    SKU: " << activationEntitlementData.sku << std::endl;
		std::cout << "    Product Name: " << activationEntitlementData.productName << std::endl;
		std::cout << "    " << planModelToString(activationEntitlementData.plan) << std::endl;
		std::cout << "    Grace Period: " << intervalToString(activationEntitlementData.gracePeriod) << std::endl;
		std::cout << "    Linger Period: " << intervalToString(activationEntitlementData.lingerPeriod) << std::endl;
		std::cout << "    Lease Period: " << intervalToString(activationEntitlementData.leasePeriod) << std::endl;
		std::cout << "    Offline Lease Period: " << intervalToString(activationEntitlementData.offlineLeasePeriod) << std::endl;
		std::cout << "    Has Maintenance: " << (activationEntitlementData.hasMaintenance ? "true" : "false") << std::endl;
		std::cout << "    Maintenance Expiry Date: " << (activationEntitlementData.maintenanceExpiryDate.has_value() ? *activationEntitlementData.maintenanceExpiryDate : "N/A") << std::endl;
		std::cout << "    Snapshot Date: " << activationEntitlementData.snapshotDate << std::endl;
	}

	void ShowActivationStateModelPanel(const Activation& activation)
	{
		std::cout << "==================== Activation Info ====================" << std::endl;
		std::cout << "Activation State: " << std::endl;
		switch (activation.getState())
		{
		case ActivationState::Active:
			SetConsoleColor(2); // Green
			std::cout << "Active";
			break;
		case ActivationState::LeaseExpired:
			SetConsoleColor(6); // Yellow
			std::cout << "Lease Expired";
			break;
		case ActivationState::EntitlementNotActive:
			SetConsoleColor(4); // Red
			std::cout << "Entitlement Not Active";
			break;
		case ActivationState::NotActivated:
			SetConsoleColor(4); // Red
			std::cout << "Not Activated";
			break;
		default:
			SetConsoleColor(7); // Default gray
			std::cout << "Unknown";
			break;
		}

		SetConsoleColor(7); // Reset to default
		std::cout << std::endl;

		if (activation.getActivationInfo().productId)
		{
			std::cout << "Product ID: " << *activation.getActivationInfo().productId << std::endl;
		}
		if (activation.getActivationInfo().seatId)
		{
			std::cout << "Seat ID: " << *activation.getActivationInfo().seatId << std::endl;
		}

		if (activation.getActivationInfo().leaseExpiry)
		{
			std::cout << "Lease Expiry: " << timeToString(*activation.getActivationInfo().leaseExpiry) << std::endl;
		}

		ShowFeaturesTable(activation.getActivationInfo().features);
		ShowAttributesTable(activation.getActivationInfo().attributes);
		std::cout << "=========================================================" << std::endl;
	}

	void ShowActivationStateModelPanel(const Persistence::PersistentData& persistenceData)
	{
		std::cout << "==================== Activation Info (Persistence) ====================" << std::endl;

		auto entitlementInfo = persistenceData.getEntitlementData();
		if (entitlementInfo)
		{
			ShowEntitlementInfoPanel(*entitlementInfo);
		}
		else
		{
			std::cout << "No Entitlement Info available." << std::endl;
		}

		auto ActivationStateModel = persistenceData.getActivationInfo();

		if (!ActivationStateModel)
		{
			DisplayHelper::WriteWarning("No ActivationStateModel available.");
			std::cout << "=======================================================================" << std::endl;
			return;
		}

		if (ActivationStateModel->productId)
		{
			std::cout << "Product ID: " << *ActivationStateModel->productId << std::endl;
		}
		else
		{
			std::cout << "Product ID: N/A" << std::endl;
		}

		if (ActivationStateModel->seatId)
		{
			std::cout << "Seat ID: " << *ActivationStateModel->seatId << std::endl;
		}
		else
		{
			std::cout << "Seat ID: N/A" << std::endl;
		}

		if (ActivationStateModel->leaseExpiry)
		{
			std::cout << "Lease Expiry: " << DisplayHelper::timeToString(*ActivationStateModel->leaseExpiry) << std::endl;
		}
		else
		{
			std::cout << "Lease Expiry: N/A" << std::endl;
		}

		if (!ActivationStateModel->features.empty())
		{
			DisplayHelper::ShowFeaturesTable(ActivationStateModel->features);
		}
		else
		{
			std::cout << "\nNo Features available.\n";
		}

		if (!ActivationStateModel->attributes.empty())
		{
			DisplayHelper::ShowAttributesTable(ActivationStateModel->attributes);
		}
		else
		{
			std::cout << "\nNo Attributes available.\n";
		}

		std::cout << "=======================================================================" << std::endl;
	}

	void ShowFeaturesTable(std::vector<ActivationFeature> features, const std::optional<std::string>& keyToHighlight /*= std::nullopt*/)
	{
		const std::size_t keyWidth = 32;
		const std::size_t typeWidth = 12;
		const std::size_t activeWidth = 12;
		const std::size_t availableWidth = 12;
		const std::size_t totalWidth = 10;

		auto truncateWithEllipsis = [](const std::string& value, std::size_t width) -> std::string
			{
				if (value.size() <= width)
				{
					return value;
				}
				if (width <= 3)
				{
					return value.substr(0, width);
				}
				return value.substr(0, width - 3) + "...";
			};

		auto formatCell = [&](const std::string& value, std::size_t width) -> std::string
			{
				std::string truncated = truncateWithEllipsis(value, width);
				if (truncated.size() < width)
				{
					truncated.append(width - truncated.size(), ' ');
				}
				return truncated;
			};

		auto formatKeyCell = [&](const std::string& value, bool highlight, std::size_t width) -> std::string
			{
				std::string cell = formatCell(value, width);
				if (!highlight)
				{
					return cell;
				}
				return std::string("\033[34m") + cell + "\033[0m";
			};

		std::cout << "\n\033[1;36m=== Available Features ===\033[0m\n";
		std::cout
			<< formatCell("Feature Key", keyWidth)
			<< formatCell("Type", typeWidth)
			<< formatCell("ActiveState", activeWidth)
			<< formatCell("Available", availableWidth)
			<< formatCell("Total", totalWidth)
			<< "\n";
		std::cout << std::string(keyWidth + typeWidth + activeWidth + availableWidth + totalWidth, '-') << "\n";

		for (auto feature : features)
		{
			bool highlightKey = keyToHighlight && feature.key == *keyToHighlight;

			std::cout
				<< formatKeyCell(feature.key, highlightKey, keyWidth)
				<< formatCell(ActivationFeature::featureTypeToString(feature.type), typeWidth)
				<< formatCell(feature.active ? std::to_string(*feature.active) : "", activeWidth)
				<< formatCell(feature.available ? std::to_string(*feature.available) : "Unlimited", availableWidth)
				<< formatCell(feature.total ? std::to_string(*feature.total) : "Unlimited", totalWidth)
				<< "\n";
		}
		std::cout << std::string(keyWidth + typeWidth + activeWidth + availableWidth + totalWidth, '-') << "\n";

		std::vector<const ActivationFeature*> usageCountFeatures;
		for (const auto& feature : features)
		{
			if (feature.type == FeatureType::UsageCount)
			{
				usageCountFeatures.push_back(&feature);
			}
		}

		if (!usageCountFeatures.empty())
		{
			auto formatOptionalUtc = [](const std::optional<std::time_t>& value) -> std::string
				{
					if (!value.has_value())
					{
						return "null";
					}

					std::time_t ts = value.value();
					std::tm tm_utc{};
#if defined(_WIN32) || defined(_WIN64)
					gmtime_s(&tm_utc, &ts);
#else
					gmtime_r(&ts, &tm_utc);
#endif

					std::ostringstream timeStream;
					timeStream << std::put_time(&tm_utc, "%Y-%m-%dT%H:%M:%S") << "Z";
					return timeStream.str();
				};

			const std::size_t usageKeyWidth = 32;
			const std::size_t usageCurrentWidth = 21;
			const std::size_t usageNextWidth = 21;

			std::cout << "\n\033[1;36m=== Usage Count Details ===\033[0m\n";
			std::cout
				<< formatCell("Feature Key", usageKeyWidth)
				<< formatCell("CurrentStart", usageCurrentWidth)
				<< formatCell("NextStart", usageNextWidth)
				<< "\n";
			std::cout << std::string(usageKeyWidth + usageCurrentWidth + usageNextWidth, '-') << "\n";

			for (const auto* feature : usageCountFeatures)
			{
				std::cout
					<< formatCell(feature->key, usageKeyWidth)
					<< formatCell(formatOptionalUtc(feature->currentUsagePeriodStart), usageCurrentWidth)
					<< formatCell(formatOptionalUtc(feature->nextUsagePeriodStart), usageNextWidth)
					<< "\n";
			}

			std::cout << std::string(usageKeyWidth + usageCurrentWidth + usageNextWidth, '-') << "\n";
		}
	}

	void ShowAttributesTable(const std::vector<ActivationAttribute>& attributes)
	{
		std::cout << "\n\033[1;36m=== Activation Attributes ===\033[0m\n";
		std::cout << std::setw(20) << "Key"
			<< std::setw(15) << "Type"
			<< std::setw(20) << "Value" << "\n";

		std::cout << std::string(55, '-') << "\n";

		for (auto attribute : attributes)
		{
			std::cout << std::setw(20) << attribute.key
				<< std::setw(15) << attribute.type
				<< std::setw(20) << (attribute.value ? *attribute.value : "null") << "\n";
		}
		std::cout << std::string(55, '-') << "\n";
	}

	std::string EscapeConsoleOutput(const std::string& input)
	{
		std::string escaped;
		for (char c : input)
		{
			if (c == '\033') // Escape ANSI codes
				escaped += "[ESC]";
			else
				escaped += c;
		}
		return escaped;
	}
}
