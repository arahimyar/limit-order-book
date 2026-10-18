cmake_minimum_required(VERSION 3.25)

project(OrderBook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git  GIT_TAG v1.14.0)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(googlebenchmark GIT_REPOSITORY https://github.com/google/benchmark.git GIT_TAG v1.8.3)

FetchContent_MakeAvailable(googletest googlebenchmark)

add_library(OrderBookLib 
    src/OrderBook.cpp
    src/display.cpp
    src/parser.cpp
)

target_include_directories(OrderBookLib PUBLIC include)

add_executable(OrderBook src/main.cpp)
target_link_libraries(OrderBook PRIVATE OrderBookLib)

enable_testing()
add_executable(OrderBookTests 
    tests/tests_limit.cpp
    tests/tests_market.cpp
    tests/tests_limitmarket_matching.cpp
    tests/tests_iceberg_matching.cpp
    tests/tests_stoploss_matching.cpp
    tests/tests_validation.cpp
    tests/tests_iceberg.cpp
    tests/tests_stoploss.cpp
    )
target_link_libraries(OrderBookTests PRIVATE OrderBookLib GTest::gtest_main)

# target_compile_options(OrderBookTests PRIVATE
#     -fsanitize=address,undefined
#     -fno-omit-frame-pointer)

# target_link_options(OrderBookTests PRIVATE
#     -fsanitize=address,undefined)

include(GoogleTest)
gtest_discover_tests(OrderBookTests)

add_executable(OrderBookBenchmarks
    benchmarks/benchmark_realistic.cpp

    benchmarks/single/benchmark_cancel_single.cpp
    benchmarks/single/benchmark_limitInsertion_single.cpp
    benchmarks/single/benchmark_icebergInsertion_single.cpp
    benchmarks/single/benchmark_stopLossInsertion_single.cpp
    benchmarks/single/benchmark_limitMatching_single.cpp
    benchmarks/single/benchmark_marketMatching_single.cpp

    benchmarks/single/benchmark_icebergMatching_single.cpp
    benchmarks/single/benchmark_stopLossMatching_single.cpp

    benchmarks/all/benchmark_limitInsertion_all.cpp
    benchmarks/all/benchmark_icebergInsertion_all.cpp
    benchmarks/all/benchmark_stopLossInsertion_all.cpp
    benchmarks/all/benchmark_cancel_all.cpp
    benchmarks/all/benchmark_limitMatching_all.cpp
    benchmarks/all/benchmark_marketMatching_all.cpp
    benchmarks/all/benchmark_icebergMatching_all.cpp
    benchmarks/all/benchmark_stopLossMatching_all.cpp
)

target_link_libraries(OrderBookBenchmarks PRIVATE OrderBookLib benchmark::benchmark_main)