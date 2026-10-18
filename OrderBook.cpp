#include "OrderBook.h"
#include <cstdint>

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
    if (order.side == Side::BUY){


    }
    else if (order.side == Side::SELL){
        
    }

};
