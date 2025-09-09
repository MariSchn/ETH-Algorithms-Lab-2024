# Algocoon Group

## 📝 Problem Description

The problem asks for the minimum cost to partition a set of figures into two non-empty groups. We are given $N$ figures, indexed from $0$ to $N-1$, and a list of $M$ limbs that connect them. Each limb is described by the two figures it connects, say $a$ and $b$, and a cost $c$ to cut it.

A partition of the figures into two groups defines a "cut". The total cost of this cut is the sum of the costs of all limbs connecting a figure in the first group to a figure in the second group. The objective is to find a partition that minimizes this total cost, under the constraint that both groups must be non-empty.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem asks for a minimum cost to "cut" a set of interconnected items into two distinct groups. This phrasing is a strong clue towards a specific family of algorithms. How can you model the figures and their connections in a way that allows you to apply a standard algorithm for finding a minimum cut?

</details>

<details>

<summary>Hint #2</summary>

This problem can be modeled using a graph. Let each figure be a nodex and each limb be an edge between the corresponding nodes. The cost associated with cutting a limb can be represented as the capacity of that edge. The problem is now equivalent to finding a minimum cut that partitions the graph's nodes into two non-empty sets.

</details>

<details>

<summary>Hint #3</summary>

A standard minimum cut is defined between a source nodex $s$ and a sink nodex $t$. This cut separates the nodes into two sets: one containing $s$ (the source side) and the other containing $t$ (the sink side). The problem requires finding the minimum cut among all possible non-trivial partitions. How can we choose $s$ and $t$ to guarantee we find the overall minimum cut? Consider fixing one nodex and exploring its relationship with all other nodes.

</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Set 1)</summary>

By reading the problem description we already get a feeling that this is going to be a **Minimum Cut** problem.

As we want to cut the individual figures, we will design our graph as follows:

- Each **figure becomes a node**.
- The **limbs become edges between nodes**, with their corresponding cost as capacity.

The main challenge we now have is, that we have to **ensure, that both people get at least one figure**. More formally this means, that there needs to be **at least one node in $S$ and $T = V\setminus S$**. 

Given the assumption on the test set 1, we know that there exists a solution where A gets figure $0$ and B gets figure $n-1$. So we can **simply use these 2 figures/nodes as source and sink**.

By choosing nodes that are already in the graph, we can ensure that $S$ and $T$ will be non-empty as obviously, after the max flow the source $s$ will be connected to the source, so $s \in S$ and the sink $t$ will be connected to the sink, so $t \in T$. 

We can then simply **calculate the Min Cut by calculating the Max Flow**, as the **Maxflow-Mincut-Theorem** states that both are equal.

### Code
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

The only difference when moving from test set 1 to test set 2 is that we can now **no longer assume that the optimal solution assigns $n-1$ to B**.
What we can simply do, is that we **loop over all nodes**, assign them to B (assign them as sink) and calculate the flow.

In the end we simply return the **minimum of the cuts we found**.

This is a pretty "brute-forcy” approach, that can be improved, see the next solution

### Code
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
A straightforward way to find the global minimum cut is to find the minimum $s-t$ cut for every possible pair of distinct nodes $(s, t)$. Since any non-trivial cut separates at least two nodes, the global minimum cut must be an $s-t$ cut for some pair $(s, t)$.

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

To make the third solution faster, we simply need to **exploit the fact that we have already used in the first 2 solutions**. In general we basically only **need 2 nodes such that one of them is in $S$ and the other in $T$**, and then we can calculate the max flow to get the min cut.

However, now we do **not know anything about which node belongs to which set**. Intuitively we would therefore just iterate over all pairs (like in the third solution). This, as we have seen, is to inefficient.

BUT, we know that **each node has to belong to either $S$ or $T$**. What we can therefore do is **fix one node** (e.g. node $0$) and **use it as a source** and **compute the max flow to all other nodes** (/consider all other nodes as sinks). If $0 \in S$, then this will already yield the min cut. 
This is because for at least one other node $i$, we will basically have the same situation as in Test set 1, where this also worked. 
Note, that up until now we are basically **doing the same as in the second Solution**.

This will only fail if $0 \in T$, since then our assumption that $0 \in S$ does no longer hold. **BUT** if $0 \in T$, we can do the **exact same as before**, just that we know use $0$ as a sink and compute the max flow from all other nodes (consider every other node a source). 
With this we have then covered both cases $0 \in S$ and $0 \in T$, and can therefore say for sure, that we have found the min cut, while only needing to run max flow $O(2N)$ times instead of $O(N^2)$ times.

### Code
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