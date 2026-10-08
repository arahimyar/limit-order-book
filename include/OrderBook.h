#pragma once

#include <cstdint>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <functional>
#include <optional>
#include <queue>

#include "Order.h"

class OrderBook{
    private:
        std::map<Price, std::list<Order>, std::greater<Price>> bids;
        std::map<Price, std::list<Order>> asks;
        std::unordered_map<OrderID, OrderLocation> orderLookup;
        std::vector<TradeInfo> tradeHistory;
        std::map<Price, std::list<Order>> stopLoss_bids;
        std::map<Price, std::list<Order>, std::greater<Price>> stopLoss_asks;
        std::queue<Order> pendingStopLoss_Buys;
        std::queue<Order> pendingStopLoss_Sells;

        void addOrder(Order& order);

        template <typename Book>
        void matchLimitOrder(Order& order, Book& book){
            std::optional<Price> leastTradePrice;
            std::optional<Price> greatestTradePrice;
            while (!book.empty() 
                    && order.quantityRemaining > 0 
                    && (order.side == Side::BUY ? order.price.value() >= book.begin() -> first : order.price.value() <= book.begin() -> first)){
                auto& orders = book.begin() -> second;
                Order& firstOrder = orders.front();
                Quantity amount_traded = std::min(firstOrder.quantityRemaining, order.quantityRemaining);
                TradeInfo trade {
                        order.type,
                        firstOrder.type,
                        order.side == Side::BUY ? order.id : firstOrder.id,
                        order.side == Side::BUY ? firstOrder.id : order.id,
                        firstOrder.price.value(),
                        amount_traded,
                        std::chrono::system_clock::now()
                    };
                if (!leastTradePrice || trade.price < *leastTradePrice){
                    leastTradePrice = trade.price;
                }
                if (!greatestTradePrice || trade.price > *greatestTradePrice){
                    greatestTradePrice = trade.price;
                }
                tradeHistory.push_back(trade);
                order.quantityRemaining -= amount_traded;
                firstOrder.quantityRemaining -= amount_traded;
                if (firstOrder.type == OrderType::ICEBERG){
                    auto& iceberg = std::get<IcebergData>(firstOrder.typeSpecificData);
                    iceberg.quantityTotalRemaining -= amount_traded;
                    if (firstOrder.quantityRemaining == 0 && iceberg.quantityTotalRemaining > 0){
                        firstOrder.quantityRemaining = std::min(iceberg.peakSize, iceberg.quantityTotalRemaining);
                        orders.splice(orders.end(), orders, orders.begin());
                    }
                }                

                if (firstOrder.quantityRemaining == 0){
                    orderLookup.erase(firstOrder.id);
                    orders.erase(orders.begin());
                    if (orders.empty()){
                        book.erase(book.begin());
                    }
                }
            }

            while (greatestTradePrice.has_value() && !stopLoss_bids.empty() && greatestTradePrice >= stopLoss_bids.begin()->first){
                for (auto& stopOrders : stopLoss_bids.begin()->second){
                    pendingStopLoss_Buys.push(stopOrders);
                }
                stopLoss_bids.erase(stopLoss_bids.begin());
            }
            matchStopLossOrder(pendingStopLoss_Buys, asks, stopLoss_bids);

            while (leastTradePrice.has_value() && !stopLoss_asks.empty() && leastTradePrice <= stopLoss_asks.begin()->first){
                for (auto& stopOrders : stopLoss_asks.begin()->second){
                    pendingStopLoss_Sells.push(std::move(stopOrders));
                }
                stopLoss_asks.erase(stopLoss_asks.begin());
            }
            matchStopLossOrder(pendingStopLoss_Sells, bids, stopLoss_asks);
        };

        template <typename Book>
        void matchMarketOrder(Order& order, Book& book){
            std::optional<Price> leastTradePrice;
            std::optional<Price> greatestTradePrice;
            while (!book.empty() && order.quantityRemaining > 0){
                auto& orders = book.begin() -> second;
                Order& bestAvailable = orders.front();
                Quantity amount_traded = std::min(order.quantityRemaining, bestAvailable.quantityRemaining);
                TradeInfo trade {
                            order.type,
                            bestAvailable.type, 
                            order.side == Side::BUY ? order.id : bestAvailable.id,
                            order.side == Side::BUY ? bestAvailable.id : order.id,
                            bestAvailable.price.value(),
                            amount_traded,
                            std::chrono::system_clock::now()
                };
                if (!leastTradePrice || trade.price < *leastTradePrice){
                    leastTradePrice = trade.price;
                }
                if (!greatestTradePrice || trade.price > *greatestTradePrice){
                    greatestTradePrice = trade.price;
                }
                tradeHistory.push_back(trade);
                bestAvailable.quantityRemaining -= amount_traded;
                order.quantityRemaining -= amount_traded;
                if (bestAvailable.type == OrderType::ICEBERG){
                    auto& iceberg = std::get<IcebergData>(bestAvailable.typeSpecificData);
                    iceberg.quantityTotalRemaining -= amount_traded;
                    if (bestAvailable.quantityRemaining == 0 && iceberg.quantityTotalRemaining > 0){
                        bestAvailable.quantityRemaining = std::min(iceberg.quantityTotalRemaining, iceberg.peakSize);
                        orders.splice(orders.end(), orders, orders.begin());
                    }
                }
                if (bestAvailable.quantityRemaining == 0){
                    const auto bestAvailable_ID = bestAvailable.id;
                    orderLookup.erase(bestAvailable_ID);
                    orders.erase(orders.begin());
                    if (orders.empty()){
                        book.erase(book.begin());
                    }
                }
            }

            while (greatestTradePrice.has_value() && !stopLoss_bids.empty() && greatestTradePrice >= stopLoss_bids.begin()->first){
                for (auto& stopOrders : stopLoss_bids.begin()->second){
                    pendingStopLoss_Buys.push(stopOrders);
                }
                stopLoss_bids.erase(stopLoss_bids.begin());
            }
            matchStopLossOrder(pendingStopLoss_Buys, asks, stopLoss_bids);

            while (leastTradePrice.has_value() && !stopLoss_asks.empty() && leastTradePrice <= stopLoss_asks.begin()->first){
                for (auto& stopOrders : stopLoss_asks.begin()->second){
                    pendingStopLoss_Sells.push(std::move(stopOrders));
                }
                stopLoss_asks.erase(stopLoss_asks.begin());
            }
            matchStopLossOrder(pendingStopLoss_Sells, bids, stopLoss_asks);
        };

        template <typename Book>
        void matchIcebergOrder(Order& order, Book& book){
            std::optional<Price> leastTradePrice;
            std::optional<Price> greatestTradePrice;
            auto& iceberg = std::get<IcebergData>(order.typeSpecificData);
            while (!book.empty() && order.quantityRemaining > 0
                    && (order.side == Side::BUY ? order.price.value() >= book.begin() -> first : order.price.value() <= book.begin() -> first)){
                auto& orders = book.begin() -> second;
                Order& bestOrder = orders.front();
                Quantity amount_traded = std::min(order.quantityRemaining, bestOrder.quantityRemaining);
                TradeInfo trade {
                            order.type,
                            bestOrder.type, 
                            order.side == Side::BUY ? order.id : bestOrder.id,
                            order.side == Side::BUY ? bestOrder.id : order.id,
                            bestOrder.price.value(),
                            amount_traded,
                            std::chrono::system_clock::now()
                };
                if (!leastTradePrice || trade.price < *leastTradePrice){
                    leastTradePrice = trade.price;
                }
                if (!greatestTradePrice || trade.price > *greatestTradePrice){
                    greatestTradePrice = trade.price;
                }
                tradeHistory.push_back(trade);
                order.quantityRemaining -= amount_traded;
                iceberg.quantityTotalRemaining -= amount_traded;
                bestOrder.quantityRemaining -= amount_traded;
                if (bestOrder.quantityRemaining == 0){
                    orderLookup.erase(bestOrder.id);
                    orders.erase(orders.begin());
                    if (orders.empty()){
                        book.erase(book.begin());
                    }
                }
            }

            while (greatestTradePrice.has_value() && !stopLoss_bids.empty() && greatestTradePrice >= stopLoss_bids.begin()->first){
                for (auto& stopOrders : stopLoss_bids.begin()->second){
                    pendingStopLoss_Buys.push(stopOrders);
                }
                stopLoss_bids.erase(stopLoss_bids.begin());
            }
            matchStopLossOrder(pendingStopLoss_Buys, asks, stopLoss_bids);

            while (leastTradePrice.has_value() && !stopLoss_asks.empty() && leastTradePrice <= stopLoss_asks.begin()->first){
                for (auto& stopOrders : stopLoss_asks.begin()->second){
                    pendingStopLoss_Sells.push(std::move(stopOrders));
                }
                stopLoss_asks.erase(stopLoss_asks.begin());
            }
            matchStopLossOrder(pendingStopLoss_Sells, bids, stopLoss_asks);
        };

        template <typename Queue, typename Book, typename StopBook>
        void matchStopLossOrder(Queue& stop_queue, Book& book, StopBook& stopBook){
            while (!stop_queue.empty()){
                Order order = std::move(stop_queue.front());
                stop_queue.pop();

                while (!book.empty() && order.quantityRemaining > 0){
                    auto& orders = book.begin() -> second;
                    Order& bestAvailable = orders.front();
                    Quantity amount_traded = std::min(order.quantityRemaining, bestAvailable.quantityRemaining);
                    Price price_traded = bestAvailable.price.value();
                    TradeInfo trade {
                                order.type,
                                bestAvailable.type, 
                                order.side == Side::BUY ? order.id : bestAvailable.id,
                                order.side == Side::BUY ? bestAvailable.id : order.id,
                                price_traded,
                                amount_traded,
                                std::chrono::system_clock::now()
                    };
                    tradeHistory.push_back(trade);
                    bestAvailable.quantityRemaining -= amount_traded;
                    order.quantityRemaining -= amount_traded;
                    if (bestAvailable.type == OrderType::ICEBERG){
                        auto& iceberg = std::get<IcebergData>(bestAvailable.typeSpecificData);
                        iceberg.quantityTotalRemaining -= amount_traded;
                        if (bestAvailable.quantityRemaining == 0 && iceberg.quantityTotalRemaining > 0){
                            bestAvailable.quantityRemaining = std::min(iceberg.quantityTotalRemaining, iceberg.peakSize);
                            orders.splice(orders.end(), orders, orders.begin());
                        }
                    }
                    if (bestAvailable.quantityRemaining == 0){
                        const auto bestAvailable_ID = bestAvailable.id;
                        orderLookup.erase(bestAvailable_ID);
                        orders.erase(orders.begin());
                        if (orders.empty()){
                            book.erase(book.begin());
                        }
                    }

                    while (!stopBook.empty() && 
                        (order.side == Side::BUY ? stopBook.begin()->first <= price_traded : stopBook.begin()->first>= price_traded)){
                        auto& stopOrders = stopBook.begin()->second;
                        for (auto& stopOrder : stopOrders) {
                            stop_queue.push(std::move(stopOrder));
                        }
                        stopBook.erase(stopBook.begin());
                    }
                }
                
            }
        };

    public:
        void buyLimit(OrderID id, Price price, Quantity quantity) {
            Order order = Order{id, OrderType::LIMIT, Side::BUY, price, quantity, quantity};
            addOrder(order);
        };

        void sellLimit(OrderID id, Price price, Quantity quantity) {
            Order order = Order{id, OrderType::LIMIT, Side::SELL, price, quantity, quantity};
            addOrder(order);
        };

        void buyMarket(OrderID id, Quantity quantity){
            Order order = Order{id, OrderType::MARKET, Side::BUY, std::nullopt, quantity, quantity};
            addOrder(order);
        };
        void sellMarket(OrderID id, Quantity quantity){
            Order order = Order{id, OrderType::MARKET, Side::SELL, std::nullopt, quantity, quantity};
            addOrder(order);
        };

        void buyIceberg(OrderID id, Price price, Quantity quantity, Quantity peak) {
            Order order = Order{id, OrderType::ICEBERG, Side::BUY, price, quantity, std::min(peak, quantity), IcebergData{peak, quantity}};
            addOrder(order);
        };

        void sellIceberg(OrderID id, Price price, Quantity quantity, Quantity peak) {
            Order order = Order{id, OrderType::ICEBERG, Side::SELL, price, quantity, std::min(peak, quantity), IcebergData{peak, quantity}};
            addOrder(order);
        };

        void buyStopLoss(OrderID id, Price stopPrice, Quantity quantity) {
            Order order = Order{id, OrderType::STOP_LOSS, Side::BUY, std::nullopt, quantity, quantity, StopLossData{stopPrice}};
            addOrder(order);
        };

        void sellStopLoss(OrderID id, Price stopPrice, Quantity quantity) {
            Order order = Order{id, OrderType::STOP_LOSS, Side::SELL, std::nullopt, quantity, quantity, StopLossData{stopPrice}};
            addOrder(order);
        };

        bool cancelOrder(OrderID orderID);

        std::optional<Price> getSpread() const;
        std::optional<Price> getBestBid() const;
        std::optional<Price> getBestAsk() const;

        std::optional<Order> getOrderInfo(OrderID) const;
        std::map<Price, PriceLevel, std::greater<Price>> getCurrentBids() const;
        std::map<Price, PriceLevel> getCurrentAsks() const;
        std::map<Price, PriceLevel, std::greater<Price>> getCurrentStops_Asks() const;
        std::map<Price, PriceLevel> getCurrentStops_Bids() const;

        const std::vector<TradeInfo>& getTradeHistory()  const;
};


