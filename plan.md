Add Try catch to API


==============


QUANT RESEARCH PROJECT

Working title:
Execution-Aware Statistical Arbitrage Using Limit-Order-Book Information

OBJECTIVE

Build a quantitative trading research project combining:

A C++ FIFO (price-time priority) limit-order-book engine
Historical market-data replay
Python statistical-arbitrage research
Order-book-based execution modeling

The C++ component demonstrates:

C++ ability
Data structures and algorithms
Trading infrastructure knowledge
Market microstructure understanding
Performance engineering

The Python component demonstrates:

Statistical modeling
Quantitative research
Backtesting
Statistical arbitrage
Experimental rigor
CENTRAL RESEARCH QUESTION

Can limit-order-book information improve the timing and/or execution
of a pairs-trading strategy?

Do NOT attempt to build a general-purpose HFT system.

The order book exists primarily to:

Learn trading infrastructure
Demonstrate C++ ability
Provide realistic execution information
Generate market-microstructure features for research
PART I — C++ LIMIT ORDER BOOK

Implement a simple FIFO / price-time-priority order book.

Core objects:

Order
PriceLevel
OrderBook
Trade
MatchingEngine

Required functionality:

Limit orders
Market orders
Order cancellation
Partial fills
FIFO matching
Multiple price levels
Order IDs
Trade generation
Best bid / best ask

Important design considerations:

Use integer prices/ticks rather than floating-point prices
Maintain FIFO queues at each price
Maintain an efficient OrderID -> order lookup
Keep matching logic separate from book representation
PART II — TESTING AND VALIDATION

Before doing research, establish that the order book is correct.

Unit tests:

Basic order insertion
FIFO priority
Price priority
Partial fills
Complete fills
Market orders
Cancellation
Multiple price levels
Empty-book behavior
Edge cases

Randomized/property testing:

Generate random sequences of:

Add
Cancel
Market order
Limit order

Compare the main implementation against a slower,
obviously-correct reference implementation.

Goal:

The optimized implementation should produce identical
book states and trades to the reference implementation.

PART III — PERFORMANCE

Benchmark:

Order insertion
Cancellation
Matching
Throughput (events/sec)
Latency
p50 latency
p95 latency
p99 latency
Memory usage

Potential comparison:

std::map-based price levels
Alternative/bucket-based representation

Do not optimize prematurely.

First establish correctness.
Then profile.
Then optimize.

PART IV — HISTORICAL MARKET-DATA REPLAY

Obtain appropriate historical order-event / limit-order-book data.

Build a replay system:

Historical events
|
v
C++ Order Book
|
v
Reconstructed Book
|
v
Trades + Book States + Features

Validate reconstruction where possible against the
historical data.

Extract:

Bid
Ask
Mid-price
Spread
Bid depth
Ask depth
Order arrivals
Cancellations
Executions
Trade direction
Order-flow imbalance
Queue/depth information
PART V — STATISTICAL ARBITRAGE

Implement a relatively simple pairs-trading strategy.

Example:

Spread:

S_t = log(P_A,t) - beta * log(P_B,t)

Standardized spread:

Z_t = (S_t - mean(S)) / std(S)

Example trading rule:

Z > threshold:
Short spread

Z < -threshold:
Long spread

Initially keep the strategy simple.

Focus on research quality rather than strategy complexity.

PART VI — ORDER-BOOK INFORMATION

Investigate whether order-book information provides
incremental information for the pairs strategy.

Potential features:

Bid-ask spread
Order-book imbalance
Order-flow imbalance
Depth
Recent trade imbalance
Cancellation intensity
Short-term volatility
Queue imbalance

Example research question:

Does LOB information help determine whether a pair
dislocation is likely to mean-revert soon?

Compare:

MODEL A:
Pairs signal only

versus

MODEL B:
Pairs signal + LOB information

PART VII — EXECUTION MODEL

Use the C++ order book to make the backtest more realistic.

Do NOT assume every trade occurs at the mid-price.

Given a strategy order:

BUY 5,000 shares


simulate execution against the book.

Measure:

Fill quantity
Execution price
Spread cost
Slippage
Market impact
Partial fills
Time to fill
Queue position where possible

Compare:

Gross strategy P&L

versus

Execution-adjusted P&L

PART VIII — BACKTESTING RIGOR

Avoid common sources of false alpha.

Use:

Train/test separation
Walk-forward testing where appropriate
No look-ahead bias
Transaction costs
Realistic execution assumptions
Parameter sensitivity analysis
Multiple time periods
Out-of-sample evaluation

Report:

Return
Sharpe ratio
Volatility
Maximum drawdown
Turnover
Hit rate
Transaction costs
Capacity / position-size sensitivity
PART IX — FINAL RESEARCH QUESTIONS

Possible final questions:

Does order-book imbalance predict short-term movements
in the pair spread?

Does LOB information improve the timing of pairs trades?

Does LOB information improve execution quality?

How quickly does any predictive information decay?

Does the apparent alpha survive realistic execution costs?

How does strategy profitability change with order size?

FINAL PROJECT STRUCTURE

quant-project/

cpp/
    order_book/
    matching_engine/
    replay/

python/
    data/
    features/
    stat_arb/
    execution/
    backtest/

tests/

benchmarks/

research/
    notebooks/
    figures/
    results/

README.md

THREE-WEEK TARGET

WEEK 1:
C++ infrastructure

FIFO order book
Matching engine
Cancellation
Partial fills
Tests
Randomized validation
Basic benchmarks

Deliverable:
A correct, clean, tested C++ order-book engine.

WEEK 2:
Market data + quantitative research

Historical data
Event replay
Book reconstruction
Feature generation
Basic pairs strategy
Initial statistical analysis

Deliverable:
A functioning research pipeline.

WEEK 3:
Execution + final research

Connect strategy to simulated execution
Transaction costs
Slippage
Market impact
Out-of-sample testing
Robustness checks
Visualizations
Final analysis
README / documentation

Deliverable:
A coherent research project with defensible empirical results.

CORE PRINCIPLE

The project should NOT be:

"I built an order book and a trading bot."

It should be:

"I built the market-microstructure infrastructure necessary to
investigate whether order-book information improves an
execution-aware statistical-arbitrage strategy."

The C++ order book demonstrates engineering ability.

The statistical-arbitrage research demonstrates quantitative ability.

The execution model connects the two.

SUCCESS CRITERION

A successful project does NOT require finding a profitable strategy.

A negative result is acceptable and potentially interesting.

For example:

"Order-book imbalance provides statistically significant
short-horizon predictive power, but the effect disappears after
transaction costs and realistic execution."

That is a legitimate research finding.

The goal is to demonstrate:

Mathematical reasoning
Statistical rigor
C++ proficiency
Quantitative research ability
Understanding of market microstructure
Ability to build and evaluate a realistic trading system