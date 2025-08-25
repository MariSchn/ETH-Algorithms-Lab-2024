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
                boost::property <boost::edge_weight_t, long> > > > > graph; // new! weightmap corresponds to costs

typedef traits::vertex_descriptor vertex_desc;
typedef boost::graph_traits<graph>::edge_descriptor edge_desc;
typedef boost::graph_traits<graph>::out_edge_iterator out_edge_it; // Iterator

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
  int N, M, S; std::cin >> N >> M >> S;
  
  std::vector<int> limits(S);
  for(int i = 0; i < S; ++i) {
    std::cin >> limits[i];
  }
  
  std::vector<int> property_to_state(M);
  for(int i = 0; i < M; ++i) {
    std::cin >> property_to_state[i];
  }
  
  std::vector<std::vector<int>> bids(N, std::vector<int>(M));
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < M; ++j) {
      std::cin >> bids[i][j];
    }
  }
  
  // ===== BUILD GRAPH =====
  graph G(N + M + S);
  edge_adder adder(G);
  auto c_map = boost::get(boost::edge_capacity, G);
  auto r_map = boost::get(boost::edge_reverse, G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  
  // Add Source and Sink
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  // Connect Source to Bidders and Bidders to Sites
  for(int i = 0; i < N; ++i) {
    adder.add_edge(v_source, i, 1, 0);
    
    for(int j = 0; j < M; j++) {
      adder.add_edge(i, N + j, 1, 100 - bids[i][j]);
    }
  }
  
  // Connect Sites to States
  for(int i = 0; i < M; ++i) {
    adder.add_edge(N + i, N + M + property_to_state[i] - 1, 1, 0);
  }
  
  // Connect States to Sink
  for(int i = 0; i < S; ++i) {
    adder.add_edge(N + M + i, v_sink, limits[i], 0);
  }

  // ====== CALCULATE FLOW AND COST =====
  boost::successive_shortest_path_nonnegative_weights(G, v_source, v_sink);
  int cost = boost::find_flow_cost(G);

  int sold = 0;
  out_edge_it e, eend;
  for(boost::tie(e, eend) = boost::out_edges(boost::vertex(v_sink, G), G); e != eend; ++e)
    sold += rc_map[*e] - c_map[*e];  

  int revenue = 100 * sold - cost;
  std::cout << sold << " " << revenue << std::endl;
}


int main() {
  int T; std::cin >> T;
  
  while(T--) {
    solve();
  }
}