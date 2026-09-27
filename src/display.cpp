#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <vector>
#include "OrderBook.h"
#include "Order.h"
#include "display.h"


// Helper function to center string in output
std::string center(const std::string& text, std::size_t width)
{
    if (text.length() >= width){
        return text;
    }

    std::size_t total_padding = width - text.length();
    std::size_t left_padding = total_padding / 2;
    std::size_t right_padding = total_padding - left_padding;

    return std::string(left_padding, ' ') + text + std::string(right_padding, ' ');
};

void displayCurrentBook(const OrderBook& book, std::size_t N){
    constexpr const char* RED = "\033[31m";
    constexpr const char* GREEN = "\033[32m";
    constexpr const char* STANDARD = "\033[0m";

    if (N == 0){
        return;
    }
    
    std::uint32_t bid_ask_width = 25;
    int price_padding = 3;

    const auto& bids = book.getBids();
    const auto& asks = book.getAsks();

    Price max_price;
    if (asks.empty() && bids.empty()){
        std::cout << "No orders in order book.\n";
        return;
    }
    else if(asks.empty()){
        max_price = bids.begin() -> first;
    }
    else{
        max_price = asks.begin() -> first;
        auto asks_iter = asks.begin();
        for(std::size_t i = 0; i < N && asks_iter != asks.end(); i++, asks_iter++){
            max_price = asks_iter -> first;
        }
    }
    std::string price_string = std::to_string(max_price);
    int price_width = price_string.length() + 2*price_padding;


    std::uint32_t max_quantity = 0;
    auto asks_iter = asks.begin();
    for(std::size_t i = 0; i < N && asks_iter != asks.end(); i++, asks_iter++){
            max_quantity = std::max(max_quantity, asks_iter -> second.totalQuantity);
    }
    auto bids_iter = bids.begin();
    for(std::size_t i = 0; i < N && bids_iter != bids.end(); i++, bids_iter++){
            max_quantity = std::max(max_quantity, bids_iter -> second.totalQuantity);
    }

    std::uint32_t block_value = std::max<std::uint32_t>(1, max_quantity/bid_ask_width);

    std::cout << center("ASK SIZE", bid_ask_width) << center("PRICE", price_width) << center("BIDD  SIZE", bid_ask_width) << "\n";

    std::vector<std::map<Price, std::list<Order>>::iterator>> recent_asks;
    asks_iter = asks.begin();
    for(std::size_t i = 0; i < N && asks_iter != asks.end(); i++, asks_iter++){
        recent_asks.push_back(asks_iter);
    }

    for(auto iter = recent_asks.rbegin(); iter != recent_asks.rend(); iter++){
        std::uint32_t bar_size = (iter -> second.totalQuantity) / block_value;
        std::string bars = std::string(bar_size, '█');
        cout << std::right << std::setw(bid_ask_width) << bars;
        cout << RED << center(iter -> first, price_width) << STANDARD << "\n";
    }

    std::size_t count = 0;
    for (const auto& [price, level] : bids){
        if (count++ >= N){
            break;
        }
        cout << std::string(bid_ask_width, ' ');
        cout << GREEN << center(price, price_width) << STANDARD;
        std::uint32_t bar_size = (level.totalQuantity) / block_value;
        std::string bars = std::string(bar_size, '█');
        cout << std::left << std::setw(bid_ask_width) << bars << "\n";
    }
};

void displayRecentTrades(const OrderBook& book, std::size_t N){
    const auto trades = book.getTradeHistory();
    
    for(std::size_t i = 0; i < N; i++){
        TradeInfo trade = trades[trades.size(() - 1 - i)];
        std::cout << "Trade time: " << trade.time << "\n";
        if (order.type == OrderType::LIMIT){
            std::cout << "Type: Limit Order \n";
        }
        else{
            std::cout << "Type: Marker Order \n";
        }
        std::cout << "Buyer ID: " << trade.buyOrderID << "\n";  
        std::cout << "Seller ID: " << trade.sellOrderID << "\n";    
        std::cout << "Quantity: " << trade.quantity << "\n";  
        std::cout << "Price: $" << std::fixed << std::setprecision(2) << trade.price << "\n";  
    }
};

void displayBookStatus(const OrderBook& book){
    std::cout << "Current spread: $" << std::fixed << std::setprecision(2) << book.getSpread() << "\n";
    std::cout << "Current best bid: $" << std::fixed << std::setprecision(2) << book.getBestBid() << "\n";
    std::cout << "Current best ask: $" << std::fixed << std::setprecision(2) << book.getBestAsk() << "\n";
};

void displayOrderInfo(const OrderBook& book, OrderID ID){
    const auto orderOptional = book.getOrderInfo(ID);
    if (!orderOptional.has_value()){
        std::cout << "ERROR: Order not found. \n";
    }
    
    const Order& order = *orderOptional;
    std::cout << "Order ID: " << order.id << "\n";
    if (order.type == Side::BUY){
        std::cout << "Side: Bid" << "\n";
    }
    else{
        std::cout << "Side: Ask" << "\n";
    }
    std::cout << "Price: " << std::fixed << std::setprecision(2) << order.price << "\n";
    std::cout << "Original Quantity: " << order.quantityOriginal << "\n";
    std::cout << "Remaining Quantity: " << order.quantityRemaining << "\n";
    std::cout << "Quantity Filled: " << order.quantityOriginal - order.quantityRemaining  << "\n";    
};