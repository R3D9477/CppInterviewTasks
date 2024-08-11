#pragma once

#include "Order.hpp"

#include <algorithm>
#include <functional>
#include <list>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include <iostream>

using OrderPrirityContainerBase_t = std::vector<Order>;
using OrderPrirityCache_t = std::unordered_map<std::string, OrderPrirityContainerBase_t::iterator>;

struct OrdersContainer_t
{
    using value_type = Order;
    using reference = Order &;
    using const_reference = const Order &;
    using size_type = std::size_t;

    void push_back(Order order)
    {
        const bool reallocation = size() >= capacity() - 1;

        m_InternalContianer.push_back(order);

        if (reallocation)
        {
            for (auto itOrder = begin(); itOrder != end(); ++itOrder)
                m_OrdersCache[itOrder->orderId()] = itOrder;
        }
        else
            m_OrdersCache[order.orderId()] = std::prev(m_InternalContianer.end());
    }

    OrderPrirityContainerBase_t::iterator begin()
    {
        return m_InternalContianer.begin();
    }

    OrderPrirityContainerBase_t::iterator end()
    {
        return m_InternalContianer.end();
    }

    const Order &front() const
    {
        return m_InternalContianer.front();
    }

    const Order &back() const
    {
        return m_InternalContianer.back();
    }

    bool empty() const
    {
        return m_InternalContianer.empty();
    }

    void pop_back()
    {
        if (!m_InternalContianer.empty())
        {
            auto itOrder = m_OrdersCache.find(m_InternalContianer.back().orderId());
            if (itOrder != m_OrdersCache.end())
                m_OrdersCache.erase(itOrder);
            m_InternalContianer.pop_back();
        }
    }

    void reserve(const std::size_t size)
    {
        m_InternalContianer.reserve(size);
        m_OrdersCache.reserve(size);
    }

    std::size_t capacity() const
    {
        return m_InternalContianer.capacity();
    }

    std::size_t size() const
    {
        return m_InternalContianer.size();
    }

    OrderPrirityCache_t::iterator cacheBegin()
    {
        return m_OrdersCache.begin();
    }

    OrderPrirityCache_t::iterator cacheFind(const std::string &orderId)
    {
        return m_OrdersCache.find(orderId);
    }

    OrderPrirityCache_t::iterator cacheEnd()
    {
        return m_OrdersCache.end();
    }

    bool erase(OrderPrirityContainerBase_t::iterator itOrder)
    {
        if (itOrder != m_InternalContianer.end())
        {
            m_OrdersCache.erase(m_OrdersCache.find(itOrder->orderId()));
            m_InternalContianer.erase(itOrder);
            return true;
        }
        return false;
    }

    bool erase(OrderPrirityCache_t::iterator itCache)
    {
        if (itCache != m_OrdersCache.end())
        {
            std::cout << "erase: " << itCache->second->orderId() << " / ";
            m_InternalContianer.erase(itCache->second);

            std::cout << itCache->first << std::endl;
            m_OrdersCache.erase(itCache);

            return true;
        }
        return false;
    }

private:
    OrderPrirityContainerBase_t m_InternalContianer;
    OrderPrirityCache_t m_OrdersCache;
};

static bool compareOrders(const Order &a, const Order &b)
{
    return a.qty() < b.qty();
}
using OrdersComparer_t = std::function<bool(const Order &, const Order &)>;

using OrdersPriorityQueueBase_t = std::priority_queue<Order, OrdersContainer_t, OrdersComparer_t>;

template <std::size_t DefaultReservedSize> struct OrdersPriorityQueue : private OrdersPriorityQueueBase_t
{
    OrdersPriorityQueue() : OrdersPriorityQueueBase_t{&compareOrders}
    {
        reserve(DefaultReservedSize);
    }

    void reserve(const std::size_t size)
    {
        c.reserve(size);
    }

    std::size_t size() const
    {
        return c.size();
    }

    bool empty() const
    {
        return static_cast<const OrdersPriorityQueueBase_t &>(*this).empty();
    }

    Order top()
    {
        return static_cast<OrdersPriorityQueueBase_t &>(*this).top();
    }

    void pop()
    {
        static_cast<OrdersPriorityQueueBase_t &>(*this).pop();
    }

    void addOrder(Order &order)
    {
        push(order);
    }

    void cancelOrder(const std::string &orderId)
    {
        this->c.erase(this->c.cacheFind(orderId));
        //    std::make_heap(this->c.begin(), this->c.end(), compareOrders);
    }

    OrderPrirityContainerBase_t::iterator begin()
    {
        return c.begin();
    }

    OrderPrirityContainerBase_t::iterator end()
    {
        return c.end();
    }

    OrderPrirityCache_t::iterator cacheBegin()
    {
        return c.cacheBegin();
    }
    OrderPrirityCache_t::iterator cacheEnd()
    {
        return c.cacheEnd();
    }
};
