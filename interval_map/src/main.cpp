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
        else if (val == m_valBegin) // the first entry in m_map must not contain the same value as m_valBegin
            return;

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
    interval_map<int, char> map{'-'};

    // [4:A][6:-][7:B][9:-]
    map.assign(4, 6, 'A');
    map.assign(7, 9, 'B');
    map.print();

    // [4:A][12:-]
    map.assign(6, 10, 'A');
    map.assign(10, 12, 'A');
    map.print();

    // [4:A][5:B][7:C][9:A][12:-]
    map.assign(5, 7, 'B');
    map.assign(7, 9, 'C');
    map.print();

    // [4:A][5:B][6:D][8:C][9:A][12:-]
    map.assign(6, 8, 'D');
    map.print();

    // [3:-][4:T][5:B][6:D][8:C][9:A][12:-]
    map.assign(3, 5, 'T');
    map.print();

    std::cout << std::endl;

    // [3:-][4:T][5:B][6:D][8:B][10:A][12:-]
    map.assign(8, 10, 'B');
    map.print();
    // [3:-][4:T][5:B][6:D][7:B][10:A][12:-]
    map.assign(7, 8, 'B');
    map.print();
    // [3:-][4:T][5:B][10:A][12:-]
    map.assign(6, 7, 'B');
    map.print();

    std::cout << std::endl;

    // [-1:-][4:T][5:B][10:A][12:B][15:-]
    map.assign(-1, 0, 'B');
    map.assign(12, 15, 'B');
    map.print();

    std::cout << std::endl;

    // [-1:-][4:T][5:-][6:B][7:-][8:B][10:A][12:B][15:-]
    map.assign(5, 6, '-');
    map.assign(7, 8, '-');
    map.print();

    // [-1:-][4:T][5:-][8:B][10:A][12:B][15:-]
    map.assign(6, 7, '-');
    map.print();

    // [-1:-]
    map.assign(0, 15, '-');
    map.print();
}

void test_SO()
{

    // first test case
    interval_map<int, char> fooh{'-'};
    /*fooh.assign(2, 5, 'a');
    fooh.print();
    std::cout << fooh[6] << std::endl << std::endl;

    // second test case
    //  expected : z  b  z
    fooh = interval_map<int, char>{'-'};
    fooh.assign(1, 4, 'b');
    std::cout << fooh[0] << " " << fooh[1] << " " << fooh[5] << std::endl;

    // third test case
    //  expected: A
    fooh = interval_map<int, char>{'-'};
    fooh.assign(1, 6, 'A');
    fooh.assign(2, 4, 'B');
    std::cout << fooh[5] << std::endl;
    fooh.print();

    // forth test case
    fooh = interval_map<int, char>{'-'};
    // expected [0,'a'],[1,'z']
    fooh.assign(0, 1, 'a');
    fooh.print();*/

    // fifth test case
    //  expected [0,'f']
    fooh = interval_map<int, char>{'-'};
    fooh.assign(1, 2, 'c');
    fooh.assign(2, 3, 'd');
    fooh.assign(3, 4, 'e');
    fooh.assign(4, 15, 'g'); // [1:c][2:d][3:e][4:g][15:-]
    fooh.print();
    fooh.assign(0, 10, 'f');
    fooh.print();
    std::cout << std::endl;

    // sixth test case
    //  expected: 0,'d'  2,'c'
    fooh = interval_map<int, char>{'-'};
    fooh.assign(1, 4, 'c');
    fooh.assign(0, 2, 'd');
    fooh.print();
    std::cout << std::endl;
}

int main()
{
    // Failed with "Non-cannonical"

    // IntervalMapTest();

    test_SO();

    return 0;
}
