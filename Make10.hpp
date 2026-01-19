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
    int n; std::cin >> n;
	std::vector<int> nums(n);
	for(auto& a: nums) std::cin >> a;
	int target = 10;
    //-----------------------------------------

    auto start = std::chrono::high_resolution_clock::now();
	
    auto res = Make10(nums).benchmark(target);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
	for(auto& a : res) std::cout << a << " ";
    std::cout << std::fixed << std::setprecision(3);
    std::cout << elapsed.count() << "\n";
    return 0;
}
