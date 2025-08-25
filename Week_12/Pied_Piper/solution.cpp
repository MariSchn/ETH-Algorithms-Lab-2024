#include <iostream>
#include <vector>

void solve() {
  // ===== READ INPUT =====
  int n, m; std::cin >> n >> m;

  std::vector<std::vector<std::pair<int, int>>> incoming(n);
  std::vector<std::vector<std::pair<int, int>>> outgoing(n);

  for (int i = 0; i < m; ++i) {
    int u, v, f; std::cin >> u >> v >> f;

    outgoing[u].push_back(std::make_pair(v, f));
    incoming[v].push_back(std::make_pair(u, f));
  }

  // ===== SOLVE =====
  std::vector<std::vector<long>> dp(n, std::vector<long>(n, -1));

  // Fill the values for the initial out/in edges of 0.
  for (std::pair<int, int> &el : outgoing[0]) {
    dp[el.first][0] = el.second;
  }
  for (std::pair<int, int> &el : incoming[0]) {
    dp[0][el.first] = el.second;
  }

  // Iterate over the DP diagonally, i.e.
  // k = 1 -> (0, 1), (1, 0)
  // k = 2 -> (0, 2), (1, 1), (2, 0)
  // etc.
  for (int k = 1; k < n * 2; k++) {
    for (int i = 0; i <= k; i++) {
      int j = k - i;
      // Make sure that we are actually still within the bounds of the DP
      bool in_matrix = i < n && j < n;
      // The value in the neighbor must not be -1, otherwise there is no way
      // at all to go from 0 to i and from j back to 0 (impossible path), subsequently any
      // path that continues from i, j would also be impossible.
      bool valid_solution = dp[i][j] != -1;
      
      if (in_matrix && valid_solution) {
        for (auto &el : outgoing[i]) {
          // Node n - 1 is an edge case, and it must not be strictly more positive.
          if ((el.first > j && el.first > i) || el.first == n - 1) {
            dp[el.first][j] = std::max(dp[el.first][j], dp[i][j] + el.second);
          }
        }

        for (auto &el : incoming[j]) {
          // Node n - 1 is an edge case, and it must not be strictly more positive.
          if ((el.first > j && el.first > i) || el.first == n - 1) {
            dp[i][el.first] = std::max(dp[i][el.first], dp[i][j] + el.second);
          }
        }
      }
    }
  }

  // ===== OUTPUT =====
  std::cout << dp[n - 1][n - 1] << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;;
  while(n_tests--) { solve(); }
}