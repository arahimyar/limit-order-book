#include "OrderBook.h"
#include <cstdint>
#include <algorithm>
#include <iterator>
#include <functional>
#include <chrono>
#include <stdexcept>

void OrderBook::addOrder(Order& order){
    if(orderLookup.contains(order.id)){
        throw std::invalid_argument("Order ID already exists.");
    }

    if (order.quantity == 0){
        throw std::invalid_argument(("Order quantity must be strictly positive."));
    }
    

    if (order.type == OrderType::LIMIT){
        if (order.side == Side::BUY){
            matchLimitOrder(order, asks);
        }

        else if (order.side == Side::SELL){
            matchLimitOrder(order, bids);
        }
        
        
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
    else if (order.type == OrderType::MARKET) {
        if (order.side == Side::BUY) {
            matchMarketOrder(order, asks);
        } 
        else {
            matchMarketOrder(order, bids);
        }
    }

};

bool OrderBook::cancelOrder(OrderID orderID){
    auto iter = orderLookup.find(orderID);
    if (iter == orderLookup.end()){
        return false;
    }
    const auto& location = iter -> second;
    if (location.side == Side::BUY) {
        auto priceLevel = bids.find(location.price);
        priceLevel->second.erase(location.iterator);

        if (priceLevel->second.empty()) {
            bids.erase(priceLevel);
        }
    }
    else {
        auto priceLevel = asks.find(location.price);
        priceLevel->second.erase(location.iterator);

        if (priceLevel->second.empty()) {
            asks.erase(priceLevel);
        }
    }
     orderLookup.erase(iter);

    return true;
};

template <typename Book>
void OrderBook::matchLimitOrder(Order& order, Book& book){
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

        if (firstOrder.quantity === 0){
            orderLookup.erase(firstOrder.id);
            orders.erase(orders.begin());
            if (orders.empty()){
                book.erase(book.begin());
            }

        }
    }
};

template <typename Book>
void OrderBook::matchMarketOrder(Order& order, Book& book){
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



