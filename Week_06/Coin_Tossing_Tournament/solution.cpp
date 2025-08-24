#include<iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

using traits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
using graph = boost::adjacency_list<boost::vecS, 
                                    boost::vecS, 
                                    boost::directedS, 
                                    boost::no_property,
                                    boost::property<boost::edge_capacity_t, long,
                                                    boost::property<boost::edge_residual_capacity_t, long,
                                                                    boost::property<boost::edge_reverse_t, traits::edge_descriptor>>>>;
using vertex_desc = traits::vertex_descriptor;
using edge_desc = traits::edge_descriptor;


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
  int n, m; std:: cin >> n >> m;
  int score_sum = 0;
  
  // 0  to      n - 1 -> Players
  // n  to  n + m - 1 -> Games
  graph G(n + m); 
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  // Connect Source -> Games and Games -> Players
  for(int i = 0; i < m; ++i) {
    // Each game gives 1 point -> Connect source to game with capacity 1
    adder.add_edge(v_source, n + i, 1);
    
    // Connect Game to Players depending on outcome
    int a, b, c; std::cin >> a >> b >> c;
    
    if(c == 1) {         // Player a wins -> Connect to a
      adder.add_edge(n + i, a, 1);
    } else if (c == 2) { // Player b wins -> Connect to b
      adder.add_edge(n + i, b, 1);
    } else {             // Don't know -> Connect to both
      adder.add_edge(n + i, a, 1);
      adder.add_edge(n + i, b, 1);
    }
  }
  
  // Connect Players -> Sink
  for(int i = 0; i < n; ++i) {
    int s; std::cin >> s;
    score_sum += s;
    
    adder.add_edge(i, v_sink, s);
  }
  
  // ===== CALCULATE MAX FLOW =====
  int flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  // ===== OUTPUT =====
  if(score_sum == m && flow == score_sum) {
    std::cout << "yes" << std::endl;
  } else {
    std::cout << "no" << std::endl;
  }
}

int main() {
  int n_tests; std::cin >> n_tests;
  
  while(n_tests--) {
    solve();
  }
}