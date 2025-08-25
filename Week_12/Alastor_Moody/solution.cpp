#include <iostream>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

// ===== DIJKSTRA GRAPH =====
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
  boost::no_property, boost::property<boost::edge_weight_t, int> >      dijkstra_graph;
typedef boost::property_map<dijkstra_graph, boost::edge_weight_t>::type weight_map;
typedef boost::graph_traits<dijkstra_graph>::edge_descriptor            dijkstra_edge_desc;
typedef boost::graph_traits<dijkstra_graph>::vertex_descriptor          dijkstra_vertex_desc;

// ===== FLOW GRAPH =====
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor>>>> flow_graph;

typedef traits::vertex_descriptor flow_vertex_desc;
typedef traits::edge_descriptor flow_edge_desc;

// Custom edge adder class, highly recommended
class edge_adder {
  flow_graph &G;

 public:
  explicit edge_adder(flow_graph &G) : G(G) {}

  void add_edge(int from, int to, long capacity) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    const auto e = boost::add_edge(from, to, G).first;
    const auto rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  }
};

void solve() {
  // ===== READ INPUT =====
  int n, m, s, p; std::cin >> n >> m >> s >> p;
  
  std::vector<std::tuple<int, int, int, int>> edges; edges.reserve(m);
  dijkstra_graph dijkstra_G(n);

  for(int i = 0; i < m; ++i) {
    int u, v, c, d; std::cin >> u >> v >> c >> d;
    
    edges.emplace_back(u, v, c, d);
    boost::add_edge(u, v, d, dijkstra_G);
  }
  
  // ===== BUILD SUBGRAPH CONTAINING SHORTEST PATHS =====
  std::vector<int> source_dist_map(n);
  std::vector<int> target_dist_map(n);
  
  boost::dijkstra_shortest_paths(dijkstra_G, s, boost::distance_map(boost::make_iterator_property_map(source_dist_map.begin(), boost::get(boost::vertex_index, dijkstra_G))));
  boost::dijkstra_shortest_paths(dijkstra_G, p, boost::distance_map(boost::make_iterator_property_map(target_dist_map.begin(), boost::get(boost::vertex_index, dijkstra_G))));
  
  int min_dist = source_dist_map[p];

  // Build Subgraph
  flow_graph G(n);
  edge_adder adder(G);
  
  for(const std::tuple<int, int, int, int> &edge : edges) {
    int u = std::get<0>(edge);
    int v = std::get<1>(edge);
    int c = std::get<2>(edge);
    int d = std::get<3>(edge);
    
    // Check if the edge is part of any shortest path
    // Check if the distances are individually smaller than min_dist to avoid overflows
    if(source_dist_map[u] + target_dist_map[v] + d <= min_dist && source_dist_map[u] <= min_dist && target_dist_map[v] <= min_dist) {
      adder.add_edge(u, v, c);
    }
    if(source_dist_map[v] + target_dist_map[u] + d <= min_dist && source_dist_map[v] <= min_dist && target_dist_map[u] <= min_dist) {
      adder.add_edge(v, u, c);
    }
  }
  
  // ===== DETERMINE MAX FLOW (= MAX PEOPLE) =====
  long flow = boost::push_relabel_max_flow(G, s, p);

  // ===== OUTPUT =====
  std::cout << flow << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}