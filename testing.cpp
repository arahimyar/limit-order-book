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

// Validating orders

TEST(OrderBookTest, DuplicateID){
    OrderBook book;

book.buyLimit(1, 1, 1);
    EXPECT_THROW(book.buyLimit(1, 2, 2), std::invalid_argument);
};

TEST(OrderBookTest, NonPositivePrice){
    OrderBook book;
    EXPECT_THROW(book.buyLimit(2, 0, 2), std::invalid_argument)
};

TEST(OrderBookTest, NonPositiveQuantity){
    OrderBook book;
    EXPECT_THROW(book.buyLimit(3, 2, 0), std::invalid_argument);
};

// Market orders

TEST(OrderBookTest, MarketBuy_Single){
    OrderBook book;

    book.sellLimit(1,2,3);
    book.buyMarket(4,3);

    EXPECT_TRUE(book.getCurrentBids().empty());
    EXPECT_TRUE(book.getCurrentAsks().empty());

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].type, OrderType::MARKET);
    EXPECT_EQ(trades[0].buyOrderID, 4);
    EXPECT_EQ(trades[0].sellOrderID, 1);
    EXPECT_EQ(trades[0].price, 2);
    EXPECT_EQ(trades[0].quantity, 3);
};

TEST(OrderBookTest, MarketSell_Single){
    OrderBook book;
    
    book.buyLimit(1,2,3);
    book.sellMarket(4,3);

    EXPECT_TRUE(book.getCurrentBids().empty());
    EXPECT_TRUE(book.getCurrentAsks().empty());

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].type, OrderType::MARKET);
    EXPECT_EQ(trades[0].buyOrderID, 1);
    EXPECT_EQ(trades[0].sellOrderID, 4);
    EXPECT_EQ(trades[0].price, 2);
    EXPECT_EQ(trades[0].quantity, 3);
}

TEST(OrderBookTest, MarketBuy_Single){
    OrderBook book;
    book.sellLimit(1,1,1);
    book.marketBuy(2,1);
    EXCEPT_EQ(book.bids.size(), 3)
}




// Cancelling orders

// Order book state

// Trading history & matching
