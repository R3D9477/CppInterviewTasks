#include "OrderCache.hpp"

#include "gtest/gtest.h"

#include <sstream>

class OrderCacheTest_Loading : public ::testing::Test
{
protected:
    OrderCache m_orderCache;

    std::size_t m_orderCounter = 0U;
    std::string getNextOrderId()
    {
        std::stringstream s;
        s << "OrdId" << (++m_orderCounter);
        return s.str();
    }
};

TEST_F(OrderCacheTest_Loading, BuySell_getMatchingSizeForSecurity_Augmenta_5000)
{
    m_orderCache.clear();

    for (std::size_t i = 0U; i < 5000U; ++i)
    {
        m_orderCache.addOrder({getNextOrderId(), "SecId3", "Sell", 100U, "User1", "Company1"});
        m_orderCache.addOrder({getNextOrderId(), "SecId3", "Sell", 200U, "User3", "Company2"});
        m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 300U, "User2", "Company1"});
        m_orderCache.addOrder({getNextOrderId(), "SecId3", "Sell", 400U, "User5", "Company2"});
        m_orderCache.addOrder({getNextOrderId(), "SecId2", "Sell", 500U, "User2", "Company1"});
        m_orderCache.addOrder({getNextOrderId(), "SecId2", "Buy", 600U, "User3", "Company2"});
        m_orderCache.addOrder({getNextOrderId(), "SecId2", "Sell", 700U, "User1", "Company1"});
        m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 800U, "User2", "Company1"});
        m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 900U, "User5", "Company2"});
        m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 1000U, "User1", "Company1"});
        m_orderCache.addOrder({getNextOrderId(), "SecId2", "Sell", 1100U, "User6", "Company2"});
    }

    const auto matched_secid1 = m_orderCache.getMatchingSizeForSecurity("SecId1");
    ASSERT_EQ(matched_secid1, 3750000U);

    const auto matched_secid2 = m_orderCache.getMatchingSizeForSecurity("SecId2");
    ASSERT_EQ(matched_secid2, 2625000U);

    const auto matched_secid3 = m_orderCache.getMatchingSizeForSecurity("SecId3");
    ASSERT_EQ(matched_secid3, 0U);
}
