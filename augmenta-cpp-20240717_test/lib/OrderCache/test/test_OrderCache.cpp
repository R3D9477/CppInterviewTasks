#include "OrderCache.hpp"

#include "gtest/gtest.h"

#include <sstream>

class OrderCacheTest : public ::testing::Test
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

TEST_F(OrderCacheTest, BuySell_getMatchingSizeForSecurity_SingleSecurity)
{
    m_orderCache.clear();

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 1000U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 500U, "User2", "CompanyB"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 500U);
}

TEST_F(OrderCacheTest, BuySell_getMatchingSizeForSecurity_NonZero_SingleSecurity)
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
}*/

TEST_F(OrderCacheTest, BuySell_CancelOrder_getMatchingSizeForSecurity_NonZero_SingleSecurity)
{
    m_orderCache.clear();

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 100U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 200U, "User1", "CompanyA"});

    const auto furtherCancelOrderId1 = getNextOrderId();
    m_orderCache.addOrder({furtherCancelOrderId1, "SecId1", "Sell", 300U, "User1", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 400U, "User1", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 500U, "User2", "CompanyB"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 600U, "User3", "CompanyC"});

    // ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"),
    //           1000U); // B600[S400+S200], B500[S300+S100] => S400+S200+S300+S100 = 1000

    m_orderCache.cancelOrder(furtherCancelOrderId1);

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"),
              500U); // B600[S400+S200], B500[S100] => S400+S200+S100 = 500
}

/*TEST_F(OrderCacheTest, BuySell_CancelOrder_getMatchingSizeForSecurity_SingleSecurity)
{
    m_orderCache.clear();

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 100U, "User1", "CompanyA"});

    const auto furtherCancelOrderId1 = getNextOrderId();
    m_orderCache.addOrder({furtherCancelOrderId1, "SecId1", "Sell", 200U, "User1", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 300U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 400U, "User1", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 500U, "User2", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 600U, "User3", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 700U, "User3", "CompanyA"});

    const auto furtherCancelOrderId2 = getNextOrderId();
    m_orderCache.addOrder({furtherCancelOrderId2, "SecId1", "Sell", 50U, "User2", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 60U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 70U, "User1", "CompanyA"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 380U);

    m_orderCache.cancelOrder(furtherCancelOrderId1);
    m_orderCache.cancelOrder(furtherCancelOrderId2);

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 130U);

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 5U, "User1", "CompanyA"});

    const auto furtherCancelOrderId3 = getNextOrderId();
    m_orderCache.addOrder({furtherCancelOrderId3, "SecId1", "Sell", 5U, "User1", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 5U, "User1", "CompanyA"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 145U);

    m_orderCache.cancelOrder(furtherCancelOrderId3);

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 140U);
}

TEST_F(OrderCacheTest, BuySell_CancelOrderForUser_getMatchingSizeForSecurity_SingleSecurity)
{
    m_orderCache.clear();

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 100U, "User1", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 200U, "User1", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 300U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 400U, "User1", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 500U, "User2", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 600U, "User3", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 700U, "User3", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 50U, "User2", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 1000U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 2000U, "User1", "CompanyA"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 3250U);

    m_orderCache.cancelOrdersForUser("User2");
    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 2700U);

    m_orderCache.cancelOrdersForUser("User3");
    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 4000U);
}

TEST_F(OrderCacheTest, BuySell_CancelOrderMinQty_getMatchingSizeForSecurity_SingleSecurity)
{
    m_orderCache.clear();

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 100U, "User1", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 200U, "User1", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 300U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 400U, "User1", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 500U, "User2", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 600U, "User3", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Buy", 700U, "User3", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 50U, "User2", "CompanyA"});

    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 1000U, "User1", "CompanyA"});
    m_orderCache.addOrder({getNextOrderId(), "SecId1", "Sell", 2000U, "User1", "CompanyA"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 3250U);

    m_orderCache.cancelOrdersForSecIdWithMinimumQty("SecId1", 500U);

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId1"), 1700U);
}
*/

TEST_F(OrderCacheTest, BuySell_getMatchingSizeForSecurity_Augmenta1_SecId2)
{
    m_orderCache.clear();

    m_orderCache.addOrder({"OrdId2", "SecId2", "Sell", 3000U, "User2", "CompanyB"});
    m_orderCache.addOrder({"OrdId4", "SecId2", "Buy", 600U, "User4", "CompanyC"});
    m_orderCache.addOrder({"OrdId5", "SecId2", "Buy", 100U, "User5", "CompanyB"});
    m_orderCache.addOrder({"OrdId7", "SecId2", "Buy", 2000U, "User7", "CompanyE"});
    m_orderCache.addOrder({"OrdId8", "SecId2", "Sell", 5000U, "User8", "CompanyE"});

    ASSERT_EQ(m_orderCache.getMatchingSizeForSecurity("SecId2"), 2700U);

    /*
    - SecId2
      - OrdId2 matches quantity 600 against OrdId4
      - OrdId2 matches quantity 2000 against OrdId7
      - OrdId2 has a total matched quantity of 2600

      - OrdId8 matches quantity 100 against OrdId5 only
      - OrdId8 has a remaining qty of 4900

      - OrdId4 had its quantity fully allocated to match OrdId2

      - No remaining qty on OrdId4 for the remaining 4900 of OrdId8
      - Total quantity matched for SecId2 is 2700. (2600 + 100)

      - Note: there are other combinations of matches among the orders which
        would lead to the same result of 2700 total qty matching
    */
}

/*TEST_F(OrderCacheTest, BuySell_getMatchingSizeForSecurity_Augmenta1_All)
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
}*/
