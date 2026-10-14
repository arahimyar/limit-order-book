#include <benchmark/benchmark.h>
#include <random>
#include <vector>
#include "OrderBook.h"

enum class Operation{
    BUY_LIMIT,
    SELL_LIMIT,
    BUY_MARKET,
    SELL_MARKET,
    BUY_ICEBERG,
    SELL_ICEBERG,
    BUY_STOPLOSS,
    SELL_STOPLOSS,
    CANCEL
};

static void BM_realistic(benchmark::State& state){
    const int N = state.range(0);
    const int MOD = 10;
    std::mt19937 gen(42);
    std::vector<Operation> operations(N);
    std::vector<int> cancel_offsets(N);


    // 40% Limit, 10% Market, 20% Iceberg, 20% Stop Loss, 10% Cancel
    for (int i = 0; i < N; i++){
        int rand_state = std::uniform_int_distribution<int>(1, 100)(gen);
        
        if (rand_state < 20){
            operations[i]=(Operation::BUY_LIMIT);
        }
        else if (20 <= rand_state && rand_state < 40){
            operations[i]=(Operation::SELL_LIMIT);
        }
        else if (40 <= rand_state && rand_state < 45){
            operations[i]=(Operation::BUY_MARKET);
        }
        else if (45 <= rand_state && rand_state < 50){
            operations[i]=(Operation::SELL_MARKET);
        }
        else if (50 <= rand_state && rand_state < 60){
            operations[i]=(Operation::BUY_ICEBERG);
        }
        else if (60 <= rand_state && rand_state < 70){
            operations[i]=(Operation::SELL_ICEBERG);
        }
        else if (70 <= rand_state && rand_state < 80){
            operations[i]=(Operation::BUY_STOPLOSS);
        }
        else if (80 <= rand_state && rand_state < 90){
            operations[i]=(Operation::SELL_STOPLOSS);
        }
        else{
            operations[i]=(Operation::CANCEL);
        }

        cancel_offsets[i] = std::uniform_int_distribution<int>(1, 25)(gen);

    }
 
    for (auto _ : state){
        OrderBook book;
        state.ResumeTiming();
        for (int i  = 0; i < N; i++){
            switch (operations[i]){
                case Operation::BUY_LIMIT:
                    book.buyLimit(i+1, 97 + (i % MOD), 50);
                    break;
                case Operation::SELL_LIMIT:
                    book.sellLimit(i+1, 103 + (i % MOD), 50);
                    break;
                case Operation::BUY_MARKET:
                    book.buyMarket(i+1, 15);
                    break;
                case Operation::SELL_MARKET:
                    book.sellMarket(i+1, 15);
                    break;
                case Operation::BUY_ICEBERG:
                    book.buyIceberg(i+1, 97 + (i % MOD), 30, 10);
                    break;
                case Operation::SELL_ICEBERG:
                    book.sellIceberg(i+1, 103 + (i % MOD), 30, 10);
                    break;
                case Operation::BUY_STOPLOSS:
                    book.buyStopLoss(i+1, 108, 15);
                    break;
                case Operation::SELL_STOPLOSS:
                    book.sellStopLoss(i+1, 99, 15);
                    break;
                default:
                    if (i - cancel_offsets[i] > 0){
                        book.cancelOrder(i - cancel_offsets[i]);
                    }
                    break;

            }
        }
        benchmark::DoNotOptimize(book);
        state.PauseTiming();
    }
    state.SetItemsProcessed(state.iterations() * N);
    state.SetComplexityN(N);

};

BENCHMARK(BM_realistic)->RangeMultiplier(10)->Range(100, 1000000) -> Complexity();