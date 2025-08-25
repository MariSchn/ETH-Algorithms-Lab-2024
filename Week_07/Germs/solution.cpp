#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;

typedef Triangulation::Finite_vertices_iterator VertexIterator;
typedef Triangulation::Edge_circulator EdgeCirculator;

typedef K::Point_2 Point;

int squared_distance_to_time(K::FT d) {
  // Calculate exact time
  K::FT t = std::sqrt(std::sqrt(d) - 0.5);
  
  // Round up to next integer;
  int out = std::ceil(CGAL::to_double(t));
  while (out < t) out += 1;
  while (out-1 >= t) out -= 1;
  
  return out;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  while(true) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    if(n == 0) { break; } // Terminate if end of input is reached
    
    int l, b, r, t; std::cin >> l >> b >> r >> t;
    
    std::vector<Point> points; points.reserve(n);
    for(int i = 0; i < n; ++i) {
      int x, y; std::cin >> x >> y;
      points.emplace_back(x, y);
    }
    
    // ===== TRIANGULATION =====
    Triangulation tri;
    tri.insert(points.begin(), points.end());
    
    // ===== DETERMINE DEATH DISTANCES =====
    std::vector<double> death_distances; death_distances.reserve(n);
    for(VertexIterator v = tri.finite_vertices_begin(); v != tri.finite_vertices_end(); ++v) {
      Point p = v->point();
      
      // Calculate distance to border
      double x_dist = std::min(p.x() - l, r - p.x());
      double y_dist = std::min(p.y() - b, t - p.y());
      double border_dist = std::min(x_dist, y_dist);
      double squared_border_dist = std::pow(border_dist, 2);
      
      // Find the shortest edge distance
      double shortest_edge = std::numeric_limits<double>::max();
      
      EdgeCirculator e = tri.incident_edges(v);
      if(e != 0) {  // Check if the circulator is valid
        do {
          if (!tri.is_infinite(e)) {  
            // Note, we have to divide by 4 here as both germs grow towards each other
            // Therefore we have to divide by 2, but since we are considering squared distances
            // we divide by 4
            shortest_edge = std::min(shortest_edge, tri.segment(e).squared_length() / 4);
          }
        } while (++e != tri.incident_edges(v));
      }
      
      // Add the death distance to the vector
      death_distances.push_back(std::min(squared_border_dist, shortest_edge));
    }
    
    // ==== CALCULATE QUERY TIMES =====
    std::sort(death_distances.begin(), death_distances.end());
    
    std::cout << squared_distance_to_time(death_distances[0]) << " " <<
                squared_distance_to_time(death_distances[std::floor(death_distances.size() / 2)]) << " " <<
                 squared_distance_to_time(death_distances[death_distances.size() - 1]) << std::endl;
  }
}