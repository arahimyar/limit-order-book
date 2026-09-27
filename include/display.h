#pragma once
#include "OrderBook.h"

void displayCurrentBook(const OrderBook& book,  std::size_t N = 5);
void displayRecentTrades(const OrderBook& book, std::size_t N = 5);
void displayBookStatus(const OrderBook& book);
void displayOrderInfo(const OrderBook& book, OrderID ID);
