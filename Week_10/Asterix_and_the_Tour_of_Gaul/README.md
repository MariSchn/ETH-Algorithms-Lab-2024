# Astérix and the Tour of Gaul

## 📝 Problem Description

The goal is to devise a transportation plan for a selection of food items along a fixed route to achieve the maximum possible total significance.

The route consists of $n$ stops, labeled $s_0, s_1, \ldots, s_{n-1}$. For each leg of the journey between consecutive stops $s_i$ and $s_{i+1}$, there is a capacity limit $c_i$, which is the maximum number of items that can be carried simultaneously on that segment.

You are given a list of $m$ potential food items. Each item is defined by a pickup stop $s_a$, a drop-off stop $s_b$, and a cultural significance value $d$. You can choose to transport at most one of each listed item. When arriving at a stop, any items destined for that stop are unloaded before new items are picked up.

Your task is to select a subset of the $m$ items to transport such that the sum of their significance values is maximized, without violating the capacity constraint $c_i$ on any segment of the journey. The output should be this maximum possible total significance.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem requires you to maximize a certain value (total significance) subject to a set of constraints (the carrying capacities on each leg of the trip). This structure is characteristic of a class of optimization problems. Think about what general algorithmic paradigms are suitable for such problems.
</details>
<details>
<summary>Hint #2</summary>
Consider representing the problem as a network. The stops along the tour can be modeled as nodes in this network. The act of transporting items can be thought of as sending a "flow" through the network. How could you represent the capacities of the tour segments and the significance of the items within this network model?
</details>
<details>
<summary>Hint #3</summary>
This problem can be effectively solved by modeling it as a **minimum-cost maximum-flow** problem. To maximize the total significance, you can aim to minimize the total "cost", where the cost is related to the negative significance of the items.

The graph should include nodes for each stop, a source node, and a sink node. Edges in the graph can represent two things:
1. The physical path between consecutive stops.
2. The "shortcut" path for transporting an item from its pickup stop to its drop-off stop.

The capacities and costs of these edges must be set carefully to reflect the problem's constraints and objectives.
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1, 2, 3, 4)</summary>

### Core Idea: Minimum-Cost Maximum-Flow

This problem asks us to maximize a total value under capacity constraints, which strongly suggests a network flow formulation. Specifically, it can be modeled as a **minimum-cost maximum-flow** problem. The general idea is to associate the *significance* of an item with a negative *cost* in the flow network. By finding a flow that minimizes the total cost, we effectively maximize the total significance.

### Graph Construction

To model this, we construct a directed graph with the following components:
- **Nodes**: We create one node for each of the $n$ stops, plus a global source node `S` and a global sink node `T`.
- **Edges and Capacities**:
    - **Path Edges**: For each segment of the tour from stop $s_i$ to $s_{i+1}$, we add an edge from node $i$ to node $i+1$. The capacity of this edge is set to $c_i$, directly modeling the transportation limit for that leg of the journey.
    - **Item Edges**: For each food item, defined by a pickup stop $a$, drop-off stop $b$, and significance $d$, we add an edge from node $a$ to node $b$. The capacity of this edge is 1, as we can transport at most one of each specific item.
- **Source and Sink**: The source `S` and sink `T` are connected to the stop nodes to allow flow to enter and leave the network.

### Handling Costs and Negative Weights

The `successive_shortest_path_nonnegative_weights` algorithm, which is efficient, requires all edge weights (costs) to be non-negative. However, our goal to maximize significance naturally leads to negative costs (cost = $-d$). To address this, we perform a **cost transformation**.

Let $M$ be a large constant, greater than the maximum possible significance of an item. In the code, this is `MAX_SIGNIFICANCE = 256` (since $d \le 27$).

1.  **Path Edge Cost**: We assign a cost of $M$ to each path edge from node $i$ to node $i+1$. Traveling from stop $a$ to $b$ along the main path would therefore incur a cost of $M \cdot (b-a)$.

2.  **Item Edge Cost**: For an item transported from $a$ to $b$ with significance $d$, we set the cost of its corresponding edge to $M \cdot (b-a) - d$.

This transformation ensures all edge costs are non-negative (since $M > d$). More importantly, it cleverly encodes the significance as a *cost saving*. For a trip from $a$ to $b$, choosing the direct item edge is $d$ units cheaper than traversing the corresponding path edges. The algorithm will therefore prioritize paths that use item edges with high significance to minimize the total cost.

The provided code uses a specific (and non-standard) way to connect the source and sink to model the capacity constraints across the entire path, but the core logic of cost transformation remains the same. After the min-cost flow is computed, the total significance can be recovered from the resulting flow and cost values.

The final maximum significance is calculated as `flow * MAX_SIGNIFICANCE - cost`. This formula effectively reverses the cost transformation to extract the sum of significances.

This approach is correct but might be too slow if the number of items ($m$) is very large, as it creates one edge for every single item.

**Code**
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
    auto w_map = boost::get(boost::edge_weight, G);
    const edge_desc e = boost::add_edge(from, to, G).first;
    const edge_desc rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
    w_map[e] = cost;
    w_map[rev_e] = -cost;
  }
};

const int MAX_SIGNIFICANCE = 256;

void solve() {
  int n, m; std::cin >> n >> m;
  
  std::vector<int> capacities(n - 1);
  for(int i = 0; i < n - 1; ++i) { std::cin >> capacities[i]; }
  
  std::vector<std::vector<int>> items(m, std::vector<int>(3));
  for(int i = 0; i < m; ++i) { std::cin >> items[i][0] >> items[i][1] >> items[i][2]; }
  
  graph G(n);
  edge_adder adder(G);  
  auto c_map = boost::get(boost::edge_capacity, G);
  auto r_map = boost::get(boost::edge_reverse, G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  // Model path segments and capacities
  for(int i = 0; i < n - 1; ++i) { 
    adder.add_edge(v_source, i, capacities[i], 0);
    adder.add_edge(i, i+1, capacities[i], MAX_SIGNIFICANCE); 
    adder.add_edge(i + 1, v_sink, capacities[i], 0);
  }
  
  // Add edges for items with transformed costs
  for(int i = 0; i < m; ++i) {
    int a = items[i][0];
    int b = items[i][1];
    int d = items[i][2];
    adder.add_edge(a, b, 1, MAX_SIGNIFICANCE * (b - a) - d); 
  }
  
  boost::successive_shortest_path_nonnegative_weights(G, v_source, v_sink);
  long cost = boost::find_flow_cost(G);
    
  long flow = 0;
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
### Problem with the First Approach

The previous min-cost max-flow solution is correct, but its performance suffers when the number of available items ($m$) is large (e.g., up to $10^5$). Creating an edge for every single item results in a graph that is too large, causing the algorithm to exceed the time limit on the final test set.

### Optimization: Pruning Useless Items

The key observation for optimization is that we don't need to consider all $m$ items. The number of items that can be transported between any two stops $a$ and $b$ is fundamentally limited by the "bottleneck capacity" of the path between them. This bottleneck is the minimum capacity of all segments on the path from $a$ to $b$.
Let $c'_{ab} = \min_{i=a}^{b-1} \{c_i\}$. We can transport at most $c'_{ab}$ items from stop $a$ to stop $b$.

If there are more than $c'_{ab}$ items available for the route from $a$ to $b$, it's impossible to transport all of them. Since our goal is to maximize total significance, it is always optimal to choose the $c'_{ab}$ items with the highest significance values and ignore the rest.

### Optimized Algorithm

This insight leads to a powerful pre-processing step before building the graph:
1.  **Group Items**: Instead of processing items one by one, we first group all available items by their pickup and drop-off stops $(a, b)$. A 3D vector or a map can be used for this.
2.  **Filter by Bottleneck Capacity**: For each pair of stops $(a, b)$ that has items associated with it:
    a. Calculate the bottleneck capacity $c'_{ab} = \min_{i=a}^{b-1} \{c_i\}$.
    b. If the number of items for this $(a, b)$ pair exceeds $c'_{ab}$, sort them by significance in descending order.
    c. Select only the top $c'_{ab}$ items with the highest significance. Discard the others as they could never be chosen in an optimal solution.
3.  **Build Graph**: Construct the same min-cost max-flow graph as in the first solution, but **only add item edges for the filtered, high-significance items**.

This optimization dramatically reduces the number of edges in the graph, especially when $m$ is large and the capacities $c_i$ are small. The resulting smaller graph can be solved much more quickly by the min-cost flow algorithm, allowing it to pass all test sets within the time limit.

**Code**
```cpp
// ===== STD INCLUDES =====
#include <iostream>
#include <vector>
#include <algorithm>

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
    auto w_map = boost::get(boost::edge_weight, G);
    const edge_desc e = boost::add_edge(from, to, G).first;
    const edge_desc rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
    w_map[e] = cost;
    w_map[rev_e] = -cost;
  }
};

const int MAX_SIGNIFICANCE = 256;

void solve() {
  int n, m; std::cin >> n >> m;
  
  std::vector<int> capacities(n - 1);
  for(int i = 0; i < n - 1; ++i) { std::cin >> capacities[i]; }
  
  // Group items by start and end stops
  std::vector<std::vector<std::vector<int>>> items_by_route(n, std::vector<std::vector<int>>(n));
  for(int i = 0; i < m; ++i) { 
    int a, b, d; std::cin >> a >> b >> d;
    items_by_route[a][b].push_back(d);
  }
  
  graph G(n);
  edge_adder adder(G);  
  auto c_map = boost::get(boost::edge_capacity, G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  // Model path segments and capacities
  for(int i = 0; i < n - 1; ++i) { 
    adder.add_edge(v_source, i, capacities[i], 0);
    adder.add_edge(i, i+1, capacities[i], MAX_SIGNIFICANCE); 
    adder.add_edge(i + 1, v_sink, capacities[i], 0);
  }
  
  // Add pruned set of item edges
  for(int a = 0; a < n; ++a) {
    for(int b = a + 1; b < n; ++b) {
      if (items_by_route[a][b].empty()) continue;

      int bottleneck_capacity = 101; // Max capacity is 100
      for(int i = a; i < b; ++i) { bottleneck_capacity = std::min(bottleneck_capacity, capacities[i]); }
      
      auto& current_items = items_by_route[a][b];
      
      // If we have more items than capacity, sort and take the best ones
      if(current_items.size() > bottleneck_capacity) {
        std::sort(current_items.rbegin(), current_items.rend());
        current_items.resize(bottleneck_capacity);
      }
      
      // Add edges for the selected items
      for(int significance : current_items) {
        adder.add_edge(a, b, 1, (b - a) * MAX_SIGNIFICANCE - significance);
      }
    }
  }
  
  boost::successive_shortest_path_nonnegative_weights(G, v_source, v_sink);
  long cost = boost::find_flow_cost(G);
  
  long flow = 0;
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

```