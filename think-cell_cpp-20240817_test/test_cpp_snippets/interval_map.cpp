/*

Task Description
interval_map<K,V> is a data structure that associates keys of type K with values of type V. It is designed to be used
efficiently in situations where intervals of consecutive keys are associated with the same value. Your task is to
implement the assign member function of this data structure, which is outlined below.

interval_map<K, V> is implemented on top of std::map. For more information on std::map, you may refer to
cppreference.com.

Each key-value-pair (k,v) in interval_map<K,V>::m_map means that the value v is associated with all keys from k
(including) to the next key (excluding) in m_map. The member interval_map<K,V>::m_valBegin holds the value that is
associated with all keys less than the first key in m_map.

Example: Let M be an instance of interval_map<int,char> where

M.m_valBegin=='A',
M.m_map=={ (1,'B'), (3,'A') },
Then M represents the mapping

...
-2 -> 'A'
-1 -> 'A'
0 -> 'A'
1 -> 'B'
2 -> 'B'
3 -> 'A'
4 -> 'A'
5 -> 'A'
...
The representation in the std::map must be canonical, that is, consecutive map entries must not contain the same value:
..., (3,'A'), (5,'A'), ... is not allowed. Likewise, the first entry in m_map must not contain the same value as
m_valBegin. Initially, the whole range of K is associated with a given initial value, passed to the constructor of the
interval_map<K,V> data structure.

Key type K

besides being copyable and assignable, is less-than comparable via operator<, and
does not implement any other operations, in particular no equality comparison or arithmetic operators.
Value type V

besides being copyable and assignable, is equality-comparable via operator==, and
does not implement any other operations.

*/

#include <iostream>
#include <limits>
#include <map>
#include <string>

struct KTypeInfo
{
    KTypeInfo(int value) : m_value{value}
    {
    }
    KTypeInfo(const KTypeInfo &other) : m_value{other.m_value}
    {
    }

    KTypeInfo &operator=(const KTypeInfo &other)
    {
        this->m_value = other.m_value;
        return *this;
    }

    bool operator<(const KTypeInfo &other) const
    {
        return m_value < other.m_value;
    }

    friend std::ostream &operator<<(std::ostream &os, const KTypeInfo &);

private:
    int m_value;
};
std::ostream &operator<<(std::ostream &os, const KTypeInfo &key)
{
    os << key.m_value;
    return os;
}

struct KValInfo
{
    KValInfo(char value) : m_value{value}
    {
    }
    KValInfo(const KValInfo &other) : m_value{other.m_value}
    {
    }

    bool operator==(const KValInfo &other) const
    {
        return m_value == other.m_value;
    }

    friend std::ostream &operator<<(std::ostream &os, const KValInfo &);

private:
    char m_value;
};
std::ostream &operator<<(std::ostream &os, const KValInfo &val)
{
    os << val.m_value;
    return os;
}

template <typename K, typename V> class interval_map
{
    friend void IntervalMapTest();
    V m_valBegin;
    std::map<K, V> m_map;

public:
    // constructor associates whole range of K with val
    interval_map(V const &val) : m_valBegin(val)
    {
    }

    // Assign value val to interval [keyBegin, keyEnd).
    // Overwrite previous values in this interval.
    // Conforming to the C++ Standard Library conventions, the interval
    // includes keyBegin, but excludes keyEnd.
    // If !( keyBegin < keyEnd ), this designates an empty interval,
    // and assign must do nothing.
    void assign(K const &keyBegin, K const &keyEnd, V const &val)
    {
        // INSERT YOUR SOLUTION HERE
        if (!(keyBegin < keyEnd))
            return;

        auto keyBeginShifted = keyBegin;

        if (m_map.size() > 0)
        {
            // * The member interval_map<K,V>::m_valBegin holds the value that is associated with all keys less than the
            // first key in m_map

            // * The first entry in m_map must not contain the same value as m_valBegin

            // [], if default value is 'A'

            // +[5,7:B] => possible case1: [5:B]
            // +[5,7:B] => possible case2: [5:B][7:A] (add default range as a tail)

            // +[7,8:C] => possible case1: [5:B][7:C]
            // +[7,8:C] => possible case2: [5:B][7:C][8:A] (add default range as a tail)

            if (keyBegin < m_map.begin()->first)
            {
                // +[4,5:V] => [4:A][5:B][7:C][8,A] (with default range as a tail)
                // +[3,4:T] => [3:A][5:B][7:C][8,A] (with default range as a tail)
                // +[2,3:Y] => [2:A][5:B][7:C][8,A] (with default range as a tail)

                // +[1,6:L] => possible case1: [1:A][5:B][7:C] (just extend the lower border of the storage)
                // +[1,6:L] => possible case2: [1:A][7:C] (clear the first one non-default range)
                // +[1,6:L] => possible case3: [1:A][5:L][6:B][7:C] (partially clear the first non-default range 5:B])
                //              [2:A][5:B][7:C][8,A] => [1:A] + [5:L] + [6:B] + [7:C] + [8:A]
                {
                    if (m_map.begin()->second == m_valBegin)
                        m_map.erase(m_map.begin()); // the lower key have to be replaced

                    keyBeginShifted = m_map
                                          .begin() // get the beginning of the first one non-default range
                                          ->first; // fill remain keys from newly added range with new value [1,6:L] =>
                                                   // [1,5:A]+[5,6:L]

                    m_map.insert(m_map.begin(), {keyBegin, m_valBegin}); // add new lower key

                    if (!(keyBeginShifted < keyEnd))
                        return;
                }
            }
        }

        // [0..3:A] [3..6:C]
        //      [2..4:B]
        // ---> find (3)
        // ---> get previous value = A
        // ---> insert last positions of the new range at 4 with parent value A
        auto itRangeEnd = m_map.lower_bound(keyEnd); // get first existing iterator near the right border
        if (itRangeEnd != m_map.end())               // insertion into existing "parent" range
        {
            auto parentRangeValue = std::prev(itRangeEnd)->second; // because of "but excludes keyEnd"
            itRangeEnd = m_map.insert(m_map.end(), std::pair(keyEnd, parentRangeValue));
        }
        else
        {
            itRangeEnd =
                m_map.insert(m_map.end(), std::pair(keyEnd, m_valBegin)); // insertion at the end of the storage
        }

        auto itRangeBegin = m_map.insert_or_assign(m_map.lower_bound(keyBeginShifted), keyBeginShifted, val);

        m_map.erase(std::next(itRangeBegin),
                    itRangeEnd); // remove all "expired" iterators to not valid anymore ranges between new itRangeBegin
                                 // and itRangeEnd

        auto itLeft = itRangeBegin == m_map.begin() ? std::next(itRangeBegin) : itRangeBegin;
        const auto itRight = std::next(itRangeEnd);
        while (itLeft != itRight)
        {
            itLeft = itLeft->second == std::prev(itLeft)->second ? m_map.erase(itLeft) : std::next(itLeft);
        }
    }

    // look-up of the value associated with key
    V const &operator[](K const &key) const
    {
        auto it = m_map.upper_bound(key);
        if (it == m_map.begin())
        {
            return m_valBegin;
        }
        else
        {
            return (--it)->second;
        }
    }

    void print()
    {
        for (auto it = m_map.begin(); it != m_map.end(); ++it)
        {
            std::cout << "[" << it->first << ":" << it->second << "]";
        }
        std::cout << std::endl;
    }
};

void IntervalMapTest()
{
    interval_map<KTypeInfo, KValInfo> m{'A'};

    m.assign(5, 7, 'B');
    m.assign(7, 8, 'C');
    m.print();

    m.assign(4, 5, 'V');
    m.assign(3, 4, 'T');
    m.assign(2, 3, 'Y');
    m.print();

    m.assign(1, 6, 'L');
    m.print();
}

/*void IntervalMapTest1()
{
    interval_map<KTypeInfo, KValInfo> m{'A'};

    m.assign(0, 3, 'B');
    m.assign(3, 7, 'D');
    m.assign(3, 4, 'C');
    m.assign(4, 5, 'C');

    for (auto it = m.m_map.begin(); it != m.m_map.end(); ++it)
    {
        std::cout << "[" << it->first << ":" << it->second << "]";
    }
    std::cout << std::endl;

    m.assign(0, 50, 'A');

    for (auto it = m.m_map.begin(); it != m.m_map.end(); ++it)
    {
        std::cout << "[" << it->first << ":" << it->second << "]";
    }
    std::cout << std::endl;

    m.assign(0, 3, 'B');
    m.assign(3, 5, 'D');
    m.assign(10, 20, 'C');
    m.assign(20, 40, 'C');

    for (auto it = m.m_map.begin(); it != m.m_map.end(); ++it)
    {
        std::cout << "[" << it->first << ":" << it->second << "]";
    }
    std::cout << std::endl;
}*/

int main()
{
    IntervalMapTest();
    return 0;
}
