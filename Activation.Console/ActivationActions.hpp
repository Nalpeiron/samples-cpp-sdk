#pragma once 

#include "Activation.hpp"
#include "DisplayHelper.hpp"
#include "SDKExceptions.hpp"
#include "LicensingApiException.hpp"
#include "ActivationCodeCredentialsModel.hpp"
#include "Helpers.hpp"
#include <optional>
#include <iostream>
#include <functional>
#include <ctime>
#include <iomanip>
#include <sstream>

#ifdef __APPLE__
#include <termios.h>
#include <unistd.h>

struct TerminalRawMode {
	termios original;
	TerminalRawMode() {
		tcgetattr(STDIN_FILENO, &original);
		termios raw = original;
		raw.c_lflag &= ~(ICANON | ECHO); // disable canonical mode & echo
		tcsetattr(STDIN_FILENO, TCSANOW, &raw);
	}
	~TerminalRawMode() {
		tcsetattr(STDIN_FILENO, TCSANOW, &original);
	}
};
#endif

#include "ActiveFeatureSet.hpp"
#include "ReadonlyFeatureSet.hpp"

namespace ActivationActions
{
	using namespace ZentitleLicensingClient;

	// Helper to format DateTime similar to C# format
	std::string FormatDateTime(const std::time_t& time)
	{
		std::tm tm = *std::localtime(&time);
		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
		return oss.str();
	}

	// Generic helper function to handle exceptions with a custom error message
	void ExecuteWithErrorHandling(const std::string& errorContext, const std::function<void()>& action)
	{
		try
		{
			action();
		}
		catch (LicensingApiException& ex)
		{
			ApiError error = ex.getApiError();
			DisplayHelper::WriteError(errorContext + ": " + error.toString());
		}
		catch (const SDKException& ex)
		{
			DisplayHelper::WriteError(errorContext + ": " + std::string(ex.what()));
		}
		catch (const std::exception& ex)
		{
			DisplayHelper::WriteError("Unexpected error in " + errorContext + ": " + std::string(ex.what()));
		}
	}

	void Initialize(Activation& activation)
	{
		ExecuteWithErrorHandling("Initialization failed", [&]()
			{
				auto initializeFuture = activation.initialize();
				initializeFuture.get();
				DisplayHelper::WriteSuccess("Initialization successful.");
			});
	}

	void ActivateWithCode(Activation& activation)
	{
		ExecuteWithErrorHandling("Activation failed", [&]()
			{
				std::string activationCode;
				std::cout << "Enter activation code: ";
				std::getline(std::cin, activationCode);


				std::shared_ptr<ActivationCodeCredentialsModel> credentials = std::make_shared<ActivationCodeCredentialsModel>(
					activationCode
				);

				std::string seatName;
				std::cout << "Enter seat name (keep empty for no seat name): ";
				std::getline(std::cin, seatName);

				std::string editionId;
				std::cout << "Enter edition ID (keep empty for default edition): ";
				std::getline(std::cin, editionId);

				auto activationFuture = activation.activate(credentials, seatName, editionId);
				auto activationInfo = activationFuture.get();

				DisplayHelper::ShowActivationStateModelPanel(activation);

				DisplayHelper::WriteSuccess("Activation successful.");
			});
	}

	void GenerateOfflineActivationRequest(Activation& activation)
	{
		ExecuteWithErrorHandling("Generating offline activation request failed", [&]()
			{
				DisplayHelper::WriteWarning(
					"Make sure that the product/entitlement that you want to use for the offline activation has the " +
					std::string("[Offline Lease Period] initialized, ") +
					std::string("otherwise the offline seat activation will not be possible.")
				);

				std::string activationCode;
				std::cout << "Enter activation code: ";
				std::getline(std::cin, activationCode);

				std::string seatName;
				std::cout << "Enter seat name (keep empty for no seat name): ";
				std::getline(std::cin, seatName);

				std::cout << "Generating activation request token..." << std::endl;

				auto tokenFuture = activation.generateOfflineActivationRequestToken(
					activationCode,
					seatName,
					std::string("")
				);
				auto token = tokenFuture.get();

				std::cout << "Activation request token (copy and use in the End User Portal):" << std::endl;
				DisplayHelper::WriteSuccess(token);
			});
	}

	void PullRemoteState(Activation& activation)
	{
		ExecuteWithErrorHandling("Failed to pull remote state", [&]()
			{
				std::cout << "Pulling current activation state from the server..." << std::endl;
				auto activationInfo = activation.pullRemoteState();
				DisplayHelper::ShowActivationStateModelPanel(activation);
			});
	}

	void PullPersistedState(Activation& activation)
	{
		ExecuteWithErrorHandling("Failed to pull persistent data", [&]()
			{
				std::cout << "Pulling current activation state from the local storage..." << std::endl;
				auto persistenceDataFuture = activation.pullPersistedState();
				auto persistenceData = persistenceDataFuture.get();

				if (persistenceData.isEmpty())
				{
					DisplayHelper::WriteError("No persistent data found.");
					return;
				}

				DisplayHelper::ShowActivationStateModelPanel(persistenceData);
			});
	}

	void RefreshLease(Activation& activation)
	{
		ExecuteWithErrorHandling("Refreshing lease failed", [&]()
			{
				std::cout << "Refreshing current activation..." << std::endl;
				auto previousLeaseExpiryOpt = activation.getActivationInfo().leaseExpiry;

				if (!previousLeaseExpiryOpt.has_value())
				{
					throw SDKException("No previous lease expiry found");
				}
				auto previousLeaseExpiry = previousLeaseExpiryOpt.value();

				auto leaseFuture = activation.refreshLease();
				auto refreshed = leaseFuture.get();

				if (!activation.getActivationInfo().leaseExpiry.has_value())
				{
					throw SDKException("No lease expiry found after refresh");
				}

				auto currentLeaseExpiry = activation.getActivationInfo().leaseExpiry.value();

				if (!refreshed)
				{
					std::cout << "Activation lease period could not be refreshed, please activate again. Current lease expiry is "
						<< FormatDateTime(currentLeaseExpiry) << std::endl;
				}
				else
				{
					auto newLeaseExpiry = currentLeaseExpiry;
					std::cout << "Activation lease successfully refreshed from ["
						<< FormatDateTime(previousLeaseExpiry) << "] to ["
						<< FormatDateTime(newLeaseExpiry) << "]" << std::endl;
				}
			});
	}

	void RefreshLeaseOffline(Activation& activation)
	{
		ExecuteWithErrorHandling("Refreshing offline lease failed", [&]()
			{
				std::string refreshToken;
				std::cout << "Enter offline refresh token: ";

#ifdef __APPLE__
				TerminalRawMode raw;
				char ch;
				while (std::cin.get(ch)) {
					std::cout << ch << std::flush; // echo manually
					if (ch == '\n') break;         // finish on Enter
					refreshToken += ch;
				}
#else
				std::getline(std::cin, refreshToken);
#endif

				auto persistedStateFuture = activation.pullPersistedState();
				persistedStateFuture.wait();

				auto persistedState = persistedStateFuture.get();

				auto activationState = persistedState.getActivationInfo();
				auto previousLeaseExpiryOpt = activationState->leaseExpiry;

				auto leaseFuture = activation.refreshLeaseOffline(refreshToken);
				leaseFuture.wait();
				leaseFuture.get();

				persistedStateFuture = activation.pullPersistedState();
				persistedStateFuture.wait();

				persistedState = persistedStateFuture.get();
				activationState = persistedState.getActivationInfo();
				auto currentLeaseExpiryOpt = activationState->leaseExpiry;

				if (previousLeaseExpiryOpt && currentLeaseExpiryOpt)
				{
					std::cout << "Activation lease successfully refreshed from ["
						<< FormatDateTime(previousLeaseExpiryOpt.value()) << "] to ["
						<< FormatDateTime(currentLeaseExpiryOpt.value()) << "]" << std::endl;
				}
				else
				{
					std::cout << "Lease expiry info missing. Please verify the activation state." << std::endl;
				}
			});
	}

	void Deactivate(Activation& activation)
	{
		ExecuteWithErrorHandling("Deactivation failed", [&]()
			{
				std::cout << "Deactivating the license..." << std::endl;
				auto deactivateFuture = activation.deactivate();
				ActivationOperationRes success = deactivateFuture.get();
				if (success.IsSuccess())
				{
					DisplayHelper::WriteSuccess("Deactivation successful.");
				}
				else
				{
					DisplayHelper::WriteError("Deactivation failed.");
				}
			});
	}

	void DeactivateOffline(Activation& activation)
	{
		ExecuteWithErrorHandling("Offline deactivation failed", [&]()
			{
				std::cout << "Deactivating the offline license..." << std::endl;
				auto deactivateFuture = activation.deactivateOffline();
				auto offlineDeactivationToken = deactivateFuture.get();

				if (!offlineDeactivationToken.empty())
				{
					std::cout << "Offline deactivation token (copy and use in the End User Portal):" << std::endl;
					DisplayHelper::WriteSuccess(offlineDeactivationToken);
				}
				else
				{
					DisplayHelper::WriteError("Offline deactivation failed.");
				}
			});
	}

	void GetActivationEntitlement(Activation& activation)
	{
		ExecuteWithErrorHandling("Failed to retrieve activation entitlement", [&]()
			{
				std::cout << "Retrieving the entitlement..." << std::endl;
				auto entitlementFuture = activation.getActivationEntitlement();
				auto entitlement = entitlementFuture.get();

				if (entitlement.isEmpty())
				{
					DisplayHelper::WriteError("No activation entitlement found.");
					return;
				}

				DisplayHelper::ShowEntitlementInfoPanel(entitlement);
			});
	}

	void ShowActivationInfo(Activation& activation)
	{
		auto persistedStateFuture = activation.pullPersistedState();
		persistedStateFuture.wait();
		auto persistedState = persistedStateFuture.get();
		DisplayHelper::ShowActivationStateModelPanel(persistedState);
	}

	void CheckoutFeature(Activation& activation)
	{
		ExecuteWithErrorHandling("Feature checkout failed", [&]()
			{
				std::shared_ptr<IFeatureSet> features = activation.features();
				if (auto activeFeatureSet = std::dynamic_pointer_cast<ActiveFeatureSet>(features))
				{
					// Filter features that have availability > 0
					auto featuresVector = activeFeatureSet->getFeaturesAsVector();
					std::vector<ActivationFeature> availableFeatures;

					for (const auto& feature : featuresVector)
					{
						if (feature.type != FeatureType::Bool)
						{
							availableFeatures.push_back(feature);
						}
					}

					if (availableFeatures.empty())
					{
						DisplayHelper::WriteError("There are no features eligible for checkout");
						return;
					}

					std::cout << "Following features can be checked out:" << std::endl;
					DisplayHelper::ShowFeaturesTable(availableFeatures);

					std::string featureKey;
					std::cout << "Select feature to checkout (or type 'None' to cancel): ";
					std::getline(std::cin, featureKey);

					if (featureKey == "None")
					{
						return;
					}

					int amountToCheckout;
					std::cout << "Specify amount to checkout: ";
					std::string amountStr;
					std::getline(std::cin, amountStr);
					if (!InputHelper::TryParseInt(amountStr, amountToCheckout) || amountToCheckout <= 0)
					{
						DisplayHelper::WriteError("Invalid amount. Please enter a positive integer.");
						return;
					}

					std::cout << "Checking out " << amountToCheckout << " "
						<< (amountToCheckout > 1 ? "features" : "feature")
						<< " with key '" << featureKey << "'" << std::endl;

					auto checkoutFuture = activeFeatureSet->checkoutFeature(featureKey, amountToCheckout);
					checkoutFuture.get();

					std::cout << "Feature successfully checked out!" << std::endl;

					// Get updated features and display them
					auto updatedFeatures = activeFeatureSet->getFeaturesAsVector();
					DisplayHelper::ShowFeaturesTable(updatedFeatures, featureKey);
				}
				else
				{
					DisplayHelper::WriteError("Feature checkout is not allowed.");
				}
			});
	}

	void ReturnFeature(Activation& activation)
	{
		ExecuteWithErrorHandling("Feature return failed", [&]()
			{
				std::shared_ptr<IFeatureSet> features = activation.features();
				if (auto activeFeatureSet = std::dynamic_pointer_cast<ActiveFeatureSet>(features))
				{
					// Filter features that are element pools and have active > 0
					auto featuresVector = activeFeatureSet->getFeaturesAsVector();
					std::vector<ActivationFeature> returnableFeatures;

					for (const auto& feature : featuresVector)
					{
						if (feature.type == FeatureType::ElementPool)
						{
							returnableFeatures.push_back(feature);
						}
					}

					if (returnableFeatures.empty())
					{
						DisplayHelper::WriteError("There are no features eligible for return");
						return;
					}

					std::cout << "Following features can be returned:" << std::endl;
					DisplayHelper::ShowFeaturesTable(returnableFeatures);

					std::string featureKey;
					std::cout << "Select feature to return (or type 'None' to cancel): ";
					std::getline(std::cin, featureKey);

					if (featureKey == "None")
					{
						return;
					}

					int amountToReturn;
					std::cout << "Specify amount to return: ";
					std::string amountStr;
					std::getline(std::cin, amountStr);
					if (!InputHelper::TryParseInt(amountStr, amountToReturn) || amountToReturn <= 0)
					{
						DisplayHelper::WriteError("Invalid amount. Please enter a positive integer.");
						return;
					}

					std::cout << "Returning " << amountToReturn << " "
						<< (amountToReturn > 1 ? "features" : "feature")
						<< " with key '" << featureKey << "'" << std::endl;

					auto returnFuture = activeFeatureSet->returnFeature(featureKey, amountToReturn);
					returnFuture.get();

					std::cout << "Feature successfully returned!" << std::endl;

					// Get updated features and display them
					auto updatedFeatures = activeFeatureSet->getFeaturesAsVector();
					DisplayHelper::ShowFeaturesTable(updatedFeatures, featureKey);
				}
				else
				{
					DisplayHelper::WriteError("Feature return is not allowed.");
				}
			});
	}

	void TrackFeatureUsage(Activation& activation)
	{
		ExecuteWithErrorHandling("Feature usage tracking failed", [&]()
			{
				std::shared_ptr<IFeatureSet> features = activation.features();
				if (auto activeFeatureSet = std::dynamic_pointer_cast<ActiveFeatureSet>(features))
				{
					// Filter for bool features only
					auto featuresVector = activeFeatureSet->getFeaturesAsVector();
					std::vector<ActivationFeature> boolFeatures;

					for (const auto& feature : featuresVector)
					{
						if (feature.type == FeatureType::Bool)
						{
							boolFeatures.push_back(feature);
						}
					}

					if (boolFeatures.empty())
					{
						DisplayHelper::WriteError("There are no bool features");
						return;
					}

					std::cout << "Usage can be tracked on the following features:" << std::endl;
					DisplayHelper::ShowFeaturesTable(boolFeatures);

					std::string featureKey;
					std::cout << "Select feature for tracking the usage (or type 'None' to cancel): ";
					std::getline(std::cin, featureKey);

					if (featureKey == "None")
					{
						return;
					}

					auto trackUsageFuture = activeFeatureSet->trackUsage(featureKey);
					trackUsageFuture.get();
					std::cout << "Feature usage successfully tracked!" << std::endl;
				}
				else
				{
					DisplayHelper::WriteError("Feature tracking is not allowed.");
				}
			});
	}

	void ActivateOffline(Activation& activation)
	{
		ExecuteWithErrorHandling("Offline activation failed", [&]()
			{
				std::string offlineActivationResponseToken;

#ifdef __APPLE__
				std::cout << "Enter offline activation response token (finish with Enter): ";

				TerminalRawMode raw;
				char ch;
				while (std::cin.get(ch)) {
					std::cout << ch << std::flush; // echo manually
					if (ch == '\n') break;         // finish on Enter
					offlineActivationResponseToken += ch;
				}
#else
				std::cout << "Enter offline activation response token: ";
				std::getline(std::cin, offlineActivationResponseToken);
#endif

				std::cout << "\nActivating offline..." << std::endl;

				auto offlineActivationFuture = activation.activateOffline(offlineActivationResponseToken);
				auto activationInfo = offlineActivationFuture.get();

				auto persistedStateFuture = activation.pullPersistedState();
				persistedStateFuture.wait();
				auto persistedState = persistedStateFuture.get();
				DisplayHelper::ShowActivationStateModelPanel(persistedState);
				DisplayHelper::WriteSuccess("Offline activation successful.");
			});
	}

	struct ActivationActionMapping
	{
		ActivationState state;
		std::vector<std::function<void(Activation&, const std::string&)>> actions;
		std::vector<std::string> actionNames;
	};

	std::vector<ActivationActionMapping> AvailableActions = {
		{
			ActivationState::Active,
			{
				[](Activation& activation, const std::string& host) { ShowActivationInfo(activation); },
				[](Activation& activation, const std::string& host) { PullRemoteState(activation); },
				[](Activation& activation, const std::string& host) { PullPersistedState(activation); },
				[](Activation& activation, const std::string& host) { CheckoutFeature(activation); },
				[](Activation& activation, const std::string& host) { ReturnFeature(activation); },
				[](Activation& activation, const std::string& host) { TrackFeatureUsage(activation); },
				[](Activation& activation, const std::string& host) { RefreshLease(activation); },
				[](Activation& activation, const std::string& host) { RefreshLeaseOffline(activation); },
				[](Activation& activation, const std::string& host) { Deactivate(activation); },
				[](Activation& activation, const std::string& host) { DeactivateOffline(activation); },
				[](Activation& activation, const std::string& host) { GetActivationEntitlement(activation); }
			},
			{
				"Show activation info",
				"Pull activation state from the server",
				"Pull activation state from the local storage",
				"Checkout advanced feature",
				"Return element-pool feature",
				"Track usage of a bool feature",
				"Refresh activation lease",
				"Refresh offline activation lease (with refresh token from End User Portal)",
				"Deactivate license",
				"Deactivate offline license",
				"Get entitlement associated with the activation"
			}
		},
		{
			ActivationState::LeaseExpired,
			{
				[](Activation& activation, const std::string& host) { ShowActivationInfo(activation); },
				[](Activation& activation, const std::string& host) { PullRemoteState(activation); },
				[](Activation& activation, const std::string& host) { PullPersistedState(activation); },
				[](Activation& activation, const std::string& host) { RefreshLease(activation); },
				[](Activation& activation, const std::string& host) { RefreshLeaseOffline(activation); },
				[](Activation& activation, const std::string& host) { Deactivate(activation); },
				[](Activation& activation, const std::string& host) { DeactivateOffline(activation); },
				[](Activation& activation, const std::string& host) { GetActivationEntitlement(activation); }
			},
			{
				"Show activation info",
				"Pull activation state from the server",
				"Pull activation state from the local storage",
				"Refresh activation lease",
				"Refresh offline activation lease (with refresh token from End User Portal)",
				"Deactivate license",
				"Deactivate offline license",
				"Get entitlement associated with the activation"
			}
		},
		{
			ActivationState::NotActivated,
			{
				[](Activation& activation, const std::string& host) { ShowActivationInfo(activation); },
				[](Activation& activation, const std::string& host) { PullPersistedState(activation); },
				[](Activation& activation, const std::string& host) { ActivateWithCode(activation); },
				[](Activation& activation, const std::string& host) { GenerateOfflineActivationRequest(activation); },
				[](Activation& activation, const std::string& host) { ActivateOffline(activation); }
			},
			{
				"Show activation info",
				"Pull activation state from the local storage",
				"Activate license with code",
				"Generate offline activation request (for End User Portal)",
				"Activate offline (with activation response from End User Portal)"
			}
		},
		{
			ActivationState::EntitlementNotActive,
			{
				[](Activation& activation, const std::string& host) { ShowActivationInfo(activation); },
				[](Activation& activation, const std::string& host) { PullRemoteState(activation); },
				[](Activation& activation, const std::string& host) { PullPersistedState(activation); },
				[](Activation& activation, const std::string& host) { GetActivationEntitlement(activation); },
				[](Activation& activation, const std::string& host) { ActivateWithCode(activation); },
				[](Activation& activation, const std::string& host) { GenerateOfflineActivationRequest(activation); },
				[](Activation& activation, const std::string& host) { ActivateOffline(activation); }
			},
			{
				"Show activation info",
				"Pull activation state from the server",
				"Pull activation state from the local storage",
				"Get entitlement associated with the activation",
				"Activate license with code",
				"Generate offline activation request (for End User Portal)",
				"Activate offline (with activation response from End User Portal)"
			}
		}
	};

	void RunActionByState(Activation& activation, const std::string& host)
	{
		auto currentState = activation.getState();

		auto actionMapping = std::find_if(AvailableActions.begin(), AvailableActions.end(),
			[currentState](const ActivationActionMapping& mapping) {
				return mapping.state == currentState;
			});

		if (actionMapping == AvailableActions.end())
		{
			DisplayHelper::WriteError("No actions available for current activation state");
			return;
		}

		std::cout << "Available actions for current state [" << activation.getStateAsString() << "]:" << std::endl;
		for (size_t i = 0; i < actionMapping->actionNames.size(); i++)
		{
			std::cout << i + 1 << ". " << actionMapping->actionNames[i] << std::endl;
		}

		std::cout << "Select action (1-" << actionMapping->actionNames.size() << "): ";
		std::string choiceStr;
		std::getline(std::cin, choiceStr);

		size_t choice = 0;
		if (!InputHelper::TryParseSizeT(choiceStr, choice) || choice == 0)
		{
			DisplayHelper::WriteError("Invalid selection");
			return;
		}

		choice -= 1;
		if (choice < actionMapping->actions.size())
		{
			actionMapping->actions[choice](activation, host);
		}
		else
		{
			DisplayHelper::WriteError("Invalid selection");
		}
	}

} // namespace ActivationActions
