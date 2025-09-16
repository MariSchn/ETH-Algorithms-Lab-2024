# Attack on King’s Landing

## 📝 Problem Description

A map of a city consists of a set of intersections and a set of roads connecting them. The objective is to maximize the number of roads that are made "safe." A road is considered safe if barricades are built at both of its endpoints. Troops are available to construct these barricades, subject to several constraints:
1. Troops are stationed at designated intersections known as "barracks."
2. A barricade may be constructed at an intersection only if it is reachable from a barrack within a total travel distance of $d$, where the distance between intersections is defined as the length of the shortest path of roads connecting them.
3. Intersections are classified as either regular or plazas:
    * At a regular intersection, at most one barricade may be constructed.
    * At a plaza, up to two barricades may be constructed.
4. No road directly connects two plazas.

Given the city's layout, the locations of barracks and plazas, and the maximum travel distance $d$, determine the maximum number of roads that can be made safe.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem describes a network of intersections and roads. This structure is very common in computer science. How can you model this system using a standard abstract data structure? What do the intersections and roads correspond to?

</details>

<details>

<summary>Hint #2</summary>

Making a road safe requires "using" both of its endpoint intersections. At a regular intersection, you can only do this for one road. This sounds like you are selecting a set of roads with a constraint on which intersections they use. What kind of problem involves selecting a set of connections (edges) such that no two selected connections share an endpoint (node)?

  <details>

  <summary>Answer</summary>

  This is a classic **matching problem** in graph theory. Specifically, it resembles the **maximum matching** problem, where we want to find the largest set of edges such that no two edges share a common vertex. In our case, the vertices are the intersections, and the edges are the roads.

  </details>

</details>

<details>

<summary>Hint #3</summary>

The problem has two main complexities beyond the basic model: the distance limit for troops and the special "plaza" intersections that can support two barricades.
- **Distance Limit:** How can you determine which intersections are eligible for building barricades? You need to find all intersections within a certain shortest path distance from *any* barrack. Which algorithm is suitable for finding shortest paths from a source in a network with non-negative path lengths?
Once you know this you can simply run your approach on the "filtered/reachable/valid graph".
- **Plazas:** A regular intersection can be part of at most one safe road, but a plaza can be part of two. How could you modify your abstract model to accommodate this "capacity of two" for plazas, while still using an algorithm designed for a "capacity of one"? Consider what happens if you represent a plaza not as a single "entity", but as multiple.

</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Set 1)</summary>

For the first test set, the problem is significantly simplified by two assumptions:
1.  The travel distance $d$ is large enough that troops can reach every intersection from a barrack.
2.  There are no plazas ($p=0$), meaning all intersections are regular.

Let's model the city as a **graph**, where intersections are **nodes** and roads are **edges**. To make a road safe, we must build a barricade at both ends. Since all intersections are regular, each can support at most one barricade. This means if we choose to make the road between intersections $u$ and $v$ safe, we cannot use $u$ or $v$ for any other road.

This directly corresponds to the definition of a **matching** in a graph. A matching is a set of edges where no two edges share a common node. Making a road `(u, v)` safe is equivalent to including the edge `(u, v)` in our matching. The goal of maximizing the number of safe roads is therefore equivalent to finding a **maximum cardinality matching** in the graph.
**Note**: We can easily do this using `boost::edmonds_maximum_cardinality_matching` in Boost

The overall approach is:
1.  Construct a graph where $N$ nodes represent the $N$ intersections.
2.  For each of the $M$ roads, add an undirected edge between the corresponding nodes.
3.  Run Edmonds's algorithm on this graph to find the size of the maximum matching.
4.  The result is the size of this matching.

The main limitation of this approach lies in the two assumptions made. To generalize this to solve all tests sets we mainly need to lift these.

### Code
```cpp
#include<iostream>
#include<vector>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/push_relabel_max_flow.hpp>
#include<boost/graph/max_cardinality_matching.hpp>

using traits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
using graph = boost::adjacency_list<boost::vecS,
                                    boost::vecS,
                                    boost::undirectedS>;
using vertex_desc = traits::vertex_descriptor;
using edge_desc = traits::edge_descriptor;

void solve() {
  // ===== READ INPUT & BUILD GRAPH =====
  int n, m, b, p, d; std::cin >> n >> m >> b >> p >> d;
  
  graph G(n);
  std::vector<int> barracks(b);
  std::vector<int> plazas(p);
  
  for(int i = 0; i < b; ++i) std::cin >> barracks[i];
  for(int i = 0; i < p; ++i) std::cin >> plazas[i];
  for(int i = 0; i < m; ++i) {
    int x, y, l; std::cin >> x >> y >> l;
    boost::add_edge(x, y, G);
  }
  
  // ===== FIND MAXIMUM MATCHING =====
  std::vector<int> mate_map(n); // exterior property map

  boost::edmonds_maximum_cardinality_matching(G, boost::make_iterator_property_map(mate_map.begin(), boost::get(boost::vertex_index, G)));
  int matching_size = boost::matching_size(G, boost::make_iterator_property_map(mate_map.begin(), boost::get(boost::vertex_index, G)));
  
  // ===== OUTPUT =====
  std::cout << matching_size << std::endl;
  
}

int main() {
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}

```
</details>

<details>

<summary>Final Solution</summary>

The first solution established that the core of the problem is finding a maximum matching. The full problem requires us to handle two additional constraints: the limited travel distance $d$ and the plazas.

### Handling the Distance Constraint

A barricade can only be built at an intersection if its shortest path distance from *any* barrack is at most $d$. A road can be made safe only if barricades can be built at *both* its endpoints. This means we are interested in finding a maximum matching not on the entire city graph, but on the **subgraph induced by the set of all reachable intersections**.

To determine which intersections are reachable within the allowed distance, we run Dijkstra's algorithm separately from each barrack. For every intersection, we record the minimum distance obtained across all runs. Intersections with a minimum distance less than or equal to $d$ are eligible for barricade construction.
**Note:** An alternative method is to introduce a "super-source" node connected to all barracks with zero-length edges, then run Dijkstra's algorithm once from this super-source.

After running Dijkstra, we can identify all nodes `v` where `dist[v] <= d`. These are our "active" nodes.

### Handling Plazas

A plaza can support up to two barricades, whereas a regular intersection can support only one. This means a plaza node can be an endpoint for up to two edges in our matching. The standard maximum matching algorithm assumes a node can only be part of one matched edge.

To model this, we plaza, we augment the graph with an additional "virtual" node.
- Let's say intersection `v` is a plaza. We create a new node `v_dup`.
- The original node `v` represents the first "slot" for a barricade at the plaza.
- The new node `v_dup` represents the second "slot".
- For any road connecting a regular intersection `u` to the plaza `v`, we keep the edge `(u, v)` and also add a new edge `(u, v_dup)`.

Now, if the matching algorithm picks the edge `(u, v)`, it uses the first slot. If it later picks an edge `(w, v_dup)` for some other neighbor `w`, it uses the second slot. This effectively allows the original plaza `v` to be matched twice, correctly modeling its capacity of two. Since no road connects two plazas, we don't need to consider more complex cases.

### Combined Algorithm

The final algorithm combines these two ideas:

1.  **Graph Construction:**
    *   Create a graph with $N+P$ nodes, where $N$ is the number of intersections and $P$ is the number of plazas. The extra $P$ nodes will be the duplicates for the plazas.
    *   For each road `(u, v)`:
        *   Add the edge `(u, v)` to the graph.
        *   If `u` is a plaza, add an edge from `v` to `u`'s duplicate node.
        *   If `v` is a plaza, add an edge from `u` to `v`'s duplicate node.

2.  **Find Reachable Subgraph:**
    *   Run Dijkstra's algorithm starting from all barrack locations to find the shortest distance to every other node (including the plaza duplicates).
    *   Identify the set of nodes `S` for which the shortest distance from any barrack is less than or equal to $d$.

3.  **Maximum Matching:**
    *   Filter the graph to keep only the nodes in `S` and the edges between them. The Boost library allows for this by "clearing" the nodes that are not in `S`.
    *   Run `edmonds_maximum_cardinality_matching` on this filtered subgraph.
    *   The resulting matching size is the maximum number of roads that can be made safe.

### Code
```cpp
#include<iostream>
#include<vector>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include<boost/graph/max_cardinality_matching.hpp>

using traits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
using graph = boost::adjacency_list<boost::vecS,
                                    boost::vecS,
                                    boost::undirectedS,
                                    boost::no_property,
                                    boost::property<boost::edge_weight_t, int>>;
using vertex_desc = traits::vertex_descriptor;
using edge_desc = traits::edge_descriptor;


void solve() {
  // ===== READ INPUT & BUILD GRAPH =====
  int n, m, b, p, d; std::cin >> n >> m >> b >> p >> d;
  int num_nodes = n + p;
  
  graph G(num_nodes);
  std::vector<int> barracks(b);
  std::vector<int> plazas(p);
  std::vector<bool> covered(num_nodes, false);
  
  for(int i = 0; i < b; ++i) std::cin >> barracks[i];
  for(int i = 0; i < p; ++i) std::cin >> plazas[i];
  for(int i = 0; i < m; ++i) {
    int x, y, l; std::cin >> x >> y >> l;
    boost::add_edge(x, y, l, G);
    
    // Duplicate plaza nodes
    // Could be more efficient as we check this for every single edge
    // But is fast enough
    for(int j = 0; j < p; ++j) {
      if(x == plazas[j]) {
        boost::add_edge(y, n + j, l, G);
      } else if (y == plazas[j]) {
        boost::add_edge(x, n + j, l, G);
      }
    }
  }

  // ===== FIND SUBGRAPH THAT IS COVERED BY BARRACKS =====
  for(int i = 0; i < b; ++i) {
    std::vector<int> dist_map(num_nodes); //exterior property
    boost::dijkstra_shortest_paths(G, barracks[i], boost::distance_map(boost::make_iterator_property_map(dist_map.begin(), boost::get(boost::vertex_index, G))));
  
    for(int j = 0; j < num_nodes; ++j) {
      if(dist_map[j] <= d) {
        covered[j] = true;
      }
    }
  }

  for(int i = 0; i < num_nodes; ++i) {
    if(!covered[i]) {
      boost::clear_vertex(i, G);
    }
  }

  // ===== FIND MAXIMUM MATCHING =====
  std::vector<int> mate_map(num_nodes);

  boost::edmonds_maximum_cardinality_matching(G, boost::make_iterator_property_map(mate_map.begin(), boost::get(boost::vertex_index, G)));
  int matching_size = boost::matching_size(G, boost::make_iterator_property_map(mate_map.begin(), boost::get(boost::vertex_index, G)));
  
  // ===== OUTPUT =====
  std::cout << matching_size << std::endl;
}

int main() {
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}
```
</details>

## 🧠 Learnings

<details> 

<summary> Expand to View </summary>

- Whether you use a directed or undirected Graph can have a significant impact on runtime.
- Depending on the task `boost::clear_vertex` can delete a node (by removing all of its edges) without the pain of actually deleting nodes in Boost

</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1  (15 pts / 1 s) : Correct answer      (0.463s)
   Test set 1h (05 pts / 1 s) : Correct answer      (0.485s)
   Test set 2  (25 pts / 1 s) : Correct answer      (0.059s)
   Test set 2h (05 pts / 1 s) : Correct answer      (0.059s)
   Test set 3  (15 pts / 1 s) : Correct answer      (0.496s)
   Test set 3h (05 pts / 1 s) : Correct answer      (0.464s)
   Test set 4  (25 pts / 1 s) : Correct answer      (0.683s)
   Test set 4h (05 pts / 1 s) : Correct answer      (0.668s)

Total score: 100
```