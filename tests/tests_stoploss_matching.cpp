#include <gtest/gtest.h>
#include "OrderBook.h"


// Buy Side:

TEST(OrderBookTest, StopLossNotTriggered_Buy){
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


TEST(OrderBookTest, PartialStopLossFill_Buy){
    OrderBook book;
    book.buyStopLoss(1, 100, 10);
    book.buyLimit(2, 105, 3);
    book.sellLimit(3, 105, 5);

    const auto& stop_bids = book.getCurrentStops_Bids();
    EXPECT_EQ(stop_bids.size(), 0);
    EXPECT_FALSE(book.getOrderInfo(1).has_value());

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 2);
    TradeInfo trade = trades[1];
    EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 1);
    EXPECT_EQ(trade.sellOrderID, 3);
    EXPECT_EQ(trade.price, 105);
    EXPECT_EQ(trade.quantity, 2);
};

TEST(OrderBookTest, ExcessRestingOrders_Buy){
    OrderBook book;
    book.buyStopLoss(1, 100, 10);
    book.buyLimit(2, 105, 20);
    book.sellLimit(3, 105, 40);

    const auto& stop_bids = book.getCurrentStops_Bids();
    const auto& asks = book.getCurrentAsks();
    EXPECT_TRUE(stop_bids.empty());

    EXPECT_EQ(asks.size(), 1);
    EXPECT_EQ(asks.at(105).totalQuantity, 10);
    EXPECT_EQ(asks.at(105).orderCount, 1);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 2);
    TradeInfo trade = trades[1];
    EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 1);
    EXPECT_EQ(trade.sellOrderID, 3);
    EXPECT_EQ(trade.price, 105);
    EXPECT_EQ(trade.quantity, 10);
};

TEST(OrderBookTest, FIFO_Stops_Buy){
    OrderBook book;
    book.buyStopLoss(1, 100, 10);
    book.buyStopLoss(2, 100, 10);
    book.buyLimit(3, 100, 10);
    book.sellLimit(4, 100, 30);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 3);

    TradeInfo trade = trades[1];
    EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 1);
    EXPECT_EQ(trade.sellOrderID, 4);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 10);

    trade = trades[2];
    EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 2);
    EXPECT_EQ(trade.sellOrderID, 4);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 10);
};

TEST(OrderBookTest, Stops_in_PriceOrder_Buy){
    OrderBook book;
    book.buyStopLoss(1, 100, 10);
    book.buyStopLoss(2, 101, 10);
    book.buyStopLoss(3, 102, 10);
    book.buyLimit(4, 105, 10);
    book.sellLimit(5, 105, 40);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 4);

    TradeInfo trade = trades[1];
    EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 1);
    EXPECT_EQ(trade.sellOrderID, 5);
    EXPECT_EQ(trade.price, 105);
    EXPECT_EQ(trade.quantity, 10);

    trade = trades[2];
    EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 2);
    EXPECT_EQ(trade.sellOrderID, 5);
    EXPECT_EQ(trade.price, 105);
    EXPECT_EQ(trade.quantity, 10);

    trade = trades[3];
    EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 3);
    EXPECT_EQ(trade.sellOrderID, 5);
    EXPECT_EQ(trade.price, 105);
    EXPECT_EQ(trade.quantity, 10);
};

// Sell Side:

TEST(OrderBookTest, StopLossNotTriggered_Sell){
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

TEST(OrderBookTest, PartialStopLossFill_Sell){
    OrderBook book;
    book.sellStopLoss(1, 105, 10);
    book.buyLimit(2, 100, 5);
    book.sellLimit(3, 100, 3);

    const auto& stop_asks = book.getCurrentStops_Asks();
    EXPECT_EQ(stop_asks.size(), 0);
    EXPECT_FALSE(book.getOrderInfo(1).has_value());

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 2);
    TradeInfo trade = trades[1];
    EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 2);
    EXPECT_EQ(trade.sellOrderID, 1);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 2);
};

TEST(OrderBookTest, ExcessRestingOrders_Sell){
    OrderBook book;
    book.sellStopLoss(1, 100, 10);
    book.buyLimit(2, 95, 40);
    book.sellLimit(3, 95, 20);

    const auto& stop_asks = book.getCurrentStops_Asks();
    const auto& bids = book.getCurrentBids();
    EXPECT_TRUE(stop_asks.empty());

    EXPECT_EQ(bids.size(), 1);
    EXPECT_EQ(bids.at(95).totalQuantity, 10);
    EXPECT_EQ(bids.at(95).orderCount, 1);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 2);
    TradeInfo trade = trades[1];
    EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 2);
    EXPECT_EQ(trade.sellOrderID, 1);
    EXPECT_EQ(trade.price, 95);
    EXPECT_EQ(trade.quantity, 10);
};

TEST(OrderBookTest, FIFO_Stops_Sell){
    OrderBook book;
    book.sellStopLoss(1, 100, 10);
    book.sellStopLoss(2, 100, 10);
    book.buyLimit(3, 100, 30);
    book.sellLimit(4, 100, 10);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 3);

    TradeInfo trade = trades[1];
    EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 3);
    EXPECT_EQ(trade.sellOrderID, 1);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 10);

    trade = trades[2];
    EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 3);
    EXPECT_EQ(trade.sellOrderID, 2);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 10);
};
TEST(OrderBookTest, Stops_in_PriceOrder_Sell){
    OrderBook book;
    book.sellStopLoss(1, 99, 10);
    book.sellStopLoss(2, 98, 10);
    book.sellStopLoss(3, 97, 10);
    book.buyLimit(4, 95, 40);
    book.sellLimit(5, 95, 10);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 4);

    TradeInfo trade = trades[1];
    EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 4);
    EXPECT_EQ(trade.sellOrderID, 1);
    EXPECT_EQ(trade.price, 95);
    EXPECT_EQ(trade.quantity, 10);

    trade = trades[2];
    EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 4);
    EXPECT_EQ(trade.sellOrderID, 2);
    EXPECT_EQ(trade.price, 95);
    EXPECT_EQ(trade.quantity, 10);

    trade = trades[3];
    EXPECT_EQ(trade.aggressorType, OrderType::STOP_LOSS);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 4);
    EXPECT_EQ(trade.sellOrderID, 3);
    EXPECT_EQ(trade.price, 95);
    EXPECT_EQ(trade.quantity, 10);
};