#include "SecurityOrdersMatching.hpp"

unsigned int MatchingUtils::getMatchings(
    const std::unordered_map<std::string, std::list<Order>::iterator> &ordersStorageCache,
    const std::unordered_set<std::string> &securityOrdersCache)
{
    std::vector<std::string> securityOrders(securityOrdersCache.begin(), securityOrdersCache.end());

    if (securityOrders.size() < 2U || ordersStorageCache.size() < securityOrders.size())
        return 0U;

    const auto sOrdersCmp = [&](const std::string &lOrderId, const std::string &rOrderId) {
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

    // while (addedNewRemainOrder)
    //{
    addedNewRemainOrder = false;
    auto itRemainOrderEnd = remainOrders.end();
    for (auto itRemainOrder = remainOrders.begin(); itRemainOrder != itRemainOrderEnd; ++itRemainOrder)
        if (match(*itRemainOrder))
            addedNewRemainOrder = true;
    //}
    static_cast<void>(addedNewRemainOrder);

    unsigned int totalMatchingsAmount{0U};
    for (const auto &matching : foundMatchings)
        totalMatchingsAmount += matching.getMatched();

    return totalMatchingsAmount;
}
