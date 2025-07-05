#include<iostream>
#include<vector>

// Allocate a dynamic programming table for the maximum number of attackers (100) and the maximum number of defenders (100000).
int dp[101][100001];

void solve() {
  // ===== READ INPUT =====
  int n, m, k; std::cin >> n >> m >> k;
    
  std::vector<int> defenders(n);
  for(int i = 0; i < n; i++) {
    std::cin >> defenders[i];
  }
    
  // ===== SOLVE =====

  // ===== SLIDING WINDOW =====
  // For each index i find the length of a segment with sum k that ends at i using a sliding window approach.
  std::vector<int> segments(n);
  int start = 0;
  int sum = 0;
  
  for(int end = 0; end < n; ++end) {
    sum += defenders[end];
    
    // Move start until sum is no longer larger than k
    while(sum > k) {
      sum -= defenders[start];
      ++start;
    }
    
    // If segment sum is equal to k store it
    if(sum == k) {
      segments[end] = end - start + 1;
    }
  }
  
  // ===== DYNAMIC PROGRAMMING =====
  // Fill entries with 0 defenders left with 0
  for(int i = 0; i <= n; i++) {
    dp[0][i] = 0;
  }
  
  // Fill entries with defenders left but no more segments with -1 ("fail")
  for(int i = 1; i <= m; i++) {
    dp[i][0] = -1;
  }
  
  // Bottom-Up DP
  for(int i = 1; i <= m; i++) {
    for(int j = 1; j <= n; j++) {
      const int len = segments[j-1];
      
      if (len && dp[i-1][j-len] != -1) {
        // The current segment is valid (len), and with the remaining i-1 fighters and j-len segments we can form a valid solution (dp[i-1][j-len] != -1)
        // Take the max of choosing this segment for the fighter (dp[i-1][j-len] + len) and not choosing it (dp[i][j-1]) in favor of later segments
        dp[i][j] = std::max(dp[i-1][j-len] + len, dp[i][j-1]);
      } else {
        // No valid segment found at index j -> move to next segment
        dp[i][j] = dp[i][j-1];  
      }
    }
  }
  
  // ===== OUTPUT =====
  if (dp[m][n] == -1) {
    std::cout << "fail" << std::endl;
  } else {
    std::cout << dp[m][n] << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}