# Canteen

## 📝 Problem Description

The task is to create a production and serving plan for a canteen over a period of $N$ days. For each day $i$, we are given several parameters:
*   The maximum number of meals that can be produced, $a_i$, and the production cost per meal, $c_i$.
*   The number of students who want to eat, $s_i$, and the fixed price per meal, $p_i$.

Meals that are produced but not served on a given day can be stored in a freezer and served on any subsequent day. For each night between day $i$ and day $i+1$, we know:
*   The maximum number of meals that can be stored in the freezer, $v_i$.
*   The energy cost to store one meal overnight, $e_i$.

The goal is twofold. First, determine if it's possible to serve every student who wants a meal over the $N$ days. Second, regardless of whether it's possible or not, we must find the maximum number of students that can be served and the corresponding maximum possible profit (or minimum loss) the canteen can achieve. The output should specify if serving all students is possible, followed by the maximum number of students served and the maximum profit.

## 💡 Hints

<details>
<summary>Hint #1</summary>
This problem involves managing a resource (meals) that flows through a system over time. Meals are created (production), consumed (by students), and can be passed from one day to the next (storage). This structure suggests modeling the problem as a network where we need to optimize the flow of this resource.
</details>
<details>
<summary>Hint #2</summary>
This problem can be effectively modeled as a flow network. Consider what the nodes, edges, capacities, and costs in such a network could represent. How can you model the progression of days? Each day could be a node, and edges could represent production, consumption, and storage between days.
</details>
<details>
<summary>Hint #3</summary>
The objective is to serve the maximum number of students (which is equivalent to maximizing the flow of meals to students) while also maximizing profit (which is equivalent to minimizing total cost). This dual objective is the hallmark of a **Min-Cost Max-Flow** problem. Think carefully about how to represent profit within a cost-minimization framework. A common technique is to model revenue as a negative cost.
</details>
<details>
<summary>Hint #4</summary>
Standard algorithms for Min-Cost Max-Flow can be inefficient if the graph contains edges with negative costs. The cycle-canceling algorithm, which handles such cases, is often too slow for larger constraints. Is it possible to remodel the problem to eliminate negative costs? Instead of maximizing profit directly, consider minimizing "costs" plus "lost opportunity." For example, if the maximum possible price for a meal is $P_{max}$, selling a meal for price $p_i$ can be viewed as incurring a cost of $P_{max} - p_i$. This transformation can make all edge weights non-negative, allowing for more efficient algorithms.
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1, 2)</summary>
This problem can be modeled as a **Min-Cost Max-Flow** problem. We want to maximize the number of served students (the "flow") while maximizing the profit (which is equivalent to minimizing the "cost").

### Graph Construction

We construct a flow network with a source node `S`, a sink node `T`, and one node for each of the $N$ days. The edges will represent the production, consumption, and storage of meals.

1.  **Nodes:**
    *   A source node `S`.
    *   A sink node `T`.
    *   $N$ nodes, one for each day $i \in \{0, 1, \dots, N-1\}$.

2.  **Edges:**
    *   **Production:** For each day $i$, we add an edge from `S` to node $i$. This represents the meals produced on that day.
        *   **Capacity:** $a_i$ (the maximum number of meals that can be produced).
        *   **Cost:** $c_i$ (the cost to produce one meal).
    *   **Consumption:** For each day $i$, we add an edge from node $i$ to `T`. This represents meals being sold to students.
        *   **Capacity:** $s_i$ (the number of students wanting a meal).
        *   **Cost:** $-p_i$. We use a negative cost to model the revenue (profit) from selling a meal. Minimizing a negative profit is equivalent to maximizing it.
    *   **Storage (Freezer):** For each day $i$ from $0$ to $N-2$, we add an edge from node $i$ to node $i+1$. This represents storing leftover meals overnight.
        *   **Capacity:** $v_i$ (the freezer's capacity).
        *   **Cost:** $e_i$ (the cost to freeze one meal).

<center>
<img src="https://i.imgur.com/vH9J0yA.png" alt="Graph model for the Canteen problem" width="600"/>
<em>A visual representation of the graph for N=3 days.</em>
</center>

### Algorithm

After constructing the graph, we can find the min-cost max-flow. Since our graph has negative edge weights (due to the profit edges), we cannot use simpler algorithms that require non-negative weights. A common approach for graphs with negative weights is:
1.  Compute the maximum flow from `S` to `T`, for instance, using a push-relabel algorithm. This gives us the maximum number of students we can serve.
2.  Run a cycle-canceling algorithm on the residual graph to find a flow with the same value but minimum cost.

The maximum flow value corresponds to the total number of students served ($S$). The minimum cost found represents the total net cost. Since we modeled profit as negative cost, the maximum profit ($P$) is simply the negation of the minimum cost calculated by the algorithm.

This approach is correct and will pass the first two test sets, but the `cycle_canceling` algorithm can be too slow for the largest constraints.

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
                              boost::property <boost::edge_weight_t, long>>>>> graph;

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
    c_map[rev_e] = 0;
    r_map[e] = rev_e;
    r_map[rev_e] = e;
    w_map[e] = cost;
    w_map[rev_e] = -cost;
  }
};

void solve() {
  int n; std::cin >> n;
  int total_n_students = 0;
  
  std::vector<int> production_capacity(n);
  std::vector<int> production_cost(n);
  std::vector<int> n_students(n);
  std::vector<int> menu_price(n);
  std::vector<int> freezer_capacity(n - 1);
  std::vector<int> freezer_cost(n - 1);
  
  for(int i = 0; i < n; ++i) {
    std::cin >> production_capacity[i] >> production_cost[i];
  }
  for(int i = 0; i < n; ++i) {
    std::cin >> n_students[i] >> menu_price[i];
    total_n_students += n_students[i];
  }
  for(int i = 0; i < n-1; ++i) {
    std::cin >> freezer_capacity[i] >> freezer_cost[i];
  }
  
  // Create a graph with N day-nodes, plus a source and a sink
  graph G(n + 2);
  edge_adder adder(G);
  const int v_source = n;
  const int v_sink = n + 1;
  
  // Add edges for production, consumption, and storage
  for(int i = 0; i < n; ++i) {
    adder.add_edge(v_source, i, production_capacity[i], production_cost[i]);
    adder.add_edge(i, v_sink, n_students[i], -menu_price[i]);
  }
  
  for(int i = 0; i < n-1; ++i) {
    adder.add_edge(i, i+1, freezer_capacity[i], freezer_cost[i]);
  }
  
  // Calculate Min-Cost Max-Flow
  long flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  boost::cycle_canceling(G);
  long cost = boost::find_flow_cost(G);
  
  if (total_n_students > flow) {
    std::cout << "impossible ";
  } else {
    std::cout << "possible ";
  }
  std::cout << flow << " " << -cost << std::endl;
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
The previous solution is too slow for the final test set because `boost::cycle_canceling` is needed for graphs with negative edge weights, and it can have poor worst-case performance. To achieve the required speed-up, we must eliminate these negative costs.

### Cost Transformation

The negative costs arise from modeling profit ($-p_i$) on the edges from day nodes to the sink. We can reframe the problem from "maximizing profit" to "minimizing total expense and opportunity cost."

Let's define a maximum possible price for a meal, $P_{max}$. Based on the problem constraints, the price per menu is at most 20, so we can set $P_{max} = 20$.

Instead of an edge from day $i$ to the sink having a *negative cost* of $-p_i$, we can give it a *positive cost* of $P_{max} - p_i$. This value represents the "opportunity cost" or "lost revenue" from selling the meal at price $p_i$ instead of the maximum possible price $P_{max}$.

With this change, all edge costs in the graph are now non-negative:
*   Production cost $c_i \ge 0$.
*   Freezer cost $e_i \ge 0$.
*   Opportunity cost $20 - p_i \ge 0$ (since $p_i \le 20$).

### New Profit Calculation

The algorithm now minimizes the sum of production costs, freezer costs, and opportunity costs. Let this total minimum cost be $C_{min}$ for a maximum flow of $F$ meals.

$C_{min} = (\text{Total Production Cost}) + (\text{Total Freezer Cost}) + (\text{Total Opportunity Cost})$
$C_{min} = (\text{True Expenses}) + \sum_{\text{sold meals}} (P_{max} - p_i)$
$C_{min} = (\text{True Expenses}) + F \cdot P_{max} - \sum_{\text{sold meals}} p_i$
$C_{min} = (\text{True Expenses}) + F \cdot P_{max} - (\text{True Revenue})$

Since `True Profit = True Revenue - True Expenses`, we can rearrange the equation:
$C_{min} = -(\text{True Profit}) + F \cdot P_{max}$

Therefore, the actual maximum profit can be recovered with the formula:
$$ \text{Profit} = F \cdot P_{max} - C_{min} $$

### Algorithm

Because all edge weights are now non-negative, we can use a more efficient min-cost max-flow algorithm, such as `boost::successive_shortest_path_nonnegative_weights`. This algorithm is fast enough to pass all test sets. After running it, we calculate the total flow and cost to find our final answer. Note that `successive_shortest_path_nonnegative_weights` does not return the flow value, so we must compute it manually by inspecting the flow on the edges leaving the source.

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
                              boost::property <boost::edge_weight_t, long>>>>> graph;

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
    c_map[rev_e] = 0;
    r_map[e] = rev_e;
    r_map[rev_e] = e;
    w_map[e] = cost;
    w_map[rev_e] = -cost;
  }
};

void solve() {
  int n; std::cin >> n;
  int total_n_students = 0;
  
  std::vector<int> production_capacity(n);
  std::vector<int> production_cost(n);
  std::vector<int> n_students(n);
  std::vector<int> menu_price(n);
  std::vector<int> freezer_capacity(n - 1);
  std::vector<int> freezer_cost(n - 1);
  
  for(int i = 0; i < n; ++i) {
    std::cin >> production_capacity[i] >> production_cost[i];
  }
  for(int i = 0; i < n; ++i) {
    std::cin >> n_students[i] >> menu_price[i];
    total_n_students += n_students[i];
  }
  for(int i = 0; i < n-1; ++i) {
    std::cin >> freezer_capacity[i] >> freezer_cost[i];
  }
  
  // Create a graph with N day-nodes, plus a source and a sink
  graph G(n + 2);
  edge_adder adder(G);
  const int v_source = n;
  const int v_sink = n + 1;
  const int P_MAX = 20;
  
  // Add edges with non-negative costs
  for(int i = 0; i < n; ++i) {
    adder.add_edge(v_source, i, production_capacity[i], production_cost[i]);
    adder.add_edge(i, v_sink, n_students[i], P_MAX - menu_price[i]); // Opportunity cost
  }
  
  for(int i = 0; i < n-1; ++i) {
    adder.add_edge(i, i+1, freezer_capacity[i], freezer_cost[i]);
  }
  
  // Run the efficient algorithm for non-negative weights
  boost::successive_shortest_path_nonnegative_weights(G, v_source, v_sink);
  long cost = boost::find_flow_cost(G);

  // Calculate total flow by summing flow out of the source
  long flow = 0;
  auto c_map = boost::get(boost::edge_capacity, G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  out_edge_it e, eend;
  for(boost::tie(e, eend) = boost::out_edges(boost::vertex(v_source,G), G); e != eend; ++e) {
    flow += c_map[*e] - rc_map[*e];     
  }
  
  if (total_n_students > flow) {
    std::cout << "impossible ";
  } else {
    std::cout << "possible ";
  }
  // Convert the minimized cost back to maximized profit
  std::cout << flow << " " << P_MAX * flow - cost << std::endl;
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