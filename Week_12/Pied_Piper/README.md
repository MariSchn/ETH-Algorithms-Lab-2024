# The Pied Piper of Hamelin

## 📝 Problem Description
The task is to determine the optimal route through the city of Hamelin to maximize the number of rats collected. The city consists of $N$ public squares, numbered from $0$ to $N-1$, connected by $M$ one-way streets, each with a specified number of rats.

An acceptable plan must begin and end at square $0$, pass through the main public square $N-1$, and visit each square at most once (except for square $0$, which serves as both the start and end). The route is formed by concatenating two path segments:
- A monotonically increasing path from square $0$ to square $N-1$, where each street traversed leads from a lower-numbered square to a higher-numbered one.
- A monotonically decreasing path from square $N-1$ back to square $0$, where each street traversed leads from a higher-numbered square to a lower-numbered one.

Given the configuration of squares and streets, compute the maximum total number of rats that can be collected on any single acceptable plan.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem asks for a single tour that starts at 0, goes to $N-1$, and returns to 0. Try to visualize this tour. It can be deconstructed into two distinct paths: one from square $0$ to $N-1$ and another from square $N-1$ to $0$. The key constraint is that these two paths must not share any intermediate squares.

</details>

<details>

<summary>Hint #2</summary>

This problem structure, involving finding optimal paths with specific constraints, is a strong indicator for dynamic programming. Consider building the two required paths simultaneously. What information would you need to keep track of in your DP state? To extend the paths, you certainly need to know their current endpoints.

</details>

<details>

<summary>Hint #3</summary>
A common pitfall is to calculate the best increasing path from $0$ to $N-1$ and the best decreasing path from $N-1$ to $0$ independently and then add their scores. This approach fails because the two paths might share intermediate squares, which is forbidden. Your DP state must enforce the disjointness constraint.

</details>

<details>

<summary>Hint #4</summary>

Let's define a DP state `dp[i][j]` representing the maximum score for two disjoint, monotonically increasing paths starting from square $0$ and ending at squares `i` and `j`. Why two *increasing* paths? A decreasing path from $N-1$ to $0$ is structurally similar to an increasing path from $0$ to $N-1$. Thinking about the problem as finding two disjoint increasing paths that meet at $N-1$ can simplify the logic significantly. The final answer would then be stored in `dp[n-1][n-1]`.

</details>


<details> 

<summary>Hint #5</summary>

To transition the DP state `dp[i][j]`, consider moving either endpoint forward along a valid street. For each possible next square $i'$ reachable from $i$ (with $i' > i$ and $i' \neq j$), update `dp[i'][j]` with the value of `dp[i][j]` plus the rats on the street $(i, i')$. Similarly, for each possible next square $j'$ reachable from $j$ (with $j' > j$ and $j' \neq i$), update `dp[i][j']`. This ensures the two paths remain disjoint and only move forward. Iterate over all possible transitions to propagate the maximum scores.

</details>
## ✨ Solutions

<details>

<summary>First Solution (Test Set 1)</summary>

For the first test set, the city of Hamelin possesses a special layered structure: the squares are partitioned into layers $S_0, S_1, \ldots, S_{k-1}$, with each layer containing at most $10$ squares. Streets only connect squares in consecutive layers, and every monotonically increasing path from $0$ to $N-1$ has a corresponding monotonically decreasing path from $N-1$ to $0$ that is disjoint (except for the endpoints) and collects the same number of rats.

### Approach
This structure allows us to simplify the problem significantly. Since the layers are small, we can enumerate all possible paths from $0$ to $N-1$ that respect the layering. For each such path, we know there exists a matching decreasing path that is disjoint and collects the same number of rats. Therefore, we only need to find the single best increasing path from $0$ to $N-1$; the corresponding decreasing path will automatically exist and yield the same score.

### Algorithm
Enumerate Paths: Since each layer contains at most $10$ squares, the total number of possible paths is manageable. We can use dynamic programming or even simple recursion to explore all increasing paths from $0$ to $N-1$.
Track Rat Count: For each path, sum the number of rats on the streets traversed.
Select Maximum: The answer is twice the maximum rat count found for any increasing path, as the matching decreasing path will contribute the same amount.

### Why This Works
The key insight is that the layered structure and the guarantee of a matching decreasing path eliminate the need to explicitly construct both paths or check for disjointness. The problem reduces to a straightforward path-finding task in a layered directed acyclic graph.

### Complexity
Because each layer is small, the solution runs efficiently even with exhaustive search or dynamic programming. The constraints ensure that the approach is feasible for this test set.

### Code
```cpp
#include <iostream>
#include <vector>
#include <limits>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor,
                boost::property <boost::edge_weight_t, long> > > > > flow_graph; // new! weightmap corresponds to costs
                
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
  boost::no_property, boost::property<boost::edge_weight_t, int> >      dijkstra_graph;


typedef boost::graph_traits<flow_graph>::edge_descriptor             edge_desc;
typedef boost::graph_traits<flow_graph>::vertex_descriptor           vertex_desc;
typedef boost::graph_traits<flow_graph>::out_edge_iterator           out_edge_it;

// Custom edge adder class, highly recommended
class edge_adder {
 flow_graph &G;

 public:
  explicit edge_adder(flow_graph &G) : G(G) {}
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

const int MAX_F = std::pow(10, 5);

void solve() {
  // ===== READ INPUT =====
  int n, m; std::cin >> n >> m;
  
  flow_graph flow_G(2 * n);
  dijkstra_graph dijkstra_G(2 * n);
  edge_adder adder(flow_G);
  
  for(int i = 0; i < m; ++i) {
    int u, v, f; std::cin >> u >> v >> f;
    
    if(u < v) { 
      adder.add_edge(u, v, 1, MAX_F - f);
      boost::add_edge(u, v, 1, dijkstra_G);
    } else { 
      adder.add_edge(n + u, n + v, 1, MAX_F - f); 
      boost::add_edge(n + u, n + v, 1, dijkstra_G);
    }
  }
  
  // ===== SOLVE =====
  const vertex_desc v_source = boost::add_vertex(flow_G);
  const vertex_desc v_target = boost::add_vertex(flow_G);

  boost::add_edge(n - 1, n - 1 + n, 0, dijkstra_G);
  adder.add_edge(n - 1, n - 1 + n, 1, 0);
  adder.add_edge(v_source, 0, 1, 0);
  adder.add_edge(n, v_target, 1, 0);
  
  boost::successive_shortest_path_nonnegative_weights(flow_G, v_source, v_target);
  int cost = boost::find_flow_cost(flow_G);
  
  std::vector<int> dist_map(2 * n);
  boost::dijkstra_shortest_paths(dijkstra_G, 0, boost::distance_map(boost::make_iterator_property_map(dist_map.begin(), boost::get(boost::vertex_index, dijkstra_G))));
  int num_nodes = dist_map[n];
  
  // ===== OUTPUT =====
  std::cout << MAX_F * num_nodes - cost << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;;
  while(n_tests--) { solve(); }
}
```

</details>

<details>
<summary>Final Solution</summary>

This problem can be elegantly solved using dynamic programming by modeling the required tour as two separate paths being built simultaneously.

### Core Idea: Two Disjoint Paths

The problem requires finding a tour $0 \leadsto N-1 \leadsto 0$, composed of a monotonically increasing path segment and a monotonically decreasing one. The intermediate squares of these two segments must be disjoint.

This problem can be cleverly reframed: we need to find **two vertex-disjoint, monotonically increasing paths** from square $0$ to square $N-1$. One of these paths in our model corresponds to the original increasing path, while the other corresponds to the original decreasing path. The sum of rats on these two model paths gives the total for the tour.

Why is this rephrasing valid? A decreasing path from $N-1$ to $0$ uses edges $(u, v)$ where $u > v$. An increasing path from $0$ to $N-1$ uses edges $(u, v)$ where $u < v$. While their edge properties differ, the core challenge is ensuring the set of vertices visited by each path (excluding $0$ and $N-1$) are disjoint. Modeling this as two agents moving from $0$ to $N-1$ along disjoint, monotonically increasing paths correctly captures this fundamental constraint.

### Dynamic Programming Formulation

We can define a DP state to solve this reformulated problem.

-   **State:** Let `dp[i][j]` be the maximum total number of rats collected by two vertex-disjoint, monotonically increasing paths starting from square $0$, with one path ending at square `i` and the other at `j`.

-   **Base Case:** The process starts with two paths at square $0$. The initial score is `dp[0][0] = 0`. However, in practice, we initialize by considering the first move out of square $0$. For every street $0 \to v$ with $f$ rats, we can initialize a path. For instance, `dp[v][0]` could store the value $f$.

-   **Transitions:** We build the paths by iteratively extending them. A common way to organize the computation is to process pairs of endpoints `(i, j)` in increasing order of their sum `i + j`. For a given state `dp[i][j]`, we can extend either the path ending at `i` or the path ending at `j`.

    -   To extend the path at `i`, we consider taking a street from `i` to a new square `k`. The new state would be `(k, j)`.
    -   To extend the path at `j`, we consider taking a street from `j` to a new square `k`. The new state would be `(i, k)`.

-   **Ensuring Disjointness:** The key is to guarantee that the new square `k` has not been visited by the other path. A clever trick is to enforce that `k` must have a larger index than both current endpoints (`k > i` and `k > j`). Since paths are monotonic, all squares on the path to `i` have indices less than or equal to `i`, and similarly for `j`. This condition elegantly ensures that `k` is a new, unvisited square for both paths.

-   **Reaching the Destination:** The disjointness rule is relaxed for the final square, $N-1$. A path is allowed to move to $N-1$ even if its index is not strictly greater than the other path's endpoint. This allows one path to reach $N-1$ first (e.g., state `dp[N-1][j]`), and then the second path can also terminate at $N-1$.

-   **Final Answer:** The state `dp[n-1][n-1]` represents the maximum score when both paths have successfully converged at square $N-1$, which is the solution to our problem.

The provided C++ code implements this "push-style" DP, where from a computed state `dp[i][j]`, it updates all reachable future states.

```cpp
#include <iostream>
#include <vector>

void solve() {
  // ===== READ INPUT =====
  int n, m; std::cin >> n >> m;

  std::vector<std::vector<std::pair<int, int>>> incoming(n);
  std::vector<std::vector<std::pair<int, int>>> outgoing(n);

  for (int i = 0; i < m; ++i) {
    int u, v, f; std::cin >> u >> v >> f;

    outgoing[u].push_back(std::make_pair(v, f));
    incoming[v].push_back(std::make_pair(u, f));
  }

  // ===== SOLVE =====
  std::vector<std::vector<long>> dp(n, std::vector<long>(n, -1));

  // Fill the values for the initial out/in edges of 0.
  for (std::pair<int, int> &el : outgoing[0]) {
    dp[el.first][0] = el.second;
  }
  for (std::pair<int, int> &el : incoming[0]) {
    dp[0][el.first] = el.second;
  }

  // Iterate over the DP diagonally, i.e.
  // k = 1 -> (0, 1), (1, 0)
  // k = 2 -> (0, 2), (1, 1), (2, 0)
  // etc.
  for (int k = 1; k < n * 2; k++) {
    for (int i = 0; i <= k; i++) {
      int j = k - i;
      // Make sure that we are actually still within the bounds of the DP
      bool in_matrix = i < n && j < n;
      // The value in the neighbor must not be -1, otherwise there is no way
      // at all to go from 0 to i and from j back to 0 (impossible path), subsequently any
      // path that continues from i, j would also be impossible.
      bool valid_solution = dp[i][j] != -1;
      
      if (in_matrix && valid_solution) {
        for (auto &el : outgoing[i]) {
          // Node n - 1 is an edge case, and it must not be strictly more positive.
          if ((el.first > j && el.first > i) || el.first == n - 1) {
            dp[el.first][j] = std::max(dp[el.first][j], dp[i][j] + el.second);
          }
        }

        for (auto &el : incoming[j]) {
          // Node n - 1 is an edge case, and it must not be strictly more positive.
          if ((el.first > j && el.first > i) || el.first == n - 1) {
            dp[i][el.first] = std::max(dp[i][el.first], dp[i][j] + el.second);
          }
        }
      }
    }
  }

  // ===== OUTPUT =====
  std::cout << dp[n - 1][n - 1] << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;;
  while(n_tests--) { solve(); }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1        (25 pts / 4 s) : Correct answer      (0.049s)
   Test set 2        (25 pts / 4 s) : Correct answer      (0.049s)
   Test set 3        (15 pts / 4 s) : Correct answer      (1.445s)
   Test set 4        (15 pts / 4 s) : Correct answer      (1.941s)
   Hidden test set 1 (05 pts / 4 s) : Correct answer      (0.046s)
   Hidden test set 2 (05 pts / 4 s) : Correct answer      (0.051s)
   Hidden test set 3 (05 pts / 4 s) : Correct answer      (1.437s)
   Hidden test set 4 (05 pts / 4 s) : Correct answer      (1.925s)

Total score: 100
```