#include <benchmark/benchmark.h>
#include "OrderBook.h"
#include <vector>

static void BM_IcebergInsertion_all(benchmark::State& state){
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
        state.ResumeTiming();
        for (int i  = 0; i < N; i++){
            if (i % 2 == 0){
                book.buyIceberg(i, buyPrices[i % MOD], 100, 10);
            }
            else{
                book.sellIceberg(i, sellPrices[i % MOD], 100, 10);
            }
        }
        benchmark::DoNotOptimize(book);
        state.PauseTiming();
    }
    state.SetItemsProcessed(state.iterations() * N);
    state.SetComplexityN(N);
};

BENCHMARK(BM_IcebergInsertion_all)->RangeMultiplier(10)->Range(100, 1000000)->Complexity();