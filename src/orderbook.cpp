#include "orderbook.h"
#include <iostream>
#include <cstdint>

void OrderBook::recordTrade(int buyId, int sellId, int price, int qty) {
    trades.push_back({buyId, sellId, price, qty, timeCounter++});
    // std::cout << "TRADE: buy#" << buyId << " x sell#" << sellId
    //           << " | price=" << price << " qty=" << qty << "\n";
}

void OrderBook::matchBuy(Order& incoming) {
    while (incoming.quantity > 0 && !asks.empty()) {
        auto bestAsk = asks.begin(); // lowest ask price
        if (bestAsk->first > incoming.price) break; // no cross possible

        auto& level = bestAsk->second;
        while (incoming.quantity > 0 && !level.empty()) {
            Order& resting = level.front();
            int fillQty = std::min(incoming.quantity, resting.quantity);

            recordTrade(incoming.id, resting.id, resting.price, fillQty);

            incoming.quantity -= fillQty;
            resting.quantity -= fillQty;

            if (resting.quantity == 0) {
                orderPriceLookup.erase(resting.id);
                orderSideLookup.erase(resting.id);
                level.pop_front();
            }
        }
        if (level.empty()) asks.erase(bestAsk);
    }
}

void OrderBook::matchSell(Order& incoming) {
    while (incoming.quantity > 0 && !bids.empty()) {
        auto bestBid = bids.begin(); // highest bid price
        if (bestBid->first < incoming.price) break; // no cross possible

        auto& level = bestBid->second;
        while (incoming.quantity > 0 && !level.empty()) {
            Order& resting = level.front();
            int fillQty = std::min(incoming.quantity, resting.quantity);

            recordTrade(resting.id, incoming.id, resting.price, fillQty);

            incoming.quantity -= fillQty;
            resting.quantity -= fillQty;

            if (resting.quantity == 0) {
                orderPriceLookup.erase(resting.id);
                orderSideLookup.erase(resting.id);
                level.pop_front();
            }
        }
        if (level.empty()) bids.erase(bestBid);
    }
}

void OrderBook::addLimitOrder(Order order) {
    order.timestamp = timeCounter++;

    if (order.isBuy) {
        matchBuy(order);
        if (order.quantity > 0) {
            bids[order.price].push_back(order);
            orderPriceLookup[order.id] = order.price;
            orderSideLookup[order.id] = true;
        }
    } else {
        matchSell(order);
        if (order.quantity > 0) {
            asks[order.price].push_back(order);
            orderPriceLookup[order.id] = order.price;
            orderSideLookup[order.id] = false;
        }
    }
}

void OrderBook::addMarketOrder(Order order) {
    order.timestamp = timeCounter++;
    // market order: cross at any price, so just set price to an extreme
    // and reuse the same matching logic
    if (order.isBuy) {
        order.price = INT32_MAX;
        matchBuy(order);
    } else {
        order.price = INT32_MIN;
        matchSell(order);
    }
    // leftover quantity for a market order is simply dropped (no resting)
}

void OrderBook::cancelOrder(int id) {
    auto priceIt = orderPriceLookup.find(id);
    if (priceIt == orderPriceLookup.end()) {
        // std::cout << "Cancel failed: order #" << id << " not found\n";
        return;
    }

    int price = priceIt->second;
    bool isBuy = orderSideLookup[id];
    if (isBuy) {
        auto levelIt = bids.find(price);
        if (levelIt != bids.end()) {
            auto& level = levelIt->second;
            for (auto it = level.begin(); it != level.end(); ++it) {
                if (it->id == id) {
                    level.erase(it);
                    break;
                }
            }
            if (level.empty()) bids.erase(levelIt);
        }
    } else {
        auto levelIt = asks.find(price);
        if (levelIt != asks.end()) {
            auto& level = levelIt->second;
            for (auto it = level.begin(); it != level.end(); ++it) {
                if (it->id == id) {
                    level.erase(it);
                    break;
                }
            }
            if (level.empty()) asks.erase(levelIt);
        }
    }

    orderPriceLookup.erase(id);
    orderSideLookup.erase(id);
    // std::cout << "Order #" << id << " cancelled\n";
}

void OrderBook::printBook() const {
    std::cout << "\n--- ASKS (low to high) ---\n";
    for (auto it = asks.rbegin(); it != asks.rend(); ++it) {
        int total = 0;
        for (const auto& o : it->second) total += o.quantity;
        std::cout << "  " << it->first << " : " << total << "\n";
    }
    std::cout << "--- BIDS (high to low) ---\n";
    for (const auto& entry : bids) {
        const int price = entry.first;
        const auto& level = entry.second;
        int total = 0;
        for (const auto& o : level) total += o.quantity;
        std::cout << "  " << price << " : " << total << "\n";
    }
    std::cout << "---------------------------\n\n";
}

const std::vector<Trade>& OrderBook::getTrades() const {
    return trades;
}
