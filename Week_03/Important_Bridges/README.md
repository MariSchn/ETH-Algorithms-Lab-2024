# Important Bridges

## 📝 Problem Description

You are given a set of $N$ islands, and a list of $M$ bidirectional bridges between pairs of these islands. The goal is to identify all critical bridges.

A bridge is defined as **critical** if its removal would cause the set of islands to become disconnected. This means that after removing a critical bridge, there would be at least one pair of islands that can no longer reach each other.

## 💡 Hints

<details>

<summary>Hint #1</summary>

Consider a straightforward way to test if a single connection is critical. What happens if you temporarily remove it? How can you check the property of all items still being mutually reachable? If you repeat this test for every single connection, what would be the overall time complexity? Consider if this approach is efficient enough for all constraints.

</details>

<details>

<summary>Hint #2</summary>
This problem can be modeled using graph theory. The items can be represented as vertices and the connections as edges in an undirected graph. The problem then becomes finding all "bridges" (also known as "cut-edges") in the graph. A bridge is an edge whose removal increases the number of connected components of the graph.
</details>

<details>

<summary>Hint #3</summary>

There are efficient, well-known algorithms to find all bridges in a graph in linear time, often based on a Depth-First Search (DFS). Another powerful concept is that of **biconnected components (BCCs)**. The edges of a graph can be partitioned into BCCs. A bridge has a unique relationship with these components. An edge is a bridge if and only if it is in a biconnected component of size one (i.e., the component consists of only that single edge). The Boost Graph Library provides an efficient implementation to find these components.

</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Set 1, 3)</summary>

This solution uses a **brute force approach** to identify critical bridges. The core idea is straightforward: for each edge in the graph, temporarily remove it and check if the graph becomes disconnected.

The algorithm works as follows:
1. **Build the graph** using the Boost Graph Library's `adjacency_list` and store all edges in a separate vector for iteration.
2. **Test each edge individually**: For every edge, remove it from the graph and use `boost::connected_components` to count the number of connected components.
3. **Identify critical edges**: If removing an edge results in more than one connected component, the edge is critical (a bridge).
4. **Restore the edge** and continue testing the next one.

While this approach is conceptually simple and easy to implement, it has a significant drawback in terms of efficiency. The time complexity is $O(M \cdot (N + M))$, where we perform $M$ connectivity checks, each taking $O(N + M)$ time. For large graphs, this becomes too slow, which is why it only passes the smaller test sets.

### Code

```cpp
#include<iostream>
#include<cmath>

#include<boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;
using EdgeIterator = boost::graph_traits<Graph>::edge_iterator; 


int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    // ===== READ INPUT ===== 
    int n, m; std::cin >> n >> m;
    
    Graph g(n);
    std::vector<std::pair<int, int>> edges;
    
    for(int i = 0; i < m; i++) {
      int v, w; std::cin >> v >> w;
      
      boost::add_edge(v, w, g);
      edges.push_back(std::pair<int, int>(v, w));
    }
    
    // ===== FIND CRITICAL EDGES =====
    std::vector<std::pair<int, int>> critical_edges;
    for(const std::pair<int, int>& edge : edges) {
      boost::remove_edge(edge.first, edge.second, g);
      
      std::vector<int> component(n);
      if(boost::connected_components(g, &component[0]) > 1) {
        critical_edges.push_back(std::pair<int, int>(std::min(edge.first, edge.second),
                                                     std::max(edge.first, edge.second)));
      }
      
      boost::add_edge(edge.first, edge.second, g);
    }
    
    // ===== OUTPUT =====
    std::sort(critical_edges.begin(), critical_edges.end());
    std::cout << critical_edges.size() << std::endl;
    for(const std::pair<int, int>& edge : critical_edges) {
      std::cout << edge.first << " " << edge.second << "\n";
    }
  }
}
```

</details>

<details>

<summary>Final Solution</summary>

The problem can be modeled using a graph, where each island is one node and the bridges are edges between the nodes. It remains to get a more formal definition of a critical bridge in graph theory.

### Approach: Biconnected Components

A very effective way to solve this problem is by finding the **biconnected components (BCCs)** of the graph.

-   A **biconnected component** is a maximal subgraph such that it remains connected even after removing any single vertex.
-   The edges of a graph can be partitioned into a set of biconnected components.
-   The key insight is that **an edge is a bridge if and only if it forms a biconnected component by itself**. In other words, a bridge is any edge that does not belong to a larger cycle.

This solution leverages the **Boost Graph Library**, which has a built-in function, `boost::biconnected_components`, to perform this partitioning for us efficiently.

### Implementation Steps

1.  **Graph Representation**: We model the islands and bridges using `boost::adjacency_list`. Each edge is assigned an index from $0$ to $M-1$ so we can reference them easily.

2.  **Finding BCCs**: We call `boost::biconnected_components`. This function takes the graph and a property map (here, `component_map`) as arguments. After execution, `component_map` will store, for each edge, the integer ID of the biconnected component it belongs to. The function returns the total number of BCCs found.

3.  **Identifying Bridges**:
    -   We first need to know the size (number of edges) of each BCC. We create a vector `component_n_edges` and iterate through all edges in the graph. For each edge, we find its component ID from `component_map` and increment the count for that component in `component_n_edges`.
    -   With the sizes calculated, we iterate through all edges one more time. If an edge belongs to a component with a size of exactly `1`, we know it's a bridge. We add this edge to our `critical_edges` list.

4.  **Output**: Finally, the list of critical edges is sorted lexicographically and printed in the required format. This approach has a time complexity of $O(N+M)$, which is highly efficient and passes all test cases.

```cpp
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

bool chech_critical(Graph& g, int src, int target, int n) {
    boost::remove_edge(src, target, g);
    
    std::vector<int> component(n);
    if(boost::connected_components(g, &component[0]) > 1) {
      boost::add_edge(src, target, g);
      return true;
    } else {
      boost::add_edge(src, target, g);
      return false;
    }
    
}

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
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (40 pts / 2 s) : Correct answer      (0.005s)
   Test set 2 (40 pts / 2 s) : Correct answer      (0.384s)
   Test set 3 (20 pts / 2 s) : Correct answer      (0.0s)

Total score: 100
```