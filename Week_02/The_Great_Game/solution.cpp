#include<iostream>
#include<vector>
#include<cmath>
#include<limits>

using Memo = std::vector<std::vector<int>>;
using Transitions = std::vector<std::vector<int>>;


int recursion(const Transitions& transitions, Memo& memo, int n, int pos, bool minimize) {
  // ===== BASE CASES =====
  if(pos == n) return 0;                                    // Base case: reached the end of the path
  if(memo[pos][minimize] != -1) return memo[pos][minimize]; // Memo
    
  // ===== RECURSIVE CASES =====
  if(minimize) {
    // Minimize: Loop over all possible transitions and find the minimum steps
    int min_steps = std::numeric_limits<int>::max();
    for(int next_pos : transitions[pos]) {
      min_steps = std::min(min_steps, 1 + recursion(transitions, memo, n, next_pos, false));
    }
    memo[pos][minimize] = min_steps;
  } else {
    // Maximize: Loop over all possible transitions and find the maximum steps
    int max_steps = std::numeric_limits<int>::min();
    for(int next_pos : transitions[pos]) {
      max_steps = std::max(max_steps, 1 + recursion(transitions, memo, n, next_pos, true));
    }
    memo[pos][minimize] = max_steps;
  }
  
  return memo[pos][minimize];
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    // ===== READ INPUT =====
    int n, m, r, b; std::cin >> n >> m >> r >> b;
  
    Transitions transitions(n, std::vector<int>());
    for(int i = 0; i < m; i++) {
      int u, v; std::cin >> u >> v;
      transitions[u].push_back(v);
    }
    
    // ===== SOLVE =====
    Memo r_memo(n, std::vector<int>(2, -1));
    Memo b_memo(n, std::vector<int>(2, -1));
  
    int r_n_steps = recursion(transitions, r_memo, n, r, true);
    int b_n_steps = recursion(transitions, b_memo, n, b, true);

    std::cout << (((r_n_steps < b_n_steps) || (r_n_steps == b_n_steps && r_n_steps % 2 != 0)) ? '0' : '1') << std::endl;
  }
}