# Coin Tossing Tournament

## 📝 Problem Description

You are given the results of a tournament involving $N$ players and $M$ rounds. In each round, two players compete, and one player wins, receiving one point, while the loser receives zero. For some rounds, the winner is known. For others, the outcome is unknown.

Your task is to determine if it's possible to assign winners to the unknown rounds such that the total points for each player match a given final scoreboard. The input consists of the number of players $N$, the number of rounds $M$, a list of all $M$ rounds with players $a$ and $b$, and an outcome code $c$. If $c=1$, player $a$ won; if $c=2$, player $b$ won; if $c=0$, the winner is unknown. Finally, you are given a target scoreboard with scores $s_0, s_1, \dots, s_{n-1}$ for each player. Your program should output "yes" if the scoreboard is achievable and "no" otherwise.

## 💡 Hints

<details>
<summary>Hint #1</summary>
A fundamental property of the tournament is that in every single round, exactly one point is awarded. This means that the total number of points distributed across all players must equal the total number of rounds played. Does the given scoreboard satisfy this condition? This is a necessary, but not sufficient, condition for a "yes" answer.
</details>

<details>
<summary>Hint #2</summary>
This problem can be viewed as an allocation problem. You have a set of resources (the points from each game) that need to be distributed to a set of recipients (the players). For games with a known outcome, the allocation is fixed. For games with an unknown outcome, you have a choice. How can you model this system of choices and constraints?
</details>

<details>
<summary>Hint #3</summary>
Consider modeling this problem as a flow network. A flow network is excellent for representing problems where a commodity flows from a source to a sink through a network with capacity constraints. What could represent the source of points, the players, the games, and the final destination of the points (the sink)? How would you set the capacities on the connections to model the rules of the tournament and the target scores?
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
This problem can be elegantly solved by modeling it as a maximum flow problem. We can construct a flow network where the "flow" represents the points awarded in the tournament. The goal is to see if we can push a total flow equal to the number of games, $M$, from a source to a sink while respecting all constraints.

### Network Construction

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
    *   If the max flow is equal to $M$ (and also equal to `score_sum`), it means that a valid assignment of points exists. We were able to successfully route all $M$ points from the games to the players in a way that respects every player's target score. The answer is "yes".
    *   If the max flow is less than $M$, it means it's impossible to satisfy all constraints simultaneously. Even though the total points might sum up correctly, there is no way to distribute them from the specific games to the specific players to match the target scores. This indicates a bottleneck somewhere in the network, meaning the scoreboard is unachievable. The answer is "no".

In summary, the problem is solvable if and only if `score_sum == M` and `max_flow == M`.

### C++ Implementation

The following C++ code uses the Boost Graph Library to implement this max-flow solution.

```cpp
#include <iostream>
#include <vector>
#include <numeric>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

// Define graph types for Boost Graph Library
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

// Helper class to add edges and their reverse edges for the BGL max_flow algorithm
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
    c_map[rev_e] = 0; // Reverse edge has zero capacity
    r_map[e] = rev_e;
    r_map[rev_e] = e;
  }
};

void solve() {
  int n, m;
  std::cin >> n >> m;
  
  // Create graph with nodes for players, games, source, and sink.
  // Player nodes: 0 to n-1
  // Game nodes:   n to n+m-1
  graph G(n + m); 
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  // Add edges from source to games and from games to players
  for (int i = 0; i < m; ++i) {
    int a, b, c;
    std::cin >> a >> b >> c;
    
    // Each game provides 1 point: source -> game with capacity 1
    int game_node = n + i;
    adder.add_edge(v_source, game_node, 1);
    
    // Connect game to players based on outcome
    if (c == 1) {       // Player 'a' won
      adder.add_edge(game_node, a, 1);
    } else if (c == 2) { // Player 'b' won
      adder.add_edge(game_node, b, 1);
    } else {             // Outcome unknown, connect to both
      adder.add_edge(game_node, a, 1);
      adder.add_edge(game_node, b, 1);
    }
  }
  
  // Add edges from players to sink with capacity equal to their target score
  long score_sum = 0;
  for (int i = 0; i < n; ++i) {
    int s;
    std::cin >> s;
    score_sum += s;
    
    adder.add_edge(i, v_sink, s);
  }
  
  // Calculate max flow from source to sink
  long flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  // Check if the scoreboard is possible
  if (score_sum == m && flow == m) {
    std::cout << "yes\n";
  } else {
    std::cout << "no\n";
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  int t;
  std::cin >> t;
  while (t--) {
    solve();
  }
  return 0;
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