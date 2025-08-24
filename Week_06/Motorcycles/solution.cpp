#include<iostream>
#include<vector>
#include <tuple>
#include <algorithm>
#include <CGAL/Gmpq.h>

// Intercept, Slope, Index
typedef std::tuple<int64_t, CGAL::Gmpq, int> Biker;

void solve() {
  // ===== READ INPUT =====
  int n; std::cin >> n;
  
  std::vector<Biker> bikers; bikers.reserve(n);
  for(int i = 0; i < n; ++i) {
    int64_t y_0, x_1, y_1; std::cin >> y_0 >> x_1 >> y_1;
    bikers.emplace_back(y_0, CGAL::Gmpq(y_1 - y_0, x_1), i);
  }
  
  // ===== SOLVE ======
  // Sort bikers descendingly by their intercept
  std::sort(bikers.begin(), bikers.end(), [](const Biker &a, const Biker &b){
    return std::get<0>(a) < std::get<0>(b);
  });
  
  std::vector<bool> rides_forever(n, true);
  // Iterate from the top and eliminate all riders that can not ride forever
  CGAL::Gmpq lowest_abs_slope = std::get<1>(bikers[0]);
  for(int i = 1; i < n; ++i) {
    CGAL::Gmpq curr_slope = std::get<1>(bikers[i]);
    
    // Check if the riders will intersect (either both drive up or both drive down)
    if(lowest_abs_slope > curr_slope) {
      // The bikers will cross -> Check who will continue by determining which has the lower abs slope
      if(CGAL::abs(lowest_abs_slope) <= CGAL::abs(curr_slope)) {
        rides_forever[std::get<2>(bikers[i])] = false;
      } else {
        lowest_abs_slope = curr_slope;
      }
    } else {
      // The biker[i] has a lower slope, so it either drives furhter down or is a new `lowest_abs_slope`
      if(CGAL::abs(lowest_abs_slope) >= CGAL::abs(curr_slope)) {
        lowest_abs_slope = curr_slope;
      }
    }
  }
  
  // Iterate from the bottom and eliminate all riders that can not ride forever
  // Basically the exact same as the previous loop just from the bottom
  lowest_abs_slope = std::get<1>(bikers[n-1]);
  for(int i = n - 2; i >= 0; --i) {
    CGAL::Gmpq curr_slope = std::get<1>(bikers[i]);

    if(lowest_abs_slope < curr_slope) {
      if(CGAL::abs(lowest_abs_slope) < CGAL::abs(curr_slope)) {
        rides_forever[std::get<2>(bikers[i])] = false;
      } else {
        lowest_abs_slope = curr_slope;
      }
    } else {
      if(CGAL::abs(lowest_abs_slope) >= CGAL::abs(curr_slope)) {
        lowest_abs_slope = curr_slope;
      }
    }
  }
  
  // ===== OUTPUT =====
  for(int i = 0; i < n; ++i) {
    if(rides_forever[i]) {
      std::cout << i << " ";
    }
  }
  std::cout << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}