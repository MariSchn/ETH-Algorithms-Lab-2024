#include <iostream>
#include <vector>
#include <limits>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point;

// Checks if a point is inside of a triangle, by checking if it is on the right side of every edge of the triangle
// This assumes that the individual edges are oriented such that right is inside and left is outside
bool contains(const std::vector<Point> &triangle, const Point &point) {
  return (!CGAL::left_turn(triangle[0], triangle[1], point) &&
          !CGAL::left_turn(triangle[2], triangle[3], point) &&
          !CGAL::left_turn(triangle[4], triangle[5], point));
}

void solve() {
  // ===== READ INPUT =====
  int m, n; std::cin >> m >> n;
  
  // Read hiking path
  std::vector<Point> path; path.reserve(m);
  for(int i = 0; i < m; ++i) {
    int x, y; std::cin >> x >> y;
    path.emplace_back(x, y);
  }
  
  // Read map parts
  std::vector<std::vector<Point>> triangles(n);
  for(int i = 0; i < n; ++i) {
    // Read all points
    triangles[i].reserve(6);
    for(int j = 0; j < 6; ++j) {
      int x, y; std::cin >> x >> y;
      triangles[i].emplace_back(x, y);
    }
  }
  
  // ===== SOLVE =====
  // Ensure correct orientation for all lines/point pairs (Right Side -> Inside, Left Side -> Outside)
  for(int triangle_idx = 0; triangle_idx < n; ++triangle_idx) {
    // Check for line a and swap if orientation is wrong
    if(!CGAL::right_turn(triangles[triangle_idx][0], triangles[triangle_idx][1], triangles[triangle_idx][2])) {
      std::swap(triangles[triangle_idx][0], triangles[triangle_idx][1]);
    } 
    // Check for line b and swap if orientation is wrong
    if(!CGAL::right_turn(triangles[triangle_idx][2], triangles[triangle_idx][3], triangles[triangle_idx][4])) {
      std::swap(triangles[triangle_idx][2], triangles[triangle_idx][3]);
    } 
    // Check for line c and swap if orientation is wrong
    if(!CGAL::right_turn(triangles[triangle_idx][4], triangles[triangle_idx][5], triangles[triangle_idx][0])) {
      std::swap(triangles[triangle_idx][4], triangles[triangle_idx][5]);
    } 
  }
  
  // Pre compute, which map part contains covers which part of the hiking path
  std::vector<std::vector<int>> covers(n);
  for(int triangle_idx = 0; triangle_idx < n; ++triangle_idx) {
    for(int path_idx = 0; path_idx < m - 1; ++path_idx) {
      // Check if the leg (path_idx, path_idx + 1) is covered by the triangle
      if(contains(triangles[triangle_idx], path[path_idx]) && contains(triangles[triangle_idx], path[path_idx + 1])) {
        covers[triangle_idx].push_back(path_idx);
      }
    }
  }
    
  // Sliding window over map parts to find smallest
  std::vector<int> times_covered(m - 1, 0);  // Stores for each leg of the path how often it is covered by the current window
  int n_uncovered = m - 1;                   // Number of legs that are currently not covered by the current window
  int left = 0;
  int min_size = std::numeric_limits<int>::max();
  
  for(int right = 0; right < n; ++right) {
    // Add the triangle[right] to the current window
    for(const int &leg_idx : covers[right]) {
      times_covered[leg_idx]++;
      
      // If the leg was previously not covered, decrease the n_uncovered counter
      if(times_covered[leg_idx] == 1) { n_uncovered--; } 
    }

    // Move left pointer until the not the entire path is covered
    while(n_uncovered == 0) {
      min_size = std::min(min_size, right - left + 1);
      
      // Remove the triangle[left] from the current window
      for(const int &leg_idx : covers[left]) {
        times_covered[leg_idx]--;
        
        // If the leg was is no longer covered, increase the n_uncovered counter
        if(times_covered[leg_idx] == 0) { n_uncovered++; } 
      }
      
      left++;
    }
  }
  
  // ===== OUTPUT =====
  std::cout << min_size << std::endl;
}


int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}