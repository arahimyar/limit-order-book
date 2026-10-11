#include <gtest/gtest.h>
#include "OrderBook.h"

TEST(OrderBookTest, AddBuyIcebergOrder_Single){
    OrderBook book;
    book.buyIceberg(1, 10, 100, 20);
    auto order = book.getOrderInfo(1);
    ASSERT_TRUE(order.has_value());
    EXPECT_EQ(order->id, 1);
    EXPECT_EQ(order->type, OrderType::ICEBERG);
    EXPECT_EQ(order->side, Side::BUY);
    EXPECT_EQ(order->price, 10);
    EXPECT_EQ(order->quantityOriginal, 100);
    EXPECT_EQ(order->quantityRemaining, 20);
    
    const auto& iceberg = std::get<IcebergData>(order->typeSpecificData);
    EXPECT_EQ(iceberg.quantityTotalRemaining, 100);
    EXPECT_EQ(iceberg.peakSize, 20);
    EXPECT_EQ(book.getBestBid(), 10);
};

TEST(OrderBookTest, AddSellIcebergOrder_Single){
    OrderBook book;
    book.sellIceberg(1, 10, 100, 20);
    auto order = book.getOrderInfo(1);
    ASSERT_TRUE(order.has_value());
    EXPECT_EQ(order->id, 1);
    EXPECT_EQ(order->type, OrderType::ICEBERG);
    EXPECT_EQ(order->side, Side::SELL);
    EXPECT_EQ(order->price, 10);
    EXPECT_EQ(order->quantityOriginal, 100);
    EXPECT_EQ(order->quantityRemaining, 20);
    
    const auto& iceberg = std::get<IcebergData>(order->typeSpecificData);
    EXPECT_EQ(iceberg.quantityTotalRemaining, 100);
    EXPECT_EQ(iceberg.peakSize, 20);
    EXPECT_EQ(book.getBestAsk(), 10);
};


TEST(OrderBookTest, AddBuyIcebergOrder_Multiple){
    OrderBook book;
    book.buyIceberg(1, 10, 100, 20);
    book.buyIceberg(2, 20, 200, 50);
    book.buyIceberg(3, 10, 100, 30);
    book.buyIceberg(4, 10, 120, 20);
    book.buyIceberg(5, 8, 80, 10);

    EXPECT_EQ(book.getBestBid(), 20);

    auto bids = book.getCurrentBids();
    ASSERT_EQ(bids.size(), 3);
    
    EXPECT_EQ(bids.at(8).totalQuantity, 10);
    EXPECT_EQ(bids.at(10).totalQuantity, 70);
    EXPECT_EQ(bids.at(20).totalQuantity, 50);

    EXPECT_EQ(bids.at(8).orderCount, 1);
    EXPECT_EQ(bids.at(10).orderCount, 3);
    EXPECT_EQ(bids.at(20).orderCount, 1);
};

TEST(OrderBookTest, AddSellIcebergOrder_Multiple){
    OrderBook book;
    book.sellIceberg(1, 10, 100, 20);
    book.sellIceberg(2, 20, 200, 50);
    book.sellIceberg(3, 10, 100, 30);
    book.sellIceberg(4, 10, 120, 20);
    book.sellIceberg(5, 8, 80, 10);

    EXPECT_EQ(book.getBestAsk(), 8);

    auto asks = book.getCurrentAsks();
    ASSERT_EQ(asks.size(), 3);
    
    EXPECT_EQ(asks.at(8).totalQuantity, 10);
    EXPECT_EQ(asks.at(10).totalQuantity, 70);
    EXPECT_EQ(asks.at(20).totalQuantity, 50);

    EXPECT_EQ(asks.at(8).orderCount, 1);
    EXPECT_EQ(asks.at(10).orderCount, 3);
    EXPECT_EQ(asks.at(20).orderCount, 1);
};

TEST(OrderBookTest, CancelBuyIceberg){
    OrderBook book;

    book.buyIceberg(1, 100, 10, 5);
    book.buyIceberg(2, 100, 50, 10);
    book.buyIceberg(3, 200, 20, 5);

    book.cancelOrder(2);
    auto bids = book.getCurrentBids();
    EXPECT_EQ(bids.size(), 2);
    EXPECT_EQ(bids.at(100).totalQuantity, 5);
    EXPECT_EQ(bids.at(100).orderCount, 1);

    book.cancelOrder(3);
    bids = book.getCurrentBids();
    EXPECT_EQ(bids.size(), 1);
    EXPECT_EQ(bids.find(200), bids.end());

    book.cancelOrder(1);
    bids = book.getCurrentBids();
    EXPECT_TRUE(bids.empty());
};

TEST(OrderBookTest, CancelSellIceberg){
    OrderBook book;

    book.sellIceberg(1, 100, 10, 5);
    book.sellIceberg(2, 100, 50, 10);
    book.sellIceberg(3, 200, 20, 5);

    book.cancelOrder(2);
    auto asks = book.getCurrentAsks();
    EXPECT_EQ(asks.size(), 2);
    EXPECT_EQ(asks.at(100).totalQuantity, 5);
    EXPECT_EQ(asks.at(100).orderCount, 1);

    book.cancelOrder(3);
    asks = book.getCurrentAsks();
    EXPECT_EQ(asks.size(), 1);
    EXPECT_EQ(asks.find(200), asks.end());

    book.cancelOrder(1);
    asks = book.getCurrentAsks();
    EXPECT_TRUE(asks.empty());
};


TEST(OrderBookTest, CancelNonexistentIceberg){
    OrderBook book;
    book.buyIceberg(1, 1, 1, 1);
    EXPECT_FALSE(book.cancelOrder(2));
};

TEST(OrderBookTest, LargerPeak){
    OrderBook book;

    book.buyIceberg(1, 100, 10, 500);

    auto order = book.getOrderInfo(1);
    ASSERT_TRUE(order.has_value());

    EXPECT_EQ(order->quantityOriginal, 10);
    EXPECT_EQ(order->quantityRemaining, 10);

    const auto& iceberg = std::get<IcebergData>(order->typeSpecificData);
    EXPECT_EQ(iceberg.peakSize, 500);
    EXPECT_EQ(iceberg.quantityTotalRemaining, 10);

    auto bids = book.getCurrentBids();
    EXPECT_EQ(bids.at(100).totalQuantity, 10);
};