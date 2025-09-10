# Astérix and the Tour of Gaul

## 📝 Problem Description

Devise a transportation plan for a selection of food items along a fixed route to achieve the maximum possible total significance.

The route consists of $n$ stops, labeled $s_0, s_1, \ldots, s_{n-1}$. For each leg of the journey between consecutive stops $s_i$ and $s_{i+1}$, a capacity limit $c_i$ specifies the maximum number of items that can be carried simultaneously on that segment.

A list of $m$ potential food items is provided. Each item is defined by a pickup stop $s_a$, a drop-off stop $s_b$, and a cultural significance value $d$. At most one of each listed item may be transported. Upon arrival at a stop, any items destined for that stop are unloaded before new items are picked up.

The task is to select a subset of the $m$ items for transport so that the sum of their significance values is maximized, while ensuring that the capacity constraint $c_i$ is not violated on any segment of the journey. The output should be the maximum possible total significance.

## 💡 Hints

<details>

<summary>Hint #1</summary>

This problem can be solved efficiently using a Max Flow Min Cost algorithm. The goal is to maximize the total significance of transported items, while respecting the capacity constraints on each segment of the route.

</details>

<details>

<summary>Hint #2</summary>

To model the problem as a flow network, start by representing each stop as a node. The main challenge is in constructing the edges: for each segment between stops, add an edge with capacity equal to the segment's limit. For each item, think about how to represent its possible transport as an edge in the graph.

</details>

<details>

<summary>Hint #3</summary>

The graph should include edges between consecutive stops to enforce the carrying capacity, and for each item, an edge from its pickup to its drop-off stop. The item edge should have capacity 1 (since each item can be transported at most once) and a cost that reflects its significance. How do these edges interact to ensure the solution respects all constraints?

</details>

<details>

<summary>Hint #4</summary>

To construct the graph, create a node for each stop along the route. For every segment between stops $s_i$ and $s_{i+1}$, add a directed edge from node $i$ to node $i+1$ with capacity $c_i$ (the segment's limit). For each item, add a directed edge from its pickup stop $a$ to its drop-off stop $b$ with capacity 1 and cost $-d$ (or transformed cost if using only non-negative weights). Add a source node and connect it to every stop $i$ (except the last) with capacity $c_i$, and add a sink node connected from every stop $i+1$ (except the first) with capacity $c_i$. This setup ensures that the flow models the transportation of items and enforces all capacity constraints.

</details>

<details>

<summary>Hint #5</summary>

Given the input constraints ($n$ up to 300, $m$ up to $10^5$), adding an edge for every item can make the graph too large and slow and might not be even necessary. Notice that the number of items that can be transported between any two stops is limited by the minimum capacity along the path. If there are more items for a route than its bottleneck capacity, you only need to consider the most significant ones. Pruning unnecessary items before building the graph is essential for passing all test sets efficiently.

</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Set 1, 2, 3, 4)</summary>

This problem asks us to maximize a total value under capacity constraints, which strongly suggests a network flow formulation. Specifically, it can be modeled as a **Max Flow Min Cost** problem. The general idea is to associate the *significance* of an item with a negative *cost* in the flow network. By finding a flow that minimizes the total cost, we effectively maximize the total significance.

### Graph Construction

To model this, we construct a directed graph with the following components:
- **Nodes**: We create one node for each of the $n$ stops, plus a global source node `S` and a global sink node `T`.
- **Edges**:
    - **Path Edges**: For each segment of the tour from stop $s_i$ to $s_{i+1}$, we add an edge from node $i$ to node $i+1$. The capacity of this edge is set to $c_i$, directly modeling the transportation limit for that leg of the journey.
    - **Item Edges**: For each food item, defined by a pickup stop $a$, drop-off stop $b$, and significance $d$, we add an edge from node $a$ to node $b$. The capacity of this edge is 1, as we can transport at most one of each specific item. The cost of this edge (intuitively) is $-d$.

### Why Does This Graph Construction Work?

The key insight is that the flow in this network models the transportation of items along the route, while the capacities on the edges enforce the real-world constraints: you can never carry more items than the segment allows. By connecting each stop to the next with a capacity equal to the segment limit, you ensure that the total number of items being transported at any time does not exceed the allowed capacity.

Item edges represent the option to transport a specific item from its pickup to its drop-off stop. Each item edge has capacity 1 (since you can only transport one of each item) and a cost that reflects its significance. When the flow algorithm chooses to use an item edge, it is "selecting" that item for transport, and the negative cost (or cost saving) increases the total significance.

The source and sink connections allow the flow to enter and exit the network at the appropriate points, matching the capacity constraints of the route. The flow must respect all capacities, so the solution automatically ensures that no segment is overloaded and no item is transported more than once.

If you imagine the flow without any item edges, it simply moves along the path edges, respecting the segment capacities, and the total cost is zero. When item edges are present, the flow can "shortcut" the path, picking up and dropping off items, and the cost is reduced by the significance of the items chosen. The algorithm naturally finds the optimal set of items to transport, maximizing the total significance while respecting all constraints.

### Handling Costs and Negative Weights

The `successive_shortest_path_nonnegative_weights` algorithm, which is efficient, requires all edge weights (costs) to be non-negative. However, our goal to maximize significance naturally leads to negative costs (cost = $-d$). To address this, we perform a **cost transformation**.

Let $M$ be a large constant, greater than the maximum possible significance of an item. In the code, this is `MAX_SIGNIFICANCE = 256`.

1.  **Path Edge Cost**: We assign a cost of $M$ to each path edge from node $i$ to node $i+1$. Traveling from stop $a$ to $b$ along the main path would therefore incur a cost of $M \cdot (b-a)$.

2.  **Item Edge Cost**: For an item transported from $a$ to $b$ with significance $d$, we set the cost of its corresponding edge to $M \cdot (b-a) - d$.

This transformation ensures all edge costs are non-negative (since $M > d$). More importantly, it cleverly encodes the significance as a *cost saving*. For a trip from $a$ to $b$, choosing the direct item edge is $d$ units cheaper than traversing the corresponding path edges. The algorithm will therefore prioritize paths that use item edges with high significance to minimize the total cost.

The final maximum significance is calculated as `flow * MAX_SIGNIFICANCE - cost`. This formula effectively reverses the cost transformation to extract the sum of significances.

### Code
```cpp
// ===== STD INCLUDES =====
#include <iostream>
#include <vector>

// ===== BGL INCLUDES =====
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

// ===== BGL TYPEDEFS =====
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
                              boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor,
                boost::property <boost::edge_weight_t, long> > > > > graph;

typedef boost::graph_traits<graph>::vertex_descriptor           vertex_desc;
typedef boost::graph_traits<graph>::edge_descriptor             edge_desc;
typedef boost::graph_traits<graph>::out_edge_iterator           out_edge_it;

class edge_adder {
 graph &G;

 public:
  explicit edge_adder(graph &G) : G(G) {}
  void add_edge(int from, int to, long capacity, long cost) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    auto w_map = boost::get(boost::edge_weight, G); // new!
    const edge_desc e = boost::add_edge(from, to, G).first;
    const edge_desc rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
    w_map[e] = cost;   // new assign cost
    w_map[rev_e] = -cost;   // new negative cost
  }
};

int MAX_SIGNIFICANCE = 2 << 7;

void solve() {
  // ===== READ INPUT =====
  int n, m; std::cin >> n >> m;
  
  std::vector<int> capacities(n - 1);
  for(int i = 0; i < n - 1; ++i) { std::cin >> capacities[i]; }
  
  std::vector<std::vector<int>> items(m, std::vector<int>(3));
  for(int i = 0; i < m; ++i) { std::cin >> items[i][0] >> items[i][1] >> items[i][2]; }
  
  // ===== BUILD GRAPH =====
  graph G(n);
  edge_adder adder(G);  
  auto c_map = boost::get(boost::edge_capacity, G);
  auto r_map = boost::get(boost::edge_reverse, G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  
  // Add source and sink
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  // Add source and sink connections
  for(int i = 0; i < n - 1; ++i) { 
    adder.add_edge(v_source, i, capacities[i], 0);
    adder.add_edge(i + 1, v_sink, capacities[i], 0);
  }
  
  // Add edges between subsequent nodes
  for(int i = 0; i < n - 1; ++i) { 
    adder.add_edge(i, i+1, capacities[i], MAX_SIGNIFICANCE); 
  }
  
  // Add edges for items
  for(int i = 0; i < m; ++i) {
    adder.add_edge(items[i][0], items[i][1], 1, MAX_SIGNIFICANCE * (items[i][1] - items[i][0]) - items[i][2]); 
  }
  
  // ===== SOLVE =====
    boost::successive_shortest_path_nonnegative_weights(G, v_source, v_sink);
    int cost = boost::find_flow_cost(G);
    
    int flow = 0;
    out_edge_it e, eend;
    for(boost::tie(e, eend) = boost::out_edges(boost::vertex(v_source,G), G); e != eend; ++e) {
        flow += c_map[*e] - rc_map[*e];     
    }
  
  std::cout << flow * MAX_SIGNIFICANCE - cost << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}
```
</details>

<details>

<summary>Final Solution</summary>

The previous Max Flow Min Cost solution is correct, but its performance suffers when the number of available items ($m$) is large (e.g., up to $10^5$). Creating an edge for every single item results in a graph that is too large, causing the algorithm to exceed the time limit on the final test set.

### Optimization: Pruning Useless Items

The key observation for optimization is that we don't need to consider all $m$ items. The number of items that can be transported between any two stops $a$ and $b$ is fundamentally limited by the "bottleneck capacity" of the path between them. This bottleneck is the minimum capacity of all segments on the path from $a$ to $b$.
Let $c'_{ab} = \min_{i=a}^{b-1} \{c_i\}$. We can transport at most $c'_{ab}$ items from stop $a$ to stop $b$.

If there are more than $c'_{ab}$ items available for the route from $a$ to $b$, it's impossible to transport all of them. Since our goal is to maximize total significance, it is always optimal to choose the $c'_{ab}$ items with the highest significance values and ignore the rest.

### Optimized Algorithm

This insight leads to a pre-processing step before building the graph:
1.  **Group Items**: Instead of processing items one by one, we first group all available items by their pickup and drop-off stops $(a, b)$. A 3D vector or a map can be used for this.
2.  **Filter by Bottleneck Capacity**: For each pair of stops $(a, b)$ that has items associated with it:
    1. Calculate the bottleneck capacity $c'_{ab} = \min_{i=a}^{b-1} \{c_i\}$.
    2. If the number of items for this $(a, b)$ pair exceeds $c'_{ab}$, sort them by significance in descending order.
    3. Select only the top $c'_{ab}$ items with the highest significance. Discard the others as they could never be chosen in an optimal solution.
3.  **Build Graph**: Construct the same Max Flow Min Cost graph as in the first solution, but **only add item edges for the filtered, high-significance items**.

This optimization dramatically reduces the number of edges in the graph, especially when $m$ is large and the capacities $c_i$ are small. The resulting smaller graph can be solved much more quickly by the Max Flow Min Cost algorithm, allowing it to pass all test sets within the time limit.

### Code
```cpp
// ===== STD INCLUDES =====
#include <iostream>
#include <vector>

// ===== BGL INCLUDES =====
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

// ===== BGL TYPEDEFS =====
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
                              boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor,
                boost::property <boost::edge_weight_t, long> > > > > graph;

typedef boost::graph_traits<graph>::vertex_descriptor           vertex_desc;
typedef boost::graph_traits<graph>::edge_descriptor             edge_desc;
typedef boost::graph_traits<graph>::out_edge_iterator           out_edge_it;

class edge_adder {
 graph &G;

 public:
  explicit edge_adder(graph &G) : G(G) {}
  void add_edge(int from, int to, long capacity, long cost) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    auto w_map = boost::get(boost::edge_weight, G); // new!
    const edge_desc e = boost::add_edge(from, to, G).first;
    const edge_desc rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
    w_map[e] = cost;   // new assign cost
    w_map[rev_e] = -cost;   // new negative cost
  }
};

int MAX_SIGNIFICANCE = 2 << 7;

void solve() {
  // ===== READ INPUT =====
  int n, m; std::cin >> n >> m;
  
  std::vector<int> capacities(n - 1);
  for(int i = 0; i < n - 1; ++i) { std::cin >> capacities[i]; }
  
  std::vector<std::vector<std::vector<int>>> items(n-1, std::vector<std::vector<int>>(n));   // 3D Vector: (Start x End x Significances)
  for(int i = 0; i < m; ++i) { 
    int a, b, d; std::cin >> a >> b >> d;
    items[a][b].push_back(d);
  }
  
  // ===== BUILD GRAPH =====
  graph G(n);
  edge_adder adder(G);  
  auto c_map = boost::get(boost::edge_capacity, G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  
  // Add source and sink
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  // Add source and sink connections
  for(int i = 0; i < n - 1; ++i) { 
    adder.add_edge(v_source, i, capacities[i], 0);
    adder.add_edge(i + 1, v_sink, capacities[i], 0);
  }
  
  // Add edges between subsequent nodes
  for(int i = 0; i < n - 1; ++i) { 
    adder.add_edge(i, i+1, capacities[i], MAX_SIGNIFICANCE); 
  }
  
  // Add edges for items
  for(int a = 0; a < n - 1; ++a) {
    for(int b = 0; b < n; ++b) {
      // Find the minimum capacity from a to b
      int minimum_capacity = 100;
      for(int i = a; i < b; ++i) { minimum_capacity = std::min(minimum_capacity, capacities[i]); }
      
      // Only add as many edges as the capacity maximall allows for
      if(items[a][b].size() > minimum_capacity) {
        // If we have more items than the capacity allows, only take the ones with the highest significance
        std::sort(items[a][b].begin(), items[a][b].end(), [](int a, int b) { return a > b; });
        for(int i = 0; i < minimum_capacity; ++i) { 
          adder.add_edge(a, b, 1, (b - a) * MAX_SIGNIFICANCE - items[a][b][i]);
        }
      } else {
        for(int significance : items[a][b]) {
          adder.add_edge(a, b, 1, (b - a) * MAX_SIGNIFICANCE - significance);
        }
      }
    }
  }
  
  // ===== SOLVE =====
  boost::successive_shortest_path_nonnegative_weights(G, v_source, v_sink);
  int cost = boost::find_flow_cost(G);
  
  int flow = 0;
  out_edge_it e, eend;
  for(boost::tie(e, eend) = boost::out_edges(boost::vertex(v_source,G), G); e != eend; ++e) {
      flow += c_map[*e] - rc_map[*e];     
  }
  
  std::cout << flow * MAX_SIGNIFICANCE - cost << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (20 pts / 2 s) : Correct answer      (0.092s)
   Test set 2 (20 pts / 2 s) : Correct answer      (1.198s)
   Test set 3 (20 pts / 2 s) : Correct answer      (1.196s)
   Test set 4 (20 pts / 2 s) : Correct answer      (0.112s)
   Test set 5 (20 pts / 2 s) : Correct answer      (1.544s)

Total score: 100
```