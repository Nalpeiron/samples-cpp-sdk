# Zentitle Licensing System C++ Sample Application (macOS)

## 1. Prerequisites

Install the required build tools and native dependencies:

```bash
brew install cmake curl openssl@3
```

If OpenSSL is not discovered automatically, export the Homebrew paths before configuring:

```bash
export LDFLAGS="-L$(brew --prefix openssl@3)/lib ${LDFLAGS:-}"
export CPPFLAGS="-I$(brew --prefix openssl@3)/include ${CPPFLAGS:-}"
```

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

## 3. Run the Sample

After building, the executable will be located at:

```text
build/Zentitle.Activation.Example
```

Before running:

- Edit `build/appsettings.json`.
- Keep `UseCoreLibrary` set to `true` for the current sample.
- `CoreLibPath` is generated automatically when `ZENTITLE_CPP_SDK_DIR` points to the unpacked SDK layout.
- Keep the matching `Zentitle2Core/<os_arch>/` runtime library available in the SDK package layout.

## 4. Configuration

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
- `CoreLibPath`: path to the native Zentitle core shared library (`.dylib`). When `ZENTITLE_CPP_SDK_DIR` points to the unpacked SDK layout, this path is generated automatically and usually does not need to be edited manually.
- `ApiUrl`: URL of the Zentitle Licensing API.
- `TenantId`: your Zentitle tenant identifier.
- `TenantRsaKeyModulus`: tenant RSA modulus used by the current sample configuration.
- `ProductId`: identifier of the product associated with an existing entitlement that you want to activate.

If you do not know the required tenant or product values, contact your Zentitle administrator.
