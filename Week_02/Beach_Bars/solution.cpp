#include<iostream>
#include<vector>
#include<cmath>
#include<algorithm>


int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    
    std::vector<int> parasols(n);
    for(int i = 0; i < n; i++) {
      std::cin >> parasols[i];
    }
    
    // ===== SOLVE =====
    std::sort(parasols.begin(), parasols.end());
  
    // Initialize variables to track the best solution
    int max_n = 0;              // Maximum number of parasols within 100 meters
    int min_dist = 100;         // Minimum maximum distance to a parasol within 100 meters
    std::vector<int> optimal_x; // Vector to store optimal locations
    
    // Setup sliding window
    int left = 0;
    int right = 0;
    
    while(left < n && right < n) {
      int amount = right - left + 1;                          // Number of parasols in the current window
      int size = std::abs(parasols[right] - parasols[left]);  // Size of the current window
      int max_dist = std::ceil(static_cast<float>(size) / 2); // Maximum distance to a parasol within the current window
      
      // If window is too big, instantly skip and reduce window size by advancing left
      if(size >  200) {
        left++;
        continue;
      }
      
      // Clear optimal_x vector if new optimum is found
      // Found more parasols
      if(amount > max_n) {
        optimal_x.clear();
        
        max_n = amount;
        min_dist = max_dist;
      }
      // Found same amount of parasols, but smaller maximum distance
      if(amount == max_n && max_dist < min_dist) {
        optimal_x.clear();
        
        min_dist = max_dist;
      }
      
      // Add current window if it is optimal
      if(amount == max_n && max_dist == min_dist) {
        // dist is even -> Unique midpoint (optimum)
        // dist is odd  -> Two midpoints (optimums)
        if(size % 2 == 0) {
          optimal_x.push_back(static_cast<float>(parasols[right] + parasols[left]) / 2);
        } else {
          optimal_x.push_back(std::floor(static_cast<float>(parasols[right] + parasols[left]) / 2));
          optimal_x.push_back(std::ceil(static_cast<float>(parasols[right] + parasols[left]) / 2));
        }
      }

      // Advance window
      if(right == n - 1) left++;
      else right++;
    }
    
    // ===== OUTPUT =====
    std::cout << max_n << " " << min_dist << std::endl;
    for(const int& x : optimal_x) {
      std::cout << x << " ";
    }
    std::cout << std::endl;
  }
}