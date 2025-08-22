#include <iostream>
#include <vector>
#include <limits>
#include <map>
#include <set>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel      K;
typedef std::size_t                                            Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Triangulation;
typedef Triangulation::Edge_iterator                           Edge_iterator;
typedef Triangulation::Finite_faces_iterator                   Face_iterator ;

typedef K::Point_2 Point;
typedef std::pair<Point, Index> IPoint;

int to_edge_idx(int u, int v, int n) {
  if(u < v)
    return u * n + v;
  else
    return v * n + u;
}

void solve() {
  // ===== READ INPUT =====
  int n, h; long s; std::cin >> n >> s >> h;
  
  std::vector<IPoint> trees; trees.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    trees.emplace_back(Point(x, y), i);
  }
  
  // ===== SOLVE =====
  int a_2, a_3, a_s, a_max; a_2 = a_3 = a_s = a_max = 0;

  // Triangulate trees
  Triangulation t;
  t.insert(trees.begin(), trees.end());
  
  // Find the minimum distance among the edges and count how many of these edges exist (this yields a_2)
  K::FT min_dist = std::numeric_limits<double>::max();
  for (Edge_iterator e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
    const K::FT dist = CGAL::to_double(t.segment(e).squared_length());
    
    if(dist == min_dist) {
      a_2++;
    } else if(dist < min_dist) {
      min_dist = dist;
      a_2 = 1;
    }
  }
  
  // Find the minimum radius among the faces and count how many of these faces exist (this yields a_3)
  K::FT min_radius = std::numeric_limits<double>::max();
  for (Face_iterator f = t.finite_faces_begin(); f != t.finite_faces_end(); ++f) {
    const Point center = t.circumcenter(f);  // ! REQUIRES CONSTRUCTION !
    const K::FT radius = CGAL::squared_distance(center, t.triangle(f)[0]);
    
    if(radius == min_radius) {
      a_3++;
    } else if(radius < min_radius) {
      min_radius = radius;
      a_3 = 1;
    }
  }
  
  // For each pair of tree (edge) determine minimum and maximum distance from the trees (this yields a_s)
  std::map<int, K::FT> d_min; // Edge Index -> Minimum Distance that a point p can be distant from u, v without being closer to another point
  std::map<int, K::FT> d_max; // Edge Index -> Maximum Distance that a point p can be distant from u, v without being closer to another point
  for(auto f = t.all_faces_begin(); f != t.all_faces_end(); f++) {
    if(t.is_infinite(f)) {
      // Set d_max to infinite for infinite edges
      for(int i = 0; i < 3; i++) {
        if(t.is_infinite(f->vertex(i))) {
          int idx_1 = f->vertex((i+1)%3)->info();
          int idx_2 = f->vertex((i+2)%3)->info();
          d_max[to_edge_idx(idx_1, idx_2, n)] = std::numeric_limits<double>::max();
        }
      }
    } else {
      // Calculate d_min and d_max based on the radius of the circumcircle and distance of the edge
      for(int i = 0; i < 3; i++) {
        auto v_1 = f->vertex((i+1)%3);
        auto v_2 = f->vertex((i+2)%3);
        int edge_idx = to_edge_idx(v_1->info(), v_2->info(), n);
        
        Point circumcenter = t.dual(f);
        K::FT radius = CGAL::squared_distance(circumcenter, t.triangle(f)[0]);

        // Update based on radius
        if(d_min.find(edge_idx) == d_min.end() || radius < d_min[edge_idx]) d_min[edge_idx] = radius;
        if(d_max.find(edge_idx) == d_max.end() || radius > d_max[edge_idx]) d_max[edge_idx] = radius;

        // Update based on edge length
        // Note that we can choose d_min as half of the edge length, only if there is not another point closer to that midpoint
        Point midpoint = CGAL::midpoint(v_1->point(), v_2->point());
        Point closest = t.nearest_vertex(midpoint)->point();
        if(closest == v_1->point() || closest == v_2->point())
          d_min[edge_idx] = CGAL::squared_distance(midpoint, v_1->point());
        
      }
    }
  }
  
  // Check for how many edges d_min < s < d_max
  K::FT s_exact(s); 
  for(auto it = d_min.begin(); it != d_min.end(); it++) {
    auto edge = it->first;
    if(d_min[edge] <= s_exact && s_exact <= d_max[edge])
      a_s++;
  }
  
  // Calculate a_max by iterating over the d_min and d_max and determining the d that is contained in the most intervals
  std::map<K::FT, int> enter_count;
  std::map<K::FT, int> leave_count;
  std::set<K::FT> unique_bounds;
  
  // Find all unique d_min and d_max and record how many interavls are entered and how many are left at d_min and d_max
  for (const auto& entry : d_min) {
      auto edge = entry.first;
      K::FT min_val = d_min[edge];
      K::FT max_val = d_max[edge];
  
      enter_count[min_val]++;
      leave_count[max_val]++;
  
      unique_bounds.insert(min_val);
      unique_bounds.insert(max_val);
  }
  
  // Iterate over all possible bounds d_i and find the one that overlaps the most intervals
  int current_active = 0;
  for (const auto& bound : unique_bounds) {
      current_active += enter_count[bound];
      a_max = std::max(a_max, current_active);
      current_active -= leave_count[bound];
  }
  
  // ===== OUTPUT =====
  std::cout << a_2 << " " << a_3 << " " << a_s << " " << a_max << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}