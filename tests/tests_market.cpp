#include <gtest/gtest.h>
#include "OrderBook.h"

TEST(OrderBookTest, MarketBuy_Single){
    OrderBook book;

    book.sellLimit(1,2,3);
    book.buyMarket(4,3);

    EXPECT_TRUE(book.getCurrentBids().empty());
    EXPECT_TRUE(book.getCurrentAsks().empty());

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 1);
    EXPECT_EQ(trades[0].aggressorType, OrderType::MARKET);
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
    EXPECT_EQ(trades[0].aggressorType, OrderType::MARKET);
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

    EXPECT_EQ(trade1.aggressorType, OrderType::MARKET);
    EXPECT_EQ(trade1.buyOrderID, 5);
    EXPECT_EQ(trade1.sellOrderID, 1);
    EXPECT_EQ(trade1.price, 100);
    EXPECT_EQ(trade1.quantity, 10);
    
    EXPECT_EQ(trade2.aggressorType, OrderType::MARKET);
    EXPECT_EQ(trade2.buyOrderID, 5);
    EXPECT_EQ(trade2.sellOrderID, 2);
    EXPECT_EQ(trade2.price, 100);
    EXPECT_EQ(trade2.quantity, 20);

    EXPECT_EQ(trade3.aggressorType, OrderType::MARKET);
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

    EXPECT_EQ(trade1.aggressorType, OrderType::MARKET);
    EXPECT_EQ(trade1.buyOrderID, 1);
    EXPECT_EQ(trade1.sellOrderID, 5);
    EXPECT_EQ(trade1.price, 102);
    EXPECT_EQ(trade1.quantity, 10);
    
    EXPECT_EQ(trade2.aggressorType, OrderType::MARKET);
    EXPECT_EQ(trade2.buyOrderID, 2);
    EXPECT_EQ(trade2.sellOrderID, 5);
    EXPECT_EQ(trade2.price, 102);
    EXPECT_EQ(trade2.quantity, 20);

    EXPECT_EQ(trade3.aggressorType, OrderType::MARKET);
    EXPECT_EQ(trade3.buyOrderID, 3);
    EXPECT_EQ(trade3.sellOrderID, 5);
    EXPECT_EQ(trade3.price, 101);
    EXPECT_EQ(trade3.quantity, 10);
};

TEST(OrderBookTest, MarketBuyExcess){
    OrderBook book;
    book.sellLimit(1, 100, 10);
    book.sellLimit(2, 100, 20);
    book.sellLimit(3, 100, 50);
    book.buyMarket(4, 100);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 3);
    EXPECT_TRUE(book.getCurrentAsks().empty());
    EXPECT_TRUE(book.getCurrentBids().empty());
};

TEST(OrderBookTest, MarketSellExcess){
    OrderBook book;
    book.buyLimit(1, 100, 10);
    book.buyLimit(2, 100, 20);
    book.buyLimit(3, 100, 50);
    book.sellMarket(4, 100);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 3);
    EXPECT_TRUE(book.getCurrentAsks().empty());
    EXPECT_TRUE(book.getCurrentBids().empty());
};

TEST(OrderBookTest, MarketBuyEmpty){
    OrderBook book;
    book.buyMarket(1, 1);
    const auto& trades = book.getTradeHistory();
    EXPECT_TRUE(trades.empty());
    EXPECT_TRUE(book.getCurrentAsks().empty());
    EXPECT_TRUE(book.getCurrentBids().empty());
};

TEST(OrderBookTest, MarketSellEmpty){
    OrderBook book;
    book.sellMarket(1, 1);
    const auto& trades = book.getTradeHistory();
    EXPECT_TRUE(trades.empty());
    EXPECT_TRUE(book.getCurrentAsks().empty());
    EXPECT_TRUE(book.getCurrentBids().empty());
};