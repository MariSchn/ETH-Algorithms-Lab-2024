#include<iostream>
#include<vector>
#include<queue>
#include<limits>
#include<cmath>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/tuple/tuple.hpp>

typedef  boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
  boost::property<boost::edge_capacity_t, long,
    boost::property<boost::edge_residual_capacity_t, long,
      boost::property<boost::edge_reverse_t, traits::edge_descriptor> > > >  graph;
// Interior Property Maps
typedef  boost::graph_traits<graph>::edge_descriptor      edge_desc;
typedef  boost::graph_traits<graph>::out_edge_iterator      out_edge_it;

class edge_adder {
 graph &G;

 public:
  explicit edge_adder(graph &G) : G(G) {}

  void add_edge(int from, int to, long capacity) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    const edge_desc e = boost::add_edge(from, to, G).first;
    const edge_desc rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  }
};

void solve() {
  // ===== READ INPUT =====
  int n, m; std::cin >> n >> m;
  
  std::vector<std::vector<int>> adj_mat(n, std::vector<int>(n, 0));
  for(int i = 0; i < m; ++i) {
    int a, b, c; std::cin >> a >> b >> c;
    adj_mat[a][b] += c;
  }
  
  // ===== CONSTRUCT GRAPH =====
  graph G(n);
  edge_adder adder(G);

  for(int i = 0; i < n; ++i) {
    for(int j = 0; j < n; ++j) {
      if(adj_mat[i][j]) {
        adder.add_edge(i, j, adj_mat[i][j]);
      }
    }
  }
  
  // ====== CALCULATE MIN CUT =====
  int min_cut = std::numeric_limits<int>::max();
  
  // Consider all other nodes as sinks and look for the min cut
  for(int i = 0; i < n; ++i) {
    min_cut = std::min(min_cut, (int) boost::push_relabel_max_flow(G, 0, i));
    min_cut = std::min(min_cut, (int) boost::push_relabel_max_flow(G, i, 0));
  }

  std::cout << min_cut << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}