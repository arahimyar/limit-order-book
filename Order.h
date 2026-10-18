#pragma once
#include <cstdint>
#include <chrono>

using Price = std::int64_t;
using OrderID = std::uint64_t;
using Quantity = std::uint32_t;
using Time = std::chrono::system_clock::time_point;

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
    Price price;
    Quantity quantity; 
};

struct OrderLocation{
    Side side;
    Price price;
    std::list<Order>::iterator iterator;
};

struct TradeInfo {
    OrderID buyOrderID;
    OrderID sellOrderID;
    Price price;
    Quantity  quantity;
    Time time;
}