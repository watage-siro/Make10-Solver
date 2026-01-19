#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <chrono>

#include "Make10.hpp"
int main(void) {
    // Up to 8 elements in the perms can be handled.
    // Runs in seconds up to n = 6.
    
    //-----------------------------------------
    std::vector<int> nums = {1,2,3,4,5,6};
    int target = 10;
    //-----------------------------------------

    auto start = std::chrono::high_resolution_clock::now();
	
    auto res = Make10(nums).solve(target);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Run Time:" << elapsed.count() << "s\n";
    
    std::cout << "Total Solution: " << res.size() << "\n";
    
    std::cout << "First 100 solutions:\n";
    int display = std::min(100,(int)res.size());
    for(int i = 0; i < display; ++i) {
        std::cout << i + 1 << ": ";
        std::cout << res[i] << " = " << target << "\n";
    }
    if(res.size() > 100) std::cout << "..." << "\n";
    return 0;
}
