#include <algorithm>
#include <iostream> 
#include <numeric>
#include <vector>
#include <list>
 
using std::begin;
using std::end;
using std::for_each;
 
std::list<std::vector<int>> combinations(int from, int to)
{
    if (from > to)
        return {};
 
    auto pool = std::vector<int>(to - from);
    std::iota(begin(pool), end(pool), from);
 
    auto solutions = std::list<std::vector<int>>{};
 
    for (auto a : pool)
        for (auto b : pool)
            for (auto c : pool)
                for (auto d : pool)
                    for (auto e : pool)
                        for (auto f : pool)
                            for (auto g : pool)
                                if ( a      == c + d
                                  && b + c  == e + f
                                  && d + e  ==     g )
                                    solutions.push_back({a, b, c, d, e, f, g});
    return solutions;
}
 
std::list<std::vector<int>> filter_unique(int from, int to)
{
    auto has_non_unique_values = [](const auto & range, auto target)
    {
        return std::count( begin(range), end(range), target) > 1;
    };
 
    auto results = combinations(from, to);
 
    for (auto subrange = cbegin(results); subrange != cend(results); ++subrange)
    {
        bool repetition = false;
 
        for (auto x : *subrange)
            repetition |= has_non_unique_values(*subrange, x);
 
        if (repetition)
        {
            results.erase(subrange);
            --subrange;
        }
    }
 
    return results;
}
 
template <class Container>
inline void print_range(const Container & c)
{
    for (const auto & subrange : c)
    {
        std::cout << "[";
        for (auto elem : subrange)
            std::cout << elem << ' ';
        std::cout << "\b]\n";
    }
}
 
 
int main()
{
    std::cout << "Unique-numbers combinations in range 1-7:\n";
    auto solution1 = filter_unique(1, 8);
    print_range(solution1);
    std::cout << "\nUnique-numbers combinations in range 3-9:\n";
    auto solution2 = filter_unique(3,10);
    print_range(solution2);
    std::cout << "\nNumber of combinations in range 0-9: " 
              << combinations(0, 10).size() << "." << std::endl;
 
    return 0;
}
 
