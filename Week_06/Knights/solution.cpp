#include<iostream>
#include<vector>

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

bool INCOMING = false;
bool OUTGOING = true;

int get_vertex_idx(int row, int col, bool outgoing, int n, int m) {
  return row * m + col + n * m * outgoing;
}

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
  int m, n, k, c; std::cin >> m >> n >> k >> c;
  
  // Build Graph
  graph G(m * n * 2); // * 2 Since per node, we need 2 nodes to model vertex capacity
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
    
  for(int row = 0; row < n; ++row) {
    for(int col = 0; col < m; ++col) {
      int v_incoming = get_vertex_idx(row, col, INCOMING, n, m);
      int v_outgoing = get_vertex_idx(row, col, OUTGOING, n, m);
      
      // Connect incoming to outgoing
      adder.add_edge(v_incoming, v_outgoing, c);
      
      // Connect to top intersection
      if(row != 0) adder.add_edge(v_outgoing, get_vertex_idx(row - 1, col, INCOMING, n, m), 1);
      else adder.add_edge(v_outgoing, v_sink, 1);
      
      // Connect to bottom intersection
      if(row != n-1) adder.add_edge(v_outgoing, get_vertex_idx(row + 1, col, INCOMING, n, m), 1);
      else adder.add_edge(v_outgoing, v_sink, 1);
      
      // Connect to left intersection
      if(col != 0) adder.add_edge(v_outgoing, get_vertex_idx(row, col - 1, INCOMING, n, m), 1);
      else adder.add_edge(v_outgoing, v_sink, 1);
      
      // Connect to right intersection
      if(col != m-1) adder.add_edge(v_outgoing, get_vertex_idx(row, col + 1, INCOMING, n, m), 1);
      else adder.add_edge(v_outgoing, v_sink, 1);
    }
  }
    
  // Connect source to knight positions
  for(int i = 0; i < k; ++i) {
    int x, y; std::cin >> x >> y;
    adder.add_edge(v_source, get_vertex_idx(y, x, INCOMING, n, m), 1);
  }
  
  // ===== CALCULATE MAX FLOW =====
  int flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  // ===== OUTPUT =====
  std::cout << flow << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}