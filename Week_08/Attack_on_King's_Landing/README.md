# Attack on King’s Landing

## 📝 Problem Description

You are given a map of a city, which consists of a set of intersections and a set of roads connecting them. Each road has a specific length and connects two distinct intersections.

Your goal is to make as many roads as possible "safe". A road becomes safe if a barricade is built at *both* of its ends. Troops are available to build these barricades, but they operate under several constraints:
1.  Troops are stationed at a number of specified intersections called "barracks".
2.  A barricade can only be built at an intersection if it is reachable from a barrack within a total travel distance of $d$. The distance between two intersections is the length of the shortest path of roads between them.
3.  Intersections come in two types: regular and plazas.
    *   At a **regular intersection**, at most one barricade can be built.
    *   At a **plaza**, at most two barricades can be built.
4.  No road connects two plazas directly.

Given the layout of the city, the locations of barracks and plazas, and the maximum travel distance $d$, your task is to determine the maximum number of roads that can be made safe.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem describes a network of intersections and roads. This structure is very common in computer science. How can you model this system using a standard abstract data structure? What do the intersections and roads correspond to?
</details>

<details>
<summary>Hint #2</summary>
Making a road safe requires "using" both of its endpoint intersections. At a regular intersection, you can only do this for one road. This sounds like you are selecting a set of roads with a constraint on which intersections they use. What kind of problem involves selecting a set of connections (edges) such that no two selected connections share an endpoint (vertex)?
</details>

<details>
<summary>Hint #3</summary>
The problem has two main complexities beyond the basic model: the distance limit for troops and the special "plaza" intersections that can support two barricades.
- **Distance Limit:** How can you determine which intersections are eligible for building barricades? You need to find all intersections within a certain shortest path distance from *any* barrack. Which algorithm is suitable for finding shortest paths from a source in a network with non-negative path lengths?
- **Plazas:** A regular intersection can be part of at most one safe road, but a plaza can be part of two. How could you modify your abstract model to accommodate this "capacity of two" for plazas, while still using an algorithm designed for a "capacity of one"? Consider what happens if you represent a plaza not as a single entity, but as multiple.
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1)</summary>
For the first test set, the problem is significantly simplified by two assumptions:
1.  The travel distance $d$ is large enough that troops can reach every intersection from a barrack.
2.  There are no plazas ($p=0$), meaning all intersections are regular.

Let's model the city as a **graph**, where intersections are **vertices** and roads are **edges**. To make a road safe, we must build a barricade at both ends. Since all intersections are regular, each can support at most one barricade. This means if we choose to make the road between intersections $u$ and $v$ safe, we cannot use $u$ or $v$ for any other road.

This directly corresponds to the definition of a **matching** in a graph. A matching is a set of edges where no two edges share a common vertex. Making a road `(u, v)` safe is equivalent to including the edge `(u, v)` in our matching. The goal of maximizing the number of safe roads is therefore equivalent to finding a **maximum cardinality matching** in the graph.

A standard algorithm for finding the maximum cardinality matching in a general (non-bipartite) graph is **Edmonds's Blossom Algorithm**. The Boost Graph Library provides an implementation, `boost::edmonds_maximum_cardinality_matching`, which we can use to solve this simplified version of the problem.

The overall approach is:
1.  Construct a graph where $N$ vertices represent the $N$ intersections.
2.  For each of the $M$ roads, add an undirected edge between the corresponding vertices.
3.  Run Edmonds's algorithm on this graph to find the size of the maximum matching.
4.  The result is the size of this matching.

```cpp
#include <iostream>
#include <vector>

// Boost libraries for graph representation and matching
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

// Define a type for our graph
using graph = boost::adjacency_list<boost/vecS,
                                    boost::vecS,
                                    boost::undirectedS>;

void solve() {
  // ===== READ INPUT =====
  int n, m, b, p;
  long long d; // Use long long for distance to be safe
  std::cin >> n >> m >> b >> p >> d;
  
  // For test set 1, b, p, and d are not immediately needed
  // but we must read them to advance the input stream.
  for (int i = 0; i < b; ++i) {
    int barrack_loc;
    std::cin >> barrack_loc;
  }
  // p is 0, so no plazas to read.

  // ===== BUILD GRAPH =====
  graph G(n);
  for (int i = 0; i < m; ++i) {
    int u, v, l;
    std::cin >> u >> v >> l;
    boost::add_edge(u, v, G);
  }
  
  // ===== FIND MAXIMUM MATCHING =====
  // mate_map stores the matching: mate_map[u] = v if (u,v) is in the matching
  std::vector<int> mate_map(n);

  boost::edmonds_maximum_cardinality_matching(G, 
    boost::make_iterator_property_map(mate_map.begin(), boost::get(boost::vertex_index, G)));
  
  // The matching_size function counts the number of matched vertices and divides by 2
  int matching_size = boost::matching_size(G, 
    boost::make_iterator_property_map(mate_map.begin(), boost::get(boost::vertex_index, G)));
  
  // ===== OUTPUT =====
  std::cout << matching_size << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  int t;
  std::cin >> t;
  while (t--) {
    solve();
  }
  return 0;
}
```
</details>

<details>
<summary>Final Solution</summary>
The first solution established that the core of the problem is finding a maximum matching. The full problem requires us to handle two additional constraints: the limited travel distance $d$ and the presence of plazas.

### Handling the Distance Constraint

A barricade can only be built at an intersection if its shortest path distance from *any* barrack is at most $d$. A road can be made safe only if barricades can be built at *both* its endpoints. This means we are interested in finding a maximum matching not on the entire city graph, but on the **subgraph induced by the set of all reachable intersections**.

To find this set of reachable intersections, we can use a shortest path algorithm. Since road lengths are non-negative, **Dijkstra's algorithm** is a perfect fit. We need to find the distance from the set of all barracks to all other intersections. A clean way to do this is to add a "super-source" vertex to the graph, connect it to every barrack with a zero-length edge, and then run Dijkstra once from this super-source. An equally valid approach, as implemented below, is to run Dijkstra from each barrack and find the minimum distance for each intersection over all runs.

After running Dijkstra, we can identify all vertices `v` where `dist[v] <= d`. These are our "active" vertices.

### Handling Plazas

A plaza can support up to two barricades, whereas a regular intersection can support only one. This means a plaza vertex can be an endpoint for up to two edges in our matching. The standard maximum matching algorithm assumes a vertex can only be part of one matched edge.

To model this, we use a technique called **node splitting** (or node duplication). For each plaza, we augment the graph with an additional "virtual" vertex.
- Let's say intersection `v` is a plaza. We create a new vertex `v_dup`.
- The original vertex `v` represents the first "slot" for a barricade at the plaza.
- The new vertex `v_dup` represents the second "slot".
- For any road connecting a regular intersection `u` to the plaza `v`, we keep the edge `(u, v)` and also add a new edge `(u, v_dup)`.

Now, if the matching algorithm picks the edge `(u, v)`, it uses the first slot. If it later picks an edge `(w, v_dup)` for some other neighbor `w`, it uses the second slot. This effectively allows the original plaza `v` to be matched twice, correctly modeling its capacity of two. Since no road connects two plazas, we don't need to consider more complex cases.

### Combined Algorithm

Our final algorithm combines these two ideas:

1.  **Graph Construction:**
    *   Create a graph with $N+P$ vertices, where $N$ is the number of intersections and $P$ is the number of plazas. The extra $P$ vertices will be the duplicates for the plazas.
    *   For each road `(u, v)`:
        *   Add the edge `(u, v)` to the graph.
        *   If `u` is a plaza, add an edge from `v` to `u`'s duplicate vertex.
        *   If `v` is a plaza, add an edge from `u` to `v`'s duplicate vertex.

2.  **Find Reachable Subgraph:**
    *   Run Dijkstra's algorithm starting from all barrack locations to find the shortest distance to every other vertex (including the plaza duplicates).
    *   Identify the set of vertices `S` for which the shortest distance from any barrack is less than or equal to $d$.

3.  **Maximum Matching:**
    *   Filter the graph to keep only the vertices in `S` and the edges between them. The Boost library allows for this by "clearing" the vertices that are not in `S`.
    *   Run `edmonds_maximum_cardinality_matching` on this filtered subgraph.
    *   The resulting matching size is the maximum number of roads that can be made safe.

```cpp
#include <iostream>
#include <vector>
#include <map>

// Boost libraries
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

// Define types for our graph and its properties
using graph = boost::adjacency_list<boost/vecS,
                                    boost::vecS,
                                    boost::undirectedS,
                                    boost::no_property,
                                    boost::property<boost::edge_weight_t, int>>;
using vertex_desc = boost::graph_traits<graph>::vertex_descriptor;

void solve() {
  // ===== READ INPUT =====
  int n, m, b, p;
  long long d;
  std::cin >> n >> m >> b >> p >> d;

  int num_nodes = n + p; // n original intersections + p duplicates for plazas
  graph G(num_nodes);
  
  std::vector<int> barracks(b);
  for(int i = 0; i < b; ++i) std::cin >> barracks[i];
  
  std::vector<int> plaza_locations(p);
  // Map original plaza index to its new duplicate vertex index
  std::map<int, int> plaza_to_dup; 
  for(int i = 0; i < p; ++i) {
    std::cin >> plaza_locations[i];
    plaza_to_dup[plaza_locations[i]] = n + i;
  }

  // ===== BUILD GRAPH WITH PLAZA DUPLICATION =====
  for(int i = 0; i < m; ++i) {
    int u, v, l;
    std::cin >> u >> v >> l;
    boost::add_edge(u, v, l, G);
    
    // If an endpoint is a plaza, connect the other endpoint to the plaza's duplicate as well.
    if(plaza_to_dup.count(u)) {
      boost::add_edge(v, plaza_to_dup[u], l, G);
    }
    if(plaza_to_dup.count(v)) {
      boost::add_edge(u, plaza_to_dup[v], l, G);
    }
  }

  // ===== FIND REACHABLE VERTICES =====
  std::vector<long long> final_dist(num_nodes, -1);

  for(int start_node : barracks) {
    std::vector<long long> dist_map(num_nodes);
    boost::dijkstra_shortest_paths(G, start_node,
        boost::distance_map(boost::make_iterator_property_map(dist_map.begin(), boost::get(boost::vertex_index, G))));
    
    for(int i = 0; i < num_nodes; ++i) {
        if(final_dist[i] == -1 || dist_map[i] < final_dist[i]) {
            final_dist[i] = dist_map[i];
        }
    }
  }
  
  // A road (u,v) can be secured if BOTH u and v are reachable.
  // The matching will be on a subgraph of reachable nodes.
  // We can "remove" unreachable nodes by clearing their edges.
  graph active_G(num_nodes);
  std::vector<bool> reachable(num_nodes, false);

  for(int i = 0; i < n; ++i) {
      if (final_dist[i] != -1 && final_dist[i] <= d) {
          reachable[i] = true;
      }
  }
  // Also check reachability for duplicate plaza nodes
  for(int i = 0; i < p; ++i) {
      if (final_dist[n + i] != -1 && final_dist[n + i] <= d) {
          reachable[n + i] = true;
      }
  }

  // Rebuild graph with only edges where both endpoints are reachable
  auto es = boost::edges(G);
  for (auto eit = es.first; eit != es.second; ++eit) {
      vertex_desc u = boost::source(*eit, G);
      vertex_desc v = boost::target(*eit, G);
      if (reachable[u] && reachable[v]) {
          boost::add_edge(u, v, active_G);
      }
  }

  // ===== FIND MAXIMUM MATCHING ON THE SUBGRAPH =====
  std::vector<vertex_desc> mate_map(num_nodes);
  boost::edmonds_maximum_cardinality_matching(active_G, 
    boost::make_iterator_property_map(mate_map.begin(), boost::get(boost::vertex_index, G)));
  
  int matching_size = boost::matching_size(active_G, 
    boost::make_iterator_property_map(mate_map.begin(), boost::get(boost::vertex_index, G)));
  
  // ===== OUTPUT =====
  std::cout << matching_size << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  int t;
  std::cin >> t;
  while (t--) {
    solve();
  }
  return 0;
}
```
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