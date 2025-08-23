#include<iostream>
#include<vector>
#include<cmath>
#include<limits>

using Memo = std::vector<std::vector<int>>;

int recursion(const std::vector<int>& values, Memo& memo, int m, int start, int end) {
  // Memo
  if(memo[start][end] != -1) {
    return memo[start][end];
  }

  if(end - start < m) {  
    // Base Case
    memo[start][end] = std::max(values[start], values[end]);
  } else {
    // Recursion
    
    // Calculate max winnings you can get when choosing the start-coin or end-coin
    int start_min_winnings = std::numeric_limits<int>::max();
    int end_min_winnings = std::numeric_limits<int>::max();
    for(int i = 0; i < m; i++) {
      start_min_winnings = std::min(start_min_winnings, recursion(values, memo, m, start + i + 1, end - (m - 1 - i)));
      end_min_winnings   = std::min(end_min_winnings,   recursion(values, memo, m, start + i,     end - (m - 1 - i) - 1));
    }
    start_min_winnings += values[start];
    end_min_winnings += values[end];
    
    memo[start][end] = std::max(start_min_winnings, end_min_winnings);
  }
  
  return memo[start][end];
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    int n, m, k; std::cin >> n >> m >> k;

    std::vector<int> values;
    for(int i = 0; i < n; i++) {
      int v; std::cin >> v;
      values.push_back(v);
    }
    
    int start = 0;
    int end = n - 1;
    Memo memo(n, std::vector<int>(n, -1));
    
    int res = std::numeric_limits<int>::max();
    for(int i = 0; i <= k; i++) {
      res = std::min(res, recursion(values, memo, m, start + i, end - (k - i)));
    }
    
    std::cout << res << std::endl;
  }
}