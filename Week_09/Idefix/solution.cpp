#include <iostream>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <boost/pending/disjoint_sets.hpp>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef std::size_t                                            Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;

typedef std::tuple<Index,Index,K::FT> Edge;

typedef K::Point_2 Point;
typedef std::pair<Point,Index> IPoint;

std::ostream& operator<<(std::ostream& o, const Edge& e) {
  return o << std::get<0>(e) << " " << std::get<1>(e) << " " << std::get<2>(e);
}

void solve() {
  // ===== READ INPUT =====
  int n, m, k; long s; std::cin >> n >> m >> s >> k;
  
  std::vector<IPoint> trees; trees.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    trees.emplace_back(Point(x, y), i);
  }
  
  std::vector<Point> bones; bones.reserve(m);
  for(int i = 0; i < m; ++i) {
    int x, y; std::cin >> x >> y;
    bones.emplace_back(x, y);
  }
  
  // ===== SOLVE =====
  Delaunay t;
  t.insert(trees.begin(), trees.end());
  
  // === Calculate a ===
  
  // Calculate edges with their distances
  std::vector<Edge> edges; edges.reserve(3*n);
  for (auto e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
    Index i1 = e->first->vertex((e->second+1)%3)->info();
    Index i2 = e->first->vertex((e->second+2)%3)->info();
    if(i1 > i2) { std::swap(i1, i2); }
    
    edges.emplace_back(i1, i2, t.segment(e).squared_length());
  }
  
  // Determine (connected) components using Union Find
  boost::disjoint_sets_with_storage<> uf(n);
  for (auto e = edges.begin(); e != edges.end(); ++e) {
    Index c1 = uf.find_set(std::get<0>(*e));
    Index c2 = uf.find_set(std::get<1>(*e));
    K::FT dist = std::get<2>(*e);

    if (c1 != c2 && dist <= s) {
      uf.link(c1, c2);
    }
  }
  
  // Find number of bones for radius given by s
  std::vector<int> num_bones(n, 0);
  for(const Point &bone : bones) {
    auto vertex_handle = t.nearest_vertex(bone);
    if(4 * CGAL::squared_distance(bone, vertex_handle->point()) > s) { continue; }
     
    Index component = uf.find_set(vertex_handle->info());
    num_bones[component]++;
  }
  
  // Find maximum number of bones among components
  int max_num_bones = 0;
  for(const int &n : num_bones) {
    max_num_bones = std::max(max_num_bones, n);
  }
  
  // === Calculate q ===
  // Calculate q by adding the edges between bones and trees and stopping the UF as soon as k bones are reached in one component
  std::vector<int> bones_per_component(n + m, 0);
  for(int i = 0; i < m; ++i) {
    // Find Edge for the bone i
    auto vertex_handle = t.nearest_vertex(bones[i]);
    edges.emplace_back(n + i, vertex_handle->info(), 4 * CGAL::squared_distance(bones[i], vertex_handle->point()));
    bones_per_component[n + i] = 1;
  }
  
  // Sort, now with the edges between bones and trees
  std::sort(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) -> bool {
      return std::get<2>(e1) < std::get<2>(e2);
  });
  
  // Determine (connected) components using Union Find
  boost::disjoint_sets_with_storage<> bones_uf(n + m);
  K::FT q;
  for (auto e = edges.begin(); e != edges.end(); ++e) {
    Index c1 = bones_uf.find_set(std::get<0>(*e));
    Index c2 = bones_uf.find_set(std::get<1>(*e));
    K::FT dist = std::get<2>(*e);

    if (c1 != c2) {
      bones_uf.link(c1, c2);
      Index c3 = bones_uf.find_set(std::get<0>(*e));
      
      int total_bones = bones_per_component[c1] + bones_per_component[c2];
      bones_per_component[c1] = 0;
      bones_per_component[c2] = 0;
      bones_per_component[c3] = total_bones;
      
      if(bones_per_component[c3] >= k) {
        // Found radius at which k bones can be obtained
        q = dist;
        break;
      }
    }
  }
  
  // ===== OUTPUT =====
  std::cout << std::fixed << std::setprecision(0);
  std::cout << max_num_bones << " " << q << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}