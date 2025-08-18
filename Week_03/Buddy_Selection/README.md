# Buddy Selection

## 📝 Problem Description

Given $n$ students, each being defined through $c$ characteristics, the task is to pair the students, such that the minimum amount of shared characteristics between two paired up students is maximized.

Additionally a value $f$ is given, that represents a threshold for the number of shared characteristics in any pair. Based on this value $f$ the output should be `optimal` if the maximum shared characteristics in any pair is less than or equal to $f$, and `not optimal` otherwise.

## 💡 Hints

<details>

<summary>Hint #1</summary>

Think about how to model the problem to be able to use a known algorithm, that can give you pairs, maximizing certain properties.

</details>

<details>

<summary>Hint #2</summary>

We can model this problem as a graph, where each student represents a node. In this graph we can then define the edges such that we only need to find the maximum matching in this graph to solve the problem.

</details>

<details>

<summary>Hint #3</summary>

Note that to give the final output, we do not need to find the actual maximum matching. We only need to determine, whether a matching exists that covers all students and is greater than $f$ in terms of the number of shared characteristics in any pair.

</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>

This problem can be solved by modeling it as a graph problem, specifically by searching for a matching. 

The core idea is to translate the student pairing problem into a graph as the problem in this case becomes a matching problem.
*  **Vertices**: Each of the $N$ students is represented as a vertex in an undirected graph.
*  **Edges**: Edges are added between students based on their number of shared characteristics. Since the problem only requires us to determine whether a matching exists that covers all students and is greater than $f$ in terms of the number of shared characteristics in any pair, we only need to add edges between students who share more than $f$ characteristics.

If there exists a perfect matching (matching that contains all nodes) in this graph, we know that the previous assignment, defined by $f$ is `not optimal` as there exists a matching such that all students share $> f$ characteristics. If there does not exist a matching, the previous assignment was `optimal`. 
To determine whether the matching is perfect or not, we can simply look at the amount number of edges in the matching. The matching is perfect if it contains exactly $n/2$ edges, as each edge connects $2$ students.

**Note**: To efficiently count common characteristics between any two students, we can store each student's characteristics in a set (like `std::unordered_set`). This allows for very fast lookups.


```cpp
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
```
</details>

## ⚡ Result

```plaintext

```