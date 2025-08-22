# Real Estate Market

## 📝 Problem Description

The goal is to maximize the profit from selling a set of land sites to a group of potential buyers. We are given $N$ buyers and $M$ sites. Each site is located in one of $S$ different states.

For each of the $N$ buyers, we have a list of their bids, $b_{i,j}$, which is the amount buyer $i$ is willing to pay for site $j$. The assignment of sites to buyers must adhere to the following rules:
1.  Each buyer can be assigned at most one site.
2.  Each site can be sold to at most one buyer.
3.  Each state $k$ has a limit, $l_k$, on the maximum number of sites that can be sold within that state.

The task is to determine which sites to sell and to which buyers, respecting all constraints, such that the total profit is maximized. The output should be the total number of sites sold and the corresponding maximum profit.

## 💡 Hints

<details>
<summary>Hint #1</summary>
This problem requires assigning items from one group (buyers) to another (sites) to maximize a total value, subject to certain constraints. Think about how to represent the buyers, the sites, and the possible transactions. This structure is a classic sign of an assignment problem, which can often be modeled using a network.
</details>
<details>
<summary>Hint #2</summary>
The problem can be modeled as a **Min-Cost Max-Flow** problem. You can construct a network with a source and a sink. The buyers and sites will be nodes in this network. Think about what the capacities and costs on the edges should represent. To maximize profit, you can minimize the *negative* profit. The capacities can enforce the "one-to-one" assignment rule (one buyer per site, one site per buyer).
</details>
<details>
<summary>Hint #3</summary>
The constraint on the number of sites sold per state is a key part of the problem. How can you incorporate this into your flow network? Consider adding an intermediate layer of nodes between the site nodes and the sink. These new nodes can represent the states. What should the capacity of the edges connected to these state nodes be to enforce the sales limits?
</details>
<details>
<summary>Hint #4</summary>
Using negative costs (like `-profit`) requires an algorithm like cycle-canceling, which can be slow. Notice that all bids $b_{i,j}$ are bounded by a maximum value (100). You can transform the costs to be non-negative. Instead of maximizing profit, try minimizing the "lost opportunity". For a bid $b_{i,j}$, the lost opportunity compared to the maximum possible bid of 100 is $100 - b_{i,j}$. Minimizing this quantity is equivalent to maximizing the original profit and results in non-negative edge costs, allowing for faster algorithms.
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1, 2, 3, 4)</summary>
This problem can be elegantly modeled as a **Min-Cost Max-Flow** problem on a specially constructed graph. The core idea is to find an assignment of buyers to sites that maximizes total profit while respecting all constraints.

### Modeling as Min-Cost Max-Flow

A standard maximum flow formulation can determine the maximum number of possible assignments, but it doesn't account for the profit from each assignment. A min-cost max-flow formulation, however, considers both a capacity (for flow) and a cost per unit of flow for each edge.

1.  **Capacity for Assignments:** We can use edge capacities to enforce the constraints. By setting capacities to 1 on edges related to buyers and sites, we ensure that each buyer purchases at most one site and each site is sold at most once.

2.  **Cost for Profit:** We want to *maximize* the total profit. The min-cost max-flow algorithm, by definition, *minimizes* the total cost. We can align these two objectives by a simple mathematical trick: maximizing a value is equivalent to minimizing its negative. Therefore, if a bid from buyer $i$ for site $j$ is $b_{i,j}$, we can set the cost of the corresponding edge to $-b_{i,j}$. After finding the minimum cost flow, we can negate the total cost to get our maximum profit.

### Graph Construction

We build a directed graph with several layers of nodes:
*   A single **source node** `v_source`.
*   $N$ nodes representing the **buyers**.
*   $M$ nodes representing the **sites**.
*   $S$ nodes representing the **states**.
*   A single **sink node** `v_sink`.

The edges are set up as follows:
1.  **Source to Buyers:** For each buyer $i$, add an edge from `v_source` to buyer node $i$.
    *   **Capacity:** 1 (each buyer can purchase at most one site).
    *   **Cost:** 0.
2.  **Buyers to Sites:** For each buyer $i$ and site $j$, add an edge from buyer node $i$ to site node $j$.
    *   **Capacity:** 1 (this specific assignment can happen at most once).
    *   **Cost:** $-b_{i,j}$ (the negative of the bid).
3.  **Sites to States:** For each site $j$, which belongs to state $s_j$, add an edge from site node $j$ to state node $s_j$.
    *   **Capacity:** 1 (each site can be sold at most once).
    *   **Cost:** 0.
4.  **States to Sink:** For each state $k$, add an edge from state node $k$ to `v_sink`.
    *   **Capacity:** $l_k$ (the maximum number of sites that can be sold in state $k$).
    *   **Cost:** 0.

### Solving
By running a min-cost max-flow algorithm on this graph, the total flow will correspond to the number of sites sold, and the negative of the minimum cost will be the maximum profit. Since we introduced negative costs, we must use an algorithm that supports them, such as **cycle-canceling**. This approach is correct and passes the first four test sets, but it is too slow for the final, largest test set.

**Code**
```cpp
#include<iostream>
#include<vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor,
                boost::property <boost::edge_weight_t, long> > > > > graph;

typedef traits::vertex_descriptor vertex_desc;
typedef boost::graph_traits<graph>::edge_descriptor edge_desc;

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
    c_map[rev_e] = 0; // Reverse edge has no capacity
    r_map[e] = rev_e;
    r_map[rev_e] = e;
    w_map[e] = cost;
    w_map[rev_e] = -cost;
  }
};

void solve() {
  int N, M, S; std::cin >> N >> M >> S;
  
  std::vector<int> limits(S);
  for(int i = 0; i < S; ++i) std::cin >> limits[i];
  
  std::vector<int> property_to_state(M);
  for(int i = 0; i < M; ++i) std::cin >> property_to_state[i];
  
  std::vector<std::vector<int>> bids(N, std::vector<int>(M));
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < M; ++j) {
      std::cin >> bids[i][j];
    }
  }
  
  graph G(N + M + S + 2);
  edge_adder adder(G);

  const vertex_desc v_source = N + M + S;
  const vertex_desc v_sink = N + M + S + 1;
  
  // Edges from source to buyers
  for(int i = 0; i < N; ++i) {
    adder.add_edge(v_source, i, 1, 0);
  }
  
  // Edges from buyers to sites
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < M; j++) {
      adder.add_edge(i, N + j, 1, -bids[i][j]);
    }
  }
  
  // Edges from sites to their states
  for(int i = 0; i < M; ++i) {
    adder.add_edge(N + i, N + M + property_to_state[i] - 1, 1, 0);
  }
  
  // Edges from states to sink
  for(int i = 0; i < S; ++i) {
    adder.add_edge(N + M + i, v_sink, limits[i], 0);
  }

  boost::push_relabel_max_flow(G, v_source, v_sink);
  boost::cycle_canceling(G);
  long cost = boost::find_flow_cost(G);
  
  long flow = 0;
  auto c_map = boost::get(boost::edge_capacity, G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  boost::graph_traits<graph>::out_edge_iterator e_it, e_end;
  for(boost::tie(e_it, e_end) = boost::out_edges(v_source, G); e_it != e_end; ++e_it) {
      flow += c_map[*e_it] - rc_map[*e_it];
  }
    
  std::cout << flow << " " << -cost << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  int T; std::cin >> T;
  while(T--) {
    solve();
  }
}
```
</details>
<details>
<summary>Final Solution</summary>
The previous solution was correct but too slow for the largest test cases due to the use of negative edge costs, which necessitates a slower algorithm like `cycle_canceling`. To optimize, we need to eliminate these negative costs.

### Cost Transformation

The problem states that bids are bounded: $1 \le b_{i,j} \le 100$. We can leverage this upper bound to transform our costs into non-negative values.

Instead of maximizing the profit $b_{i,j}$, we can aim to minimize the "missed potential". Let's assume the maximum possible price for any site is $C = 100$. For a transaction with bid $b_{i,j}$, the difference from the maximum is $100 - b_{i,j}$. This value represents the "cost" or "loss" relative to the best possible outcome for that single sale.

By minimizing the sum of these costs, $\sum (100 - b_{i,j})$, we are effectively maximizing the sum of the actual bids, $\sum b_{i,j}$. This transformation has a crucial advantage: since $b_{i,j} \le 100$, the new cost $100 - b_{i,j}$ is always non-negative.

### New Calculation

With non-negative costs, we can use a more efficient algorithm like **successive shortest path with non-negative weights**. The graph structure remains identical to the first solution, but the costs on the edges from buyers to sites are changed:
*   **Edge (buyer $i$, site $j$):**
    *   **Capacity:** 1
    *   **Cost:** $100 - b_{i,j}$

After running the min-cost max-flow algorithm, we get the total number of sites sold (the flow) and a total cost. This cost is the sum of all "missed potentials". The final profit must be recalculated. If we sell $k$ sites, the maximum theoretical profit would be $k \times 100$. Our actual profit is this theoretical maximum minus the total "missed potential" (the cost reported by the algorithm).

**Profit = (Total Flow $\times$ 100) - Minimum Cost**

This approach is significantly faster and passes all test cases within the time limit.

**Code**
```cpp
#include<iostream>
#include<vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor,
                boost::property <boost::edge_weight_t, long> > > > > graph;

typedef traits::vertex_descriptor vertex_desc;
typedef boost::graph_traits<graph>::edge_descriptor edge_desc;
typedef boost::graph_traits<graph>::out_edge_iterator out_edge_it;

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
    c_map[rev_e] = 0; // Reverse edge has no capacity
    r_map[e] = rev_e;
    r_map[rev_e] = e;
    w_map[e] = cost;
    w_map[rev_e] = -cost;
  }
};

void solve() {
  int N, M, S; std::cin >> N >> M >> S;
  
  std::vector<int> limits(S);
  for(int i = 0; i < S; ++i) std::cin >> limits[i];
  
  std::vector<int> property_to_state(M);
  for(int i = 0; i < M; ++i) std::cin >> property_to_state[i];
  
  std::vector<std::vector<int>> bids(N, std::vector<int>(M));
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < M; ++j) {
      std::cin >> bids[i][j];
    }
  }
  
  graph G(N + M + S + 2);
  edge_adder adder(G);

  const vertex_desc v_source = N + M + S;
  const vertex_desc v_sink = N + M + S + 1;
  const int MAX_BID = 100;

  // Edges from source to buyers
  for(int i = 0; i < N; ++i) {
    adder.add_edge(v_source, i, 1, 0);
  }
  
  // Edges from buyers to sites with transformed cost
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < M; j++) {
      adder.add_edge(i, N + j, 1, MAX_BID - bids[i][j]);
    }
  }
  
  // Edges from sites to their states
  for(int i = 0; i < M; ++i) {
    adder.add_edge(N + i, N + M + property_to_state[i] - 1, 1, 0);
  }
  
  // Edges from states to sink
  for(int i = 0; i < S; ++i) {
    adder.add_edge(N + M + i, v_sink, limits[i], 0);
  }

  boost::successive_shortest_path_nonnegative_weights(G, v_source, v_sink);
  long cost = boost::find_flow_cost(G);

  // Calculate total flow out of the source
  long sold = 0;
  auto c_map = boost::get(boost::edge_capacity, G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  out_edge_it e, e_end;
  for(boost::tie(e, e_end) = boost::out_edges(v_source, G); e != e_end; ++e) {
    sold += c_map[*e] - rc_map[*e];
  }
  
  long revenue = sold * MAX_BID - cost;
  std::cout << sold << " " << revenue << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  int T; std::cin >> T;
  while(T--) {
    solve();
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful (with warnings)

Judging solution >>>>
   Test set 1 (20 pts / 1 s) : Correct answer      (0.013s)
   Test set 2 (20 pts / 1 s) : Correct answer      (0.021s)
   Test set 3 (20 pts / 1 s) : Correct answer      (0.016s)
   Test set 4 (20 pts / 1 s) : Correct answer      (0.018s)
   Test set 5 (20 pts / 1 s) : Correct answer      (0.106s)

Total score: 100
```