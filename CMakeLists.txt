cmake_minimum_required(VERSION 3.12)
project(matching CXX)


add_definitions(-DUSERVER_POSTGRES_ENABLE_LEGACY_TIMESTAMP)
# Adding userver dependency
find_package(userver COMPONENTS core postgresql QUIET)
if(NOT userver_FOUND)  # Fallback to subdirectory usage
    # Enable userver libraries that are needed in this project
    set(USERVER_FEATURE_POSTGRESQL ON CACHE BOOL "" FORCE)

    # Compatibility mode: some systems don't support these features
    set(USERVER_FEATURE_CRYPTOPP_BLAKE2 OFF CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_GRPC_CHANNELZ OFF CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_REDIS_HI_MALLOC ON CACHE BOOL "" FORCE)

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/userver)
        message(STATUS "Using userver framework from third_party/userver")
        add_subdirectory(third_party/userver)
    else()
        message(FATAL_ERROR "Either install the userver or provide a path to it")
    endif()
endif()

userver_setup_environment()


# Common sources
add_library(${PROJECT_NAME}_objs OBJECT
    src/models/user.hpp
    src/models/user.cpp
    src/models/order.hpp
    src/models/order.cpp
    src/models/session.hpp
    src/models/balance.hpp
    src/models/balance.cpp
    src/models/deal.hpp
    src/models/deal.cpp
    
    src/handlers/lib/auth.hpp
    src/handlers/lib/auth.cpp
    src/handlers/register/register.hpp
    src/handlers/register/register.cpp
    src/handlers/login/login.hpp
    src/handlers/login/login.cpp
    src/handlers/balance/balance.hpp
    src/handlers/balance/balance.cpp
    src/handlers/add-order/add-order.hpp
    src/handlers/add-order/add-order.cpp
    src/handlers/get-order/get-order.hpp
    src/handlers/get-order/get-order.cpp
    src/handlers/get-orders/get-orders.hpp
    src/handlers/get-orders/get-orders.cpp
    src/handlers/delete-order/delete-order.hpp
    src/handlers/delete-order/delete-order.cpp
    src/handlers/get-deals/get-deals.hpp
    src/handlers/get-deals/get-deals.cpp

    src/service/match_orders.hpp
    src/service/match_orders.cpp
)
target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver::postgresql)


# The Service
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)

# Functional Tests
include(UserverTestsuite)

userver_testsuite_add_simple()


# Install
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
  message(STATUS "Set install prefix: $ENV{PREFIX}")
  file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
  set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
endif()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
