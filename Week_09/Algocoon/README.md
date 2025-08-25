# Algocoön Group

## 📝 Problem Description

The problem asks for the minimum cost to partition a set of figures into two non-empty groups. We are given $N$ figures, indexed from $0$ to $N-1$, and a list of $M$ limbs that connect them. Each limb is described by the two figures it connects, say $a$ and $b$, and a cost $c$ to cut it.

A partition of the figures into two groups defines a "cut". The total cost of this cut is the sum of the costs of all limbs connecting a figure in the first group to a figure in the second group. The objective is to find a partition that minimizes this total cost, under the constraint that both groups must be non-empty. For each sculpture described, the program should output this single minimum cost.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem asks for a minimum cost to "cut" a set of interconnected items into two distinct groups. This phrasing is a strong clue towards a specific family of algorithms. How can you model the figures and their connections in a way that allows you to apply a standard algorithm for finding a minimum cut?
</details>
<details>
<summary>Hint #2</summary>
This problem can be modeled using a graph. Let each figure be a vertex and each limb be an edge between the corresponding vertices. The cost associated with cutting a limb can be represented as the capacity of that edge. The problem is now equivalent to finding a minimum cut that partitions the graph's vertices into two non-empty sets.
</details>
<details>
<summary>Hint #3</summary>
A standard minimum cut is defined between a source vertex $s$ and a sink vertex $t$. This cut separates the vertices into two sets: one containing $s$ (the source side) and the other containing $t$ (the sink side). The problem requires finding the minimum cut among all possible non-trivial partitions. How can we choose $s$ and $t$ to guarantee we find the overall minimum cut? Consider fixing one vertex and exploring its relationship with all other vertices.
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1)</summary>
This problem can be framed as a **Minimum Cut** problem on a graph. The direct connection between cutting limbs and finding a minimum cut in a graph is a key observation.

### Graph Representation
We can model the sculpture as a graph where:
- Each of the $N$ figures is a **vertex**.
- Each limb connecting two figures is an **edge** between the corresponding vertices. Since a limb from figure $a$ to figure $b$ is the same as one from $b$ to $a$, we can think of this as an undirected connection. When modeling for max-flow, we represent this with two directed edges. The **capacity** of an edge is set to the cost of cutting the corresponding limb.

### Applying the Max-Flow Min-Cut Theorem
The famous **Max-Flow Min-Cut Theorem** states that the maximum flow between a source vertex $s$ and a sink vertex $t$ in a network is equal to the minimum capacity of an $s-t$ cut. An $s-t$ cut is a partition of the vertices into two sets, $S$ and $T$, such that $s \in S$ and $t \in T$. The capacity of the cut is the sum of capacities of all edges going from $S$ to $T$.

For the first test set, we are given a crucial hint: there is an optimal solution where you get Figure 0 and your friend gets Figure $N-1$. This directly tells us which vertices to use as the source and sink.
- We can designate **vertex 0 as the source ($s$)** and **vertex $N-1$ as the sink ($t$)**.
- By finding the maximum flow from $s$ to $t$, we simultaneously find the minimum cost to separate vertex 0 from vertex $N-1$.

Since the problem guarantees that this specific partition is optimal, a single max-flow computation is sufficient. The code handles multiple limbs between the same two figures by adding their capacities, which is implicitly done by adding parallel edges in the graph representation.

```cpp
#include<iostream>
#include<vector>
#include<queue>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/tuple/tuple.hpp>

typedef  boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
  boost::property<boost::edge_capacity_t, long,
    boost::property<boost::edge_residual_capacity_t, long,
      boost::property<boost::edge_reverse_t, traits::edge_descriptor> > > >  graph;
// Interior Property Maps
typedef  boost::graph_traits<graph>::edge_descriptor      edge_desc;
typedef  boost::graph_traits<graph>::out_edge_iterator      out_edge_it;

class edge_adder {
 graph &G;

 public:
  explicit edge_adder(graph &G) : G(G) {}

  void add_edge(int from, int to, long capacity) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    const edge_desc e = boost::add_edge(from, to, G).first;
    const edge_desc rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  }
};

void solve() {
  // ===== READ INPUT & CONSTRUCT GRAPH =====
  int n, m; std::cin >> n >> m;
  
  graph G(n);
  edge_adder adder(G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  
  // ? Accumulate parallel edges? 
  for(int i = 0; i < m; ++i) {
    int a, b, c; std::cin >> a >> b >> c;
    adder.add_edge(a, b, c);
  }
  
  // ====== CALCULATE MIN CUT =====
  // !!! For Test Set 1 we can fix source = 0, sink = n - 1 !!!
  const int v_source = 0;
  const int v_sink = n - 1;
  
  // Find a min cut via maxflow
  int flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  std::cout << flow << std::endl;
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
<summary>Second Solution (Test Set 1, 2)</summary>
For the second test set, the assumption is relaxed. We are now only guaranteed that an optimal solution exists where you get Figure 0. This means Figure 0 is in your partition (let's call it $S$), but your friend's partition ($T$) can be anchored by *any* other figure.

### Approach
Since we know Figure 0 will be on the source side of our cut, we can fix **vertex 0 as the source ($s$)**. The problem is that we don't know which vertex from the other partition to pick as the sink ($t$).

The constraint that both partitions must be non-empty implies that the friend's partition $T$ must contain at least one figure, say figure $j$. The cost of this partition is the capacity of the minimum cut separating $S$ and $T$. This cut is an $s-t$ cut where $s \in S$ and $t \in T$.

Since we know $s=0$, we can find the optimal partition by trying every other possible vertex as the sink. We iterate through all vertices $i \in \{1, 2, \dots, n-1\}$, set each one as the sink, and compute the max-flow from source 0 to sink $i$. The overall minimum cost will be the minimum value found across all these computations.

This approach is effectively a brute-force search for the best partner vertex for our fixed vertex 0.

### Algorithm
1. Build the same graph as in the first solution.
2. Fix `v_source = 0`.
3. Initialize `min_cut` to a very large value.
4. Loop through every other vertex `i` from `1` to `n-1`:
   a. Set `v_sink = i`.
   b. Calculate the max-flow from `v_source` to `v_sink`. Note that max-flow algorithms modify the graph's residual capacities, so we must reconstruct the graph for each sink candidate.
   c. Update `min_cut = min(min_cut, flow)`.
5. The final `min_cut` is the answer.

*Note: A more efficient implementation would avoid rebuilding the entire graph. One could save the initial capacities and restore them before each max-flow call. The provided code is simpler but correct for the given constraints.*

```cpp
#include<iostream>
#include<vector>
#include<queue>
#include<limits>
#include<cmath>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/tuple/tuple.hpp>

typedef  boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
  boost::property<boost::edge_capacity_t, long,
    boost::property<boost::edge_residual_capacity_t, long,
      boost::property<boost::edge_reverse_t, traits::edge_descriptor> > > >  graph;
// Interior Property Maps
typedef  boost::graph_traits<graph>::edge_descriptor      edge_desc;
typedef  boost::graph_traits<graph>::out_edge_iterator      out_edge_it;

class edge_adder {
 graph &G;

 public:
  explicit edge_adder(graph &G) : G(G) {}

  void add_edge(int from, int to, long capacity) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    const edge_desc e = boost::add_edge(from, to, G).first;
    const edge_desc rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  }
};

void solve() {
  // ===== READ INPUT & CONSTRUCT GRAPH =====
  int n, m; std::cin >> n >> m;
  
  graph G(n);
  edge_adder adder(G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  
  // ? Accumulate parallel edges? 
  for(int i = 0; i < m; ++i) {
    int a, b, c; std::cin >> a >> b >> c;
    adder.add_edge(a, b, c);
  }
  
  // ====== CALCULATE MIN CUT =====
  // !!! For Test Set 2 we can fix v_source = 0 !!!
  const int v_source = 0;
  
  int min_cut = std::numeric_limits<int>::max();
  
  // Consider all other nodes as sinks and look for the min cut
  for(int i = 0; i < n; ++i) {
    int flow = boost::push_relabel_max_flow(G, v_source, i);
    min_cut = std::min(min_cut, flow);
  }

  std::cout << min_cut << std::endl;
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
<summary>Third Solution (Test Set 3)</summary>
For the third test set, all special assumptions are removed. We need to find the **global minimum cut** of the graph, which is the non-trivial cut of minimum capacity over all possible pairs of partitions.

### Brute-Force Approach
A straightforward way to find the global minimum cut is to find the minimum $s-t$ cut for every possible pair of distinct vertices $(s, t)$. Since any non-trivial cut separates at least two vertices, the global minimum cut must be an $s-t$ cut for some pair $(s, t)$.

This leads to a simple, albeit inefficient, algorithm:
1. Build the graph from the input. To handle parallel edges cleanly, we can first accumulate the costs in an adjacency matrix and then build the graph.
2. Initialize `min_cut` to a very large value.
3. Iterate through every vertex `s` from `0` to `n-1`.
4. Inside this loop, iterate through every other vertex `t` from `0` to `n-1`.
5. If `s != t`:
   a. Calculate the max-flow from `s` to `t`. Remember to restore the graph's capacities before each calculation.
   b. Update `min_cut = min(min_cut, flow)`.
6. The final `min_cut` is the answer.

### Complexity Analysis
The number of pairs $(s, t)$ is $N \times (N-1)$, which is $O(N^2)$. The push-relabel max-flow algorithm has a complexity of roughly $O(N^3)$ in practice on general graphs. Therefore, the total time complexity of this approach is $O(N^2 \cdot N^3) = O(N^5)$. For Test Set 3 with $N \le 50$, this is feasible, but it is too slow for the full constraints.

```cpp
#include<iostream>
#include<vector>
#include<queue>
#include<limits>
#include<cmath>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/tuple/tuple.hpp>

typedef  boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
  boost::property<boost::edge_capacity_t, long,
    boost::property<boost::edge_residual_capacity_t, long,
      boost::property<boost::edge_reverse_t, traits::edge_descriptor> > > >  graph;
// Interior Property Maps
typedef  boost::graph_traits<graph>::edge_descriptor      edge_desc;
typedef  boost::graph_traits<graph>::out_edge_iterator      out_edge_it;

class edge_adder {
 graph &G;

 public:
  explicit edge_adder(graph &G) : G(G) {}

  void add_edge(int from, int to, long capacity) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    const edge_desc e = boost::add_edge(from, to, G).first;
    const edge_desc rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  }
};

void solve() {
  // ===== READ INPUT =====
  int n, m; std::cin >> n >> m;
  
  std::vector<std::vector<int>> adj_mat(n, std::vector<int>(n, 0));
  for(int i = 0; i < m; ++i) {
    int a, b, c; std::cin >> a >> b >> c;
    adj_mat[a][b] += c;
  }
  
  // ===== CONSTRUCT GRAPH =====
  graph G(n);
  edge_adder adder(G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  
  for(int i = 0; i < n; ++i) {
    for(int j = 0; j < n; ++j) {
      if(adj_mat[i][j]) {
        adder.add_edge(i, j, adj_mat[i][j]);
      }
    }
  }
  
  // ====== CALCULATE MIN CUT =====
  int min_cut = std::numeric_limits<int>::max();
  
  // Consider all other nodes as sinks and look for the min cut
  for(int v_source = 0; v_source < n; ++v_source) {
    for(int v_sink = 0; v_sink < n; ++v_sink) {
      int flow = boost::push_relabel_max_flow(G, v_source, v_sink);
      min_cut = std::min(min_cut, flow);
    }
  }

  std::cout << min_cut << std::endl;
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
The brute-force $O(N^5)$ approach is too slow for the full constraints. We need a more efficient way to find the global minimum cut. Instead of iterating through all $O(N^2)$ pairs of vertices, we can find the solution by performing only $O(N)$ max-flow computations.

### The Key Insight
Let the global minimum cut partition the vertices into sets $S_{opt}$ and $T_{opt}$. Now, let's pick an arbitrary vertex, for instance, **vertex 0**. In the optimal partition, vertex 0 must belong to either $S_{opt}$ or $T_{opt}$. This gives us two exhaustive cases:

1.  **Case A: Vertex 0 is in $S_{opt}$**.
    Since $T_{opt}$ is non-empty, it must contain at least one vertex, say $j$. The capacity of the cut $(S_{opt}, T_{opt})$ is equal to the capacity of the minimum $s-t$ cut for *any* pair of vertices $s \in S_{opt}$ and $t \in T_{opt}$. In particular, its capacity is equal to the minimum $0-j$ cut. To find the minimum cut under this case, we can compute the minimum $0-i$ cut for all other vertices $i \neq 0$. This is precisely the logic from our second solution: `min(max_flow(0, i))` for $i \in \{1, \dots, n-1\}$.

2.  **Case B: Vertex 0 is in $T_{opt}$**.
    Similarly, $S_{opt}$ must contain at least one vertex, say $i$. The capacity of the optimal cut is equal to the minimum $i-0$ cut. To cover this case, we can compute the minimum $i-0$ cut for all other vertices $i \neq 0$. This means we calculate `min(max_flow(i, 0))` for $i \in \{1, \dots, n-1\}$.

Since any optimal cut must fall into either Case A or Case B, the global minimum cut must be among the values we computed. By taking the minimum over all $2(N-1)$ max-flow computations, we are guaranteed to find the global minimum.

### Algorithm
1. Build the graph from the input, aggregating capacities of parallel edges.
2. Fix one vertex, say `v_ref = 0`.
3. Initialize `min_cut` to a very large value.
4. For each vertex `i` from `1` to `n-1`:
   a. Calculate `flow = max_flow(v_ref, i)`. Update `min_cut = min(min_cut, flow)`.
5. The resulting `min_cut` is the answer.

This reduces the number of max-flow computations from $O(N^2)$ to $O(N-1)$, yielding a total complexity of $O(N \cdot \text{MaxFlow})$, which is efficient enough to pass all test sets.

*Note: In an undirected graph, the min $s-t$ cut is the same as the min $t-s$ cut. So we only need to iterate through all `max_flow(0, i)` for `i=1...n-1`. The provided code calculates both `max_flow(0, i)` and `max_flow(i, 0)` which is redundant but correct.*

```cpp
#include<iostream>
#include<vector>
#include<queue>
#include<limits>
#include<cmath>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/tuple/tuple.hpp>

typedef  boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
  boost::property<boost::edge_capacity_t, long,
    boost::property<boost::edge_residual_capacity_t, long,
      boost::property<boost::edge_reverse_t, traits::edge_descriptor> > > >  graph;
// Interior Property Maps
typedef  boost::graph_traits<graph>::edge_descriptor      edge_desc;
typedef  boost::graph_traits<graph>::out_edge_iterator      out_edge_it;

class edge_adder {
 graph &G;

 public:
  explicit edge_adder(graph &G) : G(G) {}

  void add_edge(int from, int to, long capacity) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    const edge_desc e = boost::add_edge(from, to, G).first;
    const edge_desc rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  }
};

void solve() {
  // ===== READ INPUT =====
  int n, m; std::cin >> n >> m;
  
  std::vector<std::vector<int>> adj_mat(n, std::vector<int>(n, 0));
  for(int i = 0; i < m; ++i) {
    int a, b, c; std::cin >> a >> b >> c;
    adj_mat[a][b] += c;
  }
  
  // ===== CONSTRUCT GRAPH =====
  graph G(n);
  edge_adder adder(G);

  for(int i = 0; i < n; ++i) {
    for(int j = 0; j < n; ++j) {
      if(adj_mat[i][j]) {
        adder.add_edge(i, j, adj_mat[i][j]);
      }
    }
  }
  
  // ====== CALCULATE MIN CUT =====
  int min_cut = std::numeric_limits<int>::max();
  
  // Consider all other nodes as sinks and look for the min cut
  for(int i = 0; i < n; ++i) {
    min_cut = std::min(min_cut, (int) boost::push_relabel_max_flow(G, 0, i));
    min_cut = std::min(min_cut, (int) boost::push_relabel_max_flow(G, i, 0));
  }

  std::cout << min_cut << std::endl;
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
   Test set 1 (20 pts / 3 s) : Correct answer      (1.045s)
   Test set 2 (20 pts / 3 s) : Correct answer      (0.929s)
   Test set 3 (20 pts / 3 s) : Correct answer      (0.047s)
   Test set 4 (20 pts / 3 s) : Correct answer      (1.027s)
   Test set 5 (20 pts / 3 s) : Correct answer      (0.862s)

Total score: 100
```