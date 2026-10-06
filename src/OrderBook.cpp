#include <cstdint>
#include <algorithm>
#include <iterator>
#include <functional>
#include <chrono>
#include <stdexcept>
#include <optional>

#include "OrderBook.h"

void OrderBook::addOrder(Order& order){
    if(orderLookup.contains(order.id)){
        throw std::invalid_argument("Order ID already exists.");
    }

    if (order.quantityOriginal == 0){
        throw std::invalid_argument("Order quantity must be strictly positive.");
    }

    if (order.type == OrderType::LIMIT && !order.price) {
        throw std::invalid_argument("Limit order must have a price.");
    }
    
    if (order.type == OrderType::MARKET && order.price) {
        throw std::invalid_argument("Market order cannot have a price.");
    }

    if (order.type == OrderType::ICEBERG && !order.price) {
        throw std::invalid_argument("Iceberg order must have a price.");
    }

    if (order.type == OrderType::ICEBERG) {
        auto& iceberg = std::get<IcebergData>(order.typeSpecificData);
        if (!(iceberg.peak > 0)){
            throw std::invalid_argument("Iceberg must have strictly positive peak.");
        }
    }
    
    

    if (order.type == OrderType::LIMIT){
        if (order.side == Side::BUY){
            matchLimitOrder(order, asks);
        }

        else if (order.side == Side::SELL){
            matchLimitOrder(order, bids);
        }
        
        
        if (order.quantityRemaining > 0) {
            if (order.side == Side::BUY) {
                auto [priceLevelIter, ifPlaced] = bids.try_emplace(order.price.value());
                auto& orders = priceLevelIter->second;
                auto orderIter = orders.insert(orders.end(), order);
                orderLookup[order.id] = OrderLocation{
                    Side::BUY,
                    order.price.value(),
                    orderIter,
                    priceLevelIter
                };

            }
            else if (order.side == Side::SELL) {
                auto [priceLevelIter, ifPlaced] = asks.try_emplace(order.price.value());
                auto& orders = priceLevelIter->second;
                auto orderIter = orders.insert(orders.end(), order);
                orderLookup[order.id] = OrderLocation{
                    Side::SELL,
                    order.price.value(),
                    orderIter,
                    priceLevelIter
                };
            }
                }
    }
    else if (order.type == OrderType::MARKET) {
        if (order.side == Side::BUY) {
            matchMarketOrder(order, asks);
        } 
        else {
            matchMarketOrder(order, bids);
        }
    }

    else if (order.type == OrderType::ICEBERG){
        auto& iceberg = std::get<IcebergData>(order.typeSpecificData);
        while (iceberg.quantityTotalRemaining > 0){
            matchIcebergOrder(order, order.side == Side::BUY ? asks : bids);
            if(order.quantityRemaining > 0){
                break;
            }
            order.quantityRemaining = std::min(iceberg.peakSize, iceberg.quantityTotalRemaining);
        }
        if (order.quantityRemaining > 0){
            auto& book = (order.side == Side::BUY ? bids : asks);
            auto [priceLevelIter, ifPlaced] = book.try_emplace(order.price.value());
            auto& orders = priceLevelIter->second;
            auto orderIter = orders.insert(orders.end(), order);
            orderLookup[order.id] = OrderLocation{
                order.side,
                order.price.value(),
                orderIter,
                priceLevelIter
            };
        }
    }

};

bool OrderBook::cancelOrder(OrderID orderID){
    auto iter = orderLookup.find(orderID);
    if (iter == orderLookup.end()){
        return false;
    }
    const auto& location = iter -> second;
    location.priceLevelIter -> second.erase(location.orderIter);
    if (location.priceLevelIter -> second.empty()){
        if (location.side == Side::BUY){
            bids.erase(location.priceLevelIter);
        }
        else{
            asks.erase(location.priceLevelIter);
        }
    }
    orderLookup.erase(iter);
    return true;
};

std::optional<Price> OrderBook::getSpread() const {
    if (bids.empty() || asks.empty()){
        return std::nullopt;;
    } 
    return asks.begin() -> first - bids.begin() -> first;
}

std::optional<Price> OrderBook::getBestBid() const {
    if (bids.empty()){
        return std::nullopt;
    } 
    return bids.begin() -> first;
}

std::optional<Price> OrderBook::getBestAsk() const{
    if (asks.empty()){
        return  std::nullopt;
    }
    return asks.begin() -> first;
}

std::optional<Order> OrderBook::getOrderInfo(OrderID orderID) const{
    auto location = orderLookup.find(orderID);
    if (location == orderLookup.end()){
        return std::nullopt;
    }

    return *(location -> second.orderIter);

};

std::map<Price, PriceLevel, std::greater<Price>> OrderBook::getCurrentBids() const{
    std::map<Price, PriceLevel, std::greater<Price>> bids_map;
    for (const auto& [price, orders] : bids){
        PriceLevel& level = bids_map[price];
        for (const auto& order : orders){
            level.totalQuantity += order.quantityRemaining;
            level.orderCount++;
        }
    }
    return bids_map;
};

std::map<Price, PriceLevel> OrderBook::getCurrentAsks() const{
    std::map<Price, PriceLevel> asks_map;
    for (const auto& [price, orders] : asks){
        PriceLevel& level = asks_map[price];
        for (const auto& order : orders){
            level.totalQuantity += order.quantityRemaining;
            level.orderCount++;
        }
    }
    return asks_map;
};
const std::vector<TradeInfo>& OrderBook::getTradeHistory()  const{
    return tradeHistory;
};