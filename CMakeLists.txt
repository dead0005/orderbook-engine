cmake_minimum_required(VERSION 3.10)
project(OrderBookEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(orderbook
    src/main.cpp
    src/orderbook.cpp
)

target_include_directories(orderbook PRIVATE include)
