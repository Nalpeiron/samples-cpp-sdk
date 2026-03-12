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

target_include_directories(${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${ZENTITLE_CPP_SDK_DIR}>/../lib/${SYSTEM}/static/Include
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
