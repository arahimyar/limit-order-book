#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "parser.h"
#include "display.h"

void loading(OrderBook& book, std::istringstream& input){
    std::string filename;

    if (!(input >> filename)) {
        std::cout << "Input error. Command must be of the form: load <filename>\n";
        return;
    }

    std::ifstream file(filename);

    if (!file) {
        std::cout << "Error: could not open file: " << filename << '\n';
        return;
    }

    std::string line;
    std::size_t count = 0;

    while (std::getline(file, line)){
        if (line.empty()){
            continue;
        }

        std::istringstream inputCommand(line);
        std::string command;
        inputCommand >> command;

        if (command == "buy"){
            buying(book, inputCommand);
        }
        else if(command == "sell"){
            selling(book, inputCommand);
        }
        else if (command == "cancel"){
            canceling(book, inputCommand);
        }
        else {
            std::cout << "Unknown command in file: " << filename << "\n";
            continue;
        }
        count++;
    }
    std::cout << "Successfully loaded " << count << " commands from " << filename << ".\n";
};

void buying(OrderBook& book, std::istringstream& input){
    std::string type;
    input >> type;

    if (type == "limit") {
        std::string idString;
        std::string priceString;
        std::string quantityString;

        if (!(input >> idString >> priceString >> quantityString)) {
            std::cout << "Input error. Command must be of the form: buy limit <id> <price> <quantity>\n";
            return;
        }

        if (priceString.find('.') != std::string::npos) {
            std::cout << "Input error: price must be an integer number of cents.\n";
            return;
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
            return;
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
};

void selling(OrderBook& book, std::istringstream& input){
    std::string type;
    input >> type;

    if (type == "limit") {
        std::string idString;
        std::string priceString;
        std::string quantityString;

        if (!(input >> idString >> priceString >> quantityString)) {
            std::cout << "Input error. Command must be of the form: sell limit <id> <price> <quantity>\n";
            return;
        }

        if (priceString.find('.') != std::string::npos) {
            std::cout << "Input error: price must be an integer number of cents. \n";
            return;
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
            std::cout << "Input error. Command must be of the form: sell market <id> <quantity>\n";
            return;
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
};

void canceling(OrderBook& book, std::istringstream& input){
    OrderID id;
    if (!(input >> id)) {
        std::cout << "Input error. Command must be of the form: cancel <id>\n";
        return;
    }

    if (book.cancelOrder(id)) {
        std::cout << "Order " << id << " cancelled.\n";
    }
    else {
        std::cout << "Order " << id << " not found.\n";
    }
};

void displaying(OrderBook& book, std::istringstream& input){
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
            return;
        }
        displayOrderInfo(book, id);
    }
    else {
        std::cout << "Unknown display type: " << type << '\n';
    }
};

void runOrderBook(OrderBook& book){
    std::string line;

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break;
        }
        std::istringstream input(line);
        std::string command;
        input >> command;

        if (command == "load"){
            loading(book, input);
        }
        else if (command == "buy"){
            buying(book, input);
        }
        else if (command == "sell"){
            selling(book, input);
        }
        else if (command == "cancel"){
            canceling(book, input);
        }
        else if (command == "display"){
            displaying(book, input);
        }
        else if (command == "help") {
            std::cout
                << "\nCommands:\n"
                << "  buy limit <id> <price> <quantity>\n"
                << "  buy market <id> <quantity>\n"
                << "  sell limit <id> <price> <quantity>\n"
                << "  sell market <id> <quantity>\n"
                << "  cancel <id>\n"
                << "  load <file name>\n"
                << "  display book [N]\n"
                << "  display trades [N]\n"
                << "  display status\n"
                << "  display order <id>\n"
                << "  help\n"
                << "  quit\n"
                << "See documentation for more details.\n\n";
        }
        else if (command == "quit") {
            break;
        }
        else if (command.empty()) {
            return;
        }
        else {
            std::cout << "Unknown command: " << command << '\n';
        }
    }
};