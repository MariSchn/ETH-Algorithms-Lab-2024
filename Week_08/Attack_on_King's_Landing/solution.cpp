///4
#include<iostream>
#include<vector>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include<boost/graph/max_cardinality_matching.hpp>

using traits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
using graph = boost::adjacency_list<boost::vecS,
                                    boost::vecS,
                                    boost::undirectedS,
                                    boost::no_property,
                                    boost::property<boost::edge_weight_t, int>>;
using vertex_desc = traits::vertex_descriptor;
using edge_desc = traits::edge_descriptor;


void solve() {
  // ===== READ INPUT & BUILD GRAPH =====
  int n, m, b, p, d; std::cin >> n >> m >> b >> p >> d;
  int num_nodes = n + p;
  
  graph G(num_nodes);
  std::vector<int> barracks(b);
  std::vector<int> plazas(p);
  std::vector<bool> covered(num_nodes, false);
  
  for(int i = 0; i < b; ++i) std::cin >> barracks[i];
  for(int i = 0; i < p; ++i) std::cin >> plazas[i];
  for(int i = 0; i < m; ++i) {
    int x, y, l; std::cin >> x >> y >> l;
    boost::add_edge(x, y, l, G);
    
    // Duplicate plaza nodes
    // Could be more efficient as we check this for every single edge
    // But is fast enough
    for(int j = 0; j < p; ++j) {
      if(x == plazas[j]) {
        boost::add_edge(y, n + j, l, G);
      } else if (y == plazas[j]) {
        boost::add_edge(x, n + j, l, G);
      }
    }
  }

  // ===== FIND SUBGRAPH THAT IS COVERED BY BARRACKS =====
  for(int i = 0; i < b; ++i) {
    std::vector<int> dist_map(num_nodes); //exterior property
    boost::dijkstra_shortest_paths(G, barracks[i], boost::distance_map(boost::make_iterator_property_map(dist_map.begin(), boost::get(boost::vertex_index, G))));
  
    for(int j = 0; j < num_nodes; ++j) {
      if(dist_map[j] <= d) {
        covered[j] = true;
      }
    }
  }

  for(int i = 0; i < num_nodes; ++i) {
    if(!covered[i]) {
      boost::clear_vertex(i, G);
    }
  }

  // ===== FIND MAXIMUM MATCHING =====
  std::vector<int> mate_map(num_nodes);

  boost::edmonds_maximum_cardinality_matching(G, boost::make_iterator_property_map(mate_map.begin(), boost::get(boost::vertex_index, G)));
  int matching_size = boost::matching_size(G, boost::make_iterator_property_map(mate_map.begin(), boost::get(boost::vertex_index, G)));
  
  // ===== OUTPUT =====
  std::cout << matching_size << std::endl;
}

int main() {
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}