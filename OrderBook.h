#pragma once
#include "Order.h"
#include <cstdint>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <functional>


class OrderBook{
    private:
    std::map<Price, std::list<Order>, std::greater<Price>> bids;
    std::map<Price, std::list<Order>> asks;
    std::unordered_map<OrderID, OrderLocation> orderLookup;
    void matchLimitOrder(Order& order);
    void matchMarketOrder(Order& order);
    std::vector<TradeInfo> tradeHistory;
    public:
        void addOrder(Order& order);
        void cancelOrder(OrderID orderID);
};


