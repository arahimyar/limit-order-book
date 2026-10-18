#include "OrderBook.h"
#include <cstdint>
#include <algorithm>
#include <iterator>
#include <chrono>

void OrderBook::addOrder(Order& order){
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

void OrderBook::matchOrder(Order& order){
    while (!book.empty() && order.quantity > 0 && order.price >=  start -> first){
        auto& orders = start -> second;

    }


    if (order.side == Side::BUY){
        while (!asks.empty() && order.quantity > 0 && order.price >= asks.begin() -> first){
            auto& orders = asks.begin() -> second;
            Order& firstOrder = orders.front();
            if (firstOrder.quantity <= order.quantity){
                order.quantity -= firstOrder.quantity;
                TradeInfo trade {
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
                        order.id,
                        firstOrder.id,
                        firstOrder.price,
                        order.quantity,
                        std::chrono::system_clock::now()
                    };
                tradeHistory.push_back(trade);
                return;
            }    
        }

    }
    else if (order.side == Side::SELL){
        while (!bids.empty() && order.quantity > 0 && order.price <= bids.rbegin() -> first){
            auto& orders = bids.rbegin() -> second;
            Order& firstOrder = orders.front();
            if (firstOrder.quantity <= order.quantity){
                order.quantity -= firstOrder.quantity;
                TradeInfo trade {
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
                    bids.erase(std::prev(bids.end()));
                }
            }
            else if (firstOrder.quantity > order.quantity){
                firstOrder.quantity -= order.quantity;
                TradeInfo trade {
                        firstOrder.id,
                        order.id,
                        firstOrder.price,
                        order.quantity,
                        std::chrono::system_clock::now()
                    };
                tradeHistory.push_back(trade);
                return;
            }    
        }
    }

};
