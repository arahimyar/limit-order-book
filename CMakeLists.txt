cmake_minimum_required(VERSION 3.25)

project(OrderBook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git  GIT_TAG v1.18.0)
FetchContent_MakeAvailable(googletest)

add_executable(OrderBook OrderBook.cpp main.cpp)

enable_testing()
add_executable(OrderBookTests OrderBook.cpp testing.cpp)
target_link_libraries(OrderBookTests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(OrderBookTests)