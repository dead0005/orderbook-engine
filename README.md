# Order Book Matching Engine

> *Status: In Progress** — core matching logic works, but this is a learning project and not production-grade. See [Roadmap](#roadmap) for what's left.

A simplified limit order book, written in C++, that matches buy and sell orders using price-time priority — similar in spirit to the matching engines used by exchanges, minus the networking, concurrency, and scale.

## What it does

- Maintains a live order book with separate bid (buy) and ask (sell) sides
- Supports three order types:
  - **Limit orders** — rest in the book until matched or cancelled
  - **Market orders** — fill immediately against the best available price
  - **Cancel** — removes a resting order by ID
- Matches orders using **price-time priority**: best price first, and within the same price, earliest order first (FIFO)
- Reads a sequence of orders from a CSV file and simulates them end-to-end, printing trade fills and book state as it runs

## What it doesn't do (yet)

- No concurrency — everything runs single-threaded and synchronously
- No networking — orders come from a static CSV, not a live feed
- No persistence — book state is in-memory only, resets on restart
- No self-trade prevention
- Cancel is O(log n) to find the price level, but still a linear scan within that level to find the specific order

## How it's built

| Component | File(s) | Purpose |
|---|---|---|
| `Order` | `include/Order.h` | Plain struct representing a single order |
| `Trade` | `include/Trade.h` | Plain struct representing a completed match |
| `OrderBook` | `include/OrderBook.h`, `src/OrderBook.cpp` | Core engine: insert, match, cancel |
| `main.cpp` | `src/main.cpp` | Reads `data/orders.csv` and drives the simulation |

**Data structures:**
- `std::map<price, deque<Order>>` for each side (bids sorted highest-first, asks lowest-first) — gives sorted access to the best price in O(log n)
- `std::deque<Order>` per price level for FIFO time priority
- `std::unordered_map<id, price>` side table for fast cancel lookups

## Getting started

```bash
mkdir build && cd build
cmake ..
make
./orderbook
```

Orders are read from `data/orders.csv`. Format:

```
id,side,type,price,quantity
1,B,LIMIT,100,10
2,S,MARKET,0,4
```

## Roadmap

- [ ] Add `CANCEL` as a row type in the CSV so cancels can be scripted too
- [ ] Store deque iterators alongside price in the cancel lookup table to make cancel fully O(log n)
- [ ] Add unit tests for partial fills, exact fills, empty book, and multi-level crossing
- [ ] Add a simple order book depth printout (top N levels per side)
- [ ] Explore self-trade prevention

## Why this project

Built to understand how matching engines work under the hood — specifically the data structure tradeoffs (why sorted maps over hash maps, why deques for time priority) rather than to build something exchange-grade.
