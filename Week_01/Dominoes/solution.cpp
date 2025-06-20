#include<iostream>
#include<vector>

int main() {
  std::ios_base::sync_with_stdio(false);
  int n_tests; std::cin >> n_tests;
  
  while(n_tests--) {
    // ===== READ INPUT =====
    int n_dominoes; std::cin >> n_dominoes;
    
    std::vector<int> dominos(n_dominoes);
    for (int i = 0; i < n_dominoes; i++) {
      std::cin >> dominos[i];
    }
    
    // ===== SOLVE =====
    int n_can_fall = dominos[0] - 1;  // Keep track of how many more dominoes can fall
    int n_fallen;                     // Count how many dominoes have fallen
    
    for (n_fallen = 1; n_fallen < n_dominoes; n_fallen++) {
      if (n_can_fall <= 0) break;     // Stop if no more dominoes can fall

      // Check if the new domino allows us to topple more dominoes
      n_can_fall = std::max(dominos[n_fallen] - 1, n_can_fall - 1);
    }

    std::cout << n_fallen << std::endl;
  }
}