# Alastor “Mad-Eye” Moody

## 📝 Problem Description

You are given a set of $N$ locations and $M$ bidirectional connections. A specific starting location $S$ and a destination location $P$ are identified. Each connection links two locations, say $u$ and $v$, and has two associated properties: a capacity $c$, representing the maximum number of people that can use it, and a travel time $d$.

The goal is to determine the maximum number of people that can travel from location $S$ to location $P$ subject to a critical constraint: every person must travel along a path that takes the minimum possible time. Since all individuals travel at the same speed and start simultaneously, this ensures they all arrive at location $P$ at the exact same moment. You need to output this maximum number of people.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem requires satisfying two different conditions: minimizing the travel time for everyone and maximizing the total number of people. Think about how these two objectives interact. Does one take priority over the other? The problem statement says the plan must be "cautious," meaning everyone *must* take a path of the minimum possible duration. This suggests that finding the shortest travel time is the first step.
</details>
<details>

<summary>Hint #2</summary>

This problem can be modeled using a graph, where locations are vertices and connections are edges. The problem then combines concepts from shortest path algorithms and network flow algorithms. A "cautious plan" only allows travel along edges that can be part of a shortest path from the start to the destination. How can you construct a new, smaller graph that includes *only* these valid edges?

</details>

<details>

<summary>Hint #3</summary>

To identify all edges that lie on at least one shortest path, you can use precomputed distances. First, calculate the shortest travel time from the start location $S$ to all other locations; let's call this $d_S(x)$. Second, calculate the shortest travel time from all other locations to the destination $P$; let's call this $d_P(x)$. An edge $(u, v)$ with travel time $d$ is on a shortest path from $S$ to $P$ if and only if $d_S(u) + d + d_P(v)$ is equal to the overall shortest travel time from $S$ to $P$. You can compute all $d_S$ and $d_P$ values efficiently using Dijkstra's algorithm.

</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1)</summary>
This problem can be modeled as a graph problem where locations are nodes and flyways are edges. The travel time corresponds to the edge weight (or distance), and the number of people a flyway can hold corresponds to its capacity. The overall goal is to find the maximum number of people that can travel from a source node $s$ to a sink node $p$ along paths of the shortest possible length.

For the first test set, we are given a crucial simplifying assumption: *"for every flyway there is some cautious plan that uses it."* This directly implies that **every flyway (edge) in the input is part of at least one shortest path** from $s$ to $p$.

Because all available edges are guaranteed to be on some shortest path, we don't need to worry about filtering them. Any path we construct using the given edges will satisfy the minimum time constraint. The problem is therefore reduced to a simpler one: finding the maximum number of people that can travel from $s$ to $p$ using the given network of flyways.

This is a classic **Maximum Flow** problem. We can construct a flow network where:
- The graph structure is given by the input locations and flyways.
- The capacity of each edge is the number of people $c$ the corresponding flyway can accommodate.
- We want to find the maximum flow from the source $s$ to the sink $p$.

Each unit of flow represents one person. By calculating the maximum flow, we find the maximum number of people who can travel from $s$ to $p$ concurrently, which, under the assumptions of Test Set 1, is our final answer.

```cpp
#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor>>>> graph;

typedef traits::vertex_descriptor vertex_desc;
typedef traits::edge_descriptor edge_desc;

// Custom edge adder class, highly recommended
class edge_adder {
  graph &G;

 public:
  explicit edge_adder(graph &G) : G(G) {}

  void add_edge(int from, int to, long capacity) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    const auto e = boost::add_edge(from, to, G).first;
    const auto rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  }
};

void solve() {
  // ===== READ INPUT =====
  int n, m, s, p; std::cin >> n >> m >> s >> p;
  
  graph G(n);
  edge_adder adder(G);
  
  for(int i = 0; i < m; ++i) {
    int u, v, c, d; std::cin >> u >> v >> c >> d;
    adder.add_edge(u, v, c);  // Ignore distances for now (for Test Set 1)
  }
  
  // ===== DETERMINE MAX FLOW (= MAX PEOPLE) =====
  long flow = boost::push_relabel_max_flow(G, s, p);

  // ===== OUTPUT =====
  std::cout << flow << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}
```
</details>
<details>
<summary>Final Solution</summary>
The primary challenge in the general version of the problem is that many flyways might not be part of any shortest path. A "cautious plan" forbids using such flyways. Therefore, the approach from the first solution, which assumes all edges are valid, will not work.

The core idea for the final solution is to first identify the subgraph containing **only** the edges that lie on at least one shortest path from the source $s$ to the sink $p$. Once we have this "shortest path subgraph," we can run a max-flow algorithm on it to find the maximum number of people.

The solution can be broken down into three main steps:

1.  **Calculate Shortest Distances:** We need to know the shortest travel time from $s$ to every other location, and from every location to $p$.
    -   We can find the shortest distance from $s$ to all other nodes, $d_s(v)$, by running **Dijkstra's algorithm** starting from $s$.
    -   Similarly, we find the shortest distance from all nodes to $p$, $d_p(v)$, by running Dijkstra's algorithm starting from $p$. Since the flyways are bidirectional, running Dijkstra from $p$ on the original graph gives the shortest distance to $p$.
    -   The shortest travel time from $s$ to $p$ is then simply $d_{min} = d_s(p)$.

2.  **Construct the Shortest Path Subgraph:** We build a new graph for our max-flow calculation. We iterate through every original flyway, which connects locations $u$ and $v$ with travel time $d$ and capacity $c$. A flyway can be part of a shortest path in two directions: $u \to v$ or $v \to u$.
    -   The directed edge $u \to v$ is on a shortest path if and only if the shortest path through it is equal to the overall shortest path:
        $$ d_s(u) + d + d_p(v) = d_{min} $$
    -   Similarly, the directed edge $v \to u$ is on a shortest path if:
        $$ d_s(v) + d + d_p(u) = d_{min} $$
    If a condition is met, we add the corresponding directed edge with capacity $c$ to our new flow network.

3.  **Calculate Maximum Flow:** With the shortest path subgraph constructed, the problem is reduced to a standard max-flow problem. We compute the maximum flow from $s$ to $p$ on this new graph. The resulting flow value is the maximum number of people that can travel according to a "cautious plan."

```cpp
#include <iostream>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

// ===== DIJKSTRA GRAPH =====
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
  boost::no_property, boost::property<boost::edge_weight_t, int> >      dijkstra_graph;
typedef boost::property_map<dijkstra_graph, boost::edge_weight_t>::type weight_map;
typedef boost::graph_traits<dijkstra_graph>::edge_descriptor            dijkstra_edge_desc;
typedef boost::graph_traits<dijkstra_graph>::vertex_descriptor          dijkstra_vertex_desc;

// ===== FLOW GRAPH =====
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor>>>> flow_graph;

typedef traits::vertex_descriptor flow_vertex_desc;
typedef traits::edge_descriptor flow_edge_desc;

// Custom edge adder class, highly recommended
class edge_adder {
  flow_graph &G;

 public:
  explicit edge_adder(flow_graph &G) : G(G) {}

  void add_edge(int from, int to, long capacity) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    const auto e = boost::add_edge(from, to, G).first;
    const auto rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  }
};

void solve() {
  // ===== READ INPUT =====
  int n, m, s, p; std::cin >> n >> m >> s >> p;
  
  std::vector<std::tuple<int, int, int, int>> edges; edges.reserve(m);
  dijkstra_graph dijkstra_G(n);

  for(int i = 0; i < m; ++i) {
    int u, v, c, d; std::cin >> u >> v >> c >> d;
    
    edges.emplace_back(u, v, c, d);
    boost::add_edge(u, v, d, dijkstra_G);
  }
  
  // ===== BUILD SUBGRAPH CONTAINING SHORTEST PATHS =====
  std::vector<int> source_dist_map(n);
  std::vector<int> target_dist_map(n);
  
  boost::dijkstra_shortest_paths(dijkstra_G, s, boost::distance_map(boost::make_iterator_property_map(source_dist_map.begin(), boost::get(boost::vertex_index, dijkstra_G))));
  boost::dijkstra_shortest_paths(dijkstra_G, p, boost::distance_map(boost::make_iterator_property_map(target_dist_map.begin(), boost::get(boost::vertex_index, dijkstra_G))));
  
  int min_dist = source_dist_map[p];

  // Build Subgraph
  flow_graph G(n);
  edge_adder adder(G);
  
  for(const std::tuple<int, int, int, int> &edge : edges) {
    int u = std::get<0>(edge);
    int v = std::get<1>(edge);
    int c = std::get<2>(edge);
    int d = std::get<3>(edge);
    
    // Check if the edge is part of any shortest path
    // Check if the distances are individually smaller than min_dist to avoid overflows
    if(source_dist_map[u] + target_dist_map[v] + d <= min_dist && source_dist_map[u] <= min_dist && target_dist_map[v] <= min_dist) {
      adder.add_edge(u, v, c);
    }
    if(source_dist_map[v] + target_dist_map[u] + d <= min_dist && source_dist_map[v] <= min_dist && target_dist_map[u] <= min_dist) {
      adder.add_edge(v, u, c);
    }
  }
  
  // ===== DETERMINE MAX FLOW (= MAX PEOPLE) =====
  long flow = boost::push_relabel_max_flow(G, s, p);

  // ===== OUTPUT =====
  std::cout << flow << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (25 pts / 1 s) : Correct answer      (0.028s)
   Test set 2 (25 pts / 1 s) : Correct answer      (0.024s)
   Test set 3 (25 pts / 1 s) : Correct answer      (0.009s)
   Test set 4 (25 pts / 1 s) : Correct answer      (0.028s)

Total score: 100
```