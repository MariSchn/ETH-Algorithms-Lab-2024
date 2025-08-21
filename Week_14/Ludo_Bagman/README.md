# Ludo Bagman

## 📝 Problem Description

The problem asks us to design a schedule of Quidditch matches with minimum total risk. We are given a set of teams from the East, with size $e$, and a set of teams from the West, with size $w$. All matches must be between one team from the East and one from the West.

We are provided with a list of potential matches, each having an associated risk value. These matches are categorized as either non-difficult or difficult. Our goal is to select exactly $p$ matches from this list to form the final schedule.

A schedule is considered "fair" only if every team (from both East and West) is scheduled to play in at least $l$ **non-difficult** matches. Difficult matches do not count towards this requirement for any team.

The objective is to find a fair schedule of exactly $p$ matches such that the sum of the risk values of all chosen matches is minimized. If no such schedule exists, we should indicate that.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem requires selecting a specific number of items (matches) from a larger pool to minimize a total cost, subject to certain constraints on the participants (teams). This structure, involving two distinct sets of participants (East and West teams) and connections between them, strongly suggests modeling the problem using a network. Consider how teams and potential matches could be represented in such a model.
</details>
<details>
<summary>Hint #2</summary>
This problem can be effectively modeled as a **minimum-cost maximum-flow** problem. You can represent teams as vertices and potential matches as edges in a flow network. The risk of a match corresponds to the cost of an edge. Think about how to set up the source, the sink, and the capacities to ensure you select the correct number of matches.
</details>
<details>
<summary>Hint #3</summary>
A major challenge is enforcing the constraint that each team must play *at least* $l$ non-difficult matches. This is a "lower-bound" constraint. Standard max-flow models handle upper bounds (capacities) easily, but not lower bounds. A common technique to model lower bounds is to introduce additional "helper" vertices. Consider creating a main source and sink, plus a "pseudo-source" and a "pseudo-sink," to create two distinct "types" of flow: one to satisfy the mandatory $l$ matches and another for any additional matches.
</details>
<details>
<summary>Hint #4</summary>
The final challenge is incorporating difficult matches. They contribute to the total of $p$ matches and add to the total risk, but they do *not* help satisfy the $l$ non-difficult match requirement for any team. In the flow model from the previous hint, where does the flow for "optional" matches (those beyond the mandatory $l$) originate and terminate? A difficult match can be seen as an "optional" choice. You can add edges for difficult matches in a way that they consume this "optional" flow without passing through the primary team-constraint part of the network.
</details>

## ✨ Solutions

<details>
<summary>Solution for Non-Difficult Matches (Test Sets 1-3)</summary>
This problem can be modeled as a **minimum-cost maximum-flow** problem. The structure of having two distinct sets of teams (East and West) that play against each other naturally suggests a bipartite graph structure within our flow network.

### Graph Construction

Let's define the components of our flow network. We will have vertices representing the teams, along with a source and a sink to direct the flow. The key challenge is to model the constraints:
1.  Exactly $p$ matches must be scheduled.
2.  Each of the $e+w$ teams must play at least $l$ matches.

To handle these constraints, especially the lower-bound requirement of $l$ matches, we introduce two helper vertices: a **pseudo-source** and a **pseudo-sink**.

The vertices in our graph are:
- A main source `v_source` and a main sink `v_target`.
- A `pseudo_source` and a `pseudo_target`.
- $e$ vertices for the East teams (indexed $0$ to $e-1$).
- $w$ vertices for the West teams (indexed $e$ to $e+w-1$).

The edges are constructed to model the flow of "matches":
1.  **Match Edges:** For each non-difficult match between East team $u$ and West team $v$ with risk $r$, we add a directed edge from vertex $u$ to vertex $e+v$. This edge has a **capacity of 1** (since a match can be scheduled at most once) and a **cost of $r$**.

2.  **Team Constraint Edges (East):** To enforce that each East team plays at least $l$ matches, we connect the sources to the East team vertices.
    - For each East team $i$, add an edge `v_source` $\rightarrow$ $i$ with **capacity $l$** and cost 0. This flow path represents the mandatory matches for team $i$.
    - To allow teams to play more than $l$ matches, add an edge `pseudo_source` $\rightarrow$ $i$ with a large capacity (e.g., $p$ or `MAX_L`) and cost 0.

3.  **Team Constraint Edges (West):** We build a symmetric structure on the sink side for West teams.
    - For each West team $j$, add an edge $e+j$ $\rightarrow$ `v_target` with **capacity $l$** and cost 0.
    - Add an edge $e+j$ $\rightarrow$ `pseudo_target` with a large capacity and cost 0.

4.  **Total Matches Constraint Edges:** To ensure exactly $p$ matches are scheduled in total, we limit the total flow to $p$.
    - The total flow capacity leaving `v_source` must be $p$. Since we have already used $l \cdot e$ capacity for the direct edges to East teams, we add an edge `v_source` $\rightarrow$ `pseudo_source` with **capacity $p - l \cdot e$** and cost 0.
    - Symmetrically, we add an edge `pseudo_target` $\rightarrow$ `v_target` with **capacity $p - l \cdot w$** and cost 0.

### Solving and Interpretation

With this graph, we can find the minimum-cost flow. A flow of 1 unit from `v_source` to `v_target` represents one scheduled match.
- First, we calculate the maximum possible flow in this network.
- If `max_flow` is exactly equal to $p$, it means a schedule of $p$ matches is possible. The structure of the graph ensures that if a flow of $p$ is achieved, the lower-bound constraints of $l$ matches per team must have been satisfied.
- The `successive_shortest_path_nonnegative_weights` algorithm will find the way to achieve this flow with the minimum possible cost, which is our minimum total risk.
- If `max_flow` is less than $p$, it is impossible to schedule $p$ matches, so no fair schedule exists.

```cpp
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

// Graph Type with nested interior edge properties for Cost Flow Algorithms
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor,
                boost::property <boost::edge_weight_t, long> > > > > graph;

typedef boost::graph_traits<graph>::vertex_descriptor           vertex_desc;
typedef boost::graph_traits<graph>::edge_descriptor             edge_desc;

// A large enough capacity for "unlimited" flow on certain edges.
const int MAX_CAPACITY = 250; 

// Custom edge adder class to simplify adding flow edges.
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
    w_map[rev_e] = -cost; // reverse edge has opposite cost
  }
};

void solve() {
  int e, w, m, d, p, l;
  std::cin >> e >> w >> m >> d >> p >> l;

  // Total vertices: e teams + w teams + 4 special vertices
  graph G(e + w + 4);
  edge_adder adder(G);
  
  // Define special vertices
  const vertex_desc v_source = e + w;
  const vertex_desc pseudo_source = e + w + 1;
  const vertex_desc v_target = e + w + 2;
  const vertex_desc pseudo_target = e + w + 3;
  
  // Connect sources to East teams
  adder.add_edge(v_source, pseudo_source, p - (l * e), 0);
  for(int i = 0; i < e; ++i) {
    adder.add_edge(v_source, i, l, 0);
    adder.add_edge(pseudo_source, i, MAX_CAPACITY, 0);
  }
  
  // Connect West teams to sinks
  adder.add_edge(pseudo_target, v_target, p - (l * w), 0);
  for(int i = 0; i < w; ++i) {
    adder.add_edge(e + i, v_target, l, 0);
    adder.add_edge(e + i, pseudo_target, MAX_CAPACITY, 0);
  }
  
  // Add edges for non-difficult matches
  for(int i = 0; i < m; ++i) {
    int u, v, r; std::cin >> u >> v >> r;
    adder.add_edge(u, e + v, 1, r);
  }
  
  // Difficult matches are ignored in this partial solution
  for(int i = 0; i < d; ++i) {
    int u, v, r; std::cin >> u >> v >> r;
  }
  
  // Find the min-cost max-flow
  long flow = boost::push_relabel_max_flow(G, v_source, v_target);
  boost::successive_shortest_path_nonnegative_weights(G, v_source, v_target);
  long cost = boost::find_flow_cost(G);
  
  // A schedule is valid if and only if we can schedule exactly p matches.
  if(flow == p) {
    std::cout << cost << std::endl;
  } else {
    std::cout << "No schedule!" << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}
```
</details>

<details>
<summary>Final Solution (All Test Sets)</summary>
To build the final solution, we extend the min-cost max-flow model from the previous approach to correctly handle **difficult matches**. The core graph structure for non-difficult matches remains the same.

### The Key Insight

The crucial constraint is that difficult matches contribute to the total of $p$ scheduled matches but **do not** count towards the minimum of $l$ non-difficult matches required for each team.

Our flow network already distinguishes between two "types" of flow for each team:
1.  **Mandatory Flow:** Flow that originates directly from `v_source` and goes to a team vertex. This flow is used to satisfy the `l` match requirement.
2.  **Optional Flow:** Flow that is routed through `pseudo_source` and `pseudo_target`. This flow represents matches scheduled *after* the `l` match requirement has been met for all teams.

A difficult match is, by definition, an "optional" match in the context of the fairness rule. It can be selected only as part of the flexible portion of the schedule. Therefore, we can model a difficult match as a way to consume one unit of this optional flow.

### The Final Graph Construction

The most elegant way to model this is to add a direct edge from `pseudo_source` to `pseudo_target` for each difficult match.

- For each difficult match with risk $r$, we add a directed edge `pseudo_source` $\rightarrow$ `pseudo_target` with **capacity 1** and **cost $r$**.

This construction has several advantages:
- It correctly adds the difficult match's cost to the total.
- It consumes one unit of capacity from the total of $p$ matches.
- Critically, this flow path **bypasses all team vertices**. This ensures that selecting a difficult match does not contribute to the flow being counted for any team's `l` match minimum.

The min-cost flow algorithm will now choose the cheapest combination of non-difficult and difficult matches to achieve the total flow of $p$, while respecting all constraints. It will seamlessly pick a difficult match if its risk is lower than any available non-difficult match for the "optional" slots in the schedule.

The final check remains the same: a valid schedule exists if and only if the maximum flow through the network is exactly $p$.

```cpp
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

// Graph Type with nested interior edge properties for Cost Flow Algorithms
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor,
                boost::property <boost::edge_weight_t, long> > > > > graph;

typedef boost::graph_traits<graph>::vertex_descriptor           vertex_desc;
typedef boost::graph_traits<graph>::edge_descriptor             edge_desc;

// A large enough capacity for "unlimited" flow on certain edges.
const int MAX_CAPACITY = 250;

// Custom edge adder class to simplify adding flow edges.
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
    w_map[rev_e] = -cost; // reverse edge has opposite cost
  }
};

void solve() {
  int e, w, m, d, p, l;
  std::cin >> e >> w >> m >> d >> p >> l;

  // Total vertices: e teams + w teams + 4 special vertices
  graph G(e + w + 4);
  edge_adder adder(G);
  
  // Define special vertices
  const vertex_desc v_source = e + w;
  const vertex_desc pseudo_source = e + w + 1;
  const vertex_desc v_target = e + w + 2;
  const vertex_desc pseudo_target = e + w + 3;
  
  // Connect sources to East teams
  adder.add_edge(v_source, pseudo_source, p - (l * e), 0);
  for(int i = 0; i < e; ++i) {
    adder.add_edge(v_source, i, l, 0);
    adder.add_edge(pseudo_source, i, MAX_CAPACITY, 0);
  }
  
  // Connect West teams to sinks
  adder.add_edge(pseudo_target, v_target, p - (l * w), 0);
  for(int i = 0; i < w; ++i) {
    adder.add_edge(e + i, v_target, l, 0);
    adder.add_edge(e + i, pseudo_target, MAX_CAPACITY, 0);
  }
  
  // Add edges for non-difficult matches
  for(int i = 0; i < m; ++i) {
    int u, v, r; std::cin >> u >> v >> r;
    adder.add_edge(u, e + v, 1, r);
  }
  
  // Add edges for difficult matches
  for(int i = 0; i < d; ++i) {
    int u, v, r; std::cin >> u >> v >> r;
    // This edge models a difficult match as an optional choice
    adder.add_edge(pseudo_source, pseudo_target, 1 , r);
  }
  
  // Find the min-cost max-flow
  long flow = boost::push_relabel_max_flow(G, v_source, v_target);
  boost::successive_shortest_path_nonnegative_weights(G, v_source, v_target);
  long cost = boost::find_flow_cost(G);
  
  // A schedule is valid if and only if we can schedule exactly p matches.
  if(flow == p) {
    std::cout << cost << std::endl;
  } else {
    std::cout << "No schedule!" << std::endl;
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

```