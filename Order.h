#pragma once
#include <cstdint>

using Price = std::int64_t;
using OrderID = std::uint64_t;
using Quantity = std::uint32_t;

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