///3
#include<iostream>
#include<vector>
#include<string>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/push_relabel_max_flow.hpp>

using index_map = std::unordered_map<std::pair<int, int>, int>;

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
  // ===== READ INPUT =====
  // Read Input
  int w, h; std::cin >> w >> h;
  int num_nodes = 0;
  
  std::vector<std::vector<bool>> garden(h, std::vector<bool>(w, false));
  for(int r = 0; r < h; ++r) {
    std::string row; std::cin >> row;
    
    for(int c = 0; c < w; ++c) {
      if(row[c] == '.') {
        garden[r][c] = true;
        num_nodes++;
      }
    }
  }
  
  if(num_nodes % 2 == 1) {
    // Can't tile an odd numbered of spaces
    std::cout << "no" << std::endl;
    return;
  }
  
  // Build Graph
  graph G(w * h); // It would make more sense to only add num_nodes nodes, but this makes it easier to code
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);

  for(int r = 0; r < h; ++r) {
    for(int c = 0; c < w; ++c) {
      if(garden[r][c]) {
        int v_idx = r * w + c;

        // Add alternating source and sink connections in checkerboard pattern
        if((r + c) % 2 == 0) {
          adder.add_edge(v_source, v_idx, 1);
          if(r != h-1 && garden[r+1][c]) adder.add_edge(v_idx, (r+1) * w + c, 1);
          if(c != w-1 && garden[r][c+1]) adder.add_edge(v_idx, r * w + (c + 1), 1);
        } else {
          adder.add_edge(v_idx, v_sink, 1);
          if(r != h-1 && garden[r+1][c]) adder.add_edge((r+1) * w + c, v_idx, 1);
          if(c != w-1 && garden[r][c+1]) adder.add_edge(r * w + (c + 1), v_idx, 1);
        }
      }
    }
  }
  
  // ===== CALCULATE MAX FLOW =====
  int flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  // ===== OUTPUT =====
  if(flow * 2 == num_nodes) {
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