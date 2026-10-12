#include <gtest/gtest.h>
#include "OrderBook.h"

TEST(OrderBookTest, NoMatch_Sell){
    OrderBook book;
    book.buyLimit(1, 100, 10);
    book.sellLimit(2, 101, 20);

    const auto& trades = book.getTradeHistory();
    EXPECT_TRUE(trades.empty());

    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();

    ASSERT_EQ(bids.size(), 1);
    ASSERT_EQ(asks.size(), 1);

    EXPECT_EQ(bids.at(100).totalQuantity, 10);
    EXPECT_EQ(bids.at(100).orderCount, 1);
    EXPECT_EQ(asks.at(101).totalQuantity, 20);
    EXPECT_EQ(asks.at(101).orderCount, 1);

    EXPECT_EQ(book.getBestBid(), 100);
    EXPECT_EQ(book.getBestAsk(), 101);
    EXPECT_EQ(book.getSpread(), 1);
};

TEST(OrderBookTest, NoMatch_Buy){
    OrderBook book;
    book.sellLimit(2, 101, 20);
    book.buyLimit(1, 100, 10);

    const auto& trades = book.getTradeHistory();
    EXPECT_TRUE(trades.empty());

    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();

    ASSERT_EQ(bids.size(), 1);
    ASSERT_EQ(asks.size(), 1);

    EXPECT_EQ(bids.at(100).totalQuantity, 10);
    EXPECT_EQ(bids.at(100).orderCount, 1);
    EXPECT_EQ(asks.at(101).totalQuantity, 20);
    EXPECT_EQ(asks.at(101).orderCount, 1);

    EXPECT_EQ(book.getBestBid(), 100);
    EXPECT_EQ(book.getBestAsk(), 101);
    EXPECT_EQ(book.getSpread(), 1);
};

TEST(OrderBookTest, ExactMatch_Sell){
    OrderBook book;
    book.buyLimit(1, 100, 10);
    book.sellLimit(2, 100, 10);

    const auto& trades = book.getTradeHistory();
    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();

    EXPECT_TRUE(bids.empty());
    EXPECT_TRUE(asks.empty());
    ASSERT_EQ(trades.size(), 1);

    TradeInfo trade = trades[0];
    EXPECT_EQ(trade.aggressorType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 1);
    EXPECT_EQ(trade.sellOrderID, 2);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 10);

    EXPECT_FALSE(book.getBestBid().has_value());
    EXPECT_FALSE(book.getBestAsk().has_value());
    EXPECT_FALSE(book.getSpread().has_value());

    EXPECT_FALSE(book.getOrderInfo(1).has_value());
    EXPECT_FALSE(book.getOrderInfo(2).has_value());
};

TEST(OrderBookTest, ExactMatch_Buy){
    OrderBook book;
    book.sellLimit(2, 100, 10);
    book.buyLimit(1, 100, 10);

    const auto& trades = book.getTradeHistory();
    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();

    EXPECT_TRUE(bids.empty());
    EXPECT_TRUE(asks.empty());
    ASSERT_EQ(trades.size(), 1);

    TradeInfo trade = trades[0];
    EXPECT_EQ(trade.aggressorType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 1);
    EXPECT_EQ(trade.sellOrderID, 2);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 10);

    EXPECT_FALSE(book.getBestBid().has_value());
    EXPECT_FALSE(book.getBestAsk().has_value());
    EXPECT_FALSE(book.getSpread().has_value());

    EXPECT_FALSE(book.getOrderInfo(1).has_value());
    EXPECT_FALSE(book.getOrderInfo(2).has_value());
};

TEST(OrderBookTest, PartialMatch_SellSmaller){
    OrderBook book;
    book.buyLimit(1, 100, 10);
    book.sellLimit(2, 100, 2);
    
    const auto& trades = book.getTradeHistory();
    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();

    ASSERT_EQ(trades.size(), 1);
    ASSERT_EQ(bids.size(), 1);
    ASSERT_TRUE(asks.empty());

    const TradeInfo& trade = trades[0];
    EXPECT_EQ(trade.aggressorType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 1);
    EXPECT_EQ(trade.sellOrderID, 2);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 2);

    auto buyOrder = book.getOrderInfo(1);
    ASSERT_TRUE(buyOrder.has_value());
    EXPECT_EQ(buyOrder -> quantityOriginal, 10);
    EXPECT_EQ(buyOrder -> quantityRemaining, 8);
    EXPECT_FALSE(book.getOrderInfo(2).has_value());

    EXPECT_FALSE(book.getBestAsk().has_value());
    EXPECT_FALSE(book.getSpread().has_value());

    EXPECT_EQ(book.getBestBid(), 100);
};

TEST(OrderBookTest, PartialMatch_SellLarger){
    OrderBook book;
    book.buyLimit(1, 100, 2);
    book.sellLimit(2, 100, 10);
    
    const auto& trades = book.getTradeHistory();
    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();

    ASSERT_EQ(trades.size(), 1);
    ASSERT_EQ(asks.size(), 1);
    ASSERT_TRUE(bids.empty());

    const TradeInfo& trade = trades[0];
    EXPECT_EQ(trade.aggressorType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 1);
    EXPECT_EQ(trade.sellOrderID, 2);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 2);

    auto sellOrder = book.getOrderInfo(2);
    ASSERT_TRUE(sellOrder.has_value());
    EXPECT_EQ(sellOrder -> quantityOriginal, 10);
    EXPECT_EQ(sellOrder -> quantityRemaining, 8);
    EXPECT_FALSE(book.getOrderInfo(1).has_value());

    EXPECT_FALSE(book.getBestBid().has_value());
    EXPECT_FALSE(book.getSpread().has_value());

    EXPECT_EQ(book.getBestAsk(), 100);
};

TEST(OrderBookTest, PartialMatch_BuySmaller){
    OrderBook book;
    book.sellLimit(1, 100, 10);
    book.buyLimit(2, 100, 2);
    
    const auto& trades = book.getTradeHistory();
    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();

    ASSERT_EQ(trades.size(), 1);
    ASSERT_EQ(asks.size(), 1);
    ASSERT_TRUE(bids.empty());

    const TradeInfo& trade = trades[0];
    EXPECT_EQ(trade.aggressorType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 2);
    EXPECT_EQ(trade.sellOrderID, 1);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 2);

    auto sellOrder = book.getOrderInfo(1);
    ASSERT_TRUE(sellOrder.has_value());
    EXPECT_EQ(sellOrder -> quantityOriginal, 10);
    EXPECT_EQ(sellOrder -> quantityRemaining, 8);
    EXPECT_FALSE(book.getOrderInfo(2).has_value());

    EXPECT_FALSE(book.getBestBid().has_value());
    EXPECT_FALSE(book.getSpread().has_value());

    EXPECT_EQ(book.getBestAsk(), 100);
};

TEST(OrderBookTest, PartialMatch_BuyLarger){
    OrderBook book;
    book.sellLimit(1, 100, 2);
    book.buyLimit(2, 100, 10);
    
    const auto& trades = book.getTradeHistory();
    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();

    ASSERT_EQ(trades.size(), 1);
    ASSERT_EQ(bids.size(), 1);
    ASSERT_TRUE(asks.empty());

    const TradeInfo& trade = trades[0];
    EXPECT_EQ(trade.aggressorType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 2);
    EXPECT_EQ(trade.sellOrderID, 1);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 2);

    auto buyOrder = book.getOrderInfo(2);
    ASSERT_TRUE(buyOrder.has_value());
    EXPECT_EQ(buyOrder -> quantityOriginal, 10);
    EXPECT_EQ(buyOrder -> quantityRemaining, 8);
    EXPECT_FALSE(book.getOrderInfo(1).has_value());

    EXPECT_FALSE(book.getBestAsk().has_value());
    EXPECT_FALSE(book.getSpread().has_value());

    EXPECT_EQ(book.getBestBid(), 100);
};


TEST(OrderBookTest, FIFO_Buy){
    OrderBook book;
    book.buyLimit(1, 100, 10);
    book.buyLimit(2, 100, 10);
    book.buyLimit(3, 100, 10);
    book.sellLimit(4, 100, 25);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 3);

    EXPECT_EQ(trades[0].buyOrderID, 1);
    EXPECT_EQ(trades[0].sellOrderID, 4);
    EXPECT_EQ(trades[0].quantity, 10);

    EXPECT_EQ(trades[1].buyOrderID, 2);
    EXPECT_EQ(trades[1].sellOrderID, 4);
    EXPECT_EQ(trades[1].quantity, 10);

    EXPECT_EQ(trades[2].buyOrderID, 3);
    EXPECT_EQ(trades[2].sellOrderID, 4);
    EXPECT_EQ(trades[2].quantity, 5);
};

TEST(OrderBookTest, FIFO_Sell){
    OrderBook book;
    book.sellLimit(1, 100, 10);
    book.sellLimit(2, 100, 10);
    book.sellLimit(3, 100, 10);
    book.buyLimit(4, 100, 25);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 3);

    EXPECT_EQ(trades[0].buyOrderID, 4);
    EXPECT_EQ(trades[0].sellOrderID, 1);
    EXPECT_EQ(trades[0].quantity, 10);

    EXPECT_EQ(trades[1].buyOrderID, 4);
    EXPECT_EQ(trades[1].sellOrderID, 2);
    EXPECT_EQ(trades[1].quantity, 10);

    EXPECT_EQ(trades[2].buyOrderID, 4);
    EXPECT_EQ(trades[2].sellOrderID, 3);
    EXPECT_EQ(trades[2].quantity, 5);
};


