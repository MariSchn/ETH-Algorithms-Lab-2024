#include <iostream>
#include <vector>
#include <limits>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
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

const long MAX_LONG = std::numeric_limits<long>::max();

void solve() {
  // ===== READ INPUT =====
  int n, m; std::cin >> n >> m;
  
  std::vector<int> conveniences(n);
  for(int i = 0; i < n; ++i) { 
    int s; std::cin >> s;
    conveniences[i] = s; 
  }
  
  std::vector<std::pair<int, int>> edges; edges.reserve(m);
  for(int i = 0; i < m; ++i) {
    int u, v; std::cin >> u >> v;
    edges.emplace_back(u, v);
  }
  
  // ===== SOLVE =====
  graph G(n);
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  // Add source and sink connections
  int positive_sum = 0;
  for(int i = 0; i < n; ++i) {
    if(conveniences[i] > 0) {
      adder.add_edge(v_source, i, conveniences[i]);
      positive_sum += conveniences[i];
    } else {
      adder.add_edge(i, v_sink, -conveniences[i]);
    }
  }
  
  // Add edges
  for(const std::pair<int, int> e : edges) {
    adder.add_edge(e.first, e.second, MAX_LONG);
  }
  
  long flow = boost::push_relabel_max_flow(G, v_source, v_sink);

  // ===== OUTPUT =====
  if (positive_sum - flow > 0) {
    std::cout << positive_sum - flow << std::endl;
  } else {
    std::cout << "impossible" << std::endl;
  }
  
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}