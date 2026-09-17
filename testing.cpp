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
};

TEST(OrderBookTest, MarketBuy_Partial){
    OrderBook book;

    book.sellLimit(1,2,10);
    book.buyMarket(4,3);

    EXPECT_TRUE(book.getCurrentBids().empty());

    auto ask = book.getOrderInfo(1);
    ASSERT_TRUE(ask.has_value());
    EXPECT_EQ(ask -> quantityRemaining, 7);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].quantity, 3);
};

TEST(OrderBookTest, MarketSell_Partial){
    OrderBook book;

    book.buyLimit(1,2,10);
    book.sellMarket(4,3);

    EXPECT_TRUE(book.getCurrentAsks().empty());
    
    auto bid = book.getOrderInfo(1);
    ASSERT_TRUE(bid.has_value());
    EXPECT_EQ(bid -> quantityRemaining, 7);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].quantity, 3);
};

TEST(OrderBookTest, MarketBuy_Multiple){
    OrderBook book;

    book.sellLimit(1, 100, 10);
    book.sellLimit(2, 100, 20);
    book.sellLimit(3, 101, 10);
    book.sellLimit(4, 102, 5);
    book.buyMarket(5, 40);

    EXPECT_TRUE(book.getCurrentBids().empty());
    const auto& asks = book.getCurrentAsks();
    ASSERT_EQ(asks.size(), 1);
    EXPECT_EQ(asks.begin()->first, 102);
    EXPECT_EQ(asks.begin()->second.totalQuantity, 5);
    EXPECT_EQ(asks.begin()->second.orderCount, 1);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 3);
    const auto& trade1 = trades[0];
    const auto& trade2 = trades[1];
    const auto& trade3 = trades[2];

    EXPECT_EQ(trade1.type, OrderType::MARKET);
    EXPECT_EQ(trade1.buyOrderID, 5);
    EXPECT_EQ(trade1.sellOrderID, 1);
    EXPECT_EQ(trade1.price, 100);
    EXPECT_EQ(trade1.quantity, 10);
    
    EXPECT_EQ(trade2.type, OrderType::MARKET);
    EXPECT_EQ(trade2.buyOrderID, 5);
    EXPECT_EQ(trade2.sellOrderID, 2);
    EXPECT_EQ(trade2.price, 100);
    EXPECT_EQ(trade2.quantity, 20);

    EXPECT_EQ(trade3.type, OrderType::MARKET);
    EXPECT_EQ(trade3.buyOrderID, 5);
    EXPECT_EQ(trade3.sellOrderID, 3);
    EXPECT_EQ(trade3.price, 101);
    EXPECT_EQ(trade3.quantity, 10);
};


TEST(OrderBookTest, MarketSell_Multiple){
    OrderBook book;

    book.buyLimit(1, 102, 10);
    book.buyLimit(2, 102, 20);
    book.buyLimit(3, 101, 10);
    book.buyLimit(4, 100, 5);
    book.sellMarket(5, 40);

    EXPECT_TRUE(book.getCurrentAsks().empty());
    const auto& bids = book.getCurrentBids();
    ASSERT_EQ(bids.size(), 1);
    EXPECT_EQ(bids.begin()->first, 100);
    EXPECT_EQ(bids.begin()->second.totalQuantity, 5);
    EXPECT_EQ(bids.begin()->second.orderCount, 1);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 3);
    auto trade1 = trades[0];
    auto trade2 = trades[1];
    auto trade3 = trades[2];

    EXPECT_EQ(trade1.type, OrderType::MARKET);
    EXPECT_EQ(trade1.buyOrderID, 1);
    EXPECT_EQ(trade1.sellOrderID, 5);
    EXPECT_EQ(trade1.price, 102);
    EXPECT_EQ(trade1.quantity, 10);
    
    EXPECT_EQ(trade2.type, OrderType::MARKET);
    EXPECT_EQ(trade2.buyOrderID, 2);
    EXPECT_EQ(trade2.sellOrderID, 5);
    EXPECT_EQ(trade2.price, 102);
    EXPECT_EQ(trade2.quantity, 20);

    EXPECT_EQ(trade3.type, OrderType::MARKET);
    EXPECT_EQ(trade3.buyOrderID, 3);
    EXPECT_EQ(trade3.sellOrderID, 5);
    EXPECT_EQ(trade3.price, 101);
    EXPECT_EQ(trade3.quantity, 10);
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



// Order book state

// Trading history & matching
