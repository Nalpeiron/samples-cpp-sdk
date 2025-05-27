project (Activation.Console)
cmake_minimum_required(VERSION 3.20)
message ("project: ${PROJECT_NAME}")

find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

include(${CMAKE_SOURCE_DIR}/libextern_libraries/cmake/PlatformConfig.cmake)
message("Detected platform: ${SYSTEM}")

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
    PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>/src
)

include_directories(${CMAKE_SOURCE_DIR}/libextern_libraries/aixlog-master/include)
include_directories(${CMAKE_SOURCE_DIR}/libextern_libraries/nlohmann/)
include_directories(${CURL_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} LicenseManager)
target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)

if(WIN32)

  target_link_libraries(${PROJECT_NAME} ${OPENSSL_SSL_LIBRARY})	
  target_link_libraries(${PROJECT_NAME} ${OPENSSL_CRYPTO_LIBRARY})	
  target_link_libraries(${PROJECT_NAME} ws2_32.lib)	
  target_link_libraries(${PROJECT_NAME} crypt32.lib)	
  target_link_libraries(${PROJECT_NAME} wldap32.lib)	
  target_link_libraries(${PROJECT_NAME} advapi32.lib)	
    
endif()

# Post build actions
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/samples/Activation.Console/Config/appsettings.json
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/appsettings.json
    COMMENT "Copying appsettings.json to application directory"
)
