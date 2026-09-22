#include <benchmark/benchmark.h>
#include <random>
#include <vector>
#include <random>
#include "OrderBook.h"

static void BM_realistic(benchmark::State& state){
    const int N = state.range(0);
    const int MOD = 10;
    std::mt19937 gen();
    std::vector<int> prices(N);
    std::vector<int> states(N);
    for (int i = 0; i < N; i++){
        states[i] = std::uniform_int_distribution<int>(1, 100)(gen);
        prices[i] = 100 + (i % MOD) - 5;
    }

    for (auto _ : state){
        OrderBook book;
        for (int i  = 0; i < N; i++){
            if (state[i] < 20){
                book.buyLimit(i, prices[i], 10);
            }
            else if (20 <= state[i] && states[i] < 40){
                book.sellLimit(i, prices[i], 10);
            }
            else  if (40 <= state[i] && states[i] < 60){
                if (i > 10) book.cancelOrder(i - 10);
            }
            else  if (60 <= state[i] && states[i] < 80){
                book.buyMarket(i, 10);
            }
            else{
                book.sellMarket(i, 10);
            }
        }
        benchmark::DoNotOptimize(book);
    }
    state.SetItemsProcessed(state.iterations() * N);

};