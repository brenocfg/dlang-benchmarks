#include <algorithm>
#include <iostream>
#include <string>
 
void comb(int N, int K)
{
    std::string bitmask(K, 1);
    bitmask.resize(N, 0);
 
    do {
        for (int i = 0; i < N; ++i)
        {
            if (bitmask[i]) std::cout << " " << i;
        }
        std::cout << std::endl;
    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
}
 
int main()
{
    comb(5, 3);
}
