# Kingdom Defense

## 📝 Problem Description

Given a set of $l$ locations within a kingdom, each location is assigned an initial number of stationed soldiers, $g_i$, and a required number of soldiers for defense, $d_i$.

The kingdom features $p$ directed paths, where each path $j$ leads from a starting location $f_j$ to a destination location $t_j$. Each path must be traversed by at least $c_j$ soldiers and at most $C_j$ soldiers. A single soldier may traverse a path multiple times, with each traversal contributing to the total count.

The task is to determine whether it is possible to redistribute soldiers among the locations, adhering to all path constraints, so that every location $i$ ends up with at least $d_i$ soldiers for defense and each path $j$ has been traversed by at least $c_j$ soldiers.

## 💡 Hints

<details>

<summary>Hint #1</summary>

Think about the problem in terms of resources and constraints. The soldiers are a resource that needs to be moved from where they are to where they are needed. The paths act as channels with capacity limits. This structure is common in a particular class of algorithmic problems. How can you formally model this system of locations, paths, and soldier counts?

</details>

<details>

<summary>Hint #2</summary>

This problem can be modeled as a maximum flow problem on a graph. Consider creating a network with a single source and a single sink. How would you represent the locations, the initial number of soldiers, the required number of soldiers, and the path capacities as nodes and edges in this network?

</details>

<details>

<summary>Hint #3</summary>

The most challenging constraint is the *minimum* number of soldiers, $c_j$, required on each path. A standard max-flow algorithm only handles maximum capacities. Think about how to transform the problem to incorporate these lower bounds. Since $c_j$ soldiers *must* travel from location $f_j$ to $t_j$, this portion of the movement is non-negotiable. How does this mandatory movement affect the net number of available soldiers at locations $f_j$ and $t_j$ *before* you solve for the remaining, optional soldier movements?

</details>

<details>

<summary>Hint #4</summary>

To model the minimum number of soldiers, $c_j$, required on each path, you can use the concept of *lower bounds* on flow. For each path, treat $c_j$ as a mandatory flow that must be sent from $f_j$ to $t_j$. Before constructing your network, "pre-send" these $c_j$ units by adjusting the initial soldier counts: subtract $c_j$ from the starting location and add $c_j$ to the destination. Then, set the capacity of the path in your network to $C_j - c_j$, representing the additional soldiers that can optionally traverse the path. This transformation allows you to use standard maximum flow algorithms, which only handle upper bounds, while still enforcing the minimum requirements.
</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Set 1)</summary>

For the first test set, the problem is simplified as the minimum traversal requirement for any path is zero (i.e., $c_j = 0$). This allows us to model the problem as a standard maximum flow problem, where **one unit of flow corresponds to one soldier** being moved through the network.

### Graph Construction

We can represent the kingdom as a flow network:
-   **Nodes**: Each location $i$ becomes a node in our graph. We also introduce a global **source node ($S$)** and a global **sink node ($T$)**.
-   **Edges and Capacities**:
    1.  **Supply Edges**: For each location $i$, we add a directed edge from the source $S$ to node $i$ with a capacity of $g_i$. This represents the initial number of soldiers available at that location.
    2.  **Demand Edges**: For each location $i$, we add a directed edge from node $i$ to the sink $T$ with a capacity of $d_i$. This represents the number of soldiers required at that location for defense.
    3.  **Path Edges**: For each path $j$ from location $f_j$ to $t_j$, we add a directed edge from node $f_j$ to node $t_j$ with capacity $C_j$, which is the maximum number of soldiers that can use this path.

### Determining the Final Answer

The total number of soldiers required across the entire kingdom is the sum of all demands, $\sum d_i$. We want to see if it's possible to route soldiers from their initial locations (supply from $S$) to fulfill all defense requirements (demand at $T$), without violating path capacities.

We can solve this by calculating the maximum flow from the source $S$ to the sink $T$ in the constructed network.
-   If the **max flow is equal to the total demand ($\sum d_i$)**, it means that enough soldiers could be moved through the network to satisfy the requirements of every single location. Thus, a valid plan exists.
-   If the **max flow is less than the total demand**, it's impossible to meet all defense requirements, and no such plan exists.

**Note**: A simple sanity check can be done: if the total number of soldiers in the kingdom ($\sum g_i$) is less than the total required ($\sum d_i$), a solution is impossible.

The main limitation of this solution is that it is not compatible with the minimum soldier requirements on each path ($c_j > 0$). This limits it to only solve the first test set where $c_j = 0$.

### Code
```cpp
#include<iostream>
#include<vector>


#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, 
                              boost::vecS, 
                              boost::directedS, 
                              boost::no_property,
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

void solve() {
  // ===== READ INPUT & BUILD GRAPH =====
  int l, p; std::cin >> l >> p;
  
  graph G(l);
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  int in_capacity = 0;
  int out_capacity = 0;
  for(int i = 0; i < l; ++i) {
    int g, d; std::cin >> g >> d;
    adder.add_edge(v_source, i, g);
    adder.add_edge(i, v_sink, d);

    in_capacity += g;
    out_capacity += d;
  }
  
  if(in_capacity < out_capacity) {
    // Not enough soldiers 
    std::cout << "no" << std::endl;
    return;
  }
  
  for(int i = 0; i < p; ++i) {
    int f, t, c, C; std::cin >> f >> t >> c >> C;
    adder.add_edge(f, t, C); // TODO: Change this. This should only work for first test set
  }
  
  // ===== CALCULATE MAX FLOW =====
  long flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  // ===== OUTPUT =====
  if(flow >= out_capacity) {
    std::cout << "yes" << std::endl;
  } else {
    std::cout << "no" << std::endl;
  }
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

<summary>Second Solution (Test Set 1, 2, 4)</summary>

### Approach

To handle the general case where paths have a minimum traversal requirement $c_j > 0$, we need to adapt our max-flow model. The key insight is to realize that the movement of $c_j$ soldiers along path $j$ (from $f_j$ to $t_j$) is **mandatory**. We can account for these mandatory movements *before* setting up the flow network for the remaining optional movements.

For each path $j$ from $f_j$ to $t_j$ with a minimum requirement $c_j$:
1.  We assume these $c_j$ soldiers are moved. This "costs" location $f_j$, $c_j$ soldiers and "gives" location $t_j$, $c_j$ soldiers.
2.  We can adjust the initial soldier counts accordingly: the available soldiers at $f_j$ become $g_{f_j} - c_j$, and at $t_j$ they become $g_{t_j} + c_j$.
3.  After this mandatory movement, the path from $f_j$ to $t_j$ can still be used by additional soldiers. The remaining capacity for optional movement is $C_j - c_j$.

After performing this adjustment for all paths, we have a new set of "available" soldiers at each location. Let's call this adjusted count $g'_i$. The problem is now reduced to the one from the first test set: with $g'_i$ soldiers at each location $i$, can we satisfy the demand $d_i$ using paths with capacities $C_j - c_j$?

We build the same flow network as before, but with these new values:
-   **Supply Edges**: Edge from $S$ to $i$ with capacity $g'_i$.
-   **Demand Edges**: Edge from $i$ to $T$ with capacity $d_i$.
-   **Path Edges**: Edge from $f_j$ to $t_j$ with capacity $C_j - c_j$.

The logic remains the same: a solution is possible if the max flow equals the total demand $\sum d_i$.

**Note**: This approach has a subtle bug. The adjusted soldier count $g'_i$ can become negative if a location must dispatch more mandatory soldiers than it has. Passing a negative capacity to the max-flow algorithm leads to incorrect behavior. The final solution addresses this issue.

```cpp
#include<iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, 
                              boost::vecS, 
                              boost::directedS, 
                              boost::no_property,
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

void solve() {
  // ===== READ INPUT & BUILD GRAPH =====
  int l, p; std::cin >> l >> p;
  
  graph G(l);
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  std::vector<int> in_capacities(l);
  
  int in_capacity = 0;
  int out_capacity = 0;
  for(int i = 0; i < l; ++i) {
    int g, d; std::cin >> in_capacities[i] >> d;
    adder.add_edge(i, v_sink, d); // Only add "out_capacity"

    in_capacity += in_capacities[i];
    out_capacity += d;
  }
  
  if(in_capacity < out_capacity) {
    // Not enough soldiers 
    std::cout << "no" << std::endl;
    return;
  }
  
  for(int i = 0; i < p; ++i) {
    int f, t, c, C; std::cin >> f >> t >> c >> C;
    
    // Adjust "in_capacities" based on minimum amount of soldiers along a path (c)
    in_capacities[f] -= c;
    in_capacities[t] += c;
    
    adder.add_edge(f, t, C - c);
  }
  
  // Add "in_capacities" as edges
  for(int i = 0; i < l; ++i) {
    adder.add_edge(v_source, i, in_capacities[i]);
  }
  
  // ===== CALCULATE MAX FLOW =====
  long flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  // ===== OUTPUT =====
  if(flow >= out_capacity) {
    std::cout << "yes" << std::endl;
  } else {
    std::cout << "no" << std::endl;
  }
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

### Approach

This solution refines the previous one by correctly handling the case where the adjusted initial soldier count, $g'_i$, becomes negative.

As before, we first account for the mandatory flow $c_j$ on each path by calculating an *soldier* count, $g'_i$, for each location $i$.
$$g'_i = g_i - (\text{sum of } c_j \text{ for paths starting at } i) + (\text{sum of } c_k \text{ for paths ending at } i)$$

The problem is then to use the remaining path capacities ($C_j-c_j$) to move soldiers from locations with a surplus to satisfy the final demands $d_i$.

### Logic

We build the flow network with a source $S$ and sink $T$:
-   **Path Edges**: For each path $j$ from $f_j$ to $t_j$, add an edge with capacity $C_j - c_j$.
-   **Demand Edges**: For each location $i$, add an edge from $i$ to the sink $T$ with capacity $d_i$. The total demand is $\sum d_i$.
-   **Supply Edges**: This is the crucial part. An edge from the source $S$ to a location $i$ represents the supply of soldiers that location $i$ can contribute to the network.
    -   If $g'_i \geq 0$, location $i$ has a surplus (or is balanced) after mandatory movements. It can supply up to $g'_i$ soldiers. We add an edge $S \to i$ with capacity $g'_i$.
    -   If $g'_i < 0$, location $i$ is in a **deficit**. It needs to *receive* $|g'_i|$ soldiers just to fulfill its mandatory path commitments. It has no soldiers to supply to the network. Therefore, the capacity of the edge $S \to i$ must be **0**.

By setting the supply capacity to $\max(0, g'_i)$, we correctly model that deficit locations do not contribute soldiers, they only consume them.

The final check remains the same: we compute the max flow from $S$ to $T$. If the flow is at least the total demand $\sum d_i$, a valid plan exists. A preliminary check that the total number of soldiers in the kingdom is sufficient ($\sum g_i \geq \sum d_i$) is also a good practice.

```cpp
#include<iostream>
#include<vector>
#include<cmath>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, 
                              boost::vecS, 
                              boost::directedS, 
                              boost::no_property,
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

void solve() {
  // ===== READ INPUT & BUILD GRAPH =====
  int l, p; std::cin >> l >> p;
  
  graph G(l);
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  std::vector<int> in_capacities(l);
  
  int in_capacity = 0;
  int out_capacity = 0;
  for(int i = 0; i < l; ++i) {
    int d; std::cin >> in_capacities[i] >> d;
    adder.add_edge(i, v_sink, d); // Only add "out_capacity"

    in_capacity += in_capacities[i];
    out_capacity += d;
  }
  
  if(in_capacity < out_capacity) {
    // Not enough soldiers 
    std::cout << "no" << std::endl;
    return;
  }
  
  for(int i = 0; i < p; ++i) {
    int f, t, c, C; std::cin >> f >> t >> c >> C;
    
    // Adjust "in_capacities" based on minimum amount of soldiers along a path (c)
    in_capacities[f] -= c;
    in_capacities[t] += c;
    
    adder.add_edge(f, t, C - c);
  }
  
  // Add "in_capacities" as edges
  for(int i = 0; i < l; ++i) {
    adder.add_edge(v_source, i, std::max(in_capacities[i], 0));
  }
  
  // ===== CALCULATE MAX FLOW =====
  long flow = boost::push_relabel_max_flow(G, v_source, v_sink);

  // ===== OUTPUT =====
  if(flow >= out_capacity) {
    std::cout << "yes" << std::endl;
  } else {
    std::cout << "no" << std::endl;
  }
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

## 🧠 Learnings

<details> 

<summary> Expand to View </summary>

- A minimum capacity $c$ in a flow problem can be modeled by decreasing the starts capacity by $c$ and increasing the targets capacity by $c$ (from source)
- `boost::push_relabel_max_flow` behaves very weirdly when given negative capacities .

</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (20 pts / 5 s) : Correct answer      (1.358s)
   Test set 2 (20 pts / 5 s) : Correct answer      (0.475s)
   Test set 3 (30 pts / 5 s) : Correct answer      (1.539s)
   Test set 4 (30 pts / 5 s) : Correct answer      (1.358s)

Total score: 100
```
