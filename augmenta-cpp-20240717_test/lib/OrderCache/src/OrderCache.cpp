// Todo: your implementation of the OrderCache...
#include "OrderCache.hpp"

#include <cstring>

void OrderCache::addOrder(Order order)
{
    const auto orderId = order.orderId();

    if (m_ordersCache.find(orderId) != m_ordersCache.end())
    {
        return;
    }

    auto itOrder = m_ordersStorage.end();
    if (m_ordersStorageFreeIterators.empty())
    {
        m_ordersStorage.push_back(order);
        itOrder = std::prev(m_ordersStorage.end());
    }
    else
    {
        itOrder = m_ordersStorageFreeIterators.top();
        m_ordersStorageFreeIterators.pop();

        *itOrder = order;
    }
    m_ordersCache[orderId] = itOrder;

    auto &securityCache = m_securitiesCache[order.securityId()];
    securityCache.addOrder(order);

    const auto usedId = order.user();
    if (m_userOrdersCache.find(usedId) == m_userOrdersCache.end())
        m_userOrdersCache[usedId].reserve(OrdersPerUserCacheReserverSize);
    m_userOrdersCache[usedId].insert(orderId);
}

void OrderCache::cancelOrder(const std::string &orderId)
{
    auto itOrder = m_ordersCache.find(orderId);
    if (itOrder != m_ordersCache.end())
    {
        const auto &cancelledOrder = *(itOrder->second);

        auto itSecurity = m_securitiesCache.find(cancelledOrder.securityId());
        if (itSecurity != m_securitiesCache.end())
        {
            auto &securityCache = itSecurity->second;
            securityCache.cancelOrder(orderId);
        }

        m_userOrdersCache[cancelledOrder.user()].erase(orderId);
        m_ordersStorageFreeIterators.push(itOrder->second);
        m_ordersCache.erase(itOrder);
    }
}

void OrderCache::cancelOrdersForUser(const std::string &user)
{
    auto itUser = m_userOrdersCache.find(user);
    if (itUser != m_userOrdersCache.end())
    {
        const auto &ordersOfUser = itUser->second;

        std::vector<std::string> ordersToCancel;
        ordersToCancel.reserve(ordersOfUser.size());

        for (const auto &orderId : ordersOfUser)
            ordersToCancel.push_back(orderId);

        for (const auto &orderId : ordersToCancel)
            cancelOrder(orderId);

        m_userOrdersCache.erase(itUser);
    }
}

void OrderCache::cancelOrdersForSecIdWithMinimumQty(const std::string &securityId, unsigned int minQty)
{
    auto itSecurity = m_securitiesCache.find(securityId);
    if (itSecurity != m_securitiesCache.end())
    {
        const auto &securityCache = itSecurity->second;

        std::unordered_set<std::string> ordersToCancel;
        ordersToCancel.reserve(securityCache.Orders.size());

        for (const auto &orderId : securityCache.Orders)
        {
            if (m_ordersCache[orderId]->qty() <= minQty)
                ordersToCancel.insert(orderId);
        }

        for (const auto &orderId : ordersToCancel)
            cancelOrder(orderId);
    }
}

unsigned int OrderCache::getMatchingSizeForSecurity(const std::string &securityId)
{
    const auto itSecurity = m_securitiesCache.find(securityId);
    if (itSecurity != m_securitiesCache.end())
    {
        auto &securityCache = itSecurity->second;

        return MatchingUtils::getMatchings<decltype(m_ordersCache), decltype(securityCache.Orders)>(
            m_ordersCache, securityCache.Orders);
    }

    return 0U;
}

std::vector<Order> OrderCache::getAllOrders() const
{
    return m_ordersStorage;
}
