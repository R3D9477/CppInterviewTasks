#include "OrderCache.hpp"

#include "gtest/gtest.h"

#include <sstream>

class OrderCacheTest : public ::testing::Test
{
protected:
    OrderCache m_orderCache;
    OrdersPriorityQueue<1000U> m_ordersQueue;

    std::size_t m_orderCounter = 0U;
    std::string getNextOrderId()
    {
        std::stringstream s;
        s << "OrdId" << (++m_orderCounter);
        return s.str();
    }
};

TEST_F(OrderCacheTest, OrdersPriorityQueue_addSortRemove)
{
    Order order1{"OrdId1", "SecId1", "Buy", 10U, "User1", "CompanyA"};
    Order order2{"OrdId2", "SecId1", "Buy", 100U, "User1", "CompanyA"};
    Order order3{"OrdId3", "SecId1", "Buy", 1U, "User1", "CompanyA"};

    ASSERT_EQ(true, m_ordersQueue.empty());

    m_ordersQueue.addOrder(order1);
    m_ordersQueue.addOrder(order2);
    m_ordersQueue.addOrder(order3);

    {
        ASSERT_EQ(false, m_ordersQueue.empty());
        ASSERT_EQ(3U, m_ordersQueue.size());

        const auto content = m_ordersQueue.getAll();

        auto itFromStorage1 = m_ordersQueue.cbegin();
        auto itFromStorage2 = std::next(itFromStorage1);
        auto itFromStorage3 = std::next(itFromStorage2);

        {
            const std::string orderId = "OrdId2";
            const auto &orderFromStorage = *itFromStorage1;
            ASSERT_EQ(orderFromStorage.orderId(), orderId);
            auto itFromCache = m_ordersQueue.getOrder(orderId);
            const auto &orderFromCache = *itFromCache;
            ASSERT_EQ(orderFromCache.orderId(), orderFromStorage.orderId());
        }

        {
            const std::string orderId = "OrdId1";
            const auto &orderFromStorage = *itFromStorage2;
            ASSERT_EQ(orderFromStorage.orderId(), orderId);
            auto itFromCache = m_ordersQueue.getOrder(orderId);
            const auto &orderFromCache = *itFromCache;
            ASSERT_EQ(orderFromCache.orderId(), orderFromStorage.orderId());
        }

        {
            const std::string orderId = "OrdId3";
            const auto &orderFromStorage = *itFromStorage3;
            ASSERT_EQ(orderFromStorage.orderId(), orderId);
            auto itFromCache = m_ordersQueue.getOrder(orderId);
            const auto &orderFromCache = *itFromCache;
            ASSERT_EQ(orderFromCache.orderId(), orderFromStorage.orderId());
        }
    }

    m_ordersQueue.cancelOrder("OrdId1");

    {
        ASSERT_EQ(false, m_ordersQueue.empty());
        ASSERT_EQ(2U, m_ordersQueue.size());

        const auto content = m_ordersQueue.getAll();

        auto itFromStorage1 = m_ordersQueue.cbegin();
        auto itFromStorage2 = std::next(itFromStorage1);

        {
            const std::string orderId = "OrdId2";
            const auto &orderFromStorage = *itFromStorage1;
            ASSERT_EQ(orderFromStorage.orderId(), orderId);
            auto itFromCache = m_ordersQueue.getOrder(orderId);
            const auto &orderFromCache = *itFromCache;
            ASSERT_EQ(orderFromCache.orderId(), orderFromStorage.orderId());
        }

        {
            const std::string orderId = "OrdId3";
            const auto &orderFromStorage = *itFromStorage2;
            ASSERT_EQ(orderFromStorage.orderId(), orderId);
            auto itFromCache = m_ordersQueue.getOrder(orderId);
            const auto &orderFromCache = *itFromCache;
            ASSERT_EQ(orderFromCache.orderId(), orderFromStorage.orderId());
        }
    }

    m_ordersQueue.cancelOrder("OrdId2");

    {
        ASSERT_EQ(false, m_ordersQueue.empty());
        ASSERT_EQ(1U, m_ordersQueue.size());

        const auto content = m_ordersQueue.getAll();

        auto itFromStorage1 = m_ordersQueue.cbegin();

        {
            const std::string orderId = "OrdId2";
            const auto &orderFromStorage = *itFromStorage1;
            ASSERT_EQ(orderFromStorage.orderId(), orderId);
            auto itFromCache = m_ordersQueue.getOrder(orderId);
            const auto &orderFromCache = *itFromCache;
            ASSERT_EQ(orderFromCache.orderId(), orderFromStorage.orderId());
        }
    }

    m_ordersQueue.cancelOrder("OrdId2");

    {
        ASSERT_EQ(true, m_ordersQueue.empty());
        ASSERT_EQ(0U, m_ordersQueue.size());

        const auto content = m_ordersQueue.getAll();
    }
}

/*TEST_F(OrderCacheTest, Buy_getMatchingSizeForSecurity_Zero_SingleSecurity)
{
    m_orderCache.clear();

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 1000U, "User1", "CompanyA"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 0U);
}

TEST_F(OrderCacheTest, Sell_getMatchingSizeForSecurity_Zero_SingleSecurity)
{
    m_orderCache.clear();

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 500U, "User2", "CompanyB"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 0U);
}

TEST_F(OrderCacheTest, BuySell_getMatchingSizeForSecurity_SingleSecurity_1)
{
    m_orderCache.clear();

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 1000U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 500U, "User2", "CompanyB"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 500U);
}

TEST_F(OrderCacheTest, BuySell_getMatchingSizeForSecurity_SingleSecurity_2)
{
    m_orderCache.clear();

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 100U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 200U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 300U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 400U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 500U, "User2", "CompanyB"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 600U, "User3", "CompanyC"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"),
              1000U); // B600[S400+S200], B500[S300+S100] => S400+S200+S300+S100 = 1000

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 700U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 50U, "User1", "CompanyA"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"),
              1100U); // S700[B600], B500[S400+S100] => B600+S400+S100 = 1100
}

TEST_F(OrderCacheTest, BuySell_CancelOrder_getMatchingSizeForSecurity_SingleSecurity)
{
    m_orderCache.clear();

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 100U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 200U, "User1", "CompanyA"});

    const auto furtherCancelOrderId1 = getNextOrderId();
    m_orderCache.addOrder({furtherCancelOrderId1, "SecId1", "Sell", 300U, "User1", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 400U, "User1", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 500U, "User2", "CompanyB"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 600U, "User3", "CompanyC"});

    m_orderCache.cancelOrder(furtherCancelOrderId1);

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"),
              700U); // B600[S400+S200], B500[S100] => S400+S200+S100 = 700
}

TEST_F(OrderCacheTest, BuySell_CancelOrderForUser_getMatchingSizeForSecurity_SingleSecurity)
{
    m_orderCache.clear();

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 100U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 200U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 300U, "User4", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 400U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 500U, "User2", "CompanyB"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 600U, "User3", "CompanyC"});

    m_orderCache.cancelOrdersForUser("User4");

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"),
              700U); // B600[S400+S200], B500[S100] => S400+S200+S100 = 700
}

TEST_F(OrderCacheTest, BuySell_CancelOrderMinQty_getMatchingSizeForSecurity_SingleSecurity)
{
    m_orderCache.clear();

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 100U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 200U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 350U, "User4", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 400U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 500U, "User2", "CompanyB"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 600U, "User3", "CompanyC"});

    m_orderCache.cancelOrdersForSecIdWithMinimumQty("SecId1", 200U);

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"),
              750U); // B600[S400], B500[S350] => S400+S350 = 750
}

TEST_F(OrderCacheTest, BuySell_getMatchingSizeForSecurity_Augmenta_1)
{
    m_orderCache.clear();

    m_orderCache.addOrder({"OrdId2", "SecId2", "Sell", 3000U, "User2", "CompanyB"});
    m_orderCache.addOrder({"OrdId4", "SecId2", "Buy", 600U, "User4", "CompanyC"});
    m_orderCache.addOrder({"OrdId5", "SecId2", "Buy", 100U, "User5", "CompanyB"});
    m_orderCache.addOrder({"OrdId7", "SecId2", "Buy", 2000U, "User7", "CompanyE"});
    m_orderCache.addOrder({"OrdId8", "SecId2", "Sell", 5000U, "User8", "CompanyE"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId2"), 2700U);
}

TEST_F(OrderCacheTest, BuySell_getMatchingSizeForSecurity_Augmenta_2)
{
    m_orderCache.clear();

    m_orderCache.addOrder({"OrdId1", "SecId1", "Buy", 1000U, "User1", "CompanyA"});
    m_orderCache.addOrder({"OrdId2", "SecId2", "Sell", 3000U, "User2", "CompanyB"});
    m_orderCache.addOrder({"OrdId3", "SecId1", "Sell", 500U, "User3", "CompanyA"});
    m_orderCache.addOrder({"OrdId4", "SecId2", "Buy", 600U, "User4", "CompanyC"});
    m_orderCache.addOrder({"OrdId5", "SecId2", "Buy", 100U, "User5", "CompanyB"});
    m_orderCache.addOrder({"OrdId6", "SecId3", "Buy", 1000U, "User6", "CompanyD"});
    m_orderCache.addOrder({"OrdId7", "SecId2", "Buy", 2000U, "User7", "CompanyE"});
    m_orderCache.addOrder({"OrdId8", "SecId2", "Sell", 5000U, "User8", "CompanyE"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 0U);
    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId2"), 2700U);
    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId3"), 0U);
}

TEST_F(OrderCacheTest, BuySell_getMatchingSizeForSecurity_Augmenta_3)
{
    m_orderCache.clear();

    m_orderCache.addOrder({"OrdId2", "SecId3", "Sell", 200U, "User8", "Company2"});
    m_orderCache.addOrder({"OrdId5", "SecId3", "Sell", 500U, "User7", "Company2"});
    m_orderCache.addOrder({"OrdId6", "SecId3", "Buy", 600U, "User3", "Company1"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId3"), 600U);
}

TEST_F(OrderCacheTest, BuySell_getMatchingSizeForSecurity_Augmenta_4)
{
    m_orderCache.clear();

    m_orderCache.addOrder({"OrdId1", "SecId1", "Sell", 100U, "User10", "Company2"});
    m_orderCache.addOrder({"OrdId2", "SecId3", "Sell", 200U, "User8", "Company2"});
    m_orderCache.addOrder({"OrdId3", "SecId1", "Buy", 300U, "User13", "Company2"});
    m_orderCache.addOrder({"OrdId4", "SecId2", "Sell", 400U, "User12", "Company2"});
    m_orderCache.addOrder({"OrdId5", "SecId3", "Sell", 500U, "User7", "Company2"});
    m_orderCache.addOrder({"OrdId6", "SecId3", "Buy", 600U, "User3", "Company1"});
    m_orderCache.addOrder({"OrdId7", "SecId1", "Sell", 700U, "User10", "Company2"});
    m_orderCache.addOrder({"OrdId8", "SecId1", "Sell", 800U, "User2", "Company1"});
    m_orderCache.addOrder({"OrdId9", "SecId2", "Buy", 900U, "User6", "Company2"});
    m_orderCache.addOrder({"OrdId10", "SecId2", "Sell", 1000U, "User5", "Company1"});
    m_orderCache.addOrder({"OrdId11", "SecId1", "Sell", 1100U, "User13", "Company2"});
    m_orderCache.addOrder({"OrdId12", "SecId2", "Buy", 1200U, "User9", "Company2"});
    m_orderCache.addOrder({"OrdId13", "SecId1", "Sell", 1300U, "User1", "Company1"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 300U);
    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId2"), 1000U);
    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId3"), 600U);
}

TEST_F(OrderCacheTest, BuySell_getMatchingSizeForSecurity_Augmenta_5)
{
    m_orderCache.clear();

    m_orderCache.addOrder({"OrdId1", "SecId3", "Sell", 100U, "User1", "Company1"});
    m_orderCache.addOrder({"OrdId2", "SecId3", "Sell", 200U, "User3", "Company2"});
    m_orderCache.addOrder({"OrdId3", "SecId1", "Buy", 300U, "User2", "Company1"});
    m_orderCache.addOrder({"OrdId4", "SecId3", "Sell", 400U, "User5", "Company2"});
    m_orderCache.addOrder({"OrdId5", "SecId2", "Sell", 500U, "User2", "Company1"});
    m_orderCache.addOrder({"OrdId6", "SecId2", "Buy", 600U, "User3", "Company2"});
    m_orderCache.addOrder({"OrdId7", "SecId2", "Sell", 700U, "User1", "Company1"});
    m_orderCache.addOrder({"OrdId8", "SecId1", "Sell", 800U, "User2", "Company1"});
    m_orderCache.addOrder({"OrdId9", "SecId1", "Buy", 900U, "User5", "Company2"});
    m_orderCache.addOrder({"OrdId10", "SecId1", "Sell", 1000U, "User1", "Company1"});
    m_orderCache.addOrder({"OrdId11", "SecId2", "Sell", 1100U, "User6", "Company2"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 900U);
    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId2"), 600U);
    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId3"), 0U);
}
*/
