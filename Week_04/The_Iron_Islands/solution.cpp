#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

// [Prefix Sum, Distance to 0, Waterway Index]
typedef std::tuple<int, int, int> Path;

void solve() {
  // ===== READ INPUT =====
  int n, k, w; std::cin >> n >> k >> w;
  
  std::vector<int> required_men(n);
  for(int i = 0; i < n; ++i) { std::cin >> required_men[i]; }
  
  std::vector<std::vector<int>> waterways(w);
  for(int i = 0; i < w; ++i) {
    int l; std::cin >> l;
    waterways[i] = std::vector<int>(l);

    for(int j = 0; j < l; ++j) {
      int r; std::cin >> r;
      waterways[i][j] = required_men[r];
    }
  }
  
  const int CENTER_COST = required_men[0];
  
  // ===== SOLVE =====
  int max_size = 0;
  
  // Search for maximum size on each waterway individually
  for(int i = 0; i < w; ++i) {
    int l = waterways[i].size();
    
    // Perform Sliding Window over waterway i
    int sum, left; sum = left = 0;
    for(int right = 0; right < l; ++right) {
      sum += waterways[i][right];
      
      while(sum > k) {
        sum -= waterways[i][left];
        left++;
      }
      
      if(sum == k) {
        max_size = std::max(max_size, right - left + 1);
      }
    }
  }
  
  // Search for maximum size for 2 waterways
  std::unordered_map<int, int> men_to_max_num_islands; // num_men -> most islands
  std::vector<int> prefix_sums; // stores the 
  for(int w_idx = 0; w_idx < w; ++w_idx) {
    // Calculate Prefix Sums for the current waterway i
    prefix_sums = std::vector<int>(1, 0);
    
    const int l = waterways[w_idx].size();
    int sum = 0;
    for(int i = 1; i < l; ++i) {
      sum += waterways[w_idx][i];
      if(sum >= k) { break; } // Capturing all visited islands on the current waterway would require at least all k men, so it is not possible to be used in a combination of 2 waterways
      prefix_sums.push_back(sum);
    }
    
    const int l_sums = prefix_sums.size();
    
    // Check for each island j along the current waterway if the complement (k - [sums[j]) has already been seen before, if so this would form a valid window
    for(int i = 1; i < l_sums; ++i) {
      int complement = k - prefix_sums[i] - CENTER_COST;

      if(men_to_max_num_islands.find(complement) != men_to_max_num_islands.end()) {
        // We have previously seen a waterway along which the remaining men could be used
        max_size = std::max(max_size, i + men_to_max_num_islands[complement] + 1); // + 1 to account for the center
      }
    }
    
    // Add the results from this waterway to the Hash Map
    for(int i = 1; i < l_sums; ++i) {
      // Check if there already exists an entry for the number men
      if(men_to_max_num_islands.find(prefix_sums[i]) == men_to_max_num_islands.end()) {
        // We have not encountered this amount of (remaining) men so create a new entry
        men_to_max_num_islands[prefix_sums[i]] = i;
      } else {
        // We have previously encountered this amount of (remaining) men so only update if we can now capture more islands
        men_to_max_num_islands[prefix_sums[i]] = std::max(men_to_max_num_islands[prefix_sums[i]], i);
      }
    }
  }
  
  // ===== OUTPUT =====
  std::cout << max_size << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}