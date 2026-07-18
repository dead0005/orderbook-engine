#ifndef ORDER_H
#define ORDER_H

struct Order {
    int id;
    bool isBuy;      // true = buy, false = sell
    int price;       // integer ticks, e.g. price in paise/cents
    int quantity;
    long timestamp;  // just an incrementing counter, not real clock time
};

#endif