#include <iostream>
#include <vector>
#include <tuple>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

// Graph Type with nested interior edge properties for flow algorithms
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor>>>> graph;

typedef traits::vertex_descriptor vertex_desc;
typedef traits::edge_descriptor edge_desc;

// Custom edge adder class, highly recommended
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

/*

- Solution has to include at least one city with positive b_i
- City with positive balance is only good if it has less debt than balance
- A solution must pay of all its debts and have at least 1$ left

- Test Set 2: "Unweighted" -> 

*/


void solve() {
  // ===== READ INPUT =====
  int n, m; std::cin >> n >> m;
  
  std::vector<int> balances(n);
  for(int i = 0; i < n; ++i) { std::cin >> balances[i]; }
  
  std::vector<std::tuple<int, int, int>> debts; debts.reserve(m);
  for(int i = 0; i < m; ++i) {
    int u, v, d; std::cin >> u >> v >> d;
    debts.emplace_back(u, v, d);
  }
  
  // ===== SOLVE =====
  graph G(n);
  edge_adder adder(G);
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  int sum_positive_balances = 0;
  
  // Source and Sink connections
  for(int i = 0; i < n; ++i) {
    if(balances[i] > 0) {
      adder.add_edge(v_source, i, balances[i]);
      sum_positive_balances += balances[i];
    } else {
      adder.add_edge(i, v_sink, -balances[i]);
    }
  }
  
  // Add debt edges
  for(const std::tuple<int, int, int> &debt : debts) {
    adder.add_edge(
      std::get<0>(debt),
      std::get<1>(debt),  
      std::get<2>(debt)
    );
  }
  
  long flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  // ===== OUTPUT =====
  if(flow < sum_positive_balances) {
    std::cout << "yes" << std::endl;
  } else {
    std::cout << "no" << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}