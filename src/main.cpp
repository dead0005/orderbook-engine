#include "OrderBook.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

std::vector<std::string> split(const std::string& line, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, delim)) tokens.push_back(item);
    return tokens;
}

int main() {
    OrderBook book;

    std::ifstream file("data/orders.csv");
    if (!file.is_open()) {
        std::cerr << "Could not open orders.csv\n";
        return 1;
    }

    std::string line;
    std::getline(file, line); // skip header row

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = split(line, ',');

        int id = std::stoi(tokens[0]);
        bool isBuy = (tokens[1] == "B");
        std::string type = tokens[2];
        int price = std::stoi(tokens[3]);
        int quantity = std::stoi(tokens[4]);

        Order order{id, isBuy, price, quantity, 0};

        if (type == "LIMIT") {
            book.addLimitOrder(order);
        } else if (type == "MARKET") {
            book.addMarketOrder(order);
        }

        book.printBook(); // optional: see book state after every order
    }

    return 0;
}