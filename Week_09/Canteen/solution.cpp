#include<iostream>
#include<vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
                              boost::property<boost::edge_capacity_t, long,
                              boost::property<boost::edge_residual_capacity_t, long,
                              boost::property<boost::edge_reverse_t, traits::edge_descriptor,
                              boost::property <boost::edge_weight_t, long>>>>> graph;

typedef boost::graph_traits<graph>::edge_descriptor edge_desc;
typedef boost::graph_traits<graph>::out_edge_iterator out_edge_it;

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

void solve() {
  // ===== READ INPUT =====
  int n; std::cin >> n;
  int total_n_students = 0;
  
  std::vector<int> production_capacity(n);
  std::vector<int> production_cost(n);
  std::vector<int> n_students(n);
  std::vector<int> menu_price(n);
  std::vector<int> freezer_capacity(n - 1);
  std::vector<int> freezer_cost(n - 1);
  
  for(int i = 0; i < n; ++i) {
    std::cin >> production_capacity[i] >> production_cost[i];
  }
  for(int i = 0; i < n; ++i) {
    std::cin >> n_students[i] >> menu_price[i];
    total_n_students += n_students[i];
  }
  for(int i = 0; i < n-1; ++i) {
    std::cin >> freezer_capacity[i] >> freezer_cost[i];
  }
  
  // ===== BUILD GRAPH =====
  graph G(n);
  edge_adder adder(G);
  auto c_map = boost::get(boost::edge_capacity, G);
  // auto r_map = boost::get(boost::edge_reverse, G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  
  const int v_source = boost::add_vertex(G);
  const int v_sink = boost::add_vertex(G);
  
  // Connect every node to source and sink
  for(int i = 0; i < n; ++i) {
    adder.add_edge(v_source, i, production_capacity[i], production_cost[i]);
    adder.add_edge(i, v_sink, n_students[i], 20 - menu_price[i]);
  }
  
  // Connect the nodes to subsequent nodes (freezer)
  for(int i = 0; i < n-1; ++i) {
    adder.add_edge(i, i+1, freezer_capacity[i], freezer_cost[i]);
  }
  
  // ===== CALCULATE MIN COST MAX FLOW =====
  boost::successive_shortest_path_nonnegative_weights(G, v_source, v_sink);
  int cost = boost::find_flow_cost(G);

  // Iterate over all edges leaving the source to sum up the flow values.
  int flow = 0;
  out_edge_it e, eend;
  for(boost::tie(e, eend) = boost::out_edges(boost::vertex(v_source,G), G); e != eend; ++e) {
    flow += c_map[*e] - rc_map[*e];     
  }
  
  if (total_n_students > flow) {
    std::cout << "impossible ";
  } else {
    std::cout << "possible ";
  }
  std::cout << flow << " " << 20 * flow - cost << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}