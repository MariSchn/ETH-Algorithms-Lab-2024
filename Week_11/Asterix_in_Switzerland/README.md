# Astérix in Switzerland

## 📝 Problem Description

You are given a set of $N$ provinces. Each province $p_i$ has a financial balance $b_i$, which can be positive (a surplus) or negative (a deficit). Additionally, there are $M$ debt relationships, where each relationship specifies that a province $p_i$ owes an amount $d_{i,j}$ to another province $p_j$.

Your task is to determine if it's possible to form a *free-standing* union of provinces. A non-empty subset of provinces, let's call it $X$, is considered free-standing if the sum of the balances of all provinces within $X$ is strictly greater than the total debt owed by provinces in $X$ to provinces *outside* of $X$.

Formally, a union $X$ is free-standing if the following inequality holds:
$$ \sum_{i \in X} b_i > \sum_{i \in X, j \notin X} d_{i,j} $$

The input consists of the number of provinces $N$, the number of debt relations $M$, the balance $b_i$ for each province, and the $M$ debt relations, each given as a triplet $(i, j, d_{i,j})$. You must output "yes" if at least one such free-standing union exists, and "no" otherwise.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem asks if *any* subset of provinces $X$ can form a free-standing union. This is equivalent to asking if the *best possible* union is free-standing. This suggests we need to find a way to partition the provinces into two sets: those in the union ($X$) and those outside of it. How can we define the "value" or "profit" of a given union $X$ to maximize it?
</details>

<details>
<summary>Hint #2</summary>
Let's reformulate the condition. We are looking for a non-empty set of provinces $X$ that maximizes the value $V(X) = \left( \sum_{i \in X} b_i \right) - \left( \sum_{i \in X, j \notin X} d_{i,j} \right)$. A free-standing union exists if and only if $\max_{X} V(X) > 0$.

This problem of partitioning a set of items to maximize a value, where the value depends on interactions between items in different partitions, can often be modeled as a minimum cut problem in a specially constructed flow network. Consider creating a network with a source $s$ and a sink $t$. How could you represent provinces, their balances, and their debts as components in this network?
</details>

<details>
<summary>Hint #3</summary>
Let's build a flow network. Create a source $s$, a sink $t$, and a vertex for each province $p_i$.
<ul>
    <li>For each province $p_i$ with a <b>positive balance</b> $b_i$, add a directed edge from the source $s$ to vertex $p_i$ with capacity $b_i$. These represent the assets of the potential union.</li>
    <li>For each province $p_i$ with a <b>negative balance</b> $b_i$, add a directed edge from vertex $p_i$ to the sink $t$ with capacity $-b_i$. These represent the liabilities.</li>
    <li>For each debt $d_{i,j}$ from province $p_i$ to $p_j$, add a directed edge from vertex $p_i$ to vertex $p_j$ with capacity $d_{i,j}$.</li>
</ul>
Now, consider an $s-t$ cut in this network. A cut partitions the vertices into two sets, $S$ (containing $s$) and $T$ (containing $t$). Let the set of provinces in our union $X$ correspond to the province-vertices in $S$. What does the capacity of this cut represent in terms of the original problem?
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>

This problem can be elegantly solved by transforming it into a **minimum cut problem** on a flow network. By the max-flow min-cut theorem, the value of the minimum cut is equal to the value of the maximum flow, which is computationally feasible.

### From Free-Standing Union to Minimum Cut

First, let's rephrase the objective. We are searching for a non-empty set of provinces $X$ such that its total balance exceeds its external debts.
$$ \sum_{i \in X} b_i > \sum_{i \in X, j \notin X} d_{i,j} $$
This is equivalent to finding a set $X$ that maximizes the "profit" function $P(X) = \sum_{i \in X} b_i - \sum_{i \in X, j \notin X} d_{i,j}$ and checking if this maximum profit is greater than 0.

Maximizing a function of this form is a classic application for min-cut. The core idea is to construct a graph where any cut corresponds to a partition of provinces into a union $X$ and its complement, and the capacity of the cut is related to the profit $P(X)$.

### Graph Construction

We build a directed graph with a source vertex $s$, a sink vertex $t$, and one vertex for each of the $N$ provinces.

1.  **Source to Provinces (Assets):** For every province $p_i$ with a positive balance $b_i > 0$, we add an edge from the source $s$ to the vertex for $p_i$ with capacity $b_i$. These edges represent the total potential income/assets we can have. Let $B_{pos}$ be the sum of all positive balances.

2.  **Provinces to Sink (Liabilities):** For every province $p_i$ with a negative balance $b_i < 0$, we add an edge from the vertex for $p_i$ to the sink $t$ with capacity $-b_i$. These represent the inherent costs/deficits of including these provinces in our union.

3.  **Between Provinces (Debts):** For every debt relation where province $p_i$ owes $d_{i,j}$ to province $p_j$, we add an edge from the vertex for $p_i$ to the vertex for $p_j$ with capacity $d_{i,j}$.

### Interpreting the Cut

An $s-t$ cut partitions the graph's vertices into two sets: $S$ (containing the source $s$) and $T$ (containing the sink $t$). Let our potential union $X$ be the set of provinces whose corresponding vertices are in $S$. The remaining provinces are in the complement set $Y$, corresponding to vertices in $T$.

The capacity of the cut $C(S, T)$ is the sum of capacities of all edges going from a vertex in $S$ to a vertex in $T$. These edges are:
1.  **Source to Province in $T$**: Edges $(s, p_j)$ where $p_j \in Y$. This happens only for provinces with positive balances. Their contribution to the cut is $\sum_{p_j \in Y, b_j > 0} b_j$.
2.  **Province in $S$ to Sink**: Edges $(p_i, t)$ where $p_i \in X$. This happens only for provinces with negative balances. Their contribution is $\sum_{p_i \in X, b_i < 0} (-b_i)$.
3.  **Province in $S$ to Province in $T$**: Edges $(p_i, p_j)$ where $p_i \in X$ and $p_j \in Y$. Their contribution is $\sum_{i \in X, j \in Y} d_{i,j}$, which is precisely the external debt of union $X$.

So, the capacity of the cut is:
$$ C(S,T) = \left( \sum_{p_j \in Y, b_j > 0} b_j \right) + \left( \sum_{p_i \in X, b_i < 0} (-b_i) \right) + \left( \sum_{i \in X, j \in Y} d_{i,j} \right) $$

Let's rearrange this to relate it to our profit function. Let $B_{pos} = \sum_{b_k>0} b_k$.
Notice that $\sum_{p_j \in Y, b_j > 0} b_j = B_{pos} - \sum_{p_i \in X, b_i > 0} b_i$.
Substituting this in:
$$ C(S,T) = B_{pos} - \left( \sum_{p_i \in X, b_i > 0} b_i \right) + \left( \sum_{p_i \in X, b_i < 0} (-b_i) \right) + \left( \sum_{i \in X, j \in Y} d_{i,j} \right) $$
Recognizing that $\sum_{i \in X} b_i = \sum_{p_i \in X, b_i > 0} b_i + \sum_{p_i \in X, b_i < 0} b_i = \sum_{p_i \in X, b_i > 0} b_i - \sum_{p_i \in X, b_i < 0} (-b_i)$, we can write:
$$ C(S,T) = B_{pos} - \left( \sum_{i \in X} b_i - \sum_{i \in X, j \in Y} d_{i,j} \right) = B_{pos} - P(X) $$
Therefore, the capacity of a cut corresponding to partition $X$ is $B_{pos} - P(X)$. To maximize the profit $P(X)$, we need to *minimize* the cut capacity $C(S,T)$.

The minimum possible cut capacity corresponds to the maximum possible profit. Let this minimum cut be $C_{min}$.
$$ C_{min} = B_{pos} - P_{max} $$
A free-standing union exists if $P_{max} > 0$. This is equivalent to:
$$ B_{pos} - C_{min} > 0 \implies C_{min} < B_{pos} $$

By the max-flow min-cut theorem, the minimum cut value is equal to the maximum flow value. So, we can find the answer by calculating the max flow from $s$ to $t$ and checking if it's less than the total sum of all positive balances.

### Implementation

The C++ solution below uses the Boost Graph Library to implement this logic.
1.  Read the number of provinces $n$, debts $m$, and their respective values.
2.  Initialize `sum_positive_balances` to keep track of $B_{pos}$.
3.  Create a graph with $n+2$ vertices (n provinces + source + sink).
4.  Add edges according to the construction described above.
5.  Calculate the max flow from source to sink using `boost::push_relabel_max_flow`.
6.  If `flow < sum_positive_balances`, a free-standing union exists, so we print "yes". Otherwise, we print "no".

```cpp
#include <iostream>
#include <vector>
#include <tuple>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

// Define the graph type using the Boost Graph Library.
// This setup is standard for max-flow problems.
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor>>>> graph;

typedef traits::vertex_descriptor vertex_desc;
typedef traits::edge_descriptor edge_desc;

// A helper class to simplify adding edges and their reverse edges for flow algorithms.
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
    c_map[rev_e] = 0; // Reverse edge has zero initial capacity
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  }
};

void solve() {
  // ===== Read Input =====
  int n, m;
  std::cin >> n >> m;
  
  std::vector<long> balances(n);
  for(int i = 0; i < n; ++i) {
    std::cin >> balances[i];
  }
  
  std::vector<std::tuple<int, int, int>> debts;
  debts.reserve(m);
  for(int i = 0; i < m; ++i) {
    int u, v, d;
    std::cin >> u >> v >> d;
    debts.emplace_back(u, v, d);
  }
  
  // ===== Solve using Min-Cut / Max-Flow =====
  // Create a graph with n vertices for provinces, plus a source and a sink.
  graph G(n + 2);
  edge_adder adder(G);
  const vertex_desc v_source = n;
  const vertex_desc v_sink = n + 1;
  
  long sum_positive_balances = 0;
  
  // Connect source and sink to province vertices based on balances.
  for(int i = 0; i < n; ++i) {
    if(balances[i] > 0) {
      // Asset: connect source to province with capacity = balance
      adder.add_edge(v_source, i, balances[i]);
      sum_positive_balances += balances[i];
    } else {
      // Liability: connect province to sink with capacity = -balance
      adder.add_edge(i, v_sink, -balances[i]);
    }
  }
  
  // Add edges between provinces for debt relations.
  for(const auto &debt : debts) {
    int u = std::get<0>(debt);
    int v = std::get<1>(debt);
    int d = std::get<2>(debt);
    // Debt from u to v: connect u to v with capacity = debt amount
    adder.add_edge(u, v, d);
  }
  
  // Calculate max flow from source to sink.
  long flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  // ===== Output =====
  // A free-standing union exists if max_flow < total_positive_balances
  if(flow < sum_positive_balances) {
    std::cout << "yes" << std::endl;
  } else {
    std::cout << "no" << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  
  int n_tests;
  std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
  return 0;
}
```
</details>

## ⚡ Result

```plaintext

```