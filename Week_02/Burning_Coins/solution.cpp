#include<iostream>
#include<vector>
#include<limits>


int calcMaxWinnings(const std::vector<int>& values, std::vector<std::vector<int>>& memo, int start, int end) {
  // ===== BASE CASES =====
  // Only one coin left -> No choice but to take it
  if (end - start < 2) return std::max(values[start], values[end]);
  // Already computed this subproblem -> Return the stored result from the memo
  if (memo[start][end] != -1) return memo[start][end];
  
  // ===== RECURSIVE CASE =====
  // Calculate the maximum winnings obtainable by taking either the left or right coin (values[start] or values[end])
  // and the maximum winnings obtainable from the remaining coins (values[start + 1] to values[end - 1] or values[start + 2] to values[end])
  int max_winnings_left = values[start] + std::min(calcMaxWinnings(values, memo, start + 2, end    ),
                                                   calcMaxWinnings(values, memo, start + 1, end - 1));
  int max_winnings_right = values[end] +  std::min(calcMaxWinnings(values, memo, start + 1, end - 1),
                                                   calcMaxWinnings(values, memo, start    , end - 2));

  memo[start][end] = std::max(max_winnings_left, max_winnings_right);

  return memo[start][end];
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    
    std::vector<int> values;
    for(int i = 0; i < n; i++) {
      int v; std::cin >> v;
      values.push_back(v);
    }
    
    // ===== SOLVE =====
    std::vector<std::vector<int>> memo(n, std::vector<int>(n, -1));
    std::cout << calcMaxWinnings(values, memo, 0, n - 1) << std::endl;
    
  }
}