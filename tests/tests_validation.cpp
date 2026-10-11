#include <gtest/gtest.h>
#include "OrderBook.h"

TEST(OrderBookTest, DuplicateID){
    OrderBook book;

book.buyLimit(1, 1, 1);
    EXPECT_THROW(book.buyLimit(1, 2, 2), std::invalid_argument);
    EXPECT_THROW(book.buyIceberg(1, 2, 2, 2), std::invalid_argument);
    EXPECT_THROW(book.buyStopLoss(1, 2, 2), std::invalid_argument);
};

TEST(OrderBookTest, NonPositiveQuantity){
    OrderBook book;
    EXPECT_THROW(book.buyLimit(3, 2, 0), std::invalid_argument);
    EXPECT_THROW(book.buyIceberg(3, 2, 0, 1), std::invalid_argument);
    EXPECT_THROW(book.buyStopLoss(3, 2, 0), std::invalid_argument);
};

TEST(OrderBookTest, NonPositiveIcebergPeak){
    OrderBook book;
    EXPECT_THROW(book.buyIceberg(1,1, 1, 0), std::invalid_argument);
}