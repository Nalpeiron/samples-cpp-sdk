# Zentitle Licensing System C++ Application (macOS)

## Setup Instructions

### 1. Clone or Extract the Code

Clone or extract the project source code to your local development directory.

### 2. Configure and Build with CMake
   Use the following commands to configure and build the project:

```bash
cd samples-cpp-sdk/Activation.Console
mkdir build
cd build

cmake -G"Unix Makefiles" -DZENTITLE_CPP_SDK_DIR="/path/to/Zentitle2_SDK_VERSION/SDK/src/" ..
cmake --build . --config Release
```

* -DZENTITLE_CPP_SDK_DIR="..."
Custom CMake variable used to locate the Zentitle C++ SDK (e.g. headers and compiled libraries).
Important: This path must point to the src subdirectory inside the SDK, where the actual header and source files reside.
*Example:*
```bash
-DZENTITLE_CPP_SDK_DIR="/path/to/Zentitle2_SDK_v2.1.2/SDK/src/"
```

### 4. Running the Application

After building, the output binary will be located in:

```
build/Zentitle.Activation.Example
```