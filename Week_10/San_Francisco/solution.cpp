#include <iostream>
#include <vector>

void solve() {
  // ===== READ INPUT =====
  long n, m, x, k; std::cin >> n >> m >> x >> k;
  
  std::vector<std::vector<std::pair<int, int>>> edges(n);
  for(int i = 0; i < m; ++i) {
    int u, v, p; std::cin >> u >> v >> p;
    edges[u].emplace_back(v, p);
  }
  
  // ===== SOLVE =====
  std::vector<std::vector<long>> dp(k + 1, std::vector<long>(n, 0));  // Number of Turns left x Hole -> Max Achievable Score
  
  // Fill DP table bottom up
  for(int turn = 1; turn < k + 1; ++turn) {
    for(int hole = 0; hole < n; ++hole) {
      if(edges[hole].empty()) {
        // If the node has no edges, the "free action" of going back to the beginning is performed
        dp[turn][hole] = dp[turn][0];
      } else {
        // If the node has edges the maximum achievable score is the maximum of the score achievable from its children with one move less + the score of going to that child
        for(const std::pair<int, int> &edge : edges[hole]) {
          dp[turn][hole] = std::max(dp[turn][hole], dp[turn - 1][edge.first] + edge.second);
        }
      }
    }
  }
  
  // ===== OUTPUT =====
  // Search for first turn, where score x is reached
  for(int turn = 0; turn < k + 1; ++turn) {
    if(dp[turn][0] >= x) {
      std::cout << turn << std::endl;
      return;
    }
  }
  std::cout << "Impossible" << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}