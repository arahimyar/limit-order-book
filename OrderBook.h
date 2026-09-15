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
        std::vector<TradeInfo> tradeHistory;

        template <typename Book>
        void matchLimitOrder(Order& order, Book& book){
            while (!book.empty() 
                    && order.quantity > 0 
                    && (order.side == Side::BUY ? order.price >= book.begin() -> first : order.price <= book.begin() -> first)){
                
                auto& orders = book.begin() -> second;
                Order& firstOrder = orders.front();
                Quantity amount_traded = std::min(firstOrder.quantity, order.quantity);
                TradeInfo trade {
                        OrderType::LIMIT,
                        order.side == Side::BUY ? order.id : firstOrder.id,
                        order.side == Side::BUY ? firstOrder.id : order.id,
                        firstOrder.price,
                        amount_traded,
                        std::chrono::system_clock::now()
                    };
                tradeHistory.push_back(trade);
                order.quantity -= amount_traded;
                firstOrder.quantity -= amount_traded;

                if (firstOrder.quantity == 0){
                    orderLookup.erase(firstOrder.id);
                    orders.erase(orders.begin());
                    if (orders.empty()){
                        book.erase(book.begin());
                    }

                }
            }
        };

        template <typename Book>
        void matchMarketOrder(Order& order, Book& book){
            while (!book.empty() && order.quantity > 0){
                auto& orders = book.begin() -> second;
                Order& bestAvailable = orders.front();
                Quantity amount_traded = std::min(order.quantity, bestAvailable.quantity);
                TradeInfo trade {
                            OrderType::MARKET, 
                            order.side == Side::BUY ? order.id : bestAvailable.id,
                            order.side == Side::BUY ? bestAvailable.id : order.id,
                            bestAvailable.price,
                            amount_traded,
                            std::chrono::system_clock::now()
                        };
                tradeHistory.push_back(trade);
                
                if (bestAvailable.quantity <= order.quantity){
                        order.quantity -= bestAvailable.quantity;
                        orders.erase(orders.begin());
                        orderLookup.erase(bestAvailable.id);
                        if (orders.empty()){
                            book.erase(book.begin());
                        }
                }
                else{
                    bestAvailable.quantity -= order.quantity;
                    order.quantity = 0;
                }
            }
        };


    public:
        void addOrder(Order& order);

        void buy(OrderID id, Price price, Quantity quantity, OrderType type = OrderType::LIMIT) {
            Order order = Order{id, type, Side::BUY, price, quantity};
            addOrder(order);
        }

        void sell(OrderID id, Price price, Quantity quantity, OrderType type = OrderType::MARKET) {
            Order order = Order{id, type, Side::SELL, price, quantity};
            addOrder(order);
        }

        bool cancelOrder(OrderID orderID);
        Price getSpread() const;;
        Price getBestBid() const;
        Price getBestAsk() const;

};


