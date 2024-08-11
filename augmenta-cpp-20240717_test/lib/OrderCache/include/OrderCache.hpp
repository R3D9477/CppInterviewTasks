#pragma once

#include "OrderCacheInterface.hpp"

#include "CacheConfig.hpp"
#include "SecurityOrdersCacheInfo.hpp"
#include "SecurityOrdersMatching.hpp"

#include <list>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Todo: Your implementation of the OrderCache...
class OrderCache : public OrderCacheInterface
{
public:
    OrderCache()
    {
        m_ordersCache.reserve(OrdersCacheReserverSize);
        m_userOrdersCache.reserve(UserOrdersCacheReserverSize);
        m_securitiesCache.reserve(SecuritiesCacheReserverSize);
    }

    void clear()
    {
        m_securitiesCache.clear();
        m_userOrdersCache.clear();
        m_ordersCache.clear();
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
    std::list<Order> m_ordersStorage;
    // table [ Order ID - Object ]
    std::unordered_map<std::string, decltype(m_ordersStorage)::iterator> m_ordersCache;

    // table [ User ID - Order IDs ]
    std::unordered_map<std::string, std::unordered_set<std::string>> m_userOrdersCache;
    // table [ Security ID - Security Order IDs ]
    std::unordered_map<std::string, SecurityOrdersCacheInfo> m_securitiesCache;
};
