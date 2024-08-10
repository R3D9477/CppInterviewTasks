#pragma once

#include "Order.hpp"

#include <vector>

struct MatchingInfo
{
    MatchingInfo(const Order &initOrder) : m_order{initOrder}, m_remain{initOrder.qty()}, m_matched{0U}
    {
    }

    bool apply(const Order &oppOrder)
    {
        if (m_order.side() != oppOrder.side())
        {
            if (oppOrder.qty() <= m_remain)
            {
                m_remain -= oppOrder.qty();
                m_matched += oppOrder.qty();
                return true;
            }
        }
        return false;
    }

    unsigned int getRemain() const
    {
        return m_remain;
    }

    unsigned int getMatched() const
    {
        return m_matched;
    }

private:
    const Order &m_order;
    unsigned int m_remain;
    unsigned int m_matched;
};

struct MatchingUtils
{
    template <typename TOrdersStorage, typename TSecurityOrders>
    static unsigned int getMatchings(const TOrdersStorage &ordersStorageCache, TSecurityOrders securityOrders)
    {
        if (securityOrders.size() < 2U || ordersStorageCache.size() < securityOrders.size())
            return 0U;

        unsigned int totalMatchingsAmount{0U};
        std::vector<MatchingInfo> foundMatchings;

        for (auto itSOrder = securityOrders.begin(); itSOrder != securityOrders.end(); ++itSOrder)
        {
            const auto itCache_SOrder = ordersStorageCache.find(*itSOrder);
            if (itCache_SOrder == ordersStorageCache.end())
            {
                continue;
            }

            foundMatchings.push_back({*(itCache_SOrder->second)});
            auto &lastMatch = foundMatchings.back();

            for (auto itSOrderOpp = std::next(itSOrder);
                 itSOrderOpp != securityOrders.end() && lastMatch.getRemain() > 0U;)
            {
                const auto itCache_SOrderOpp = ordersStorageCache.find(*itSOrderOpp);
                if (itCache_SOrderOpp == ordersStorageCache.end())
                {
                    ++itSOrderOpp;
                    continue;
                }

                if (lastMatch.apply(*(itCache_SOrderOpp->second)))
                {
                    itSOrderOpp = securityOrders.erase(itSOrderOpp);
                }
                else
                    ++itSOrderOpp;
            }

            totalMatchingsAmount += lastMatch.getMatched();
        }

        return totalMatchingsAmount;
    }
};
