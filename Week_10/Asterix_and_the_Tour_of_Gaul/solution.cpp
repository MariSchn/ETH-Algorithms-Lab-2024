// ===== STD INCLUDES =====
#include <iostream>
#include <vector>

// ===== BGL INCLUDES =====
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

// ===== BGL TYPEDEFS =====
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
                              boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor,
                boost::property <boost::edge_weight_t, long> > > > > graph;

typedef boost::graph_traits<graph>::vertex_descriptor           vertex_desc;
typedef boost::graph_traits<graph>::edge_descriptor             edge_desc;
typedef boost::graph_traits<graph>::out_edge_iterator           out_edge_it;

class edge_adder {
 graph &G;

 public:
  explicit edge_adder(graph &G) : G(G) {}
  void add_edge(int from, int to, long capacity, long cost) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    auto w_map = boost::get(boost::edge_weight, G); // new!
    const edge_desc e = boost::add_edge(from, to, G).first;
    const edge_desc rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
    w_map[e] = cost;   // new assign cost
    w_map[rev_e] = -cost;   // new negative cost
  }
};

int MAX_SIGNIFICANCE = 2 << 7;

void solve() {
  // ===== READ INPUT =====
  int n, m; std::cin >> n >> m;
  
  std::vector<int> capacities(n - 1);
  for(int i = 0; i < n - 1; ++i) { std::cin >> capacities[i]; }
  
  std::vector<std::vector<std::vector<int>>> items(n-1, std::vector<std::vector<int>>(n));   // 3D Vector: (Start x End x Significances)
  for(int i = 0; i < m; ++i) { 
    int a, b, d; std::cin >> a >> b >> d;
    items[a][b].push_back(d);
  }
  
  // ===== BUILD GRAPH =====
  graph G(n);
  edge_adder adder(G);  
  auto c_map = boost::get(boost::edge_capacity, G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  
  // Add source and sink
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  // Add source and sink connections
  for(int i = 0; i < n - 1; ++i) { 
    adder.add_edge(v_source, i, capacities[i], 0);
    adder.add_edge(i + 1, v_sink, capacities[i], 0);
  }
  
  // Add edges between subsequent nodes
  for(int i = 0; i < n - 1; ++i) { 
    adder.add_edge(i, i+1, capacities[i], MAX_SIGNIFICANCE); 
  }
  
  // Add edges for items
  for(int a = 0; a < n - 1; ++a) {
    for(int b = 0; b < n; ++b) {
      // Find the minimum capacity from a to b
      int minimum_capacity = 100;
      for(int i = a; i < b; ++i) { minimum_capacity = std::min(minimum_capacity, capacities[i]); }
      
      // Only add as many edges as the capacity maximall allows for
      if(items[a][b].size() > minimum_capacity) {
        // If we have more items than the capacity allows, only take the ones with the highest significance
        std::sort(items[a][b].begin(), items[a][b].end(), [](int a, int b) { return a > b; });
        for(int i = 0; i < minimum_capacity; ++i) { 
          adder.add_edge(a, b, 1, (b - a) * MAX_SIGNIFICANCE - items[a][b][i]);
        }
      } else {
        for(int significance : items[a][b]) {
          adder.add_edge(a, b, 1, (b - a) * MAX_SIGNIFICANCE - significance);
        }
      }
    }
  }
  
  // ===== SOLVE =====
  boost::successive_shortest_path_nonnegative_weights(G, v_source, v_sink);
  int cost = boost::find_flow_cost(G);
  
  int flow = 0;
  out_edge_it e, eend;
  for(boost::tie(e, eend) = boost::out_edges(boost::vertex(v_source,G), G); e != eend; ++e) {
      flow += c_map[*e] - rc_map[*e];     
  }
  
  std::cout << flow * MAX_SIGNIFICANCE - cost << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}