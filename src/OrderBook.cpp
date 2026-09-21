#include "OrderBook.h"
#include <cstdint>
#include <algorithm>
#include <iterator>
#include <functional>
#include <chrono>
#include <stdexcept>
#include <optional>

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
    

    if (order.type == OrderType::LIMIT){
        if (order.side == Side::BUY){
            matchLimitOrder(order, asks);
        }

        else if (order.side == Side::SELL){
            matchLimitOrder(order, bids);
        }
        
        
        if (order.quantityRemaining > 0) {
            if (order.side == Side::BUY) {
                auto& orders = bids[order.price.value()];
                auto iter = orders.insert(orders.end(), order);
                OrderLocation location{
                    Side::BUY,
                    order.price.value(),
                    iter
                };
                orderLookup[order.id] = location;

            }
            else if (order.side == Side::SELL) {
                auto& orders = asks[order.price.value()];
                auto iter = orders.insert(orders.end(), order);
                OrderLocation location{
                    Side::SELL,
                    order.price.value(),
                    iter
                };
                orderLookup[order.id] = location;
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

};

bool OrderBook::cancelOrder(OrderID orderID){
    auto iter = orderLookup.find(orderID);
    if (iter == orderLookup.end()){
        return false;
    }
    const auto& location = iter -> second;
    if (location.side == Side::BUY) {
        auto priceLevel = bids.find(location.price);
        priceLevel->second.erase(location.iterator);

        if (priceLevel->second.empty()) {
            bids.erase(priceLevel);
        }
    }
    else {
        auto priceLevel = asks.find(location.price);
        priceLevel->second.erase(location.iterator);

        if (priceLevel->second.empty()) {
            asks.erase(priceLevel);
        }
    }
     orderLookup.erase(iter);

    return true;
};

Price OrderBook::getSpread() const{
    if (bids.empty()){
        throw std::runtime_error("Cannot compute spread: bids is empty.");
    }
    if (asks.empty()){
        throw std::runtime_error("Cannot compute spread: asks is empty.");
    }
    Price best_bid = bids.begin() -> first;
    Price best_ask = asks.begin() -> first;
    return best_ask - best_bid;
};

Price OrderBook::getBestBid() const{
    if (bids.empty()){
        throw std::runtime_error("Cannot compute spread: bids is empty.");
    }
    Price best_bid = bids.begin() -> first;
    return best_bid;
}

Price OrderBook::getBestAsk() const{
    if (asks.empty()){
        throw std::runtime_error("Cannot compute spread: asks is empty.");
    }
    Price best_ask = asks.begin() -> first;
    return best_ask;
}

std::optional<Order> OrderBook::getOrderInfo(OrderID orderID) const{
    auto location = orderLookup.find(orderID);
    if (location == orderLookup.end()){
        return std::nullopt;
    }

    return *(location -> second.iterator);

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