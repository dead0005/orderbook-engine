#ifndef TRADE_H
#define TRADE_H

struct Trade {
    int buyOrderId;
    int sellOrderId;
    int price;
    int quantity;
    long timestamp;
};

#endif