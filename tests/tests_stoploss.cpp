#include <gtest/gtest.h>
#include "OrderBook.h"

TEST(OrderBookTest, AddBuyStopLoss_Single){
    OrderBook book;
    book.buyStopLoss(1, 2, 3);
    auto order = book.getOrderInfo(1);
    ASSERT_TRUE(order.has_value());
    EXPECT_EQ(order->id, 1);
    EXPECT_EQ(order->type, OrderType::STOP_LOSS);
    EXPECT_EQ(order->side, Side::BUY);
    EXPECT_FALSE(order->price.has_value());
    EXPECT_EQ(order->quantityOriginal, 3);
    EXPECT_EQ(order->quantityRemaining, 3);

    const auto& stoploss = std::get<StopLossData>(order->typeSpecificData);
    EXPECT_EQ(stoploss.stopPrice, 2);

    const auto& bids = book.getCurrentBids();
    EXPECT_TRUE(bids.empty());

    const auto& stop_bids = book.getCurrentStops_Bids();
    EXPECT_EQ(stop_bids.size(), 1);

};

TEST(OrderBookTest, AddSellStopLoss_Single){
    OrderBook book;
    book.sellStopLoss(1, 2, 3);
    auto order = book.getOrderInfo(1);
    ASSERT_TRUE(order.has_value());
    EXPECT_EQ(order->id, 1);
    EXPECT_EQ(order->type, OrderType::STOP_LOSS);
    EXPECT_EQ(order->side, Side::SELL);
    EXPECT_FALSE(order->price.has_value());
    EXPECT_EQ(order->quantityOriginal, 3);
    EXPECT_EQ(order->quantityRemaining, 3);

    const auto& stoploss = std::get<StopLossData>(order->typeSpecificData);
    EXPECT_EQ(stoploss.stopPrice, 2);

    const auto& asks = book.getCurrentAsks();
    EXPECT_TRUE(asks.empty());

    const auto& stop_asks = book.getCurrentStops_Asks();
    EXPECT_EQ(stop_asks.size(), 1);
};

TEST(OrderBookTest, AddBuyStopLoss_Multiple){
    OrderBook book;
    book.buyStopLoss(1, 50, 10);
    book.buyStopLoss(2, 50, 20);
    book.buyStopLoss(3, 40, 25);
    book.buyStopLoss(4, 60, 35);

    const auto& bids = book.getCurrentBids();
    EXPECT_TRUE(bids.empty());

    const auto& stop_bids = book.getCurrentStops_Bids();
    ASSERT_EQ(stop_bids.size(), 3);

    EXPECT_EQ(stop_bids.at(50).totalQuantity, 30);
    EXPECT_EQ(stop_bids.at(40).totalQuantity, 25);
    EXPECT_EQ(stop_bids.at(60).totalQuantity, 35);

    EXPECT_EQ(stop_bids.at(50).orderCount, 2);
    EXPECT_EQ(stop_bids.at(40).orderCount, 1);
    EXPECT_EQ(stop_bids.at(60).orderCount, 1);
};

TEST(OrderBookTest, AddSellStopLoss_Multiple){
    OrderBook book;
    book.sellStopLoss(1, 50, 10);
    book.sellStopLoss(2, 50, 20);
    book.sellStopLoss(3, 40, 25);
    book.sellStopLoss(4, 60, 35);

    const auto& asks = book.getCurrentAsks();
    EXPECT_TRUE(asks.empty());

    const auto& stop_asks = book.getCurrentStops_Asks();
    ASSERT_EQ(stop_asks.size(), 3);

    EXPECT_EQ(stop_asks.at(50).totalQuantity, 30);
    EXPECT_EQ(stop_asks.at(40).totalQuantity, 25);
    EXPECT_EQ(stop_asks.at(60).totalQuantity, 35);

    EXPECT_EQ(stop_asks.at(50).orderCount, 2);
    EXPECT_EQ(stop_asks.at(40).orderCount, 1);
    EXPECT_EQ(stop_asks.at(60).orderCount, 1);
};

TEST(OrderBookTest, CancelBuyStopLoss){
    OrderBook book;

    book.buyStopLoss(1, 100, 10);
    book.buyStopLoss(2, 100, 50);
    book.buyStopLoss(3, 200, 20);

    book.cancelOrder(2);
    auto stop_bids = book.getCurrentStops_Bids();
    EXPECT_EQ(stop_bids.size(), 2);
    EXPECT_EQ(stop_bids.at(100).totalQuantity, 10);
    EXPECT_EQ(stop_bids.at(100).orderCount, 1);

    book.cancelOrder(3);
    stop_bids = book.getCurrentStops_Bids();
    EXPECT_EQ(stop_bids.size(), 1);
    EXPECT_EQ(stop_bids.find(200), stop_bids.end());

    book.cancelOrder(1);
    stop_bids = book.getCurrentStops_Bids();
    EXPECT_TRUE(stop_bids.empty());
};

TEST(OrderBookTest, CancelSellStopLoss){
    OrderBook book;

    book.sellStopLoss(1, 100, 10);
    book.sellStopLoss(2, 100, 50);
    book.sellStopLoss(3, 200, 20);

    book.cancelOrder(2);
    auto stop_asks = book.getCurrentStops_Asks();
    EXPECT_EQ(stop_asks.size(), 2);
    EXPECT_EQ(stop_asks.at(100).totalQuantity, 10);
    EXPECT_EQ(stop_asks.at(100).orderCount, 1);

    book.cancelOrder(3);
    stop_asks = book.getCurrentStops_Asks();
    EXPECT_EQ(stop_asks.size(), 1);
    EXPECT_EQ(stop_asks.find(200), stop_asks.end());

    book.cancelOrder(1);
    stop_asks = book.getCurrentStops_Asks();
    EXPECT_TRUE(stop_asks.empty());
};

TEST(OrderBookTest, CancelNonexistentStopLoss){
    OrderBook book;
    book.buyStopLoss(1, 1, 1);
    EXPECT_FALSE(book.cancelOrder(2));
};
