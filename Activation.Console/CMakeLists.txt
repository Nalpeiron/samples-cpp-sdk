cmake_minimum_required(VERSION 3.20)

project (Zentitle.Activation.Example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/cmake/PlatformConfig.cmake)
message("Detected platform: ${SYSTEM}")

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

set(ZENTITLE_CPP_SDK_DIR "" CACHE PATH "Path to the unpacked Zentitle SDK source directory (.../SDK/src)")

if(ZENTITLE_CPP_SDK_DIR STREQUAL "")
    message(FATAL_ERROR
        "ZENTITLE_CPP_SDK_DIR is required.\n"
        "Set it to the SDK source directory inside the unpacked SDK package.\n"
        "Example:\n"
        "  -DZENTITLE_CPP_SDK_DIR=/path/to/Zentitle2_SDK_VERSION/SDK/src"
    )
endif()

if(NOT EXISTS "${ZENTITLE_CPP_SDK_DIR}")
    message(FATAL_ERROR
        "ZENTITLE_CPP_SDK_DIR does not exist:\n"
        "  ${ZENTITLE_CPP_SDK_DIR}\n"
        "Expected a path like:\n"
        "  /path/to/Zentitle2_SDK_VERSION/SDK/src"
    )
endif()

set(_ZENTITLE_CPP_SDK_DIR_HINT "")
if(EXISTS "${ZENTITLE_CPP_SDK_DIR}/SDK/src/CMakeLists.txt")
    set(_ZENTITLE_CPP_SDK_DIR_HINT "${ZENTITLE_CPP_SDK_DIR}/SDK/src")
elseif(EXISTS "${ZENTITLE_CPP_SDK_DIR}/src/CMakeLists.txt" AND EXISTS "${ZENTITLE_CPP_SDK_DIR}/lib")
    set(_ZENTITLE_CPP_SDK_DIR_HINT "${ZENTITLE_CPP_SDK_DIR}/src")
endif()

if(NOT EXISTS "${ZENTITLE_CPP_SDK_DIR}/CMakeLists.txt" OR NOT IS_DIRECTORY "${ZENTITLE_CPP_SDK_DIR}/src/Zentitle.Licensing.Client.CPP")
    if(_ZENTITLE_CPP_SDK_DIR_HINT)
        message(FATAL_ERROR
            "ZENTITLE_CPP_SDK_DIR must point to the SDK 'src' directory inside the unpacked package.\n"
            "Received:\n"
            "  ${ZENTITLE_CPP_SDK_DIR}\n"
            "Try:\n"
            "  ${_ZENTITLE_CPP_SDK_DIR_HINT}"
        )
    else()
        message(FATAL_ERROR
            "ZENTITLE_CPP_SDK_DIR must point to the SDK 'src' directory inside the unpacked package.\n"
            "Received:\n"
            "  ${ZENTITLE_CPP_SDK_DIR}\n"
            "Expected a path like:\n"
            "  /path/to/Zentitle2_SDK_VERSION/SDK/src"
        )
    endif()
endif()

message(STATUS "Using Zentitle CPP SDK from: ${ZENTITLE_CPP_SDK_DIR}")

set(ZENTITLE_CPP_SDK_ROOT "${ZENTITLE_CPP_SDK_DIR}" CACHE PATH "Path to Zentitle SDK root")

if(WIN32)
    set(ZENTITLE2CORE_FILENAME "Zentitle2Core.dll")
elseif(APPLE)
    set(ZENTITLE2CORE_FILENAME "libZentitle2Core.dylib")
elseif(UNIX)
    set(ZENTITLE2CORE_FILENAME "libZentitle2Core.so")
endif()

set(ZENTITLE2CORE_PATH "${ZENTITLE_CPP_SDK_ROOT}/../../Zentitle2Core/${SYSTEM}/${ZENTITLE2CORE_FILENAME}")

configure_file(
    ${CMAKE_SOURCE_DIR}/Config/appsettings.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/appsettings.json
    @ONLY
)

add_subdirectory(${ZENTITLE_CPP_SDK_DIR} ${CMAKE_BINARY_DIR}/_zentitle_cpp_sdk_build)

# List of source files
file (GLOB INCLUDE_FILES *.hpp)
file (GLOB SRC_FILES *.cpp)

add_executable(
    ${PROJECT_NAME}
    ${INCLUDE_FILES}
    ${SRC_FILES}
)

set(CURL_USE_OPENSSL ON)
set(CMAKE_USE_OPENSSL ON)

# Use the SDK headers directly from the source tree instead of the flattened,
# prebuilt `lib/${SYSTEM}/static/Include` copy. This gives a single source of
# truth for every header (notably IActivationStorage.hpp, which otherwise exists
# in both the Include dir and the source tree and collides under #pragma once),
# and lets the sample include internal SDK headers from Persistence/Storage.
set(ZEN_SDK_SRC "${ZENTITLE_CPP_SDK_DIR}/src/Zentitle.Licensing.Client.CPP")
target_include_directories(${PROJECT_NAME} PRIVATE
    ${ZEN_SDK_SRC}/Api/LicenseManager
    ${ZEN_SDK_SRC}/Api/Models
    ${ZEN_SDK_SRC}/Features
    ${ZEN_SDK_SRC}/Commons
    ${ZEN_SDK_SRC}/Persistence
    ${ZEN_SDK_SRC}/Persistence/Storage
    ${ZEN_SDK_SRC}/Zentitle2CoreLibrary
    ${ZENTITLE_CPP_SDK_DIR}/libraries/third_party/aixlog/include
)

include_directories(${ZENTITLE_CPP_SDK_DIR}/libextern_libraries/nlohmann/)
include_directories(${CURL_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} LicenseManager)
target_link_libraries(${PROJECT_NAME} CURL::libcurl)
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_BINARY_DIR}/appsettings.json"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
