#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

// Graph Type with nested interior edge properties for Cost Flow Algorithms
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor,
                boost::property <boost::edge_weight_t, long> > > > > graph; // new! weightmap corresponds to costs

typedef boost::graph_traits<graph>::vertex_descriptor           vertex_desc;
typedef boost::graph_traits<graph>::edge_descriptor             edge_desc;
typedef boost::graph_traits<graph>::out_edge_iterator           out_edge_it; // Iterator

const int MAX_L = 250;

// Custom edge adder class
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
  // std::cout << "==================================================================" << std::endl;
  // ===== READ INPUT =====
  int e, w, m, d, p, l; std::cin >> e >> w >> m >> d >> p >> l;

  graph G(e + w);
  edge_adder adder(G);
  
  // Setup source and sink
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc pseudo_source = boost::add_vertex(G);
  const vertex_desc v_target = boost::add_vertex(G);
  const vertex_desc pseudo_target = boost::add_vertex(G);
  
  adder.add_edge(v_source, pseudo_source, p - (l * e), 0);
  adder.add_edge(pseudo_target, v_target, p - (l * w), 0);
  
  // Create source and sink connections
  for(int i = 0; i < e; ++i) {
    adder.add_edge(v_source, i, l, 0);
    adder.add_edge(pseudo_source, i, MAX_L, 0);
  }
  for(int i = 0; i < w; ++i) {
    adder.add_edge(e + i, v_target, l, 0);
    adder.add_edge(e + i, pseudo_target, MAX_L, 0);
  }
  
  // Read matches
  for(int i = 0; i < m; ++i) {
    int u, v, r; std::cin >> u >> v >> r;
    adder.add_edge(u, e + v, 1, r);
  }
  
  // Read dangerous matches (ignore for now)
  for(int i = 0; i < d; ++i) {
    int u, v, r; std::cin >> u >> v >> r;
    adder.add_edge(pseudo_source, pseudo_target, 1 , r);
  }
  
  // ===== SOLVE =====
  int flow = boost::push_relabel_max_flow(G, v_source, v_target);
  boost::successive_shortest_path_nonnegative_weights(G, v_source, v_target);
  int cost = boost::find_flow_cost(G);
  
  // ===== OUTPUT =====
  // std::cout << flow << " " << cost << std::endl;
  if(flow == p) {
    std::cout << cost << std::endl;
  } else if (flow < p) {
    std::cout << "No schedule!" << std::endl;
  } else {
    std::cout << "ERROR: flow (" << flow << "( is higher than p (" << p << ")" << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}