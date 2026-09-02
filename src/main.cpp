#include "OrderBook.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <algorithm>

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

    std::vector<double> latencies;
    latencies.reserve(100000); // Prevent reallocation overhead during timing

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = split(line, ',');

        int id = std::stoi(tokens[0]);
        bool isBuy = (tokens[1] == "B");
        std::string type = tokens[2];
        int price = std::stoi(tokens[3]);
        int quantity = std::stoi(tokens[4]);

        Order order{id, isBuy, price, quantity, 0};

        auto start = std::chrono::high_resolution_clock::now();

        if (type == "LIMIT") {
            book.addLimitOrder(order);
        } else if (type == "MARKET") {
            book.addMarketOrder(order);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        latencies.push_back(duration);
    }

    if (!latencies.empty()) {
        std::sort(latencies.begin(), latencies.end());
        double min_lat = latencies.front();
        double max_lat = latencies.back();
        double p50 = latencies[latencies.size() * 0.50];
        double p90 = latencies[latencies.size() * 0.90];
        double p99 = latencies[latencies.size() * 0.99];

        std::cout << "\n--- Latency Stats (microseconds) ---\n";
        std::cout << "Count: " << latencies.size() << "\n";
        std::cout << "Min:   " << min_lat << " us\n";
        std::cout << "P50:   " << p50 << " us\n";
        std::cout << "P90:   " << p90 << " us\n";
        std::cout << "P99:   " << p99 << " us\n";
        std::cout << "Max:   " << max_lat << " us\n";
    }

    return 0;
}  // use deadlock like thing mutual exclusion