#pragma once

#include <sstream>

#include "OrderBook.h"
#include "display.h"

void buying(OrderBook& book, std::istringstream& input);
void selling(OrderBook& book, std::istringstream& input);
void cancelling(OrderBook& book, std::istringstream& input);
void loading(OrderBook& book, std::istringstream& input);
void displaying(OrderBook& book, std::istringstream& input);
void runOrderBook(OrderBook& book);