#include <iostream>
#include <vector>
#include <cmath>
#include <limits>


void solve() {       
  // ===== READ INPUT =====
  int n, k; std::cin >> n >> k;

  std::vector<int> cards(n);
  for(int i = 0; i < n; i++) {
    std::cin >> cards[i];
  }
  
  // ===== SOLVE =====
  // Sliding window approach
  int start = 0;
  long long current_sum = 0;

  // Track which window is the best so far
  int best_start = 0;
  int best_end = 0;
  long long min_diff = std::numeric_limits<long long>::max();
  
  for(int end = 0; end < n; end++) {
    // Add the new card to our window
    current_sum += cards[end];
    
    // Try to shrink the window from the left while maintaining or improving the result
    while(start <= end) {
      long long diff = std::abs(k - current_sum);
      
      // Check if current window is better
      if(diff < min_diff) {
        best_start = start;
        best_end = end;
        min_diff = diff;
        
        if(diff == 0) {
          std::cout << best_start << " " << best_end << std::endl;
          return;
        }
      }
      
      // If the current sum is smaller than k our window is too small -> Increment `end`
      if(current_sum <= k) {
        break;
      }
      // If the current sum is larger than k we need to shrink the window -> Increment `start`
      else {
        current_sum -= cards[start];
        start++;
      }
    }
  }

  std::cout << best_start << " " << best_end << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  int n_tests; std::cin >> n_tests;
  
  for(int n_test = 0; n_test < n_tests; n_test++) {
    solve();
  }
}