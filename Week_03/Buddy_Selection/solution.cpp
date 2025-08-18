///1
#include<iostream>
#include<vector>
#include<unordered_set>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/max_cardinality_matching.hpp>

using graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using vertex_desc = boost::graph_traits<graph>::vertex_descriptor;


int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    int n, c, f; std::cin >> n >> c >> f;
    
    graph g(n);
    // Store a set for each student containing their characteristics
    std::vector<std::unordered_set<std::string>> students(n, std::unordered_set<std::string>{});
    
    // Build graph where nodes are students and nodes are only connected if students have more than f common characteristics
    for(int i = 0; i < n; i++) {
      for(int j = 0; j < c; j++) {
        std::string s; std::cin >> s;
        students[i].insert(s);
      }
      
      for(int j = 0; j < n; j++) {
        int count = 0;
        
        for(const std::string& x : students[i]) {
          if(students[j].find(x) != students[j].end()) {
            count++;
          }
          
          if(count > f) {
            boost::add_edge(i, j, g);
            break;
          }
        }
      }
    }
    
    // Find maximum matching using kruskals algorithm
    std::vector<vertex_desc> mate(n);
    boost::edmonds_maximum_cardinality_matching(g, boost::make_iterator_property_map(mate.begin(), boost::get(boost::vertex_index, g)));

    // Returns the number of edges in the matching
    int matching_size = boost::matching_size(g, boost::make_iterator_property_map(mate.begin(), boost::get(boost::vertex_index, g)));
    
    std::cout << (2 * matching_size == n ? "not optimal" : "optimal") << "\n";
  }
}