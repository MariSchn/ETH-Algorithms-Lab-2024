# Coin Tossing Tournament

## 📝 Problem Description

Given the results of a tournament with $N$ players and $M$ rounds, each round involves two players competing, with one player winning and receiving one point, while the other receives zero. Some rounds have a known winner, while others have an unknown outcome.

The task is to determine whether it is possible to assign winners to the rounds with unknown outcomes so that the total points for each player match a specified final scoreboard.

## 💡 Hints

<details>

<summary>Hint #1</summary>

Try to model the problem as a graph. Each game connects exactly two players, and each player can participate in multiple games. If you draw a node for each player and a node for each game, what kind of graph structure do you get?

</details>

<details>

<summary>Hint #2</summary>

This problem is well-suited for a flow-based approach. Imagine a source node that "generates" points, which must flow through the games and then to the players, finally reaching a sink. How could you use flow to model the assignment of points from games to players, and ensure that each player receives their required score?

</details>

<details>

<summary>Hint #3</summary>

To construct the flow network:
 - Add a source node and a sink node.
 - For each game, add a node and connect the source to each game node with capacity 1 (each game gives one point).
 - For each game with a known outcome, connect the game node to the winner's player node with capacity 1. For unknown outcomes, connect the game node to both involved players with capacity 1 each.
 - For each player, connect their node to the sink with capacity equal to their required final score.
This setup ensures that the flow models the assignment of points and respects all constraints.

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

This problem can be solved by modeling it as a **maximum flow problem** in a **bipartite graph**. We can construct a flow network where the "flow" represents the points awarded in the tournament. The goal is to see if we can push a total flow equal to the number of games, $M$, from a source to a sink while respecting all constraints.

Pushing a flow of $M$ through the constructed network will mean that every point from each game has been successfully assigned to a player in such a way that all the constraints are satisfied. Specifically, it ensures that each game awards exactly one point, and each player receives exactly their target score. Therefore, if the maximum flow equals $M$, it is possible to realize the given scoreboard.

### Graph Construction

We build a directed graph with a source vertex `source`, a sink vertex `sink`, a set of nodes representing the games, and a set of nodes representing the players.

1.  **Nodes:**
    *   A single **source** node and a single **sink** node.
    *   $M$ nodes, one for each game played.
    *   $N$ nodes, one for each player.

2.  **Edges and Capacities:**
    *   **From Source to Games:** For each of the $M$ game nodes, we add an edge from the `source` to the game node. Each of these edges has a **capacity of 1**. This represents that each game generates exactly one point to be distributed.
    *   **From Games to Players:** The connections from game nodes to player nodes depend on the outcome of the round.
        *   If player $a$ **won** (outcome `c=1`): We add an edge from the corresponding game node to player $a$'s node with **capacity 1**. The point from this game *must* go to player $a$.
        *   If player $b$ **won** (outcome `c=2`): We add an edge from the game node to player $b$'s node with **capacity 1**. The point *must* go to player $b$.
        *   If the outcome is **unknown** (outcome `c=0`): We add two edges from the game node: one to player $a$'s node and one to player $b$'s node. Both edges have a **capacity of 1**. This models the choice: the single point (flow) from the game can be routed to *either* player $a$ or player $b$, but not both, because the incoming flow to the game node is only 1.
    *   **From Players to Sink:** For each player $i$, we add an edge from their node to the `sink` node. The capacity of this edge is set to their target score, $s_i$. This enforces the constraint that player $i$ cannot receive more points than their final score on the given scoreboard.

### Verification Logic

After constructing the network, we can determine if the scoreboard is achievable by checking two conditions:

1.  **Conservation of Points:** A basic sanity check is that the sum of all scores on the scoreboard, $\sum s_i$, must equal the total number of games, $M$. If `score_sum != M`, it's impossible to achieve the scoreboard because the total number of points awarded does not match the total points required.

2.  **Maximum Flow:** We calculate the maximum flow from the `source` to the `sink` in our constructed network.
    *   If the max flow is equal to $M$ (and also equal to `score_sum`), it means that a valid assignment of points exists. We were able to successfully route all $M$ points from the games to the players in a way that respects every player's target score.
    *   If the max flow is less than $M$, it means it's impossible to satisfy all constraints simultaneously. Even though the total points might sum up correctly, there is no way to distribute them from the specific games to the specific players to match the target scores. This indicates a bottleneck somewhere in the network, meaning the scoreboard is unachievable.

In summary, the problem is solvable if and only if `score_sum == M` and `max_flow == M`.

### Code
```cpp
#include<iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

using traits = boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>;
using graph = boost::adjacency_list<boost::vecS, 
                                    boost::vecS, 
                                    boost::directedS, 
                                    boost::no_property,
                                    boost::property<boost::edge_capacity_t, long,
                                                    boost::property<boost::edge_residual_capacity_t, long,
                                                                    boost::property<boost::edge_reverse_t, traits::edge_descriptor>>>>;
using vertex_desc = traits::vertex_descriptor;
using edge_desc = traits::edge_descriptor;


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
  int n, m; std:: cin >> n >> m;
  int score_sum = 0;
  
  // 0  to      n - 1 -> Players
  // n  to  n + m - 1 -> Games
  graph G(n + m); 
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  // Connect Source -> Games and Games -> Players
  for(int i = 0; i < m; ++i) {
    // Each game gives 1 point -> Connect source to game with capacity 1
    adder.add_edge(v_source, n + i, 1);
    
    // Connect Game to Players depending on outcome
    int a, b, c; std::cin >> a >> b >> c;
    
    if(c == 1) {         // Player a wins -> Connect to a
      adder.add_edge(n + i, a, 1);
    } else if (c == 2) { // Player b wins -> Connect to b
      adder.add_edge(n + i, b, 1);
    } else {             // Don't know -> Connect to both
      adder.add_edge(n + i, a, 1);
      adder.add_edge(n + i, b, 1);
    }
  }
  
  // Connect Players -> Sink
  for(int i = 0; i < n; ++i) {
    int s; std::cin >> s;
    score_sum += s;
    
    adder.add_edge(i, v_sink, s);
  }
  
  // ===== CALCULATE MAX FLOW =====
  int flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  // ===== OUTPUT =====
  if(score_sum == m && flow == score_sum) {
    std::cout << "yes" << std::endl;
  } else {
    std::cout << "no" << std::endl;
  }
}

int main() {
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
   Test set 1 (40 pts / 1 s) : Correct answer      (0.011s)
   Test set 2 (40 pts / 1 s) : Correct answer      (0.126s)
   Test set 3 (20 pts / 1 s) : Correct answer      (0.324s)

Total score: 100
```