#include<iostream>
#include<cmath>

#include<CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>
#include<CGAL/Min_circle_2.h>
#include<CGAL/Min_circle_2_traits_2.h>

using K = CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt;
using Traits = CGAL::Min_circle_2_traits_2<K>;
using MinCircle = CGAL::Min_circle_2<Traits>;
using Point = K::Point_2;

double ceil_to_double(const K::FT& x) {
  double a = std::ceil(CGAL::to_double(x));
  while (a < x) a += 1;
  while (a-1 >= x) a -= 1;
  return a;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  while(true) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    if(!n) break;
    
    std::vector<Point> points(n);
    for(int i = 0; i < n; i++) {
      long x, y; std::cin >> x >> y;
      points[i] = Point(x, y);
    }
    
    // ===== FIND MINIMUM ENCLOSING RADIUS =====
    MinCircle mc(points.begin(), points.end(), true);
    Traits::Circle c = mc.circle();
    
    // ===== OUTPUT =====  
    std::cout << std::setprecision(0) << std::fixed;
    std::cout << ceil_to_double(CGAL::sqrt(c.squared_radius())) << std::endl;
  }
}