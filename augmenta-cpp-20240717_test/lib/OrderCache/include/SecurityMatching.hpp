#pragma once

#include "Order.hpp"

#include <algorithm>
#include <vector>

struct MatchingInfo
{
    MatchingInfo(const Order &initOrder) : m_order{initOrder}, m_remain{initOrder.qty()}, m_matched{0U}
    {
    }

    bool isMatching(const Order &oppOrder)
    {
        return m_remain > 0 && m_order.side() != oppOrder.side() && m_order.user() != oppOrder.user() &&
               m_order.company() != oppOrder.company();
    }

    unsigned int applyWithRemainQty(const Order &oppOrder)
    {
        unsigned int remainQty = 0U;
        const auto qty = oppOrder.qty();
        if (m_remain < qty)
        {
            remainQty = qty - m_remain;
            m_matched += m_remain;
            m_remain = 0U;
        }
        else
        {
            m_remain -= qty;
            m_matched += qty;
        }
        return remainQty;
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
    template <typename TOrdersStorage, typename TSecurityOrders, typename OrderID_t = std::string>
    static unsigned int getMatchings(const TOrdersStorage &ordersStorageCache, TSecurityOrders securityOrders)
    {
        if (securityOrders.size() < 2U || ordersStorageCache.size() < securityOrders.size())
            return 0U;

        const auto sOrdersCmp = [&](const OrderID_t &lOrderId, const OrderID_t &rOrderId) {
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

        std::sort(securityOrders.begin(), securityOrders.end(), sOrdersCmp);

        std::vector<MatchingInfo> foundMatchings;
        std::vector<Order> remainOrders;

        auto match = [&](const Order &sOrder) {
            bool addedNewRemainOrder = false;
            bool isMatched = false;
            for (auto itMatch = foundMatchings.begin(); itMatch != foundMatchings.end() && !isMatched; ++itMatch)
            {
                if (itMatch->isMatching(sOrder))
                {
                    const auto remainQty = itMatch->applyWithRemainQty(sOrder);
                    if (remainQty > 0U)
                    {
                        remainOrders.push_back({sOrder.orderId(), sOrder.securityId(), sOrder.side(), remainQty,
                                                sOrder.user(), sOrder.company()});
                        addedNewRemainOrder = true;
                    }
                    isMatched = true;
                }
            }
            if (!isMatched)
            {
                foundMatchings.push_back({sOrder});
            }
            return addedNewRemainOrder;
        };

        bool addedNewRemainOrder = false;
        for (auto itSOrder = securityOrders.begin(); itSOrder != securityOrders.end(); ++itSOrder)
        {
            const auto itCache_SOrder = ordersStorageCache.find(*itSOrder);
            if (itCache_SOrder == ordersStorageCache.end())
            {
                continue;
            }
            const Order &sOrder = *(itCache_SOrder->second);
            if (match(sOrder))
                addedNewRemainOrder = true;
        }

        while (addedNewRemainOrder)
        {
            addedNewRemainOrder = false;
            auto itRemainOrderEnd = remainOrders.end();
            for (auto itRemainOrder = remainOrders.begin(); itRemainOrder != itRemainOrderEnd; ++itRemainOrder)
                if (match(*itRemainOrder))
                    addedNewRemainOrder = true;
        }

        unsigned int totalMatchingsAmount{0U};
        for (const auto &matching : foundMatchings)
            totalMatchingsAmount += matching.getMatched();

        return totalMatchingsAmount;
    }
};
