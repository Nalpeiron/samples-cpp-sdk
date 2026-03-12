# Zentitle Licensing System C++ Sample Application (Windows)

## 1. Prerequisites

Before building, make sure you have:

- Visual Studio 2022 or Visual Studio Build Tools 2022 with MSVC x64 tools, MSBuild, and the Windows SDK (`Desktop development with C++`)
- CMake 3.20 or newer
- `vcpkg`

If you need Visual Studio 2019 or Visual Studio Build Tools 2019 instead, use the matching generator manually and keep it aligned with the SDK documentation for your environment.

Make sure `vcpkg` is cloned and bootstrapped:

```bat
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
```

The sample uses `vcpkg` manifest mode, so dependencies are resolved automatically during CMake configure when the toolchain file is provided.

## 2. Configure and Build

The sample builds the SDK wrapper from `SDK/src` as part of the sample build.
You do not need to build the SDK separately first.

Open an `x64 Native Tools Command Prompt for Visual Studio 2022` or the equivalent prompt installed by Visual Studio Build Tools 2022, then run:

```bat
cd samples-cpp-sdk\Activation.Console

cmake -S . -B build ^
  -G "Visual Studio 17 2022" ^
  -A x64 ^
  -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
  -DVCPKG_TARGET_TRIPLET=x64-windows ^
  -DZENTITLE_CPP_SDK_DIR="C:/path/to/Zentitle2_SDK_VERSION/SDK/src"

cmake --build build --config Release
```

`ZENTITLE_CPP_SDK_DIR` must point to the `SDK/src` directory inside the unpacked SDK package.

Example:

```bat
-DZENTITLE_CPP_SDK_DIR="C:/path/to/Zentitle2_SDK_VERSION/SDK/src"
```

## 3. Run the Sample

After building, the executable will be located at:

```text
build/Release/Zentitle.Activation.Example.exe
```

Before running:

- Edit `build/Release/appsettings.json`.
- Keep `UseCoreLibrary` set to `true` for the current sample.
- `CoreLibPath` is generated automatically when `ZENTITLE_CPP_SDK_DIR` points to the unpacked SDK layout.
- Keep the matching `Zentitle2Core/<os_arch>/` runtime library available in the SDK package layout.

## 4. Configuration

After building, edit `build/Release/appsettings.json`.
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
- `CoreLibPath`: path to the native Zentitle core shared library (`.dll`). When `ZENTITLE_CPP_SDK_DIR` points to the unpacked SDK layout, this path is generated automatically and usually does not need to be edited manually.
- `ApiUrl`: URL of the Zentitle Licensing API. Find it in your Zentitle portal under `Administration -> API Credentials -> Licensing`, for example `https://<your-customer-address>/administration/api-credentials#licensing`.
- `TenantId`: your Zentitle tenant identifier. Find it in the same `Administration -> API Credentials -> Licensing` section.
- `TenantRsaKeyModulus`: tenant RSA modulus used by the current sample configuration. Find it in the same `Administration -> API Credentials -> Licensing` section.
- `ProductId`: identifier of the product associated with an existing entitlement that you want to activate. Find it in the specific entitlement.
- `Activation Code`: find it in the same entitlement. The sample asks for it at runtime and does not store it in `appsettings.json`.

If you do not know the required tenant or product values, contact your Zentitle administrator.

## 5. Troubleshooting: Long Windows Paths

If you encounter an error similar to:

```text
error MSB6003: The specified task executable "CL.exe" could not be run.
System.IO.DirectoryNotFoundException: Could not find a part of the path ...
Zentitle2CoreLibrary.dir\Release\Zentitle.<hash>.tlog
```

The likely issue is a path that is too long, especially when the project is inside OneDrive.

To fix it:

1. Move the project to a short path such as `C:\dev\zentitle`.
2. Recreate the `build` directory.
3. Re-run CMake and build again.
