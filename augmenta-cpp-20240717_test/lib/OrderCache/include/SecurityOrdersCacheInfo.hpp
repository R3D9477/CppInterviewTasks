#pragma once

#include "Order.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

template <std::size_t ReservedSize, typename OrderID_t = std::string> struct SecurityOrdersCacheInfo
{
    std::vector<OrderID_t> Orders;
    std::unordered_map<OrderID_t, typename decltype(Orders)::iterator> OrdersCache;

    SecurityOrdersCacheInfo()
    {
        Orders.reserve(ReservedSize);
        OrdersCache.reserve(ReservedSize);
    }

    void addOrder(const Order &order)
    {
        const auto &orderId = order.orderId();
        Orders.push_back(orderId);
        OrdersCache[orderId] = std::prev(Orders.end());
    }

    void cancelOrder(const OrderID_t &orderId)
    {
        auto itSecurityOrder = OrdersCache.find(orderId);
        if (itSecurityOrder != OrdersCache.end())
        {
            Orders.erase(itSecurityOrder->second);
        }
    }
};
