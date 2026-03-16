# Zentitle Licensing System C++ Sample Application (Linux)

## 1. Prerequisites

Install the required build tools and native dependencies:

```bash
sudo apt update
sudo apt install build-essential cmake libcurl4-openssl-dev libssl-dev zlib1g-dev
```

The sample builds the SDK wrapper from `SDK/src` as part of the sample build, so it also needs the SDK's native dependencies such as the zlib development package.

If you are using another Linux distribution, follow the dependency guidance from the SDK package under `SDK/src/README.Linux.md` and install the equivalent zlib development package for your distribution.

## 2. Configure and Build

The sample builds the SDK wrapper from `SDK/src` as part of the sample build.
You do not need to build the SDK separately first.

```bash
cd samples-cpp-sdk/Activation.Console

cmake -S . -B build \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Release \
  -DZENTITLE_CPP_SDK_DIR="/path/to/Zentitle2_SDK_VERSION/SDK/src"

cmake --build build
```

`ZENTITLE_CPP_SDK_DIR` must point to the `SDK/src` directory inside the unpacked SDK package.

With the `Unix Makefiles` generator, `Release` is selected through `-DCMAKE_BUILD_TYPE=Release`.
To build a debug variant, configure a separate build directory with `-DCMAKE_BUILD_TYPE=Debug`, for example:

```bash
cmake -S . -B build-debug \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DZENTITLE_CPP_SDK_DIR="/path/to/Zentitle2_SDK_VERSION/SDK/src"

cmake --build build-debug
```

Example:

```bash
-DZENTITLE_CPP_SDK_DIR="/path/to/Zentitle2_SDK_VERSION/SDK/src"
```

## 3. Configuration

After building, edit `build/appsettings.json`.
The generated file will contain the main settings used by the sample:

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

- `UseCoreLibrary`: keep this set to `true` for the current sample.
- `CoreLibPath`: path to the native Zentitle core shared library (`.so`). When `ZENTITLE_CPP_SDK_DIR` points to the unpacked SDK layout, this path is generated automatically and usually does not need to be edited manually.
- `ApiUrl`: URL of the Zentitle Licensing API. Find it in your Zentitle portal under `Administration -> API Credentials -> Licensing`, for example `https://<your-customer-address>/administration/api-credentials#licensing`.
- `TenantId`: your Zentitle tenant identifier. Find it in the same `Administration -> API Credentials -> Licensing` section.
- `TenantRsaKeyModulus`: tenant RSA modulus used by the current sample configuration. Find it in the same `Administration -> API Credentials -> Licensing` section.
- `ProductId`: identifier of the product associated with an existing entitlement that you want to activate. Find it in the specific entitlement.
- `Activation Code`: find it in the same entitlement. The sample asks for it at runtime and does not store it in `appsettings.json`.

If you do not know the required tenant or product values, contact your Zentitle administrator.

## 4. Run the Sample

After building, the executable will be located at:

```text
build/Zentitle.Activation.Example
```

Before running, keep the matching `Zentitle2Core/<os_arch>/` runtime library available in the SDK package layout.
The sample will prompt for the activation code at runtime.
