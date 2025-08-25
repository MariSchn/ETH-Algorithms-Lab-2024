#include <iostream>
#include <vector>
#include <limits>

#include <boost/pending/disjoint_sets.hpp>

typedef std::tuple<int, int, int> Edge;

const int MAX_INT = std::numeric_limits<int>::max();

void solve() {
  // ===== READ INPUT =====
  int n, source; std::cin >> n >> source;

  std::vector<Edge> edges;
  for(int j = 1; j <= n - 1; ++j) {
    for(int k = 1; k <= n - j; ++k) {
      int d; std::cin >> d;
      // Adjust for 0-based indexing
      edges.emplace_back(j - 1, j + k - 1, d);
    }
  }
  int n_edges = edges.size();
  
  // ===== FIND MST =====
  // Sort list of edges based on their distance
  std::sort(edges.begin(), edges.end(), [](const Edge &a, const Edge &b) {
    return std::get<2>(a) < std::get<2>(b);  
  });
  
  // Find MST using Kruskals Algorithm
  std::vector<Edge*> mst_edges; mst_edges.reserve(n-1);
  boost::disjoint_sets_with_storage<> mst_uf(n);
  int n_components = n;
  for(int i = 0; i < n_edges; ++i) {
    int c1 = mst_uf.find_set(std::get<0>(edges[i]));
    int c2 = mst_uf.find_set(std::get<1>(edges[i]));
    
    if(c1 != c2 ) {
      mst_uf.link(c1, c2);
      mst_edges.push_back(&edges[i]);
      if (--n_components == 1) break;
    }
  }
  
  // Find 2nd best MST by skipping one edge of the MST (otherwise Kruskal)
  int min_mst_weight = MAX_INT;
  for(const Edge *skip_edge : mst_edges) {
    int mst_weight = 0;
    
    boost::disjoint_sets_with_storage<> uf(n);
    int n_components = n;
    for(int i = 0; i < n_edges; ++i) {
      if(&edges[i] == skip_edge) { continue; }
      
      int c1 = uf.find_set(std::get<0>(edges[i]));
      int c2 = uf.find_set(std::get<1>(edges[i]));
      
      if(c1 != c2 ) {
        uf.link(c1, c2);
        mst_weight += std::get<2>(edges[i]);
        if (--n_components == 1) break;
      }
    }
    
    min_mst_weight = std::min(min_mst_weight, mst_weight);
  }
  
  // ===== OUTPUT =====
  std::cout << min_mst_weight << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}