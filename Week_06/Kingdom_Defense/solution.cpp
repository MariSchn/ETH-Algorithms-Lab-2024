#include<iostream>
#include<vector>
#include<cmath>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, 
                              boost::vecS, 
                              boost::directedS, 
                              boost::no_property,
                              boost::property<boost::edge_capacity_t, long,
                                              boost::property<boost::edge_residual_capacity_t, long,
                                                              boost::property<boost::edge_reverse_t, traits::edge_descriptor>>>> graph;
typedef traits::vertex_descriptor vertex_desc;
typedef traits::edge_descriptor edge_desc;


class edge_adder {
  graph &G;
  
  public:
    explicit edge_adder(graph &G) : G(G) {}
    
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
  // ===== READ INPUT & BUILD GRAPH =====
  int l, p; std::cin >> l >> p;
  
  graph G(l);
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  std::vector<int> in_capacities(l);
  
  int in_capacity = 0;
  int out_capacity = 0;
  for(int i = 0; i < l; ++i) {
    int d; std::cin >> in_capacities[i] >> d;
    adder.add_edge(i, v_sink, d); // Only add "out_capacity"

    in_capacity += in_capacities[i];
    out_capacity += d;
  }
  
  if(in_capacity < out_capacity) {
    // Not enough soldiers 
    std::cout << "no" << std::endl;
    return;
  }
  
  for(int i = 0; i < p; ++i) {
    int f, t, c, C; std::cin >> f >> t >> c >> C;
    
    // Adjust "in_capacities" based on minimum amount of soldiers along a path (c)
    in_capacities[f] -= c;
    in_capacities[t] += c;
    
    adder.add_edge(f, t, C - c);
  }
  
  // Add "in_capacities" as edges
  for(int i = 0; i < l; ++i) {
    adder.add_edge(v_source, i, std::max(in_capacities[i], 0));
  }
  
  // ===== CALCULATE MAX FLOW =====
  long flow = boost::push_relabel_max_flow(G, v_source, v_sink);

  // ===== OUTPUT =====
  if(flow >= out_capacity) {
    std::cout << "yes" << std::endl;
  } else {
    std::cout << "no" << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}