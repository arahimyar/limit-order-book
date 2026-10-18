#include <benchmark/benchmark.h>
#include "OrderBook.h"
#include <vector>
#include <algorithm>
#include <random>

static void BM_Cancel_single(benchmark::State& state) {
    const int N = static_cast<int>(state.range(0));
    const int MOD = std::max(5, N/20);
    
    OrderBook book;
    std::vector<int> buyPrices(MOD);
    std::vector<int> sellPrices(MOD);
    for (int i  = 0; i < MOD; i++){
        sellPrices[i] = 100 + i + MOD;
        buyPrices[i] = 100 - i;
    }

    for(int i = 0; i < N; i++){
        if (i % 2 == 0){
            book.buyLimit(i, buyPrices[i % MOD], 100);
        }
        else{
            book.sellLimit(i, sellPrices[i % MOD], 100);
        }
    }

    int targetID = N / 2;

    for (auto _ : state) {
        benchmark::DoNotOptimize(book.cancelOrder(targetID));
        state.PauseTiming();
        if (targetID % 2 == 0){
            book.buyLimit(targetID, buyPrices[targetID % MOD], 100);
        }
        else{
            book.sellLimit(targetID, sellPrices[targetID % MOD], 100);
        }
        state.ResumeTiming();
    }

    state.SetItemsProcessed(state.iterations());
    state.SetComplexityN(N);
}

BENCHMARK(BM_Cancel_single)->RangeMultiplier(10)->Range(100, 1000000)->Complexity();