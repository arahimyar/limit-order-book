#pragma once

#include <cstdint>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <functional>
#include <optional>

#include "Order.h"

class OrderBook{
    private:
        std::map<Price, std::list<Order>, std::greater<Price>> bids;
        std::map<Price, std::list<Order>> asks;
        std::unordered_map<OrderID, OrderLocation> orderLookup;
        std::vector<TradeInfo> tradeHistory;

        void addOrder(Order& order);

        template <typename Book>
        void matchLimitOrder(Order& order, Book& book){
            while (!book.empty() 
                    && order.quantityRemaining > 0 
                    && (order.side == Side::BUY ? order.price.value() >= book.begin() -> first : order.price.value() <= book.begin() -> first)){
                auto& orders = book.begin() -> second;
                Order& firstOrder = orders.front();
                Quantity amount_traded;
                if (firstOrder.type == OrderType::ICEBERG){
                    auto& iceberg = std::get<IcebergData>(firstOrder.typeSpecificData);
                    amount_traded = std::min(iceberg.quantityDisplayed, order.quantityRemaining);
                } 
                else{
                   amount_traded = std::min(firstOrder.quantityRemaining, order.quantityRemaining);
                }
                TradeInfo trade {
                        OrderType::LIMIT,
                        order.side == Side::BUY ? order.id : firstOrder.id,
                        order.side == Side::BUY ? firstOrder.id : order.id,
                        firstOrder.price.value(),
                        amount_traded,
                        std::chrono::system_clock::now()
                    };
                tradeHistory.push_back(trade);
                order.quantityRemaining -= amount_traded;
                firstOrder.quantityRemaining -= amount_traded;
                if (firstOrder.type == OrderType::ICEBERG){
                    auto& iceberg = std::get<IcebergData>(firstOrder.typeSpecificData);
                    iceberg.quantityDisplayed -= amount_traded;
                }                

                if (firstOrder.quantityRemaining == 0){
                    orderLookup.erase(firstOrder.id);
                    orders.erase(orders.begin());
                    if (orders.empty()){
                        book.erase(book.begin());
                    }
                }
                else if (firstOrder.type == OrderType::ICEBERG){
                    auto& iceberg = std::get<IcebergData>(firstOrder.typeSpecificData);
                    if (iceberg.quantityDisplayed == 0){
                    iceberg.quantityDisplayed = std::min(firstOrder.quantityRemaining, iceberg.peakSize);
                    orders.splice(orders.end(), orders, orders.begin());
                    }
                }
            }
        };

        template <typename Book>
        void matchMarketOrder(Order& order, Book& book){
            while (!book.empty() && order.quantityRemaining > 0){
                auto& orders = book.begin() -> second;
                Order& bestAvailable = orders.front();
                Quantity amount_traded = std::min(order.quantityRemaining, bestAvailable.quantityRemaining);
                TradeInfo trade {
                            OrderType::MARKET, 
                            order.side == Side::BUY ? order.id : bestAvailable.id,
                            order.side == Side::BUY ? bestAvailable.id : order.id,
                            bestAvailable.price.value(),
                            amount_traded,
                            std::chrono::system_clock::now()
                };
                tradeHistory.push_back(trade);
                
                if (bestAvailable.quantityRemaining <= order.quantityRemaining){
                        order.quantityRemaining -= bestAvailable.quantityRemaining;
                        const auto bestAvailable_ID = bestAvailable.id;
                        orderLookup.erase(bestAvailable_ID);
                        orders.erase(orders.begin());
                        if (orders.empty()){
                            book.erase(book.begin());
                        }
                }
                else{
                    bestAvailable.quantityRemaining -= order.quantityRemaining;
                    order.quantityRemaining = 0;
                }
            }
        };


    public:
        void buyLimit(OrderID id, Price price, Quantity quantity) {
            Order order = Order{id, OrderType::LIMIT, Side::BUY, price, quantity, quantity};
            addOrder(order);
        }

        void sellLimit(OrderID id, Price price, Quantity quantity) {
            Order order = Order{id, OrderType::LIMIT, Side::SELL, price, quantity, quantity};
            addOrder(order);
        }

        void buyMarket(OrderID id, Quantity quantity){
            Order order = Order{id, OrderType::MARKET, Side::BUY, std::nullopt, quantity, quantity};
            addOrder(order);
        };
        void sellMarket(OrderID id, Quantity quantity){
            Order order = Order{id, OrderType::MARKET, Side::SELL, std::nullopt, quantity, quantity};
            addOrder(order);
        };

        bool cancelOrder(OrderID orderID);

        std::optional<Price> getSpread() const;
        std::optional<Price> getBestBid() const;
        std::optional<Price> getBestAsk() const;

        std::optional<Order> getOrderInfo(OrderID) const;
        std::map<Price, PriceLevel, std::greater<Price>> getCurrentBids() const;
        std::map<Price, PriceLevel> getCurrentAsks() const;
        const std::vector<TradeInfo>& getTradeHistory()  const;
};


