#include<iostream>
#include<vector>
#include<cmath>
#include<limits>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/prim_minimum_spanning_tree.hpp>
#include<boost/graph/dijkstra_shortest_paths.hpp>

using edge_weight = boost::property<boost::edge_weight_t, int>;
using graph = boost::adjacency_list<boost::vecS, 
                                    boost::vecS, 
                                    boost::undirectedS, 
                                    boost::no_property,
                                    edge_weight>;
using edge_desc = boost::graph_traits<graph>::edge_descriptor;
using vertex_desc = boost::graph_traits<graph>::vertex_descriptor;

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    int n, e, s, a, b; std::cin >> n >> e >> s >> a >> b;
    std::vector<graph> graphs(s, graph(n));
    std::vector<std::vector<std::vector<int>>> weights(n, std::vector<std::vector<int>>(n, std::vector<int>(s, std::numeric_limits<int>::max())));
    
    for(int edge_idx = 0; edge_idx < e; edge_idx++) {
      int t1, t2; std::cin >> t1 >> t2;
      
      for(int species = 0; species < s; species++) {
        int w; std::cin >> w;
        weights[t1][t2][species] = weights[t2][t1][species] = w;
        boost::add_edge(t1, t2, edge_weight(w), graphs[species]);
      }
    }
    
    std::vector<int> hives(s);
    for(int species = 0; species < s; species++) {
      int h; std::cin >> h;
      hives[species] = s;
    }
    
    // Use Prims algorithm to create MST from the hives to get species networks
    std::vector<std::vector<int>> min_weights(n, std::vector<int>(n, std::numeric_limits<int>::max()));
    
    for(int species = 0; species < s; species++) {
      std::vector<int> pred_map(n);
      
      boost::prim_minimum_spanning_tree(graphs[species], 
                                        boost::make_iterator_property_map(pred_map.begin(), boost::get(boost::vertex_index, graphs[species])), 
                                        boost::root_vertex(hives[species]));
                                        
      for(int i = 0; i < n; i++) {
        if(i != pred_map[i]) {
          min_weights[i][pred_map[i]] = min_weights[pred_map[i]][i] = std::min(min_weights[i][pred_map[i]], weights[i][pred_map[i]][species]);
        }
      }
    }
    
    graph min_graph(n);
    for(int i = 0; i < n; i++) {
      for(int j = 0; j < n; j++) {
        if(min_weights[i][j] != std::numeric_limits<int>::max()) {
          boost::add_edge(i, j, min_weights[i][j], min_graph);
        }
      }
    }
    
    std::vector<int> dist_map(n);
    dijkstra_shortest_paths(min_graph, boost::vertex(a, min_graph), boost::distance_map(boost::make_iterator_property_map(dist_map.begin(), boost::get(boost::vertex_index, min_graph))));
  
    std::cout << dist_map[b] << std::endl;
  }
}