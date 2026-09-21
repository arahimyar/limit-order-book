#include <gtest/gtest.h>
#include "OrderBook.h"

// Adding orders
TEST(OrderBookTest, AddBuyLimitOrder_Single){
    OrderBook book;
    book.buyLimit(1, 10, 100);
    auto order = book.getOrderInfo(1);
    ASSERT_TRUE(order.has_value());
    EXPECT_EQ(order->id, 1);
    EXPECT_EQ(order->type, OrderType::LIMIT);
    EXPECT_EQ(order->side, Side::BUY);
    EXPECT_EQ(order->price, 10);
    EXPECT_EQ(order->quantityOriginal, 100);
    EXPECT_EQ(order->quantityRemaining, 100);
    EXPECT_EQ(book.getBestBid(), 10);
};

TEST(OrderBookTest, AddSellLimitOrder_Single){
    OrderBook book;
    book.sellLimit(1, 10, 100);
    auto order = book.getOrderInfo(1);
    ASSERT_TRUE(order.has_value());
    EXPECT_EQ(order->id, 1);
    EXPECT_EQ(order->type, OrderType::LIMIT);
    EXPECT_EQ(order->side, Side::SELL);
    EXPECT_EQ(order->price, 10);
    EXPECT_EQ(order->quantityOriginal, 100);
    EXPECT_EQ(order->quantityRemaining, 100);
    EXPECT_EQ(book.getBestAsk(), 10);
};

TEST(OrderBookTest, AddBuyLimitOrder_Multiple){
    OrderBook book;
    
    book.buyLimit(1, 100, 50);
    book.buyLimit(2, 101, 10);
    book.buyLimit(3, 99, 30);
    book.buyLimit(4, 100, 10);

    EXPECT_EQ(book.getBestBid(), 101);

    auto bids = book.getCurrentBids();

    ASSERT_EQ(bids.size(), 3);

    EXPECT_EQ(bids.at(99).totalQuantity, 30);
    EXPECT_EQ(bids.at(100).totalQuantity, 60);
    EXPECT_EQ(bids.at(101).totalQuantity, 10);

    EXPECT_EQ(bids.at(99).orderCount, 1);
    EXPECT_EQ(bids.at(100).orderCount, 2);
    EXPECT_EQ(bids.at(101).orderCount, 1);
};

TEST(OrderBookTest, AddSellLimitOrder_Multiple){
    OrderBook book;
    
    book.sellLimit(1, 100, 50);
    book.sellLimit(2, 101, 10);
    book.sellLimit(3, 99, 30);
    book.sellLimit(4, 100, 10);

    EXPECT_EQ(book.getBestAsk(), 99);

    auto asks = book.getCurrentAsks();

    ASSERT_EQ(asks.size(), 3);

    EXPECT_EQ(asks.at(99).totalQuantity, 30);
    EXPECT_EQ(asks.at(100).totalQuantity, 60);
    EXPECT_EQ(asks.at(101).totalQuantity, 10);

    EXPECT_EQ(asks.at(99).orderCount, 1);
    EXPECT_EQ(asks.at(100).orderCount, 2);
    EXPECT_EQ(asks.at(101).orderCount, 1);
};

// Cancelling orders

TEST(OrderBookTest, CancelBuy){
    OrderBook book;

    book.buyLimit(1, 100, 10);
    book.buyLimit(2, 100, 50);
    book.buyLimit(3, 200, 20);

    book.cancelOrder(2);
    auto bids = book.getCurrentBids();
    EXPECT_EQ(bids.size(), 2);
    EXPECT_EQ(bids.at(100).totalQuantity, 10);
    EXPECT_EQ(bids.at(100).orderCount, 1);

    book.cancelOrder(3);
    bids = book.getCurrentBids();
    EXPECT_EQ(bids.size(), 1);
    EXPECT_EQ(bids.find(200), bids.end());

    book.cancelOrder(1);
    bids = book.getCurrentBids();
    EXPECT_TRUE(bids.empty());
};

TEST(OrderBookTest, CancelSell){
    OrderBook book;

    book.sellLimit(1, 100, 10);
    book.sellLimit(2, 100, 50);
    book.sellLimit(3, 200, 20);

    book.cancelOrder(2);
    auto asks = book.getCurrentAsks();
    EXPECT_EQ(asks.size(), 2);
    EXPECT_EQ(asks.at(100).totalQuantity, 10);
    EXPECT_EQ(asks.at(100).orderCount, 1);

    book.cancelOrder(3);
    asks = book.getCurrentAsks();
    EXPECT_EQ(asks.size(), 1);
    EXPECT_EQ(asks.find(200), asks.end());

    book.cancelOrder(1);
    asks = book.getCurrentAsks();
    EXPECT_TRUE(asks.empty());
};


TEST(OrderBookTest, CancelNonexistent){
    OrderBook book;
    book.buyLimit(1, 1, 1);
    EXPECT_FALSE(book.cancelOrder(2));
};
