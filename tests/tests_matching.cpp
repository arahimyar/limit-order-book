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

// Iceberg matching tests:

TEST(OrderBookTest, Iceberg_NoMatch_Buy){
    OrderBook book;
    book.sellLimit(1, 100, 20);
    book.buyIceberg(2, 90, 10, 5);

    const auto& trades = book.getTradeHistory();
    EXPECT_TRUE(trades.empty());

    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();

    ASSERT_EQ(bids.size(), 1);
    ASSERT_EQ(asks.size(), 1);

    EXPECT_EQ(bids.at(90).totalQuantity, 5);
    EXPECT_EQ(bids.at(90).orderCount, 1);

    EXPECT_EQ(asks.at(100).totalQuantity, 20);
    EXPECT_EQ(asks.at(100).orderCount, 1);

    EXPECT_EQ(book.getBestBid(), 90);
    EXPECT_EQ(book.getBestAsk(), 100);
    EXPECT_EQ(book.getSpread(), 10);
};

TEST(OrderBookTest, Iceberg_PartialMatch_Buy){
    OrderBook book;
    book.sellLimit(1, 100, 3);
    book.buyIceberg(2, 100, 10, 5);

    const auto& trades = book.getTradeHistory();
    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();
    
    ASSERT_EQ(bids.size(), 1);
    ASSERT_TRUE(asks.empty());

    EXPECT_EQ(trades.size(), 1);
    TradeInfo trade = trades[0];
    EXPECT_EQ(trade.aggressorType, OrderType::ICEBERG);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 2);
    EXPECT_EQ(trade.sellOrderID, 1);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 3);

    EXPECT_EQ(bids.at(100).totalQuantity, 2);
    EXPECT_EQ(bids.at(100).orderCount, 1);

    auto order = book.getOrderInfo(2);
    const auto& iceberg = std::get<IcebergData>(order->typeSpecificData);
    EXPECT_EQ(iceberg.quantityTotalRemaining, 7);    
};

TEST(OrderBookTest, Iceberg_Replenish_Buy){
    OrderBook book;
    book.buyIceberg(1, 100, 50, 10);
    book.buyLimit(2, 100, 30);
    book.sellMarket(3, 10);

    const auto& trades = book.getTradeHistory();
    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();

    ASSERT_EQ(bids.size(), 1);
    ASSERT_TRUE(asks.empty());
    EXPECT_EQ(trades.size(), 1);
    
    TradeInfo trade = trades[0];
    EXPECT_EQ(trade.aggressorType, OrderType::MARKET);
    EXPECT_EQ(trade.restingType, OrderType::ICEBERG);
    EXPECT_EQ(trade.buyOrderID, 1);
    EXPECT_EQ(trade.sellOrderID, 3);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 10);

    book.sellMarket(4, 30);
    trade = trades[1];
    EXPECT_EQ(trade.aggressorType, OrderType::MARKET);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 2);
    EXPECT_EQ(trade.sellOrderID, 4);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 30);

    auto order = book.getOrderInfo(1);
    const auto& iceberg = std::get<IcebergData>(order->typeSpecificData);
    EXPECT_EQ(order->quantityRemaining, 10);
    EXPECT_EQ(iceberg.quantityTotalRemaining, 40);
};

TEST(OrderBookTest, Iceberg_TotalDepletion_Buy){
    OrderBook book;
    book.buyIceberg(1, 100, 50, 25);
    book.sellLimit(2, 100, 50);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 2);

    TradeInfo trade = trades[0];
    EXPECT_EQ(trade.aggressorType, OrderType::LIMIT);
    EXPECT_EQ(trade.restingType, OrderType::ICEBERG);
    EXPECT_EQ(trade.buyOrderID, 1);
    EXPECT_EQ(trade.sellOrderID, 2);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 25);

    trade = trades[1];
    EXPECT_EQ(trade.aggressorType, OrderType::LIMIT);
    EXPECT_EQ(trade.restingType, OrderType::ICEBERG);
    EXPECT_EQ(trade.buyOrderID, 1);
    EXPECT_EQ(trade.sellOrderID, 2);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 25);

    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();
    EXPECT_TRUE(bids.empty());
    EXPECT_TRUE(asks.empty());

    EXPECT_FALSE(book.getOrderInfo(1).has_value());
    EXPECT_FALSE(book.getOrderInfo(2).has_value());
};

TEST(OrderBookTest, Iceberg_NoMatch_Sell){
    OrderBook book;
    book.buyIceberg(2, 90, 10, 5);
    book.sellLimit(1, 100, 20);

    const auto& trades = book.getTradeHistory();
    EXPECT_TRUE(trades.empty());

    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();

    ASSERT_EQ(bids.size(), 1);
    ASSERT_EQ(asks.size(), 1);

    EXPECT_EQ(bids.at(90).totalQuantity, 5);
    EXPECT_EQ(bids.at(90).orderCount, 1);

    EXPECT_EQ(asks.at(100).totalQuantity, 20);
    EXPECT_EQ(asks.at(100).orderCount, 1);

    EXPECT_EQ(book.getBestBid(), 90);
    EXPECT_EQ(book.getBestAsk(), 100);
    EXPECT_EQ(book.getSpread(), 10);
};

TEST(OrderBookTest, Iceberg_PartialMatch_Sell){
    OrderBook book;
    book.buyLimit(1, 100, 3);
    book.sellIceberg(2, 100, 10, 5);

    const auto& trades = book.getTradeHistory();
    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();
    
    ASSERT_EQ(asks.size(), 1);
    ASSERT_TRUE(bids.empty());

    EXPECT_EQ(trades.size(), 1);
    TradeInfo trade = trades[0];
    EXPECT_EQ(trade.aggressorType, OrderType::ICEBERG);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 1);
    EXPECT_EQ(trade.sellOrderID, 2);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 3);

    EXPECT_EQ(asks.at(100).totalQuantity, 2);
    EXPECT_EQ(asks.at(100).orderCount, 1);

    auto order = book.getOrderInfo(2);
    const auto& iceberg = std::get<IcebergData>(order->typeSpecificData);
    EXPECT_EQ(iceberg.quantityTotalRemaining, 7);    
};

TEST(OrderBookTest, Iceberg_Replenish_Sell){
    OrderBook book;
    book.sellIceberg(1, 100, 50, 10);
    book.sellLimit(2, 100, 30);
    book.buyMarket(3, 10);

    const auto& trades = book.getTradeHistory();
    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();

    ASSERT_EQ(asks.size(), 1);
    ASSERT_TRUE(bids.empty());
    EXPECT_EQ(trades.size(), 1);
    
    TradeInfo trade = trades[0];
    EXPECT_EQ(trade.aggressorType, OrderType::MARKET);
    EXPECT_EQ(trade.restingType, OrderType::ICEBERG);
    EXPECT_EQ(trade.buyOrderID, 3);
    EXPECT_EQ(trade.sellOrderID, 1);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 10);

    book.buyMarket(4, 30);
    trade = trades[1];
    EXPECT_EQ(trade.aggressorType, OrderType::MARKET);
    EXPECT_EQ(trade.restingType, OrderType::LIMIT);
    EXPECT_EQ(trade.buyOrderID, 4);
    EXPECT_EQ(trade.sellOrderID, 2);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 30);

    auto order = book.getOrderInfo(1);
    const auto& iceberg = std::get<IcebergData>(order->typeSpecificData);
    EXPECT_EQ(order->quantityRemaining, 10);
    EXPECT_EQ(iceberg.quantityTotalRemaining, 40);
};

TEST(OrderBookTest, Iceberg_TotalDepletion_Sell){
    OrderBook book;
    book.sellIceberg(1, 100, 50, 25);
    book.buyLimit(2, 100, 50);

    const auto& trades = book.getTradeHistory();
    ASSERT_EQ(trades.size(), 2);

    TradeInfo trade = trades[0];
    EXPECT_EQ(trade.aggressorType, OrderType::LIMIT);
    EXPECT_EQ(trade.restingType, OrderType::ICEBERG);
    EXPECT_EQ(trade.buyOrderID, 2);
    EXPECT_EQ(trade.sellOrderID, 1);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 25);

    trade = trades[1];
    EXPECT_EQ(trade.aggressorType, OrderType::LIMIT);
    EXPECT_EQ(trade.restingType, OrderType::ICEBERG);
    EXPECT_EQ(trade.buyOrderID, 2);
    EXPECT_EQ(trade.sellOrderID, 1);
    EXPECT_EQ(trade.price, 100);
    EXPECT_EQ(trade.quantity, 25);

    const auto& bids = book.getCurrentBids();
    const auto& asks = book.getCurrentAsks();
    EXPECT_TRUE(bids.empty());
    EXPECT_TRUE(asks.empty());

    EXPECT_FALSE(book.getOrderInfo(1).has_value());
    EXPECT_FALSE(book.getOrderInfo(2).has_value());
};


