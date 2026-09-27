#include <iostream>
#include <string>
#include <sstream>
#include "parser.h"
#include "display.h"


void OrderBook_API(OrderBook& book){
    std::string line;

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break;
        }
        std::istringstream input(line);
        std::string command;
        input >> command;

        // Buying
        if (command == "buy") {
            std::string type;
            input >> type;

            if (type == "limit") {
                std::string idString;
                std::string priceString;
                std::string quantityString;

                if (!(input >> idString >> priceString >> quantityString)) {
                    std::cout << "Input error. Command must be of the form: buy limit <id> <price> <quantity>\n";
                    continue;
                }

                if (priceString.find('.') != std::string::npos) {
                    std::cout << "Input error: price must be an integer number of cents.\n";
                    continue;
                }

                try {
                    OrderID id = std::stoull(idString);
                    Price price = std::stoll(priceString);
                    Quantity quantity = std::stoull(quantityString);

                    book.buyLimit(id, price, quantity);
                }
                catch (const std::exception& e) {
                    std::cout << "Order rejected: " << e.what() << '\n';
                }
            }
            else if (type == "market") {
                std::string idString;
                std::string quantityString;

                if (!(input >> idString >> quantityString)) {
                    std::cout << "Input error. Command must be of the form: buy market <id> <quantity>\n";
                    continue;
                }

                try {
                    OrderID id = std::stoull(idString);
                    Quantity quantity = std::stoull(quantityString);

                    book.buyMarket(id, quantity);
                }
                catch (const std::exception& e) {
                    std::cout << "Order rejected: " << e.what() << '\n';
                }
            }
            else {
                std::cout << "Unknown order type: " << type << '\n';
            }
        }

        // Selling
        else if (command == "sell") {
            std::string type;
            input >> type;

            if (type == "limit") {
                std::string idString;
                std::string priceString;
                std::string quantityString;

                if (!(input >> idString >> priceString >> quantityString)) {
                    std::cout << "Input error. Command must be of the form: buy limit <id> <price> <quantity>\n";
                    continue;
                }

                if (priceString.find('.') != std::string::npos) {
                    std::cout << "Input error: price must be an integer number of cents. \n";
                    continue;
                }

                try {
                    OrderID id = std::stoull(idString);
                    Price price = std::stoll(priceString);
                    Quantity quantity = std::stoull(quantityString);

                    book.sellLimit(id, price, quantity);
                }
                catch (const std::exception& e) {
                    std::cout << "Order rejected: " << e.what() << '\n';
                }
            }
            else if (type == "market") {
                std::string idString;
                std::string quantityString;

                if (!(input >> idString >> quantityString)) {
                    std::cout << "Input error. Command must be of the form: buy market <id> <quantity>\n";
                    continue;
                }

                try {
                    OrderID id = std::stoull(idString);
                    Quantity quantity = std::stoull(quantityString);

                    book.sellMarket(id, quantity);
                }
                catch (const std::exception& e) {
                    std::cout << "Order rejected: " << e.what() << '\n';
                }
            }
            else {
                std::cout << "Unknown order type: " << type << '\n';
            }
        }

        // Cancel order
        else if (command == "cancel") {
            OrderID id;

            if (!(input >> id)) {
                std::cout << "Input error. Command must be of the form: cancel <id>\n";
                continue;
            }

            if (book.cancelOrder(id)) {
                std::cout << "Order " << id << " cancelled.\n";
            }
            else {
                std::cout << "Order " << id << " not found.\n";
            }
        }

        // Display 
        else if (command == "display") {
            std::string type;
            input >> type;

            if (type == "book") {
                std::size_t N = 10;
                input >> N;

                displayCurrentBook(book, N);
            }
            else if (type == "trades") {
                std::size_t N = 5;
                input >> N;

                displayRecentTrades(book, N);
            }
            else if (type == "status") {
                displayBookStatus(book);
            }
            else if (type == "order") {
                OrderID id;

                if (!(input >> id)) {
                    std::cout << "Input error. Command must be of the form: display order <id>\n";
                    continue;
                }

                displayOrderInfo(book, id);
            }
            else {
                std::cout << "Unknown display type: " << type << '\n';
            }
        }

        // Help 
        else if (command == "help") {
            std::cout
                << "\nCommands:\n"
                << "  buy limit <id> <price> <quantity>\n"
                << "  buy market <id> <quantity>\n"
                << "  sell limit <id> <price> <quantity>\n"
                << "  sell market <id> <quantity>\n"
                << "  cancel <id>\n"
                << "  display book [N]\n"
                << "  display trades [N]\n"
                << "  display status\n"
                << "  display order <id>\n"
                << "  help\n"
                << "  quit\n"
                << "See documentation for more details.\n\n";
        }

        // Quit
        else if (command == "quit") {
            break;
        }
        else if (command.empty()) {
            continue;
        }
        else {
            std::cout << "Unknown command: " << command << '\n';
        }
    }
};