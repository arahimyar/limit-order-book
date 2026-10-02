# Limit Order Book

A C++ price-time priority limit order book supporting limit and market orders, O(1) order cancellation, and (something about terminal here). (Something about order speed and benchmarks here).

## Background

An order book is a tool used in financial markets to keep track of incoming orders to trade a financial instrument. Orders that wish to buy the underlying are called *bids* and those that wish to sell are called *asks*. The orders specify a bound on the price they are willing to pay (for bids, this is an upper bound, and for sells this is a lower bound) as well as the quantity (i.e. number of units) they are willing to buy/sell. When the price of a bid exceeds or is equal to the price of an ask, as transaction occurs. Orders that are not able to be matched are stored in the order book.



## Design

## Testing

## Performance

## Usage

## Build & Installation

