#include<iostream>
#include<vector>
#include<algorithm>
#include<stack>
#include<tuple>
#include<cmath>


using IntPair = std::pair<int, int>;

IntPair standsOn(int ball_idx, int n_balls) {
  if(ball_idx >= (n_balls - 1) / 2) {
    return std::make_pair(-1, -1);
  } else {
    return std::make_pair(2 * ball_idx + 1, 
                          2 * ball_idx + 2);
  }
}


void solve() {
  // ===== READ INPUT =====
  int n_balls; std::cin >> n_balls;
  std::vector<int> explosion_times(n_balls); 
  std::vector<IntPair> t_idx_pairs(n_balls);
  std::vector<bool> diffused(n_balls, false);
  for(int i = 0; i < n_balls; i++) {
    int t; std::cin >> t;
    
    explosion_times[i] = t;
    t_idx_pairs[i] = std::make_pair(t, i);
  }
  
  // ===== SOLVE =====
  // Sort with respect to explosion time (ascending) and index (descending)
  std::sort(t_idx_pairs.begin(), t_idx_pairs.end(), [](const IntPair &a, const IntPair &b) {
    if(a.first != b.first) {
      return a.first < b.first;
    } else {
      return a.second > b.second;
    }
  });
  
  // Try to defuse the most urgent ball
  int elapsed_time = 0;
  for(int i = 0; i < n_balls; ++i) {
    // Check if bomb was already diffused
    if(diffused[t_idx_pairs[i].second]) {
      continue;
    }
    
    std::stack<int> to_diffuse_stack;
    to_diffuse_stack.push(t_idx_pairs[i].second);
    
    while(!to_diffuse_stack.empty()) {
      int to_diffuse_idx = to_diffuse_stack.top();
      
      // Check if bomb already exploded
      if(explosion_times[to_diffuse_idx] <= elapsed_time) {
        std::cout << "no" << std::endl;
        return;
      }
      
      int depends_on_1, depends_on_2;
      std::tie(depends_on_1, depends_on_2) = standsOn(to_diffuse_idx, n_balls);
      
      // Check if bottom was reached or balls beneath were already diffused
      if((depends_on_1 == -1 && depends_on_2 == -1) || 
         (diffused[depends_on_1] && diffused[depends_on_2])) {
        diffused[to_diffuse_idx] = true;
        elapsed_time++;
        to_diffuse_stack.pop();
      } else {
        if(!diffused[depends_on_1]) to_diffuse_stack.push(depends_on_1);
        if(!diffused[depends_on_2]) to_diffuse_stack.push(depends_on_2);
      }
    }
  }
  
  std::cout << "yes" << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}