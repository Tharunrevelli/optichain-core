cmake_minimum_required(VERSION 3.12)
project(OptiChainCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(CORE_SOURCES
    src/network.cpp
    src/inventory_policy.cpp
    src/discrete_event_sim.cpp
    src/spare_parts_optimizer.cpp
    src/warranty_model.cpp
)

add_library(optichain_lib STATIC ${CORE_SOURCES})

add_executable(optichain_engine src/main.cpp)
target_link_libraries(optichain_engine PRIVATE optichain_lib)

enable_testing()
add_executable(run_tests tests/test_suite.cpp)
target_link_libraries(run_tests PRIVATE optichain_lib)
add_test(NAME OptiChainUnitTests COMMAND run_tests)
