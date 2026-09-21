cmake_minimum_required(VERSION 3.25)

project(OrderBook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git  GIT_TAG v1.14.0)
FetchContent_MakeAvailable(googletest)

add_library(OrderBookLib 
    src/Order.cpp 
    src/OrderBook.cpp
)

target_include_directories(OrderBookLib PUBLIC include)

add_executable(OrderBook src/main.cpp)
target_link_libraries(OrderBook PRIVATE OrderBookLib)

enable_testing()
add_executable(OrderBookTests 
    tests/tests_limit.cpp
    tests/tests_market.cpp
    tests/tests_matching.cpp
    tests/tests_validation.cpp
)
target_link_libraries(OrderBookTests PRIVATE OrderBookLib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(OrderBookTests)