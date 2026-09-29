#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <sstream>
#include <ctime>

#include "OrderBook.h"
#include "Order.h"
#include "display.h"

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

// Formats integer prices from cents to a dollar-cent format
std::string formatPrice(std::int64_t price, int scale = 100) {
    std::string sign = (price < 0) ? "-" : "";
    std::int64_t dollars = std::abs(price/scale);
    std::int64_t cents = std::abs(price % scale);
    return sign + std::to_string(dollars) + "." + (cents < 10 ? "0" : "") + std::to_string(cents);
};

std::string makeBars(std::size_t count)
{
    std::string bars;
    for (std::size_t i = 0; i < count; ++i) {
        bars += "█";
    }
    return bars;
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

    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();

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
    std::string price_string = formatPrice(max_price);
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
    std::cout << "\n";
    std::cout << center("ASK SIZE", bid_ask_width) << center("PRICE", price_width) << center("BID SIZE", bid_ask_width) << "\n";

    std::vector<decltype(asks.begin())> recent_asks;
    asks_iter = asks.begin();
    for(std::size_t i = 0; i < N && asks_iter != asks.end(); i++, asks_iter++){
        recent_asks.push_back(asks_iter);
    }

    for (auto it = recent_asks.rbegin(); it != recent_asks.rend(); it++) {
        const auto& iter = *it;
        std::uint32_t bar_size = (iter -> second.totalQuantity) / block_value;
        std::string bars = makeBars(bar_size);
        std::string padding(bid_ask_width - bar_size, ' ');
        std::cout << padding << bars;
        std::cout << RED << center(formatPrice(iter -> first), price_width) << STANDARD;
        std::cout << std::string(bid_ask_width, ' ') << '\n';
    }

    std::size_t count = 0;
    for (const auto& [price, level] : bids){
        if (count++ >= N){
            break;
        }

        std::uint32_t bar_size = (level.totalQuantity) / block_value;
        std::string bars = makeBars(bar_size);
        std::string padding(bid_ask_width - bar_size, ' ');
        std::cout << std::string(bid_ask_width, ' ');
        std::cout << GREEN << center(formatPrice(price), price_width) << STANDARD;
        std::cout << bars << padding << "\n";
    }
    block_value > 1 ? std::cout << "SCALE: █ ~ " << block_value << " units \n\n" : std::cout << "SCALE: █ ~ " << block_value << " unit \n\n";
};

void displayRecentTrades(const OrderBook& book, std::size_t N){
    const auto& trades = book.getTradeHistory();
    N = std::min(N, trades.size());
    std::cout << "\n";
    for(std::size_t i = 0; i < N; i++){
        const auto& trade = trades[trades.size() - 1 - i];
        auto time = std::chrono::system_clock::to_time_t(trade.time);
        std::cout << "Time: " << std::ctime(&time);
        if (trade.type == OrderType::LIMIT){
            std::cout << "Type: Limit Order \n";
        }
        else{
            std::cout << "Type: Market Order \n";
        }
        std::cout << "Buyer ID: " << trade.buyOrderID << "\n";  
        std::cout << "Seller ID: " << trade.sellOrderID << "\n";    
        std::cout << "Quantity: " << trade.quantity << "\n";  
        std::cout << "Price: $" << formatPrice(trade.price) << "\n";
        std::cout<<"\n";
    }
};

void displayBookStatus(const OrderBook& book) {
    const auto bestBid = book.getBestBid();
    const auto bestAsk = book.getBestAsk();
    const auto spread  = book.getSpread();

    std::cout << "\nCurrent best bid: " << (bestBid ? "$" + formatPrice(*bestBid) : "N/A") << "\n";
    std::cout << "Current best ask: " << (bestAsk ? "$" + formatPrice(*bestAsk) : "N/A") << "\n";
    std::cout << "Current spread: " << (spread  ? "$" + formatPrice(*spread)  : "N/A") << "\n";
    std::cout << "\n";
};

void displayOrderInfo(const OrderBook& book, OrderID ID){
    const auto orderOptional = book.getOrderInfo(ID);
    if (!orderOptional.has_value()){
        std::cout << "ERROR: Order not found. \n";
        return;
    }
    
    const Order& order = *orderOptional;
    std::cout << "\nOrder ID: " << order.id << "\n";
    if (order.side == Side::BUY){
        std::cout << "Side: Bid" << "\n";
    }
    else{
        std::cout << "Side: Ask" << "\n";
    }
    std::cout << "Price: " << formatPrice(*order.price) << "\n";
    std::cout << "Original Quantity: " << order.quantityOriginal << "\n";
    std::cout << "Remaining Quantity: " << order.quantityRemaining << "\n";
    std::cout << "Quantity Filled: " << order.quantityOriginal - order.quantityRemaining  << "\n";    
    std::cout << "\n";
};