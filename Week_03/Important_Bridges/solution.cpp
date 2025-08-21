#include<iostream>
#include<cmath>

#include<boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/biconnected_components.hpp>

using EdgeIndex = boost::property<boost::edge_index_t, int>;
using Graph = boost::adjacency_list<boost::vecS, 
                                    boost::vecS, 
                                    boost::undirectedS, 
                                    boost::no_property, 
                                    EdgeIndex>;
using EdgeIterator = boost::graph_traits<Graph>::edge_iterator; 

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    // ===== READ INPUT ===== 
    int n, m; std::cin >> n >> m;
    
    Graph g(n);
    
    for(int i = 0; i < m; i++) {
      int v, w; std::cin >> v >> w;
      
      boost::add_edge(v, w, EdgeIndex(i), g);
    }
    
    // ===== FIND CRITICAL EDGES =====
    std::vector<int> edge_component(m);
    auto component_map = boost::make_iterator_property_map(edge_component.begin(), boost::get(boost::edge_index, g));
    
    int n_components = boost::biconnected_components(g, component_map);
    
    std::vector<int> component_n_edges(n_components);
    EdgeIterator e_beg, e_end;
    for(boost::tie(e_beg, e_end) = boost::edges(g); e_beg != e_end; ++e_beg) {
      component_n_edges[component_map[*e_beg]]++;
    }
    
    std::vector<std::pair<int, int>> critical_edges;
    for(boost::tie(e_beg, e_end) = boost::edges(g); e_beg != e_end; ++e_beg) {
      if(component_n_edges[component_map[*e_beg]] == 1) {
        critical_edges.push_back(std::pair<int, int>(std::min(boost::source(*e_beg, g), boost::target(*e_beg, g)),
                                                     std::max(boost::source(*e_beg, g), boost::target(*e_beg, g))));
      }
    }

    // ===== OUTPUT =====
    std::sort(critical_edges.begin(), critical_edges.end());
    std::cout << critical_edges.size() << std::endl;
    for(const std::pair<int, int>& edge : critical_edges) {
      std::cout << edge.first << " " << edge.second << "\n";
    }
  }
}