#include "OrderBook.h"
#include <cstdint>
#include <algorithm>
#include <iterator>
#include <functional>
#include <chrono>

void OrderBook::addOrder(Order& order){
    if (order.quantity <= 0){
        return;
    }
    if (order.type == OrderType::LIMIT){
        matchLimitOrder(order);
        if (order.quantity > 0) {
            if (order.side == Side::BUY) {
                auto& orders = bids[order.price];
                auto iter = orders.insert(orders.end(), order);
                OrderLocation location{
                    Side::BUY,
                    order.price,
                    iter
                };
                orderLookup[order.id] = location;

            }
            else if (order.side == Side::SELL) {
                auto& orders = asks[order.price];
                auto iter = orders.insert(orders.end(), order);
                OrderLocation location{
                    Side::SELL,
                    order.price,
                    iter
                };
                orderLookup[order.id] = location;
            }
                }
    }
    else if (order.type == OrderType::MARKET){
        matchMarketOrder(order);
    }
    
};

void OrderBook::cancelOrder(OrderID orderID){
    auto iter = orderLookup.find(orderID);
    if (iter == orderLookup.end()){
        return;
    }
    auto location = iter -> second;
    if (location.side == Side::BUY){
        bids[location.price].erase(location.iterator);
    }
    else if (location.side == Side::SELL){
        asks[location.price].erase(location.iterator);
    }
    orderLookup.erase(orderID);
};

void OrderBook::matchLimitOrder(Order& order){
    if (order.side == Side::BUY){
        while (!asks.empty() && order.quantity > 0 && order.price >= asks.begin() -> first){
            auto& orders = asks.begin() -> second;
            Order& firstOrder = orders.front();
            if (firstOrder.quantity <= order.quantity){
                order.quantity -= firstOrder.quantity;
                TradeInfo trade {
                        OrderType::LIMIT,
                        order.id,
                        firstOrder.id,
                        firstOrder.price,
                        firstOrder.quantity,
                        std::chrono::system_clock::now()
                    };
                tradeHistory.push_back(trade);
                orderLookup.erase(firstOrder.id);
                orders.erase(orders.begin());
                if (orders.empty()){
                    asks.erase(asks.begin());
                }

            }
            else if (firstOrder.quantity > order.quantity){
                firstOrder.quantity -= order.quantity;
                TradeInfo trade {
                        OrderType::LIMIT,
                        order.id,
                        firstOrder.id,
                        firstOrder.price,
                        order.quantity,
                        std::chrono::system_clock::now()
                    };
                order.quantity = 0;
                tradeHistory.push_back(trade);
                return;
            }    
        }

    }
    else if (order.side == Side::SELL){
        while (!bids.empty() && order.quantity > 0 && order.price <= bids.begin() -> first){
            auto& orders = bids.begin() -> second;
            Order& firstOrder = orders.front();
            if (firstOrder.quantity <= order.quantity){
                order.quantity -= firstOrder.quantity;
                TradeInfo trade {
                        OrderType::LIMIT,
                        firstOrder.id,
                        order.id,
                        firstOrder.price,
                        firstOrder.quantity,
                        std::chrono::system_clock::now()
                    };
                tradeHistory.push_back(trade);
                orderLookup.erase(firstOrder.id);
                orders.erase(orders.begin());
                if (orders.empty()){
                    bids.erase(bids.begin());
                }
            }
            else if (firstOrder.quantity > order.quantity){
                firstOrder.quantity -= order.quantity;
                TradeInfo trade {
                        OrderType::LIMIT, 
                        firstOrder.id,
                        order.id,
                        firstOrder.price,
                        order.quantity,
                        std::chrono::system_clock::now()
                    };
                    order.quantity = 0;
                tradeHistory.push_back(trade);
                return;
            }    
        }
    }

};

void OrderBook::matchMarketOrder(Order& order){
    if (order.side == Side::BUY){
        while (!asks.empty() && order.quantity > 0){
            auto& orders = asks.begin() -> second;
            Order& bestAsk = orders.front();
            if (bestAsk.quantity < order.quantity){
                TradeInfo trade {
                        OrderType::MARKET, 
                        bestAsk.id,
                        order.id,
                        bestAsk.price,
                        bestAsk.quantity,
                        std::chrono::system_clock::now()
                    };
                    tradeHistory.push_back(trade);
                    order.quantity -= bestAsk.quantity;
                    orders.erase(orders.begin());
                    orderLookup.erase(bestAsk.id);
                    if (orders.empty()){
                        asks.erase(asks.begin());
                    }
            }
            else{
                TradeInfo trade {
                        OrderType::MARKET, 
                        bestAsk.id,
                        order.id,
                        bestAsk.price,
                        order.quantity,
                        std::chrono::system_clock::now()
                    };
                tradeHistory.push_back(trade);
                bestAsk.quantity -= order.quantity;
                order.quantity = 0;
            }
        };
    }

    else if (order.side == Side::SELL){
        while (!bids.empty() && order.quantity > 0){
            auto& orders = bids.begin() -> second;
            Order& bestBid = orders.front();
            if (bestBid.quantity < order.quantity){
                TradeInfo trade {
                        OrderType::MARKET, 
                        bestBid.id,
                        order.id,
                        bestBid.price,
                        bestBid.quantity,
                        std::chrono::system_clock::now()
                    };
                    tradeHistory.push_back(trade);
                    order.quantity -= bestBid.quantity;
                    orders.erase(orders.begin());
                    orderLookup.erase(bestBid.id);
                    if (orders.empty()){
                        bids.erase(bids.begin());
                    }
            }
            else{
                TradeInfo trade {
                        OrderType::MARKET, 
                        bestBid.id,
                        order.id,
                        bestBid.price,
                        order.quantity,
                        std::chrono::system_clock::now()
                    };
                    tradeHistory.push_back(trade);
                    bestBid.quantity -= order.quantity;
                    order.quantity = 0;
            }
        };
        
    }

};