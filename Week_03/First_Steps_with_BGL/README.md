# First Steps with BGL

## 📝 Problem Description

Given a graph, the task is to find:

- The total weight of the minimum spanning tree (MST), i.e. the sum of all edge weights contained in the MST.
- The longest (minimum) distance from the starting node (index 0) to any other node.

## 💡 Hints

<details>

<summary>Hint #1</summary>

Instead of implementing MST and single source shortest path algorithms from scratch, the goal is to use the implementations provided by BGL.

The main challenge is to correctly set up the BGL graph data structure with weighted edges and then call the `boost::kruskal_minimum_spanning_tree` and `boost::dijkstra_shortest_paths` functions with the correct parameters. Pay close attention to how BGL uses property maps to handle edge weights and store output distances.

</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>

The problem is pretty straight forward in terms of the algorithms that need to be used, as the problem description essentially already tells us how to solve it. As this problem is ment as an introduction into BGL, the more challenging part of this problem is to setup and find the correct functions from BGL to solve it. Subsequently this solution will focus on the code and not the algorithms themself.

### Creating the Graph

In BGL the type `boost::adjacency_list` is used to represent the graph. We specify the storage for the graph's vertices and edges, as well as any properties we want to associate with them in the type itself, which is why we usually create a `typedef` for the graph.
```cpp
typedef boost::property<boost::edge_weight_t, int> edge_weight;
typedef boost::adjacency_list<
    boost::vecS,        // Use a vector for each nodes neighbors
    boost::vecS,        // Use a vector to store all the neighbor vectors
    boost::undirectedS, // The graph is undirected
    boost::no_property, // No vertex property
    edge_weight         // Add edge weights as an (interior) edge property
> weighted_graph;

typedef boost::graph_traits<weighted_graph>::edge_descriptor edge_desc; // Used later
```

We can then simply set up the graph by creating an instance of `weighted_graph` and adding edges to it using the `boost::add_edge(<source>, <target>, <weight>, <graph>)` function.

**Note**: To create an undirected graph we would simply also use `boost::no_property` for the edge properties in the `typedef` and simply not pass a weight to the `boost::add_edge` function.

### Find MST Total Weight

First we need to find the MST itself. For this we can use `boost::kruskal_minimum_spanning_tree`. To also store the edges of the MST, we need to pass an additional vector to this call, which stores the MST edges. To uniquely identify an edge we use an edge descriptor type which we defined earlier as `edge_desc`. We simply get this type using `boost::graph_traits` which can also provide many other useful types for the graph (type) you pass it. Here we only use it to get the edge descriptor type for our graph. 

This provides us with all the edges of the MST, which we can now use to calculate the total weight. Note, that having all the edges does not mean that we have all their weights. We only know which edges are part of the MST now. Therefore we first get all edges from our graph by using `boost::get` where we specify that we want the `boost::edge_weight` property. Then we can simply iterate over all the MST edges and sum up their weight to get the total MST weight.

### Find Longest Distance

To find the longest distance we naturally use **Dijkstra's algorithm**. This is implemented under `boost::dijkstra_shortest_paths`. Similarly to `boost::kruskal_minimum_spanning_tree`, the call itself does not return you all the distances. We need to manually create an exterior property map which stores the distances for each node. Therefore, we create a `std::vector<int>` of length `n` to hold the distances. We can then pass this vector as a parameter to the `boost::dijkstra_shortest_paths` function by making it into a property map. See the code below to see details on this (but no need to memorize it).

After filling the distance map by calling `boost::dijkstra_shortest_paths` we can simply determine its maximum.

```cpp
#include<iostream>
#include<vector>
#include<cmath>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/kruskal_min_spanning_tree.hpp>
#include<boost/graph/dijkstra_shortest_paths.hpp>

typedef boost::property<boost::edge_weight_t, int> edge_weight;
typedef boost::adjacency_list<boost::vecS, // Use a vector for each nodes neighbors
                              boost::vecS, // Use a vector to store all the neighbor vectors
                              boost::undirectedS, // The graph is undirected
                              boost::no_property, // No vertex property
                              edge_weight // Add edge weights as an (interior) edge property
                             > weighted_graph;
typedef boost::graph_traits<weighted_graph>::edge_descriptor edge_desc;


int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    int n, m; std::cin >> n >> m;
    
    // Create and read graph
    weighted_graph g(n);
    for(int i = 0; i < m; i++) {
      int x, y, w; std::cin >> x >> y >> w;
      boost::add_edge(x, y, edge_weight(w), g);
    }
    
    // Execute Kruskals Algorithm
    std::vector<edge_desc> mst;
    boost::kruskal_minimum_spanning_tree(g, std::back_inserter(mst));
    
    boost::property_map<weighted_graph, boost::edge_weight_t>::type weight_map = boost::get(boost::edge_weight, g); // Get all the edge weights from the graph
    
    int mst_weight_sum = 0;
    for(std::vector<edge_desc>::iterator it = mst.begin(); it != mst.end(); ++it) {
      mst_weight_sum += weight_map[*it];
    }
    
    // Calculate the distance from node 0 to all other nodes
    std::vector<int> dist_map(n); // Create the exterior property to store the distance to node 0 for each node
    
    boost::dijkstra_shortest_paths(g, 0, // Pass the graph and the source node
                                   boost::distance_map(boost::make_iterator_property_map(dist_map.begin(),
                                                                                         boost::get(boost::vertex_index, g)
                                                                                         )));
    
    int max_dist = 0;
    for(const int& dist : dist_map) {
      max_dist = std::max(max_dist, dist);
    }
    
    std::cout << mst_weight_sum << " " << max_dist << std::endl;
  }
}
```
</details>

## ⚡ Result

```plaintext

```