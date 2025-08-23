#include<iostream>
#include<vector>

#include<CGAL/Exact_predicates_inexact_constructions_kernel.h>

using K = CGAL::Exact_predicates_inexact_constructions_kernel;

int main() {
  std::ios_base::sync_with_stdio(false);
  
  while(true) {
    int n; std::cin >> n;
    if(!n) break;
    
    long x, y, a, b; std::cin >> x >> y >> a >> b;
    K::Ray_2 ray(K::Point_2(x, y), K::Point_2(a, b));

    bool hit = false;

    for(int j = 0; j < n; ++j) {
      long r, s, t, u; std::cin >> r >> s >> t >> u;
  
      K::Segment_2 segment(K::Point_2(r, s), K::Point_2(t, u));
      
      if(CGAL::do_intersect(ray, segment)) {
        for(; j < n - 1; ++j) std::cin >> r >> s >> t >> u;
        hit = true;
        break;
      }
    }
    
    if (hit) std::cout << "yes" << std::endl;
    else std::cout << "no" << std::endl;
  }
}