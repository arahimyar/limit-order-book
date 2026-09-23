#include <iostream>
#include "OrderBook.h"
#include "Order.h"
#include "display.h"


void displayCurrentBook(const OrderBook& book){

};

void displayRecentTrades(const OrderBook& book, std::size_t N){

};

void displayBookStatus(const OrderBook& book){

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
    std::cout << "Price: " << order.price << "\n";
    std::cout << "Original Quantity: " << order.quantityOriginal << "\n";
    std::cout << "Remaining Quantity: " << order.quantityRemaining << "\n";
    std::cout << "Quantity Filled: " << order.quantityOriginal - order.quantityRemaining  << "\n";    
};