#include <gtest/gtest.h>
#include "OrderBook.h"

TEST(OrderBookTest, DuplicateID){
    OrderBook book;

book.buyLimit(1, 1, 1);
    EXPECT_THROW(book.buyLimit(1, 2, 2), std::invalid_argument);
};

TEST(OrderBookTest, NonPositiveQuantity){
    OrderBook book;
    EXPECT_THROW(book.buyLimit(3, 2, 0), std::invalid_argument);
};