cmake_minimum_required(VERSION 3.20)

project (Zentitle.Activation.Example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

set(LICENSE_MANAGER_DIR "" CACHE PATH "Path to License Manager directory")

if(NOT EXISTS "${LICENSE_MANAGER_DIR}")
    message(FATAL_ERROR "LICENSE_MANAGER_DIR is not set or path does not exist. Please set -DLICENSE_MANAGER_DIR=/path/to/LicenseManager")
endif()

message(STATUS "Using License Manager from: ${LICENSE_MANAGER_DIR}")

set(SDK_ROOT "${LICENSE_MANAGER_DIR}" CACHE PATH "Path to SDK root")
add_subdirectory(${LICENSE_MANAGER_DIR} ${CMAKE_BINARY_DIR}/_license_manager_build)

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
    PUBLIC $<BUILD_INTERFACE:${LICENSE_MANAGER_DIR}>/src
	PUBLIC $<BUILD_INTERFACE:${LICENSE_MANAGER_DIR}>/src/Zentitle.Licensing.Client.CPP/Api/Models
)

include_directories(${LICENSE_MANAGER_DIR}/libextern_libraries/aixlog-master/include)
include_directories(${LICENSE_MANAGER_DIR}/libextern_libraries/nlohmann/)
include_directories(${CURL_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} LicenseManager)
target_link_libraries(${PROJECT_NAME} CURL::libcurl)
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)

# Post build actions
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/Config/appsettings.json
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/appsettings.json
    COMMENT "Copying appsettings.json to application directory"
)
