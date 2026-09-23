#include <benchmark/benchmark.h>
#include "OrderBook.h"
#include <vector>

static void BM_MarketMatching(benchmark::State& state){
    const int N = state.range(0);
    const int MOD = std::max(5, N/20);
    std::vector<int> buyPrices(MOD);
    std::vector<int> sellPrices(MOD);
    for (int i  = 0; i < MOD; i++){
        sellPrices[i] = 100 + i + MOD;
        buyPrices[i] = 100 - i;
    }

    for (auto _ : state){
        state.PauseTiming();
        OrderBook book;
        for (int i  = 0; i < N; i++){
            if (i % 2 == 0){
                book.buyLimit(i, buyPrices[i % MOD], 10);
            }
            else{
                book.sellLimit(i, sellPrices[i % MOD], 10);
            }
        }
        state.ResumeTiming();
        for (int i = 0; i < N; i++){
            if (i%2 == 0){
                book.sellMarket(N+i, 10);
            }
            else{
                book.buyMarket(N+i, 10);
            }
        }
        benchmark::DoNotOptimize(book);
    }
    state.SetItemsProcessed(state.iterations() * N);
};

BENCHMARK(BM_MarketMatching)->RangeMultiplier(10)->Range(100, 1000000);
