#include <gtest/gtest.h>
#include "OrderBook.h"


TEST(OrderBookTest, NoTrigger_Buy){
    OrderBook book;
    book.buyStopLoss(1, 100, 10);
    book.buyLimit(2, 95, 10);
    book.sellLimit(3, 95, 5);

    const auto& stop_bids = book.getCurrentStops_Bids();
    const auto& bids = book.getCurrentBids();
    
    EXPECT_EQ(stop_bids.size(), 1);
    EXPECT_EQ(stop_bids.at(100).totalQuantity, 10);
    EXPECT_EQ(stop_bids.at(100).orderCount, 1);

    EXPECT_EQ(bids.size(), 1);
    EXPECT_EQ(bids.at(95).totalQuantity, 5);
    EXPECT_EQ(bids.at(95).orderCount, 1);

    const auto& order = book.getOrderInfo(1);
    EXPECT_TRUE(order.has_value());
    EXPECT_EQ(order->quantityRemaining, 10);
};


TEST(OrderBookTest, PartialFill_Buy){
    OrderBook book;
    book.buyStopLoss(1, 100, 10);
    book.buyLimit(2, 105, 3);
    book.sellLimit(3, 105, 5);

    const auto& stop_bids = book.getCurrentStops_Bids();
    EXPECT_EQ(stop_bids.size(), 0);
    EXPECT_FALSE(book.getOrderInfo(1).has_value());

    const auto& trades = book.getTradeHistory();
    EXPECT_EQ(trades.size(), 1);
    // TradeInfo trade = trades[1];
    // EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    // EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    // EXPECT_EQ(trade.buyOrderID, 1);
    // EXPECT_EQ(trade.sellOrderID, 3);
    // EXPECT_EQ(trade.price, 105);
    // EXPECT_EQ(trade.quantity, 2);
};

TEST(OrderBookTest, ExcessOrders_Buy){

};

TEST(OrderBookTest, FIFO_Stops_Buy){

};

TEST(OrderBookTest, PriceOrder_Buy){

};

// Sell:

TEST(OrderBookTest, NoTrigger_Sell){
    OrderBook book;
    book.sellStopLoss(1, 95, 10);
    book.buyLimit(2, 100, 5);
    book.sellLimit(3, 100, 10);

    const auto& stop_asks = book.getCurrentStops_Asks();
    const auto& asks = book.getCurrentAsks();
    
    EXPECT_EQ(stop_asks.size(), 1);
    EXPECT_EQ(stop_asks.at(95).totalQuantity, 10);
    EXPECT_EQ(stop_asks.at(95).orderCount, 1);

    EXPECT_EQ(asks.size(), 1);
    EXPECT_EQ(asks.at(100).totalQuantity, 5);
    EXPECT_EQ(asks.at(100).orderCount, 1);

    const auto& order = book.getOrderInfo(1);
    EXPECT_TRUE(order.has_value());
    EXPECT_EQ(order->quantityRemaining, 10);
};

TEST(OrderBookTest, PartialFill_Sell){

};

TEST(OrderBookTest, ExcessOrders_Sell){

};

TEST(OrderBookTest, FIFO_Stops_Sell){

};

TEST(OrderBookTest, PriceOrder_Sell){

};