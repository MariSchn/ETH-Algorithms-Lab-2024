#include<iostream>
#include<type_traits>
#include<limits>
#include<algorithm>

#include<CGAL/Exact_predicates_exact_constructions_kernel.h>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Point = K::Point_2;
using Segment = K::Segment_2;
using Ray = K::Ray_2;

double floor_to_double(const K::FT& x) {
  double a = std::floor(CGAL::to_double(x));
  while (a > x) a -= 1;
  while (a+1 <= x) a += 1;
  return a;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  while(true) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    if(!n) break;
    
    long x, y, a, b; std::cin >> x >> y >> a >> b;
    Point o(x, y), d(a, b);
    Ray ray(o, d);
    
    std::vector<Segment> segments(n);
    for(int i = 0; i < n; ++i) {
      long r, s, t, u; std::cin >> r >> s >> t >> u;
      segments[i] = Segment(Point(r, s), Point(t, u));
    }
    std::random_shuffle(segments.begin(), segments.end());
    
    // ===== FIND FIRST HIT =====
    bool found = false;
    Segment curr_segment;
    
    for(const Segment& segment : segments) {
      if(!found) {
        if(CGAL::do_intersect(ray, segment)) {
          std::result_of<K::Intersect_2(Ray, Segment)>::type its = CGAL::intersection(ray, segment);
          found = true;
          
          if(const Point* its_p = boost::get<Point>(&*its)) {
            curr_segment = Segment(ray.source(), *its_p);
          } else if (const Segment* its_s = boost::get<Segment>(&*its)) {
            Point source = its_s->source();
            Point target = its_s->target();
            
            curr_segment = Segment(ray.source(),
                                   CGAL::squared_distance(o, source) < CGAL::squared_distance(o, target) ? source : target);
          } else {
            throw std::runtime_error("Undefined intersection");
          }
        }
      } else {
        if(CGAL::do_intersect(curr_segment, segment)) {
          std::result_of<K::Intersect_2(Segment, Segment)>::type its = CGAL::intersection(curr_segment, segment);
          
          if(const Point* its_p = boost::get<Point>(&*its)) {
            curr_segment = Segment(curr_segment.source(), *its_p);
          } else if (const Segment* its_s = boost::get<Segment>(&*its)) {
            Point source = its_s->source();
            Point target = its_s->target();
            
            curr_segment = Segment(o,
                                   CGAL::squared_distance(o, source) < CGAL::squared_distance(o, target) ? source : target);
          } else {
            throw std::runtime_error("Undefined intersection");
          }
        }
      }
    }
    
    // ===== OUTPUT =====
    if(found) {
      std::cout << std::setprecision(0) << std::fixed;
      std::cout << floor_to_double(curr_segment.target().x()) << " " << floor_to_double(curr_segment.target().y()) << std::endl;
    } else {
      std::cout << "no" << std::endl;
    }
  }
}