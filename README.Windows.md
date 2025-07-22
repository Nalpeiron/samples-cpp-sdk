# Zentitle Licensing System C++ Application (Windows)

## Setup Instructions

### 1. Clone or Extract the Code

Clone or extract the project source code to your local development directory.

### 2. Install Dependencies with vcpkg

Make sure `vcpkg` is cloned and bootstrapped:

```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

Install required packages (example):

```bash
vcpkg install openssl curl
```

### 3. Configure and Build with CMake

Open a **Developer Command Prompt for Visual Studio 2022**, then run:

```bash
mkdir build
cd build

cmake .. ^
  -G "Visual Studio 17 2022" ^
  -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake" ^
  -DZENTITLE_CPP_SDK_DIR="C:/path/to/Zentitle2_SDK_VERSION/SDK/src/"
  -DCURL_CA_BUNDLE="C:/path/to/Zentitle2_SDK_VERSION/samples/Activation.Console/Config/cert/cacert.pem"

cmake --build . --config Release
```

Parameter explanations:

* -G "Visual Studio 17 2022"
Specifies the generator to use – in this case, the one for Visual Studio 2022. This tells CMake to generate project files compatible with that version.

* -DCMAKE_TOOLCHAIN_FILE="..."
Informs CMake to use the specified vcpkg toolchain file. This allows automatic integration of vcpkg-managed dependencies.

* -DZENTITLE_CPP_SDK_DIR="..."
Custom CMake variable used to locate the Zentitle C++ SDK (e.g. headers and compiled libraries).
Important: This path must point to the src subdirectory inside the SDK, where the actual header and source files reside.
Example:
```bash
-DZENTITLE_CPP_SDK_DIR="C:/Users/pawel/OneDrive/Documents/praca/Zentitle2_SDK_v2.1.2/SDK/src/"
```

* -DCURL_CA_BUNDLE="..."
Tells libcurl (used internally by the SDK or app) where to find the CA certificate bundle (cacert.pem). This is necessary to verify TLS/HTTPS connections.

Replace all placeholder paths (e.g. C:/path/to/vcpkg, Zentitle2_SDK_VERSION) with the actual paths on your system.

### 4. Running the Application

After building, the output binary will be located in:

```
build/Release/Zentitle.Activation.Example.exe
```