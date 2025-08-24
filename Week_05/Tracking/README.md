# Tracking

## 📝 Problem Description

The objective is to find the minimum travel time for a journey between a designated starting city, $x$, and a final destination city, $y$. The journey takes place over a network of $n$ cities and $m$ connecting roads. Each road has a specific travel time.

A special condition applies to this journey: certain roads are classified as "river roads." Any valid path from city $x$ to city $y$ must traverse at least $k$ of these river roads. A particular river road can be used multiple times, and each traversal counts towards the total of $k$. The output should be the minimum possible time for a journey that satisfies this condition.

## 💡 Hints

<details>
<summary>Hint #1</summary>
This problem is a variation of the classic shortest path problem. What is the standard algorithm for finding the shortest path between two points in a network where all travel times (weights) are non-negative?
</details>
<details>
<summary>Hint #2</summary>
Simply running a standard shortest path algorithm on the cities and roads will not work, as it doesn't account for the constraint of using at least $k$ river roads. The state of our search needs to include more than just the current city. How can we augment the search state to keep track of the number of river roads traversed so far?
</details>
<details>
<summary>Hint #3</summary>
Consider creating a "layered" version of the city network. Imagine having $k+1$ copies, or layers, of the network. Layer $i$ could represent all paths that have used exactly $i$ river roads. How would you define the connections (roads) within a single layer and, more importantly, *between* different layers? Traversing a regular road would keep you in the same layer, while traversing a river road would move you to the next layer.
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1, 2)</summary>
This problem asks for the shortest path from a starting city $x$ to a destination city $y$ with an additional constraint. The presence of non-negative travel times suggests that Dijkstra's algorithm is a suitable foundation for a solution.

A simple application of Dijkstra's algorithm would find the shortest path but would ignore the constraint about using a minimum number of river roads. For the first two test sets, we have the simplifying assumption that $k=1$, meaning exactly one river road must be part of the path.

This suggests that any valid path will look like this: a shortest path from the start city $x$ to one end of a river road (say, city $u$), a traversal across that river road to city $v$, and finally a shortest path from $v$ to the destination city $y$.

We can solve this by decomposing the problem:
1.  **Calculate all-pairs shortest paths from $x$:** Run Dijkstra's algorithm starting from city $x$. This gives us the shortest travel time from $x$ to every other city in the network. Let's call this `dist_from_x`.
2.  **Calculate all-pairs shortest paths from $y$:** Run Dijkstra's algorithm starting from city $y$. For an undirected network, this gives the shortest travel time from any city to $y$. Let's call this `dist_to_y`.
3.  **Combine the paths:** Iterate through every river road in the network. For each river road connecting cities $u$ and $v$ with travel time $w$, we can form a complete, valid path in two ways:
    *   $x \rightarrow \dots \rightarrow u \xrightarrow{\text{river}} v \rightarrow \dots \rightarrow y$. The total time is `dist_from_x[u] + w + dist_to_y[v]`.
    *   $x \rightarrow \dots \rightarrow v \xrightarrow{\text{river}} u \rightarrow \dots \rightarrow y$. The total time is `dist_from_x[v] + w + dist_to_y[u]`.

By calculating these potential path times for every river road and finding the minimum among them, we can determine the shortest path that uses at least one river road.

```cpp
#include<iostream>
#include<vector>
#include<limits>
#include<cmath>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/dijkstra_shortest_paths.hpp>

using EdgeWeight = boost::property<boost::edge_weight_t, int>;
using Graph = boost::adjacency_list<boost::vecS, 
                                    boost::vecS,
                                    boost::undirectedS,
                                    boost::no_property,
                                    EdgeWeight>;
using VertexDescriptor = boost::graph_traits<Graph>::vertex_descriptor;
using EdgeDescriptor = boost::graph_traits<Graph>::edge_descriptor;


int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    // ===== READ INPUT =====
    int n, m, k, x, y; std::cin >> n >> m >> k >> x >> y;
    
    Graph g(n);
    std::vector<std::pair<int, int>> river_edges;
    for(int i = 0; i < m; i++) {
      int a, b, c, d; std::cin >> a >> b >> c >> d;
      boost::add_edge(a, b, EdgeWeight(c), g);
      
      if(d) river_edges.push_back(std::make_pair(a, b));
    }

    // ===== FIND SHORTEST PATH =====
    std::vector<int> start_dist(n);
    std::vector<int> end_dist(n);
    boost::dijkstra_shortest_paths(g, x, boost::distance_map(boost::make_iterator_property_map(start_dist.begin(), boost::get(boost::vertex_index, g))));
    boost::dijkstra_shortest_paths(g, y, boost::distance_map(boost::make_iterator_property_map(end_dist.begin(), boost::get(boost::vertex_index, g))));
    
    int min_dist = std::numeric_limits<int>::max();
    
    for(std::pair<int, int> river_edge : river_edges) {
      int a = river_edge.first;
      int b = river_edge.second;
      int edge_weight = boost::get(boost::edge_weight_t(), g, boost::edge(a, b, g).first);
      
      min_dist = std::min(min_dist, std::min(start_dist[a] + edge_weight + end_dist[b], 
                                             start_dist[b] + edge_weight + end_dist[a]));
    }

    // ===== OUTPUT =====
    std::cout << min_dist << std::endl;
  }
}
```
</details>
<details>
<summary>Final Solution</summary>
The previous approach of running Dijkstra twice and iterating over river roads works well for $k=1$. However, it does not generalize to cases where $k>1$, as we would need to consider all combinations of $k$ river roads, which is computationally infeasible.

The constraint that $k$ is small (up to 10) is a strong hint. This suggests a solution where the complexity might depend on $k$. The key idea is to augment the state in our shortest path search. Instead of just tracking the current city, we also need to track the number of river roads traversed. A state can be defined by the pair `(current_city, num_rivers_used)`.

This leads to the construction of a **layered graph**.
We can imagine creating $k+1$ copies of the original city network, stacked as layers $0, 1, \dots, k$. A node in this new graph can be identified by `(city, layer)`, where `layer` corresponds to the number of river roads used to reach that city.

### Layered Graph Construction
1.  **Nodes:** We create a new, larger graph with $n \times (k+1)$ nodes. A node with index `i * n + u` corresponds to the state `(city u, layer i)`.
2.  **Regular Roads:** A regular road between cities $u$ and $v$ with travel time $w$ does not change the river road count. Therefore, for each layer $i \in [0, k]$, we add an edge between node `(u, i)` and node `(v, i)` with weight $w$.
3.  **River Roads:** A river road between $u$ and $v$ with travel time $w$ is used to transition between layers.
    *   To increment the river road count, for each layer $i \in [0, k-1]$, we add an edge connecting node `(u, i)` to node `(v, i+1)` with weight $w$. Since roads are bidirectional, this connection allows travel from $u$ to $v$ (or $v$ to $u$) while moving from layer $i$ to $i+1$.
    *   To satisfy the "at least $k$" requirement, once we have used $k$ river roads (i.e., we are in layer $k$), any further traversal of a river road should keep us in a valid state. We model this by adding an edge for the river road between `(u, k)` and `(v, k)`. This allows paths to use more than $k$ river roads.

### Finding the Solution
After constructing this layered graph, we can find the answer with a single run of Dijkstra's algorithm. We start the search from the node `(x, 0)`—representing the start city $x$ in layer 0 (having used 0 river roads). The final answer is the shortest distance to the node `(y, k)`. Any path reaching `(y, k)` will have used *exactly* $k$ river roads to transition through the layers, or *more than* $k$ by using additional river roads within layer $k$.

The code below implements this layered graph strategy. It builds the graph with $n \times (k+1)$ vertices and adds edges according to the logic described above. A single Dijkstra run then efficiently finds the required minimum travel time.

```cpp
#include<iostream>
#include<vector>
#include<limits>
#include<cmath>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/dijkstra_shortest_paths.hpp>

using EdgeWeight = boost::property<boost::edge_weight_t, int>;
using Graph = boost::adjacency_list<boost::vecS, 
                                    boost::vecS,
                                    boost::undirectedS,
                                    boost::no_property,
                                    EdgeWeight>;
using VertexDescriptor = boost::graph_traits<Graph>::vertex_descriptor;
using EdgeDescriptor = boost::graph_traits<Graph>::edge_descriptor;


int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    // ===== READ INPUT =====
    int n, m, k, x, y; std::cin >> n >> m >> k >> x >> y;
    
    std::vector<std::tuple<int, int, int>> edges;
    std::vector<std::tuple<int, int, int>> river_edges;
    
    for(int i = 0; i < m; i++) {
      int a, b, c, d; std::cin >> a >> b >> c >> d;
      if(d) { river_edges.emplace_back(a, b, c); }
      else { edges.emplace_back(a, b, c); }
    }

    // ===== FIND SHORTEST PATH =====
    // Construct multi-layered Graph
    Graph g(n * (k + 1));
    
    // Add regular edges
    for(const std::tuple<int, int, int> edge : edges) {
      int s = std::get<0>(edge);
      int t = std::get<1>(edge);
      int w = std::get<2>(edge);
      
      // Add edge in all layers of the graph
      for(int i = 0; i < k + 1; ++i) {
        boost::add_edge(i * n + s, i * n + t, EdgeWeight(w), g);
      }
    }
    
    // Add river edges
    for(const std::tuple<int, int, int> edge : river_edges) {
      int s = std::get<0>(edge);
      int t = std::get<1>(edge);
      int w = std::get<2>(edge);
      
      // Add edge in all layers of the graph
      for(int i = 0; i < k; ++i) {
        boost::add_edge(s +  i      * n, t +  i      * n, EdgeWeight(w), g);
        boost::add_edge(s + (i + 1) * n, t +  i      * n, EdgeWeight(w), g);
        boost::add_edge(s +  i      * n, t + (i + 1) * n, EdgeWeight(w), g);
        boost::add_edge(s + (i + 1) * n, t + (i + 1) * n, EdgeWeight(w), g);
      }
    }
    
    std::vector<int> dist_map(n * (k + 1));
    boost::dijkstra_shortest_paths(g, x, boost::distance_map(boost::make_iterator_property_map(dist_map.begin(), boost::get(boost::vertex_index, g))));
    
    // ===== OUTPUT =====
    std::cout << dist_map[k * n + y] << std::endl;
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1        (15 pts / 0.600 s) : Correct answer      (0.001s)
   Hidden test set 1 (05 pts / 0.600 s) : Correct answer      (0.001s)
   Test set 2        (15 pts / 0.600 s) : Correct answer      (0.015s)
   Hidden test set 2 (05 pts / 0.600 s) : Correct answer      (0.015s)
   Test set 3        (25 pts / 1.800 s) : Correct answer      (0.174s)
   Hidden test set 3 (05 pts / 1.800 s) : Correct answer      (0.174s)
   Test set 4        (25 pts / 1.600 s) : Correct answer      (0.175s)
   Hidden test set 4 (05 pts / 1.600 s) : Correct answer      (0.175s)

Total score: 100
```