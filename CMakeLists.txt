cmake_minimum_required(VERSION 3.12)
project(bifrost CXX)

find_package(userver COMPONENTS core postgresql QUIET)

if (NOT userver_FOUND)  # Fallback to subdirectory usage
    # Enable userver libraries that are needed in this project
    set(USERVER_FEATURE_POSTGRESQL ON CACHE BOOL "" FORCE)

    # Compatibility mode: some systems don't support these features
    set(USERVER_FEATURE_CRYPTOPP_BLAKE2 OFF CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_GRPC_CHANNELZ OFF CACHE BOOL "" FORCE)
    set(USERVER_FEATURE_REDIS_HI_MALLOC ON CACHE BOOL "" FORCE)

    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/userver)
        message(STATUS "Using userver framework from third_party/userver")
        add_subdirectory(third_party/userver)
    else ()
        message(FATAL_ERROR "Either install the userver or provide a path to it")
    endif ()
endif ()

userver_setup_environment()

# Common sources
add_library(${PROJECT_NAME}_objs OBJECT
        src/app/application_component.hpp
        src/app/application_component.cpp
        src/app/auth/auth_component.hpp
        src/app/auth/auth_component.cpp
        src/app/registry/registry_component.hpp
        src/app/registry/registry_component.cpp

        src/app/chat/chat.hpp
        src/app/chat/chat.cpp

        src/app/registry/stand_alone/user_registry.hpp
        src/app/registry/stand_alone/user_registry.cpp

        src/app/auth/stand_alone/auth_manager.hpp
        src/app/auth/stand_alone/auth_manager.cpp

        src/app/registry/stand_alone/receiver.hpp
        src/app/registry/stand_alone/receivers/single_receiver.hpp
        src/app/registry/stand_alone/receivers/single_receiver.cpp

        src/handlers/chat/socket_manager.hpp
        src/handlers/chat/socket_manager.cpp
        src/handlers/auth/auth_handler.hpp
        src/handlers/auth/auth_handler.cpp
        src/db/sql.hpp
        src/utils/token/token.cpp
        src/utils/conversion/message_conversion.hpp
        src/utils/metrics/socket_metrics.hpp
        src/utils/metrics/socket_metrics.cpp
)
include_directories(src)

target_link_libraries(${PROJECT_NAME}_objs PUBLIC userver::postgresql)

# The Service
add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objs)

userver_testsuite_add_simple()

## Unit Tests
#add_executable(${PROJECT_NAME}_unittest
#    src/hello_test.cpp
#)
#target_link_libraries(${PROJECT_NAME}_unittest PRIVATE ${PROJECT_NAME}_objs userver::utest)
#add_google_tests(${PROJECT_NAME}_unittest)


## Benchmarks
#add_executable(${PROJECT_NAME}_benchmark
#    src/hello_benchmark.cpp
#)
#target_link_libraries(${PROJECT_NAME}_benchmark PRIVATE ${PROJECT_NAME}_objs userver::ubench)
#add_google_benchmark_tests(${PROJECT_NAME}_benchmark)


## Functional Tests
include(UserverTestsuite)


# Install
include(GNUInstallDirs)

if (DEFINED ENV{PREFIX})
    message(STATUS "Set install prefix: $ENV{PREFIX}")
    file(TO_CMAKE_PATH "$ENV{PREFIX}" PREFIX_PATH)
    set(CMAKE_INSTALL_PREFIX ${PREFIX_PATH})
endif ()

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PROJECT_NAME})
install(FILES ${CONFIGS_FILES} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
