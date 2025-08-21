#include<iostream>
#include<vector>
#include<cmath>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/kruskal_min_spanning_tree.hpp>
#include<boost/graph/dijkstra_shortest_paths.hpp>

typedef boost::property<boost::edge_weight_t, int> edge_weight;
typedef boost::adjacency_list<boost::vecS,        // Use a vector for each nodes neighbors
                              boost::vecS,        // Use a vector to store all the neighbor vectors
                              boost::undirectedS, // The graph is undirected
                              boost::no_property, // No vertex property
                              edge_weight         // Add edge weights as an (interior) edge property
                             > weighted_graph;
typedef boost::graph_traits<weighted_graph>::edge_descriptor edge_desc;


int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    int n, m; std::cin >> n >> m;
    
    // Create and read graph
    weighted_graph g(n);
    for(int i = 0; i < m; i++) {
      int x, y, w; std::cin >> x >> y >> w;
      boost::add_edge(x, y, edge_weight(w), g);
    }
    
    // Execute Kruskals Algorithm
    std::vector<edge_desc> mst;
    boost::kruskal_minimum_spanning_tree(g, std::back_inserter(mst));
    
    boost::property_map<weighted_graph, boost::edge_weight_t>::type weight_map = boost::get(boost::edge_weight, g); // Get all the edge weights from the graph
    
    int mst_weight_sum = 0;
    for(std::vector<edge_desc>::iterator it = mst.begin(); it != mst.end(); ++it) {
      mst_weight_sum += weight_map[*it];
    }
    
    // Calculate the distance from node 0 to all other nodes
    std::vector<int> dist_map(n); // Create the exterior property to store the distance to node 0 for each node
    
    boost::dijkstra_shortest_paths(g, 0, // Pass the graph and the source node
                                   boost::distance_map(boost::make_iterator_property_map(dist_map.begin(),
                                                                                         boost::get(boost::vertex_index, g)
                                                                                         )));
    
    int max_dist = 0;
    for(const int& dist : dist_map) {
      max_dist = std::max(max_dist, dist);
    }
    
    std::cout << mst_weight_sum << " " << max_dist << std::endl;
  }
}