# Zentitle Licensing System C++ Application

## Overview
This project is a C++ application that interacts with the Zentitle licensing system to manage software activations. It allows users to activate their software with a code, retrieve activation information, and manage licensing details.

## Features
* Activation with a code
* Displaying activation information
* Dynamic loading of a core library for device fingerprinting
* Configuration via appsettings.json

## Prerequisites

Before building the project, make sure you have the following:
- **CMake** (3.20 or higher recommended)
- Zentitle2 SDK package (C++ wrapper for the Zentitle Licensing API)

### Before running the application, make sure to configure the required values in the appsettings.json file
```json
{
  "UseCoreLibrary": true,
  "CoreLibPath": "full/path/to/core/library",
  "Licensing": {
    "ApiUrl": "",
    "TenantId": "",
	"TenantRsaKeyModulus": "",
    "ProductId": ""
  }
}
```

* UseCoreLibrary * Set to true to use the native Zentitle core library.
* CoreLibPath *	Full absolute path to the native Zentitle core shared library (.dll, .so, or .dylib depending on platform). Example: /usr/local/lib/libZentitle2Core.so
In typical setups, this value will be automatically inferred by the application based on the ZENTITLE_CPP_SDK_DIR path provided at build time, 
so you usually don't need to modify it manually.
* ApiUrl	The URL of the Zentitle Licensing API. 
* TenantId * Your organization's unique tenant ID in Zentitle. 
* TenantRsaKeyModulus * Tenant RSA key’s modulus
* ProductId	* The ID of the product you want to activate. 

Tip: If you're unsure about the values for TenantId or ProductId, please contact your Zentitle administrator or check your Zentitle account dashboard.

##  Build Instructions
Platform-specific build instructions are located in the following files:

Linux Build Guide: 		README.Linux.md
Windows Build Guide:	README.Windows.md
macOS Build Guide: 		README.MacOS.md

These guides cover how to install required dependencies (cURL, OpenSSL), set up your environment, and build the SDK using CMake.
