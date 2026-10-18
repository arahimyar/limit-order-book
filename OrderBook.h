#pragma once
#include "Order.h"
#include <cstdint>
#include <map>
#include <unordered_map>
#include <list>


class OrderBook{
    private:
    std::map<Price, std::list<Order>> bids;
    std::map<Price, std::list<Order>> asks;
    std::unordered_map<OrderID, std::list<OrderLocation>::iterator> orderLookup;
    void matchOrder(Order& order);
    
    public:
        void addOrder(Order& order);
        void cancelOrder(OrderID orderID);
};


