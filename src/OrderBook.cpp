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
        if (!(iceberg.peakSize > 0)){
            throw std::invalid_argument("Iceberg must have strictly positive peak.");
        }
    }
    
    

    if (order.type == OrderType::LIMIT){
        MatchResult res;
        if (order.side == Side::BUY){
            res = matchLimitOrder(order, asks);
        }

        else if (order.side == Side::SELL){
            res = matchLimitOrder(order, bids);
        }
        
        if (order.quantityRemaining > 0) {
            if (order.side == Side::BUY) {
                auto [priceLevelIter, ifPlaced] = bids.try_emplace(order.price.value());
                auto& orders = priceLevelIter->second;
                auto orderIter = orders.insert(orders.end(), order);
                orderLookup[order.id] = OrderLocation{
                    BookType::LIVE,
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
                    BookType::LIVE,
                    Side::SELL,
                    order.price.value(),
                    orderIter,
                    priceLevelIter
                };
            }
        }
        if (res.tradeOccurred()){
            stopLossTrigger(*res.lowestTradePrice, *res.greatestTradePrice);
        }
    }
    else if (order.type == OrderType::MARKET) {
        MatchResult res;
        if (order.side == Side::BUY) {
            res = matchMarketOrder(order, asks);
        } 
        else {
            res = matchMarketOrder(order, bids);
        }
        if (res.tradeOccurred()){
            stopLossTrigger(*res.lowestTradePrice, *res.greatestTradePrice);
        }
    }

    else if (order.type == OrderType::ICEBERG){
        MatchResult res;
        auto& iceberg = std::get<IcebergData>(order.typeSpecificData);
        while (iceberg.quantityTotalRemaining > 0){
            if (order.side == Side::BUY){
                res = matchIcebergOrder(order, asks);
            }
            else{
                res = matchIcebergOrder(order, bids);
            }
            
            if(order.quantityRemaining > 0){
                break;
            }
            order.quantityRemaining = std::min(iceberg.peakSize, iceberg.quantityTotalRemaining);
        }
        if (order.quantityRemaining > 0){
            if (order.side == Side::BUY){
            auto [priceLevelIter, ifPlaced] = bids.try_emplace(order.price.value());
            auto& orders = priceLevelIter->second;
            auto orderIter = orders.insert(orders.end(), order);
            orderLookup[order.id] = OrderLocation{
                BookType::LIVE,
                order.side,
                order.price.value(),
                orderIter,
                priceLevelIter
                };
            }
            else{
            auto [priceLevelIter, ifPlaced] = asks.try_emplace(order.price.value());
            auto& orders = priceLevelIter->second;
            auto orderIter = orders.insert(orders.end(), order);
            orderLookup[order.id] = OrderLocation{
                BookType::LIVE,
                order.side,
                order.price.value(),
                orderIter,
                priceLevelIter
                };
            }
        }
        if (res.tradeOccurred()){
            stopLossTrigger(*res.lowestTradePrice, *res.greatestTradePrice);
        }
    }

    else if (order.type == OrderType::STOP_LOSS){
        auto& stopLoss = std::get<StopLossData>(order.typeSpecificData);
        if (order.side == Side::BUY){
            auto [priceLevelIter, ifPlaced] = stopLoss_bids.try_emplace(stopLoss.stopPrice);
            auto& orders = priceLevelIter->second;
            auto orderIter = orders.insert(orders.end(), order);
            orderLookup[order.id] = OrderLocation{
                    BookType::STOP,
                    order.side,
                    stopLoss.stopPrice,
                    orderIter,
                    priceLevelIter
                };
        }
        else{
            auto [priceLevelIter, ifPlaced] = stopLoss_asks.try_emplace(stopLoss.stopPrice);
            auto& orders = priceLevelIter->second;
            auto orderIter = orders.insert(orders.end(), order);
            orderLookup[order.id] = OrderLocation{
                    BookType::STOP,
                    order.side,
                    stopLoss.stopPrice,
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
            if(location.bookType == BookType::STOP){
                stopLoss_bids.erase(location.priceLevelIter);
            }
            else{
                bids.erase(location.priceLevelIter);
            }

        }
        else{
            if(location.bookType == BookType::STOP){
                stopLoss_asks.erase(location.priceLevelIter);
            }
            else{
                asks.erase(location.priceLevelIter);
            }

        }
    }
    orderLookup.erase(iter);
    return true;
};

void OrderBook::stopLossTrigger(Price leastTradePrice, Price greatestTradePrice){
    while (!stopLoss_bids.empty() && greatestTradePrice >= stopLoss_bids.begin()->first){
        for (auto& stopOrders : stopLoss_bids.begin()->second){
            pendingStopLoss_Buys.push(std::move(stopOrders));
        }
        stopLoss_bids.erase(stopLoss_bids.begin());
    }
    matchStopLossOrder(pendingStopLoss_Buys, asks, stopLoss_bids);

    while (!stopLoss_asks.empty() && leastTradePrice <= stopLoss_asks.begin()->first){
        for (auto& stopOrders : stopLoss_asks.begin()->second){
            pendingStopLoss_Sells.push(std::move(stopOrders));
        }
        stopLoss_asks.erase(stopLoss_asks.begin());
    }
    matchStopLossOrder(pendingStopLoss_Sells, bids, stopLoss_asks);
}

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

std::map<Price, PriceLevel, std::greater<Price>> OrderBook::getCurrentStops_Asks() const{
    std::map<Price, PriceLevel, std::greater<Price>> stopAsks_map;
    for (const auto& [price, orders] : stopLoss_asks){
        PriceLevel& level = stopAsks_map[price];
        for (const auto& order : orders){
            level.totalQuantity += order.quantityRemaining;
            level.orderCount++;
        }
    }
    return stopAsks_map;
};
std::map<Price, PriceLevel> OrderBook::getCurrentStops_Bids() const{
    std::map<Price, PriceLevel> stopBids_map;
    for (const auto& [price, orders] : stopLoss_bids){
        PriceLevel& level = stopBids_map[price];
        for (const auto& order : orders){
            level.totalQuantity += order.quantityRemaining;
            level.orderCount++;
        }
    }
    return stopBids_map;
};


const std::vector<TradeInfo>& OrderBook::getTradeHistory()  const{
    return tradeHistory;
};