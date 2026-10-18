#pragma once
#include "Order.h"
#include <cstdint>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>


class OrderBook{
    private:
    std::map<Price, std::list<Order>> bids;
    std::map<Price, std::list<Order>> asks;
    std::unordered_map<OrderID, OrderLocation> orderLookup;
    void matchOrder(Order& order);
    std::vector<TradeInfo> tradeHistory;
    public:
        void addOrder(Order& order);
        void cancelOrder(OrderID orderID);
};


