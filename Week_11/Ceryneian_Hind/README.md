# Ceryneian Hind

## 📝 Problem Description
Given a graph consisting of $N$ locations and $M$ directed edges, each node $i$ is assigned an integer convenience score $s_i$.

A "semi-dead end" is defined as a non-empty subset of nodes $S$ such that no path starts from a node in $S$ and ends at a node outside of $S$.

The objective is to determine a semi-dead end $S$ that maximizes the sum of convenience scores for all nodes within $S$. If every possible semi-dead end yields a total score that is zero or negative, the task is deemed impossible and should be reported as such. Otherwise, the maximum achievable positive score should be provided.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem asks you to partition all locations into two sets: the semi-dead end $S$ and the rest of the locations, $V \setminus S$. The defining property of $S$ is that no path goes from a location in $S$ to a location in $V \setminus S$. This structure, a partition of elements with constraints on connections between the partitions, is a strong indicator that the problem can be modeled as a minimum cut problem in a graph.

</details>

<details>

<summary>Hint #2</summary>

The goal is to maximize the sum of scores in the set $S$. This is a maximization problem. Minimum cut, on the other hand, is a minimization problem. It's often helpful to rephrase the maximization objective as a minimization one. Consider the total sum of all *positive* convenience scores on the map. Let this be $P$. Any semi-dead end $S$ we choose will have a score of $\sum_{i \in S} s_i$. Maximizing this is equivalent to minimizing $P - \sum_{i \in S} s_i$. Can you express this "loss" in terms of the locations we *don't* choose for $S$ and the negative-score locations we *do* choose for $S$?

</details>

<details>

<summary>Hint #3</summary>

Let's build a flow network. Create a source node `source` and a sink node `sink`.
1.  For every location `i` with a positive score $s_i > 0$, add a directed connection from `source` to `i` with capacity $s_i$.
2.  For every location `i` with a negative score $s_i < 0$, add a directed connection from `i` to `sink` with capacity $-s_i$.
3.  For every original path from location `u` to `v`, add a directed connection from `u` to `v` in our new network. What should its capacity be? The condition is that no path can leave the set $S$. This means if `u` is in $S$ and `v` is not, this configuration should be "forbidden". We can forbid it by assigning an infinite capacity to the connection `(u, v)`.

Now, consider any `source-sink` cut in this network. The cut will partition the locations. The capacity of this cut corresponds exactly to the "loss" we identified in the previous hint. By the max-flow min-cut theorem, finding the minimum cut is equivalent to finding the maximum flow.

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

This problem can be solved by transforming it into a minimum cut problem in a specially constructed flow network. The core idea relies on the **max-flow min-cut theorem**.

### Problem Reformulation
We want to partition the set of all locations $V$ into two disjoint sets: our target semi-dead end $S$ and its complement $T = V \setminus S$. The goal is to maximize the total score of nodes in $S$:
$$ \text{maximize} \left( \sum_{i \in S} s_i \right) $$
This is equivalent to minimizing a "loss" or "cost". Let $P$ be the sum of all positive convenience scores, i.e., $P = \sum_{i \in V, s_i > 0} s_i$. The maximization problem can be rewritten as:
$$ \text{maximize} \left( \sum_{i \in S} s_i \right) \iff \text{minimize} \left( P - \sum_{i \in S} s_i \right) $$
The loss term can be further expanded:
$$ P - \sum_{i \in S} s_i = \sum_{\substack{i \in V \\ s_i > 0}} s_i - \left( \sum_{\substack{i \in S \\ s_i > 0}} s_i + \sum_{\substack{i \in S \\ s_i < 0}} s_i \right) = \sum_{\substack{i \in T \\ s_i > 0}} s_i + \sum_{\substack{i \in S \\ s_i < 0}} (-s_i) $$
This new objective is what we will minimize. It represents the cost of our partition: the sum of positive scores we "lose" by placing them in $T$, plus the sum of penalties we "incur" by including negative-score locations in $S$.

### Graph Construction
We build a flow network with a source `v_source` and a sink `v_sink`:
1.  **Source Edges:** For each location $i$ with a positive score $s_i > 0$, we add an edge from `v_source` to node $i$ with capacity $s_i$. If we cut this edge, it means node $i$ is on the sink-side of the cut (in $T$), and we pay a cost of $s_i$.
2.  **Sink Edges:** For each location $i$ with a negative score $s_i < 0$, we add an edge from node $i$ to `v_sink` with capacity $-s_i$. If we cut this edge, it means node $i$ is on the source-side of the cut (in $S$), and we pay a cost of $-s_i$.
3.  **Path Edges:** For each original path from location $u$ to $v$, we add an edge from node $u$ to node $v$ with **infinite capacity**. This is the crucial step that enforces the semi-dead end property. Any finite `source-sink` cut cannot place $u$ on the source-side ($S$) and $v$ on the sink-side ($T$) simultaneously, because this would require cutting an edge of infinite capacity. This perfectly matches the definition of a semi-dead end: no paths can go from $S$ to $T$.

### Calculating the Result
A minimum `source-sink` cut in this graph partitions the nodes into a source set (our desired semi-dead end $S$, plus `v_source`) and a sink set ($T$, plus `v_sink`). The capacity of this minimum cut is exactly the minimum possible value for our loss expression: $\sum_{i \in T, s_i > 0} s_i + \sum_{i \in S, s_i < 0} (-s_i)$.

By the max-flow min-cut theorem, the value of the minimum cut is equal to the value of the maximum flow from `v_source` to `v_sink`. So, we can compute the max flow to find this minimum loss.

The final maximum convenience score is:
$$ \text{Max Score} = (\text{Sum of all positive scores}) - (\text{Max Flow}) $$
If this result is not strictly positive, no suitable semi-dead end exists.

### Code

```cpp
#include <iostream>
#include <vector>
#include <limits>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor>>>> graph;

typedef traits::vertex_descriptor vertex_desc;
typedef traits::edge_descriptor edge_desc;

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

const long MAX_LONG = std::numeric_limits<long>::max();

void solve() {
  // ===== READ INPUT =====
  int n, m; std::cin >> n >> m;
  
  std::vector<int> conveniences(n);
  for(int i = 0; i < n; ++i) { 
    int s; std::cin >> s;
    conveniences[i] = s; 
  }
  
  std::vector<std::pair<int, int>> edges; edges.reserve(m);
  for(int i = 0; i < m; ++i) {
    int u, v; std::cin >> u >> v;
    edges.emplace_back(u, v);
  }
  
  // ===== SOLVE =====
  graph G(n);
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  // Add source and sink connections
  int positive_sum = 0;
  for(int i = 0; i < n; ++i) {
    if(conveniences[i] > 0) {
      adder.add_edge(v_source, i, conveniences[i]);
      positive_sum += conveniences[i];
    } else {
      adder.add_edge(i, v_sink, -conveniences[i]);
    }
  }
  
  // Add edges
  for(const std::pair<int, int> e : edges) {
    adder.add_edge(e.first, e.second, MAX_LONG);
  }
  
  long flow = boost::push_relabel_max_flow(G, v_source, v_sink);

  // ===== OUTPUT =====
  if (positive_sum - flow > 0) {
    std::cout << positive_sum - flow << std::endl;
  } else {
    std::cout << "impossible" << std::endl;
  }
  
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
   Test set 1        (25 pts / 0.500 s) : Correct answer      (0.016s)
   Test set 2        (25 pts / 0.500 s) : Correct answer      (0.024s)
   Test set 3        (15 pts / 0.500 s) : Correct answer      (0.019s)
   Test set 4        (15 pts / 1.000 s) : Correct answer      (0.045s)
   Hidden test set 1 (05 pts / 0.500 s) : Correct answer      (0.016s)
   Hidden test set 2 (05 pts / 0.500 s) : Correct answer      (0.02s)
   Hidden test set 3 (05 pts / 0.500 s) : Correct answer      (0.018s)
   Hidden test set 4 (05 pts / 1.000 s) : Correct answer      (0.047s)

Total score: 100
```