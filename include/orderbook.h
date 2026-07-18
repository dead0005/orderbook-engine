#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include <map>
#include <deque>
#include <unordered_map>
#include <vector>
#include "order.h"
#include "trade.h"

class OrderBook {
public:
    void addLimitOrder(Order order);
    void addMarketOrder(Order order);
    void cancelOrder(int id);
    void printBook() const;
    const std::vector<Trade>& getTrades() const;

private:
    // bids: highest price first
    std::map<int, std::deque<Order>, std::greater<int>> bids;
    // asks: lowest price first
    std::map<int, std::deque<Order>> asks;

    // id -> price, so cancelOrder doesn't need to scan the whole book
    std::unordered_map<int, int> orderPriceLookup;
    std::unordered_map<int, bool> orderSideLookup; // id -> isBuy

    std::vector<Trade> trades;
    long timeCounter = 0;

    void matchBuy(Order& incoming);
    void matchSell(Order& incoming);
    void recordTrade(int buyId, int sellId, int price, int qty);
};

#endif
