#include<iostream>
#include<vector>

int main() {
  std::ios_base::sync_with_stdio(false);
  int n_tests; std::cin >> n_tests;
  
  while(n_tests--) {
    // ===== READ INPUT =====
    int n; std::cin >> n;

    // Add extra element as i <= j. So i == j is valid.
    std::vector<int> bits(n + 1, 0);
    for(int i = 1; i < n + 1; i++) {
      std::cin >> bits[i];
    }
  
    // ===== SOLVE =====
    int n_even = 0;
    int prefix_sum = 0;  // Keep track of the sum of all previous bits  
    for(int i = 0; i < n + 1; i++) {
      prefix_sum += bits[i];
      
      if (prefix_sum % 2 == 0) {
        n_even++;
      }
    }
    
    int n_odd = (n + 1) - n_even; // Calculate the number of odd pairs as (total - n_even)
    // Use combinatorial counting to find the number of even pairs
    int even_pairs = (n_even * (n_even - 1)) / 2;  // Even + Even -> Even
    int odd_pairs = (n_odd * (n_odd - 1)) / 2;     // Odd + Odd -> Even

    std::cout << even_pairs + odd_pairs << std::endl;
  }
}