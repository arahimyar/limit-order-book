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

        template <typename Book>
        void OrderBook::matchLimitOrder(Order& order, Book& book);

        template <typename Book>
        void matchMarketOrder(Order& order, Book& book);
        std::vector<TradeInfo> tradeHistory;

    public:
        void addOrder(Order& order);
        bool cancelOrder(OrderID orderID);
};


