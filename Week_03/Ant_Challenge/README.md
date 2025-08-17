# Ant Challenge

## 📝 Problem Description

You are given a graph consisting of $N$ trees connected through directed edges with travel times. There are $S$ different species of insects in this forest/graph.

Each species has its own "private network" of routes. This network is established through an exploration process starting from the species' designated hive tree. The network expands one tree at a time by choosing the edge that is quickest to reach from the territory already explored. This process continues until all $N$ trees are part of the network.

The task is to determine the minimum time required to transport a breadcrumb from a given start tree $a$ to a finish tree $b$. The journey can utilize routes from any species' private network. If a particular route is part of multiple private networks, the carrier can be switched to the species that can traverse that route the fastest.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem describes a specific process for how each species builds its "private network": "starting from its hive located at a particular tree, each species explored new trees one at a time. The next tree to be explored was always the one fastest to be reached from the already explored territory."

Does this method of network construction remind you of a classic algorithm for finding a special kind of subgraph?

</details>

<details>

<summary>Hint #2</summary>

The algorithm which is used to construct the private networks is **Prim's algorithm**. This implies that the private network for each species is a Minimum Spanning Tree (MST) of the forest, where the edge weights are the travel times for that specific species.

Once you have these $S$ different MSTs (one for each species), the next step is to figure out how to combine them to find the overall fastest path.

</details>

<details>

<summary>Hint #3</summary>

For every edge between two trees, you need to consider the travel times for all species, which have this edge in their private network. This means for every edge, we have a list of possible travel times we could use for this edge. As we want to find the fastest route, we should keep track of the minimum travel time for each edge across all species.

</details>

<details>

<summary>Hint #4</summary>

We can create a new graph where we include all the edges from the private networks of all species, but with weights that reflect the minimum travel time for each edge across all species. This combined graph will allow us to find the fastest route from the start tree to the finish tree using Dijkstra's algorithm.

</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
This problem can be solved by breaking it down into three main parts:

1.  For each species, determine its "private network".
2.  Combine these private networks into a single, optimized network.
3.  Find the shortest path from the start to the finish tree in this combined network.

### Step 1: Modeling the Private Networks

The problem describes how each species forms its network: starting from its hive, it always expands to the nearest unexplored tree. This is precisely the definition of **Prim's algorithm** for finding a **Minimum Spanning Tree (MST)**. Therefore, for each of the $S$ species, we can model their private network as the MST of the forest graph, using that species' specific travel times as edge weights.

In code we can easily obtain all the different MSTs, by creating a separate graph for each species and running Prim's algorithm on them with the given hive node.

### Step 2: Combining the Networks

The journey from tree $a$ to tree $b$ can use any edge that belongs to *at least one* species' private network (i.e., its MST). If an edge `(u, v)` is in the MST of multiple species, we are allowed to use the species that traverses it the fastest.

This leads to the construction of a new, combined graph.
-   The **vertices** of this graph are the $N$ trees.
-   An **edge** `(u, v)` exists in this combined graph if it is part of the MST for at least one species.
-   The **weight** of an edge `(u, v)` in this combined graph is the minimum travel time for that edge across all species whose MST includes it.

In code we just loop over all the edges from the MSTs for all species and only take the minimum edge and add it to the new graph. Note to actually get the edges from the MST in `boost` we use a predecessor map, which for every node gives us its predecessor (if the node was discovered), which we can then use to look up the weight/travel time for the edge.

### Step 3: Finding the Shortest Path

After constructing the combined graph with the optimized edge weights, the original problem is reduced to finding the shortest path from the start vertex $a$ to the destination vertex $b$. This is a classic application of **Dijkstra's algorithm**.

```cpp
#include<iostream>
#include<vector>
#include<cmath>
#include<limits>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/prim_minimum_spanning_tree.hpp>
#include<boost/graph/dijkstra_shortest_paths.hpp>

using edge_weight = boost::property<boost::edge_weight_t, int>;
using graph = boost::adjacency_list<boost::vecS, 
                                    boost::vecS, 
                                    boost::undirectedS, 
                                    boost::no_property,
                                    edge_weight>;
using edge_desc = boost::graph_traits<graph>::edge_descriptor;
using vertex_desc = boost::graph_traits<graph>::vertex_descriptor;

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    int n, e, s, a, b; std::cin >> n >> e >> s >> a >> b;
    std::vector<graph> graphs(s, graph(n));
    std::vector<std::vector<std::vector<int>>> weights(n, std::vector<std::vector<int>>(n, std::vector<int>(s, std::numeric_limits<int>::max())));
    
    for(int edge_idx = 0; edge_idx < e; edge_idx++) {
      int t1, t2; std::cin >> t1 >> t2;
      
      for(int species = 0; species < s; species++) {
        int w; std::cin >> w;
        weights[t1][t2][species] = weights[t2][t1][species] = w;
        boost::add_edge(t1, t2, edge_weight(w), graphs[species]);
      }
    }
    
    std::vector<int> hives(s);
    for(int species = 0; species < s; species++) {
      int h; std::cin >> h;
      hives[species] = s;
    }
    
    // Use Prims algorithm to create MST from the hives to get species networks
    std::vector<std::vector<int>> min_weights(n, std::vector<int>(n, std::numeric_limits<int>::max()));
    
    for(int species = 0; species < s; species++) {
      std::vector<int> pred_map(n);
      
      boost::prim_minimum_spanning_tree(graphs[species], 
                                        boost::make_iterator_property_map(pred_map.begin(), boost::get(boost::vertex_index, graphs[species])), 
                                        boost::root_vertex(hives[species]));
                                        
      for(int i = 0; i < n; i++) {
        if(i != pred_map[i]) {
          min_weights[i][pred_map[i]] = min_weights[pred_map[i]][i] = std::min(min_weights[i][pred_map[i]], weights[i][pred_map[i]][species]);
        }
      }
    }
    
    graph min_graph(n);
    for(int i = 0; i < n; i++) {
      for(int j = 0; j < n; j++) {
        if(min_weights[i][j] != std::numeric_limits<int>::max()) {
          boost::add_edge(i, j, min_weights[i][j], min_graph);
        }
      }
    }
    
    std::vector<int> dist_map(n);
    dijkstra_shortest_paths(min_graph, boost::vertex(a, min_graph), boost::distance_map(boost::make_iterator_property_map(dist_map.begin(), boost::get(boost::vertex_index, min_graph))));
  
    std::cout << dist_map[b] << std::endl;
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Testing solution >>>>
   Test set 1 (20 pts / 1 s) : Correct answer      (0.351s)
   Test set 2 (20 pts / 1 s) : Correct answer      (0.383s)
   Test set 3 (20 pts / 1 s) : Correct answer      (0.258s)
   Test set 4 (40 pts / 1 s) : Correct answer      (0.379s)

Total score: 100
```