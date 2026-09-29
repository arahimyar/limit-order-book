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

// Prices are null for market orders
struct Order{
    OrderID id;
    OrderType type;
    Side side;
    std::optional<Price> price;
    Quantity quantityOriginal; 
    Quantity quantityRemaining;
};

// orderIter is used to locate order in the price level linked list
// priceLevelIter is used to locate price level in std::map of prices
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

// totalQuantity is the total number of units at that price level
// orderCount is the number of orders at that price level
struct PriceLevel{
    Quantity totalQuantity = 0;
    OrderCount orderCount = 0;
};