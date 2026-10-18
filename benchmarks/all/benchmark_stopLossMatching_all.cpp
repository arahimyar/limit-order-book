#include <benchmark/benchmark.h>
#include "OrderBook.h"
#include <vector>


static void BM_StopLossMatching_all(benchmark::State& state){
    const int N = state.range(0);
    const int MOD = std::max(5, N/20);
    std::vector<int> buyStopPrices(MOD);
    std::vector<int> sellStopPrices(MOD);
    for (int i  = 0; i < MOD; i++){
        sellStopPrices[i] = 105 + i + MOD;
        buyStopPrices[i] = 105 - i;
    }

    for (auto _ : state){
        state.PauseTiming();
        OrderBook book;
        for (int i  = 0; i < N; i++){
            if (i % 2 == 0){
                book.buyLimit(i, 80 + (i%20), 250);
            }
            else{
                book.sellLimit(i, 110 + (i%20), 250);
            }
        }

        for (int i = 0; i < N; i++){
            if (i%2 == 0){
                book.buyStopLoss(2*N + i, buyStopPrices[i%MOD], 10);
            }
            else{
                book.sellStopLoss(2*N + i, sellStopPrices[i%MOD], 10);
            }
        }
        state.ResumeTiming();
        for (int i = 0; i < N; i++){
            if (i%2 == 0){
                book.buyLimit(3*N+i, 110, 10);
            }
            else{
                book.sellLimit(3*N+i, 80, 10);
            }
        }
        benchmark::DoNotOptimize(book);
        state.PauseTiming();
    }
    state.SetItemsProcessed(state.iterations() * N);
    state.SetComplexityN(N);
};

BENCHMARK(BM_StopLossMatching_all)->RangeMultiplier(10)->Range(100, 1000000)->Complexity();