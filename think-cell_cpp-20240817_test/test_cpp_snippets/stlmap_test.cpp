#include <iostream>
#include <map>

int main()
{
    std::map<int, int> m{{1, 100}, {3, 300}, {4, 400}};

    std::cout << m.lower_bound(1)->second << " / " << m.upper_bound(1)->second << std::endl;
    std::cout << m.lower_bound(2)->second << " / " << m.upper_bound(2)->second << std::endl;
    std::cout << m.lower_bound(3)->second << " / " << m.upper_bound(3)->second << std::endl;
    std::cout << m.lower_bound(4)->second << " / " << m.upper_bound(4)->second << std::endl;

    std::cout << std::endl;

    std::cout << m.lower_bound(1)->second << " / " << m.upper_bound(1)->second << std::endl;
    std::cout << m.lower_bound(2)->second << " / " << m.upper_bound(2)->second << std::endl;
    std::cout << m.lower_bound(3)->second << " / " << m.upper_bound(3)->second << std::endl;
    std::cout << m.lower_bound(4)->second << " / " << m.upper_bound(4)->second << std::endl;

    auto itRangeBegin = m.lower_bound(2);
    m.insert_or_assign(itRangeBegin, 2, 200);

    std::cout << std::endl;

    std::cout << m.lower_bound(1)->second << " / " << m.upper_bound(1)->second << std::endl;
    std::cout << m.lower_bound(2)->second << " / " << m.upper_bound(2)->second << std::endl;
    std::cout << m.lower_bound(3)->second << " / " << m.upper_bound(3)->second << std::endl;
    std::cout << m.lower_bound(4)->second << " / " << m.upper_bound(4)->second << std::endl;

    std::cout << std::endl;

    return 0;
}
