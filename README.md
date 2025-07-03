# Zentitle Licensing System C++ Application

## Overview
This project is a C++ application that interacts with the Zentitle licensing system to manage software activations. It allows users to activate their software with a code, retrieve activation information, and manage licensing details.

## Features
* Activation with a code
* Displaying activation information
* Dynamic loading of a core library for device fingerprinting
* Configuration via appsettings.json

## Test the SDK Using the Sample Application
1. Navigate to the build directory

```/Activation.Console```

2. Before running the application, make sure to configure the required values in the appsettings.json file
```json
{
  "UseCoreLibrary": true,
  "CoreLibPath": "full/path/to/core/library",
  "Licensing": {
    "ApiUrl": "",
    "TenantId": "",
    "ProductId": "",
    "SeatName": ""
  }
}
```

- UseCoreLibrary	Set to true to use the native Zentitle core library.
- CoreLibPath	Full absolute path to the native Zentitle core shared library (.dll, .so, or .dylib depending on platform). Example: /usr/local/lib/libZentitle2Core.so
- ApiUrl	The URL of the Zentitle Licensing API. 
- TenantId	Your organization's unique tenant ID in Zentitle. 
- ProductId	The ID of the product you want to activate. 
- SeatName	A user-defined identifier for the current machine/seat. Example: dev-machine-01 or johns-laptop. Should be unique per installation.

Tip: If you're unsure about the values for TenantId or ProductId, please contact your Zentitle administrator or check your Zentitle account dashboard.

3. Run the application:

```./Activation.Console```

##  Build Instructions
Platform-specific build instructions are located in the following files:

Linux Build Guide: 		README.Linux
Windows Build Guide:	README.Windows
macOS Build Guide: 		README.MacOS

These guides cover how to install required dependencies (cURL, OpenSSL), set up your environment, and build the SDK using CMake.
