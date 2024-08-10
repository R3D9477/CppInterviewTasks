#pragma once

#include "Order.hpp"

#include <algorithm>
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

    template <typename TOrdersStorageCache> void sortOrdersByQty(const TOrdersStorageCache &ordersStorageCache)
    {
        const auto sortOrders = [&](const OrderID_t &lOrderId, const OrderID_t &rOrderId) {
            const auto itLOrder = ordersStorageCache.find(lOrderId);
            if (itLOrder != ordersStorageCache.end())
            {
                const auto itROrder = ordersStorageCache.find(rOrderId);
                if (itROrder != ordersStorageCache.end())
                {
                    return itLOrder->second->qty() > itROrder->second->qty();
                }
            }
            return false;
        };

        std::sort(Orders.begin(), Orders.end(), sortOrders);
    }
};
