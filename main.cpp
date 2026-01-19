#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <sstream>

#include "Make10.hpp"

int main(void) {
    int n; std::cin >> n;
    std::vector<int> nums(n);
    for(auto& a: nums) std::cin >> a;
    int target = 10;

    auto start = std::chrono::high_resolution_clock::now();
    auto res = Make10(nums).benchmark(target);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    std::ostringstream oss;
    for (int i = 0; i < n; ++i) {
        if (i) oss << ",";
        oss << nums[i];
    }

    // CSV 出力
    std::cout << n << ",";
    std::cout << "\"" << oss.str() << "\",";
    std::cout << res[0] << ",";
    std::cout << res[1] << ",";
    std::cout << res[2] << ",";
    std::cout << std::fixed << std::setprecision(6) << elapsed.count() << "\n";

    return 0;
}