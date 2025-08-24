#include<iostream>
#include<vector>
#include<limits>
#include<cmath>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/dijkstra_shortest_paths.hpp>

using EdgeWeight = boost::property<boost::edge_weight_t, int>;
using Graph = boost::adjacency_list<boost::vecS, 
                                    boost::vecS,
                                    boost::undirectedS,
                                    boost::no_property,
                                    EdgeWeight>;
using VertexDescriptor = boost::graph_traits<Graph>::vertex_descriptor;
using EdgeDescriptor = boost::graph_traits<Graph>::edge_descriptor;


int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    // ===== READ INPUT =====
    int n, m, k, x, y; std::cin >> n >> m >> k >> x >> y;
    
    std::vector<std::tuple<int, int, int>> edges;
    std::vector<std::tuple<int, int, int>> river_edges;
    
    for(int i = 0; i < m; i++) {
      int a, b, c, d; std::cin >> a >> b >> c >> d;
      if(d) { river_edges.emplace_back(a, b, c); }
      else { edges.emplace_back(a, b, c); }
    }

    // ===== FIND SHORTEST PATH =====
    // Construct multi-layered Graph
    Graph g(n * (k + 1));
    
    // Add regular edges
    for(const std::tuple<int, int, int> edge : edges) {
      int s = std::get<0>(edge);
      int t = std::get<1>(edge);
      int w = std::get<2>(edge);
      
      // Add edge in all layers of the graph
      for(int i = 0; i < k + 1; ++i) {
        boost::add_edge(i * n + s, i * n + t, EdgeWeight(w), g);
      }
    }
    
    // Add river edges
    for(const std::tuple<int, int, int> edge : river_edges) {
      int s = std::get<0>(edge);
      int t = std::get<1>(edge);
      int w = std::get<2>(edge);
      
      // Add edge in all layers of the graph
      for(int i = 0; i < k; ++i) {
        boost::add_edge(s +  i      * n, t +  i      * n, EdgeWeight(w), g);
        boost::add_edge(s + (i + 1) * n, t +  i      * n, EdgeWeight(w), g);
        boost::add_edge(s +  i      * n, t + (i + 1) * n, EdgeWeight(w), g);
        boost::add_edge(s + (i + 1) * n, t + (i + 1) * n, EdgeWeight(w), g);
      }
    }
    
    std::vector<int> dist_map(n * (k + 1));
    boost::dijkstra_shortest_paths(g, x, boost::distance_map(boost::make_iterator_property_map(dist_map.begin(), boost::get(boost::vertex_index, g))));
    
    // ===== OUTPUT =====
    std::cout << dist_map[k * n + y] << std::endl;
  }
}