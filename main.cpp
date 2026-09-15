#include "OrderBook.h"
#include "Order.h"
#include <iostream>
#include <stdexcept>

int main() {

    // ============================================================
    // 1. Partial fill
    // ============================================================

    {
        std::cout << "\n--- Partial Fill ---\n";

        OrderBook book;

        Order ask{
            1,
            OrderType::LIMIT,
            Side::SELL,
            100,
            100
        };

        Order buy{
            2,
            OrderType::LIMIT,
            Side::BUY,
            100,
            25
        };

        book.addOrder(ask);
        book.addOrder(buy);

        std::cout << "Buy remaining quantity: "
                  << buy.quantity << '\n';

        // Expected: 0
        // Ask should have 75 remaining.
    }


    // ============================================================
    // 2. FIFO at the same price
    // ============================================================

    {
        std::cout << "\n--- FIFO ---\n";

        OrderBook book;

        Order ask1{
            1,
            OrderType::LIMIT,
            Side::SELL,
            100,
            50
        };

        Order ask2{
            2,
            OrderType::LIMIT,
            Side::SELL,
            100,
            50
        };

        Order buy1{
            3,
            OrderType::LIMIT,
            Side::BUY,
            100,
            25
        };

        book.addOrder(ask1);
        book.addOrder(ask2);
        book.addOrder(buy1);

        std::cout << "Buy remaining quantity: "
                  << buy1.quantity << '\n';

        // Expected: 0
        // ask1 should have 25 remaining.
        // ask2 should still have 50 remaining.
    }


    // ============================================================
    // 3. Price priority
    // ============================================================

    {
        std::cout << "\n--- Price Priority ---\n";

        OrderBook book;

        Order worseAsk{
            1,
            OrderType::LIMIT,
            Side::SELL,
            101,
            50
        };

        Order betterAsk{
            2,
            OrderType::LIMIT,
            Side::SELL,
            100,
            50
        };

        Order buy{
            3,
            OrderType::LIMIT,
            Side::BUY,
            101,
            25
        };

        book.addOrder(worseAsk);
        book.addOrder(betterAsk);
        book.addOrder(buy);

        std::cout << "Buy remaining quantity: "
                  << buy.quantity << '\n';

        // Expected: 0
        // betterAsk (#2) should be filled.
        // worseAsk (#1) should remain untouched.
    }


    // ============================================================
    // 4. Cancellation
    // ============================================================

    {
        std::cout << "\n--- Cancellation ---\n";

        OrderBook book;

        Order ask{
            1,
            OrderType::LIMIT,
            Side::SELL,
            100,
            50
        };

        book.addOrder(ask);

        bool cancelled = book.cancelOrder(1);

        std::cout << "Cancellation successful: "
                  << std::boolalpha << cancelled << '\n';

        // Expected: true

        bool cancelledAgain = book.cancelOrder(1);

        std::cout << "Cancel nonexistent order: "
                  << cancelledAgain << '\n';

        // Expected: false
    }


    // ============================================================
    // 5. Duplicate order ID
    // ============================================================

    {
        std::cout << "\n--- Duplicate Order ID ---\n";

        OrderBook book;

        Order order1{
            1,
            OrderType::LIMIT,
            Side::BUY,
            100,
            50
        };

        Order order2{
            1,  // Same ID
            OrderType::LIMIT,
            Side::BUY,
            101,
            25
        };

        book.addOrder(order1);

        try {
            book.addOrder(order2);
            std::cout << "ERROR: duplicate ID was accepted\n";
        }
        catch (const std::invalid_argument& e) {
            std::cout << "Correctly rejected duplicate ID: "
                      << e.what() << '\n';
        }
    }


    // ============================================================
    // 6. Market order consumes multiple price levels
    // ============================================================

    {
        std::cout << "\n--- Market Order ---\n";

        OrderBook book;

        Order ask1{
            1,
            OrderType::LIMIT,
            Side::SELL,
            100,
            50
        };

        Order ask2{
            2,
            OrderType::LIMIT,
            Side::SELL,
            101,
            50
        };

        Order ask3{
            3,
            OrderType::LIMIT,
            Side::SELL,
            102,
            50
        };

        Order marketBuy{
            4,
            OrderType::MARKET,
            Side::BUY,
            0,
            125
        };

        book.addOrder(ask1);
        book.addOrder(ask2);
        book.addOrder(ask3);

        book.addOrder(marketBuy);

        std::cout << "Market buy remaining quantity: "
                  << marketBuy.quantity << '\n';

        // Expected: 0
        //
        // Filled:
        //   50 @ 100
        //   50 @ 101
        //   25 @ 102
        //
        // Remaining:
        //   25 @ 102
    }


    std::cout << "\nAll smoke tests completed.\n";

    return 0;
}
