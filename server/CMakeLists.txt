cmake_minimum_required(VERSION 3.20)

project(sider CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(Threads REQUIRED)

file(GLOB_RECURSE SERVER_SOURCES 
    "lib/*.h"
    "src/Command/*.cpp"
    "src/Socket/*.cpp"
    "src/Storage/*.cpp"
    "src/Storage/Entries/*.cpp"
    "src/Type/*.cpp"
)

file(GLOB_RECURSE PRODUCTION_SOURCES 
    "src/Time.cpp"
)

file(GLOB_RECURSE TEST_SOURCES
    "tests/Command/*.cpp"
)

add_executable(sider main.cpp ${SERVER_SOURCES} ${PRODUCTION_SOURCES})
add_executable(sider-tests tests.cpp ${SERVER_SOURCES} ${TEST_SOURCES})

target_link_libraries(sider PRIVATE
    spdlog::spdlog
    fmt::fmt
)

target_link_libraries(sider-tests PRIVATE
    GTest::GTest
    GTest::Main
    Threads::Threads
    spdlog::spdlog
    fmt::fmt
)
 
 enable_testing()
 add_test(NAME sider-tests COMMAND sider-tests) 
