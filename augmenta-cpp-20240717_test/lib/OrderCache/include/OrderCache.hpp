#pragma once

#include "OrderCacheInterface.hpp"

#include "OrdersMatching.hpp"
#include "OrdersPriorityQueue.hpp"

#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Todo: Your implementation of the OrderCache...
class OrderCache : public OrderCacheInterface
{
public:
    static constexpr std::size_t OrdersCacheReserverSize = 1000000U;      // Predicted amount of operations
    static constexpr std::size_t UserOrdersCacheReserverSize = 50000U;    // Predicted amount of users
    static constexpr std::size_t OrdersPerUserCacheReserverSize = 10000U; // Predicted amount of orders of each user
    static constexpr std::size_t SecuritiesCacheReserverSize = 10000U;    // Predicted amount of orders per security

public:
    OrderCache()
    {
        m_ordersStorage.reserve(OrdersCacheReserverSize);
        m_ordersCache.reserve(OrdersCacheReserverSize);
        m_userOrdersCache.reserve(UserOrdersCacheReserverSize);
        m_securitiesCache.reserve(SecuritiesCacheReserverSize);
    }

    void clear()
    {
        m_securitiesCache.clear();
        m_userOrdersCache.clear();
        m_ordersCache.clear();
        while (!m_ordersStorageFreeIterators.empty())
            m_ordersStorageFreeIterators.pop();
        m_ordersStorage.clear();
    }

    void addOrder(Order order) override;

    void cancelOrder(const std::string &orderId) override;

    void cancelOrdersForUser(const std::string &user) override;

    void cancelOrdersForSecIdWithMinimumQty(const std::string &securityId, unsigned int minQty) override;

    unsigned int getMatchingSizeForSecurity(const std::string &securityId) override;

    std::vector<Order> getAllOrders() const override;

private:
    // orders registry
    std::vector<Order> m_ordersStorage;
    // table [ Order ID - Object ]
    std::unordered_map<std::string, decltype(m_ordersStorage)::iterator> m_ordersCache;
    // free items in orders registry
    std::stack<decltype(m_ordersStorage)::iterator> m_ordersStorageFreeIterators;
    // table [ User ID - Order IDs ]
    std::unordered_map<std::string, std::unordered_set<std::string>> m_userOrdersCache;
    // table [ Security ID - Orders Chain ]
    std::unordered_map<std::string, OrdersPriorityQueue<SecuritiesCacheReserverSize>> m_securitiesCache;
};
