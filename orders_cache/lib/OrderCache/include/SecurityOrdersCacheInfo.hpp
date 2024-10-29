#pragma once

#include "Order.hpp"

#include "CacheConfig.hpp"

#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

struct SecurityOrdersCacheInfo
{
    std::unordered_set<std::string> Orders;

    SecurityOrdersCacheInfo()
    {
        Orders.reserve(SecuritiesCacheReserverSize);
    }

    void addOrder(const Order &order)
    {
        const auto &orderId = order.orderId();
        Orders.insert(orderId);
    }

    void cancelOrder(const std::string &orderId)
    {
        auto itSecurityOrder = Orders.find(orderId);
        if (itSecurityOrder != Orders.end())
        {
            Orders.erase(itSecurityOrder);
        }
    }
};
