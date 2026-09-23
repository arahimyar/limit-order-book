#pragma once

#include <cstdint>
#include <chrono>
#include <list>
#include <optional>
#include <map>

using Price = std::int64_t;
using OrderID = std::uint64_t;
using Quantity = std::uint32_t;
using Time = std::chrono::system_clock::time_point;
using OrderCount = std::uint32_t;

enum class OrderType{
    LIMIT,
    MARKET
};

enum class Side{
    BUY,
    SELL
};

struct Order{
    OrderID id;
    OrderType type;
    Side side;
    std::optional<Price> price;
    Quantity quantityOriginal; 
    Quantity quantityRemaining;
};

struct OrderLocation{
    Side side;
    Price price;
    std::list<Order>::iterator orderIter;
    std::map<Price, std::list<Order>>::iterator priceLevelIter;
};

struct TradeInfo {
    OrderType type;
    OrderID buyOrderID;
    OrderID sellOrderID;
    Price price;
    Quantity  quantity;
    Time time;
};

struct PriceLevel{
    Quantity totalQuantity = 0;
    OrderCount orderCount = 0;
};