#include<iostream>
#include<vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Triangulation = CGAL::Delaunay_triangulation_2<K>;
using Point = K::Point_2;


int main() {
  std::ios_base::sync_with_stdio(false);
  
  while(true) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    if (n == 0) break;  // Test if input was terminated
    
    std::vector<Point> restaurants(n);
    for(int i = 0; i < n; ++i) {
      int x, y; std::cin >> x >> y;
      restaurants[i] = Point(x, y);
    }
    
    int m; std::cin >> m;
    std::vector<Point> new_restaurants(m);
    for(int i = 0; i < m; ++i) {
      int x, y; std::cin >> x >> y;
      new_restaurants[i] = Point(x, y);
    }
    
    // ===== CONSTRUCT TRIANGULATION & CALCULATE DISTANCEES =====
    Triangulation t;
    t.insert(restaurants.begin(), restaurants.end());
    
    // Disable scientific notation (e+10) in output
    std::cout << std::setprecision(0) << std::fixed;
    for(const Point& query_point : new_restaurants) {
      Point closest_point = t.nearest_vertex(query_point)->point();
      K::FT distance = CGAL::squared_distance(query_point, closest_point);
      
      std::cout << distance << std::endl;
    }
  }
}