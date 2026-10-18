#include <benchmark/benchmark.h>
#include <vector>
#include <algorithm>
#include <random>

#include "OrderBook.h"


static void BM_StopLossMatch_single(benchmark::State& state) {
    const int N = static_cast<int>(state.range(0));
    const int orders_per_level = 20;
    const int num_levels = std::max(1, N/orders_per_level);
    
    OrderBook book;
    std::vector<int> buyPrices(num_levels);
    std::vector<int> sellPrices(num_levels);

    for(int i = 0; i < num_levels; i++){
        sellPrices[i] = 50001 + i;
        buyPrices[i] = 49999 - i;
    }

    int idCounter = 0;
    for(int level = 0; level < num_levels; level++){
        for (int i = 0; i < orders_per_level; i++){
            if(level%2 == 0){
                book.buyStopLoss(idCounter++, sellPrices[level], 100);
            }
            else{
                book.sellStopLoss(idCounter++,buyPrices[level], 100);
            }
        }
    }

    for (auto _ : state) {
        if (idCounter % 2 == 0){
            book.buyLimit(idCounter, 50001, 100);
            book.sellLimit(++idCounter, 50001, 200);
        }
        else{
            book.buyLimit(idCounter, 49998, 200);
            book.sellLimit(++idCounter, 49998, 100);
        }
        state.PauseTiming();
        idCounter++;
        for (int i = 0; i < orders_per_level; i++){
            if (idCounter % 2 == 0){
                book.buyStopLoss(idCounter + i, 50001, 100);
            }
            else{
                book.sellStopLoss(idCounter + i, 49998, 100);
            }
        }
        idCounter += orders_per_level;
        state.ResumeTiming();
    }

    state.SetItemsProcessed(state.iterations());
    state.SetComplexityN(N);
}

BENCHMARK(BM_StopLossMatch_single)->RangeMultiplier(10)->Range(100, 1000000)->Complexity();