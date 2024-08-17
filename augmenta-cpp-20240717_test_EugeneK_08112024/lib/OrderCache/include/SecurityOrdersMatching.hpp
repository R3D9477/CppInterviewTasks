#pragma once

#include "Order.hpp"

#include <algorithm>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
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
    static unsigned int getMatchings(
        const std::unordered_map<std::string, std::list<Order>::iterator> &ordersStorageCache,
        const std::unordered_set<std::string> &securityOrdersCache);
};
