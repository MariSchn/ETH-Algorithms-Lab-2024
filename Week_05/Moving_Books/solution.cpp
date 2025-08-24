#include<iostream>
#include<vector>
#include<algorithm>
#include<cmath>

void solve() {
    // ===== READ INPUT =====
    int n_friends, n_books; std::cin >> n_friends >> n_books;
    
    std::vector<int> strengths(n_friends), weights(n_books);
    
    for(int i = 0; i < n_friends; ++i) std::cin >> strengths[i];
    for(int i = 0; i < n_books; ++i) std::cin >> weights[i];

    
    // ===== CALCULATE SOLUTION =====
    std::sort(strengths.begin(), strengths.end(), std::greater<int>());
    std::sort(weights.begin(), weights.end(), std::greater<int>());

    if (strengths[0] < weights[0]) {
      std::cout << "impossible" << std::endl;
      return;
    }
    
    int low = std::ceil(static_cast<double>(n_books) / static_cast<double>(n_friends));
    int high = n_books;
    
    while(low < high) {
      int middle = std::floor((low + high) / 2);
      int used_friends = std::ceil(static_cast<double>(n_books) / static_cast<double>(middle));
      
      bool can_carry = true;
      
      for(int i = 0; i < used_friends; i++) {
        if(weights[i * middle] > strengths[i]) {
          can_carry = false;
          break;
        }
      }
      
      if(can_carry) {
        high = middle;
      } else {
        low = middle+1;
      }
    }

    // ===== OUTPUT =====
    std::cout << low * 3 - 1 << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}