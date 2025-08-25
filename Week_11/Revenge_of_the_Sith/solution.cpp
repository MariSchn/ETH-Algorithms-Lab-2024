#include <iostream>
#include <vector>
#include <stack>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <boost/pending/disjoint_sets.hpp>
#include <vector>
#include <tuple>
#include <algorithm>
#include <iostream>

// Epic kernel is enough, no constructions needed, provided the squared distance
// fits into a double (!)
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// we want to store an index with each vertex
typedef int                                                    Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;
typedef Delaunay::Vertex_handle Vertex_handle;


typedef std::tuple<Index,Index,K::FT> Edge;
typedef std::vector<Edge> EdgeV;

typedef std::pair<K::Point_2,Index> IPoint;

std::ostream& operator<<(std::ostream& o, const Edge& e) {
  return o << std::get<0>(e) << " " << std::get<1>(e) << " " << std::get<2>(e);
}

const bool DEBUG = false;

bool possible(const std::vector<IPoint> planets, int k, int n, K::FT r_squared) {
  // Triangulate the points starting from k
  Delaunay t;
  t.insert(planets.begin() + k, planets.end());
  
  // Extract edges
  EdgeV edges;
  edges.reserve(3*n); // there can be no more in a planar graph
  for (auto e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
    Index i1 = e->first->vertex((e->second+1)%3)->info();
    Index i2 = e->first->vertex((e->second+2)%3)->info();
    // ensure smaller index comes first
    if (i1 > i2) std::swap(i1, i2);
    edges.emplace_back(i1, i2, t.segment(e).squared_length());
  }
  
  // Sort edges
  std::sort(edges.begin(), edges.end(),
      [](const Edge& e1, const Edge& e2) -> bool {
        return std::get<2>(e1) < std::get<2>(e2);
            });
  
  // Setup and initialize Union-Find Data Structure
  boost::disjoint_sets_with_storage<> uf(n);
  std::vector<int> component_sizes(n, 1);
  for (EdgeV::const_iterator e = edges.begin(); e != edges.end(); ++e) {
    Index v1 = std::get<0>(*e);
    Index v2 = std::get<1>(*e);
    
    // Determine Components of Endpoints
    Index c1 = uf.find_set(v1);
    Index c2 = uf.find_set(v2);
    K::FT dist = std::get<2>(*e);
    
    if (
      c1 != c2 && 
      dist <= r_squared
    ) {
      int size1 = component_sizes[c1];
      int size2 = component_sizes[c2];
      uf.link(c1, c2);
      
      Index c3 = uf.find_set(v1);
      component_sizes[c3] = size1 + size2;
    }
  }
  
  // Find biggest component
  int max_size = 0;
  for(int i = 0; i < n; ++i) {
    max_size = std::max(max_size, component_sizes[i]);
  }

  return max_size >= k;
}

void solve() {
  // std::cout << "========================================" << std::endl;
  // ===== READ INPUT =====
  int n; long r; K::FT r_squared;
  std::cin >> n >> r;
  r_squared = r;
  r_squared = r_squared * r_squared;

  std::vector<IPoint> planets; planets.reserve(n);
  for (Index i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    planets.emplace_back(K::Point_2(x, y), i);
  }
  
  // ===== SOLVE =====
  // Binary search over possible k
  int min = 0;
  int max = n;
  
  while(min < max) {
    int middle = (min + max) / 2;
    if (DEBUG) std::cout << "=== k=" << middle << " ===" << std::endl; 
    
    if(min >= max - 1) {
      break;
    }
    
    if(possible(planets, middle, n, r_squared)) {
      if (DEBUG) std::cout << "k=" << middle << " is possible" << std::endl;
      min = middle;
    } else {
      if (DEBUG) std::cout << "k=" << middle << " is not possible" << std::endl;
      max = middle;
    }
  }
  
  // ===== OUTPUT =====
  std::cout << min << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}