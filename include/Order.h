#pragma once

#include <cstdint>
#include <chrono>
#include <list>
#include <optional>
#include <map>
#include <variant>

using Price = std::int64_t;
using OrderID = std::uint64_t;
using Quantity = std::uint32_t;
using Time = std::chrono::system_clock::time_point;
using OrderCount = std::uint32_t;

enum class OrderType{
    LIMIT,
    MARKET,
    ICEBERG,
    STOP_LOSS
};

enum class Side{
    BUY,
    SELL
};

enum class BookType{
    LIVE,
    STOP
};

// quantityTotalRemaining is the total number or orders
// For iceberg orders the peak size takes the place of quantityRemaining in the Order struct
struct IcebergData{
    Quantity peakSize;
    Quantity quantityTotalRemaining;
};

struct StopLossData{
    Price stopPrice;
};

// Prices are null for market orders
struct Order{
    OrderID id;
    OrderType type;
    Side side;
    std::optional<Price> price;
    Quantity quantityOriginal; 
    Quantity quantityRemaining;
    std::variant<std::monostate, IcebergData, StopLossData> typeSpecificData;
};

// orderIter is used to locate order in the price level linked list
// priceLevelIter is used to locate price level in std::map of prices
struct OrderLocation{
    BookType bookType;
    Side side;
    Price price;
    std::list<Order>::iterator orderIter;
    std::map<Price, std::list<Order>>::iterator priceLevelIter;
};

struct TradeInfo {
    OrderType aggressorType;
    OrderType restingType;
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

struct MatchResult{
    std::optional<Price> lowestTradePrice;
    std::optional<Price> highestTradePrice;
    bool tradeOccurred() const{
        return lowestTradePrice.has_value();
    };
};