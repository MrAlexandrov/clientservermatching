cmake_minimum_required(VERSION 3.12)
project(Client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

find_package(cpr REQUIRED)

add_executable(Client client.cpp)
target_link_libraries(Client PRIVATE cpr::cpr OpenSSL::SSL)