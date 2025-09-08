# Real Estate Market

## 📝 Problem Description

The goal is to maximize the profit from selling a set of land sites to a group of potential buyers. We are given $N$ buyers and $M$ sites. Each site is located in one of $S$ different states.

For each of the $N$ buyers, we have a list of their bids, $b_{i,j}$, which is the amount buyer $i$ is willing to pay for site $j$. The assignment of sites to buyers must adhere to the following rules:
1.  Each buyer can be assigned at most one site.
2.  Each site can be sold to at most one buyer.
3.  Each state $k$ has a limit, $l_k$, on the maximum number of sites that can be sold within that state.

The task is to determine which sites to sell and to which buyers, respecting all constraints, such that the total profit is maximized.

## 💡 Hints

<details>

<summary>Hint #1</summary>

This problem requires assigning items from one group (buyers) to another (sites) to maximize a total value, subject to certain constraints. Think about how to represent the buyers, the sites, and the possible transactions. This structure is a classic sign of an assignment problem, which can often be modeled using a flow network.

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

You simply need to adjust how the final output is calculated.

</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Set 1, 2, 3, 4)</summary>

When reading the problem, we can notice, that we we have 2 “sets”. The set of all sites/lands and the set of all bidders. As we only have connections in-between them we could model this as a **Bipartite Graph**. We would now have to find an assignment between bidders and sites in this graph that maximizes our profit. Usually something like this would be done using Max Flow, but for this we have several problems.

1. Each bidder can only buy one site, which would make us set the capacities to $1$. But this ignores the price, which we want to maximize
2. If we choose the price as capacities, we do not have any good way to ensure that only 1 bidder can buy a site
3. We can not model the limit of the number of sites sold per state

Problem 1. and 2. boil down to the fact, that we **only have 1 value per edge**, but we actually **want to store 2 information**:

1. If a bidder buys something (what we would usually model with the capacity)
2. The price the bidder pays for the site (which we want to maximize)

This leads to using a Min Cost Max Flow approach

Therefore, we can **set all the capacities to** $1$, to ensure that **one bidder can only buy $1$ site**, and **each site can only be bought by $1$  bidder** (standard bipartite Max Flow). 
For the cost this leaves us with the bid $b_{ij}$ connecting bidder $i$ with site $j$. However we **do not want to minimize this,** instead we want to maximize it. Therefore, we can just **flip the sign** and use the cost $-b_{ij}$ instead of $b_{ij}$. We now only have to flip the sign of the final cost in the end.

**Note**: Of course, we only want the cost to take effect in between bidders and sites. So we simply set the cost to $0$ for all other edges

The last problem can easily be solved now, by **adding an additional “layer” between the sites and the sink**. This layer will **model our states**. Each site is then connected to the state it is in. To ensure that in each state, we only sell as many sites as we are allowed to, we can now **set the capacity from the site to the sink, to exactly that limit,** to ensure that no more sites are sold.

This will then solve the first 4 test cases, but for the 5th the code is too slow, as we get a `Time Limit Exceeded`

### Code
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
                boost::property <boost::edge_weight_t, long> > > > > graph; // new! weightmap corresponds to costs

typedef traits::vertex_descriptor vertex_desc;
typedef boost::graph_traits<graph>::edge_descriptor edge_desc;
typedef boost::graph_traits<graph>::out_edge_iterator out_edge_it; // Iterator

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

void solve() {
  // ===== READ INPUT =====
  int N, M, S; std::cin >> N >> M >> S;
  
  std::vector<int> limits(S);
  for(int i = 0; i < S; ++i) {
    std::cin >> limits[i];
  }
  
  std::vector<int> property_to_state(M);
  for(int i = 0; i < M; ++i) {
    std::cin >> property_to_state[i];
  }
  
  std::vector<std::vector<int>> bids(N, std::vector<int>(M));
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < M; ++j) {
      std::cin >> bids[i][j];
    }
  }
  
  // ===== BUILD GRAPH =====
  graph G(N + M + S);
  edge_adder adder(G);

  // Add Source and Sink
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  // Connect Source to Bidders and Bidders to Sites
  for(int i = 0; i < N; ++i) {
    adder.add_edge(v_source, i, 1, 0);
    
    for(int j = 0; j < M; j++) {
      adder.add_edge(i, N + j, 1, - bids[i][j]);
    }
  }
  
  // Connect Sites to States
  for(int i = 0; i < M; ++i) {
    adder.add_edge(N + i, N + M + property_to_state[i] - 1, 1, 0);
  }
  
  // Connect States to Sink
  for(int i = 0; i < S; ++i) {
    adder.add_edge(N + M + i, v_sink, limits[i], 0);
  }

  // ====== CALCULATE FLOW AND COST =====
  int flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  boost::cycle_canceling(G);
  int cost = boost::find_flow_cost(G);
    
  // ===== OUTPUT =====
  std::cout << flow << " " << -cost << std::endl;
}


int main() {
  int T; std::cin >> T;
  
  while(T--) {
    solve();
  }
}
```
</details>

<details>

<summary>Final Solution</summary>

The problem with the previous code was that we are **limited to using** `boost::cycle_canceling` as we have **negative costs**. This is slower in comparison to `boost::successive_shortest_path_nonnegative_weights` but we have to deal with the negative costs.

However, the problem also gives us an **upper bound on the bids**, being $b_{ij} \leq 100$. We can exploit this by **interpreting the cost differently**.

Lets assume that we **expect to sell each site for the maximum price**, which is $100$ as stated in the problem. This would yield us a profit of $n_\text{sold} \cdot 100$. However, we will probably not sell every site for $100$. What we can do now, is that we **choose the cost as the difference $100 - b_{ij}$ between the price we expected ($100$) and the actual price we got** $b_{ij}$. This will:

1. Ensure **all the costs are non-negative**, as $b_{ij} \leq 100$
2. If we minimize the cost, we **minimize the amount of money we “lost out on”**, which will yield the highest profit

If it is easier for you, you can also just say we “remapped”/re-”parametrized” the cost to be non-negative

We now only need to adjust how we calculate the profit:

$$
\text{profit} = n_\text{sold} \cdot 100 - \text{cost}
$$

### Code
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
                boost::property <boost::edge_weight_t, long> > > > > graph; // new! weightmap corresponds to costs

typedef traits::vertex_descriptor vertex_desc;
typedef boost::graph_traits<graph>::edge_descriptor edge_desc;
typedef boost::graph_traits<graph>::out_edge_iterator out_edge_it; // Iterator

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

void solve() {
  // ===== READ INPUT =====
  int N, M, S; std::cin >> N >> M >> S;
  
  std::vector<int> limits(S);
  for(int i = 0; i < S; ++i) {
    std::cin >> limits[i];
  }
  
  std::vector<int> property_to_state(M);
  for(int i = 0; i < M; ++i) {
    std::cin >> property_to_state[i];
  }
  
  std::vector<std::vector<int>> bids(N, std::vector<int>(M));
  for(int i = 0; i < N; ++i) {
    for(int j = 0; j < M; ++j) {
      std::cin >> bids[i][j];
    }
  }
  
  // ===== BUILD GRAPH =====
  graph G(N + M + S);
  edge_adder adder(G);
  auto c_map = boost::get(boost::edge_capacity, G);
  auto r_map = boost::get(boost::edge_reverse, G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  
  // Add Source and Sink
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  // Connect Source to Bidders and Bidders to Sites
  for(int i = 0; i < N; ++i) {
    adder.add_edge(v_source, i, 1, 0);
    
    for(int j = 0; j < M; j++) {
      adder.add_edge(i, N + j, 1, 100 - bids[i][j]);
    }
  }
  
  // Connect Sites to States
  for(int i = 0; i < M; ++i) {
    adder.add_edge(N + i, N + M + property_to_state[i] - 1, 1, 0);
  }
  
  // Connect States to Sink
  for(int i = 0; i < S; ++i) {
    adder.add_edge(N + M + i, v_sink, limits[i], 0);
  }

  // ====== CALCULATE FLOW AND COST =====
  boost::successive_shortest_path_nonnegative_weights(G, v_source, v_sink);
  int cost = boost::find_flow_cost(G);

  int sold = 0;
  out_edge_it e, eend;
  for(boost::tie(e, eend) = boost::out_edges(boost::vertex(v_sink, G), G); e != eend; ++e)
    sold += rc_map[*e] - c_map[*e];  

  int revenue = 100 * sold - cost;
  std::cout << sold << " " << revenue << std::endl;
}


int main() {
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