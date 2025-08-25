#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <set>
#include <queue>

// ===== BOOST INCLUDES AND TYPEDEFS =====
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bipartite.hpp>
#include <boost/graph/connected_components.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> graph;

// ===== CGAL INCLUDES AND TYPEDEFS =====
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef int                                                    Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;

typedef Tds::Vertex_handle          Vertex_handle;
typedef Delaunay::Edge_iterator     Edge_iterator;
typedef Delaunay::Vertex_iterator   Vertex_iterator;
typedef Delaunay::Vertex_circulator Vertex_circulator;

typedef K::Point_2               Point;
typedef std::pair<Point, Index> IPoint;


void solve() {
  // ===== READ INPUT =====
  int n, m, r; std::cin >> n >> m >> r;
  long r_squared = std::pow(r, 2);
  
  std::vector<IPoint> stations; stations.reserve(n);
  std::vector<IPoint> holmes_positions; holmes_positions.reserve(m);
  std::vector<IPoint> watson_positions; watson_positions.reserve(m);
  
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    stations.emplace_back(Point(x, y), i);
  }
  for(int i = 0; i < m; ++i) {
    int a_x, a_y, b_x, b_y; std::cin >> a_x >> a_y >> b_x >> b_y;
    holmes_positions.emplace_back(Point(a_x, a_y), i);
    watson_positions.emplace_back(Point(b_x, b_y), i);
  }
  
  // ===== SOLVE =====
  // Build graph using Triangulation
  // Note, that we are not able to just loop over all edges and keep those who are shorter than r
  // This is because the Triangulation will just give us the edges for the shortest distances "around" each vertex v
  // However, there might be other vertices that still lie in the radius of r
  // but the Triangulation does not directly connect them to v, as there is a closer vertex
  // Therefore, we need to do a BFS from all nodes to determine their connectivity
  Delaunay t;
  t.insert(stations.begin(), stations.end());
  
  graph G(n);
  for (Vertex_iterator v = t.finite_vertices_begin(); v != t.finite_vertices_end(); ++v) {
    // Perform BFS for every Vertex to determine its connectivity
    std::queue<Vertex_handle> q;
    std::set<Vertex_handle> visited;
    
    q.push(v);
    visited.insert(v);
    
    while(!q.empty()) {
      Vertex_handle curr_v = q.front(); q.pop();
      
      // Connect v to curr_v, as only vertices within radius r are pushed to q
      if(v->info() != curr_v->info()) {
        boost::add_edge(v->info(), curr_v->info(), G);
      }
      
      // Iterate over all neighbors and potentially add them to q
      Vertex_circulator neighbor_v = t.incident_vertices(curr_v);
      do {
        if(
            !t.is_infinite(neighbor_v) && // Check if the neighbor is the infinite vertex (happens at boundary)
            visited.find(neighbor_v) == visited.end() && // Check if neighbor has been visited before
            CGAL::squared_distance(v->point(), neighbor_v->point()) <= r_squared // Check if neighbor is close enough
          ) {
            q.push(neighbor_v);
            visited.insert(neighbor_v);
          }
      } while(++neighbor_v != t.incident_vertices(curr_v));
    }
  }
  
  // Check if G is bipartite. If not early termination
  if(!is_bipartite(G)) {
    std::cout << std::string(m, 'n') << std::endl;
    return;
  } 
  
  // Determine the connected components, to quickly check if a_i and b_i are connected
  std::vector<int> component_map(n);
  boost::connected_components(G, &component_map[0]);
  
  // Check for every pair a_i b_i if they are connected/reachable
  for(int i = 0; i < m; ++i) {
    Point a = holmes_positions[i].first;
    Point b = watson_positions[i].first;
    
    // Check if they are "directly reachable"
    if(CGAL::squared_distance(a, b) <= r_squared) {
      std::cout << "y";
      continue;
    } 
    
    Vertex_handle neighbor_a = t.nearest_vertex(a);
    Vertex_handle neighbor_b = t.nearest_vertex(b);
    
    long dist_a = CGAL::squared_distance(a, neighbor_a->point());
    long dist_b = CGAL::squared_distance(b, neighbor_b->point());

    // Check if a and b are close enough to the stations and if they are in the same component
    if(dist_a <= r_squared && dist_b <= r_squared && 
       component_map[neighbor_a->info()] == component_map[neighbor_b->info()]) {
        std::cout << "y";   
     } else {
        std::cout << "n";
     }
  }
  std::cout << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}