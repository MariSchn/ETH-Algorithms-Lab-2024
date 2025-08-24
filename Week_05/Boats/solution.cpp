#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

void solve() {
  // ===== READ INPUT =====
  int n; std::cin >> n;
  
  std::vector<std::pair<int, int>> boats; boats.reserve(n);
  for(int i = 0; i < n; ++i) {
    int l, p; std::cin >> l >> p;
    boats.emplace_back(l, p);
  }
  
  // ===== SOLVE =====
  std::sort(boats.begin(), boats.end(), [](const std::pair<int, int> &a, const std::pair<int, int> &b){
    return a.second < b.second;
  });
  
  int n_boats = 0;
  int right_end = std::numeric_limits<int>::min();
  int prev_right_end = std::numeric_limits<int>::min();
  
  for(const std::pair<int, int> boat : boats) {
    int length = boat.first;
    int ring_pos = boat.second;
    
    if(ring_pos >= right_end) {
      // Boat can be placed
      prev_right_end = right_end;
      
      // Determine where the next right end is going to be
      if(right_end + length >= ring_pos) { right_end = right_end + length; }
      else { right_end = ring_pos; }
      
      n_boats++;
    } else {
      // Boat can not be placed, Check if we should replace the current right most boat
      if(prev_right_end + length < right_end) {
        // Current Boat is a better fit -> Replace
        if(prev_right_end + length >= ring_pos) { right_end = prev_right_end + length; }
        else { right_end = ring_pos; }
      }
    }
  }
  
  // ===== OUTPUT =====
  std::cout << n_boats << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}