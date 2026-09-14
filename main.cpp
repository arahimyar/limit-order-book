#include "OrderBook.h"
#include "Order.h"
#include <iostream>


int main() {
    OrderBook book;

    Order ask1{
        1,
        OrderType::LIMIT,
        Side::SELL,
        100,
        50
    };

    Order ask2{
        2,
        OrderType::LIMIT,
        Side::SELL,
        101,
        100
    };

    book.addOrder(ask1);
    book.addOrder(ask2);

    Order buy1{
        3,
        OrderType::LIMIT,
        Side::BUY,
        101,
        100
    };

    book.addOrder(buy1);

    std::cout << "Buy remaining quantity: " << buy1.quantity << '\n';

    Order sell1{
        4,
        OrderType::MARKET,
        Side::SELL,
        0,
        25
    };

    book.addOrder(sell1);

    std::cout << "Market sell remaining quantity: " << sell1.quantity << '\n';

    // Should print:
    // Market sell remaining quantity: 0

    return 0;
}