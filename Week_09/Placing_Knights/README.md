# Placing Knights

## 📝 Problem Description

Given a square chessboard of size $N \times N$, some of its squares may be unusable "holes". The task is to determine the maximum number of knights that can be placed on the available squares of the board such that no two knights threaten each other.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The core of the problem lies in the relationships between squares. Placing a knight on one square makes a specific set of other squares unavailable. How can you model this system of constraints and relationships? Think about representing the board squares and the "threatens" relationship in a more abstract way.

</details>

<details>

<summary>Hint #2</summary>

Try coloring the board's squares alternatingly, like a standard chessboard. Observe a knight's move: from a square of one color (e.g., where `row + column` is even), to which color square does it always land? This property reveals a fundamental structure of the problem.

</details>

<details>

<summary>Hint #3</summary>

The problem is equivalent to finding a maximum set of squares where no two are connected by a knight's move. This is precisely the definition of a **Maximum Independent Set (MIS)** in a graph where squares are vertices and knight moves are edges. For the special type of graph revealed in Hint #2 (a bipartite graph), a famous result, **Konig's Theorem**, connects the MIS to the **Maximum Matching**. A maximum matching can be computed efficiently using a max-flow algorithm.

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>


This problem is best solved by modeling the chessboard as a graph, where each available square is a node and edges connect squares that a knight can reach in one move. Since the board may have holes, this approach naturally handles unavailable squares.

The goal is to find the largest set of squares (nodes) such that no two are connected by a knight's move. This is the **Maximum Independent Set**. Placing knights on these squares ensures no two threaten each other.

By coloring the board like a chessboard (alternating black and white), we see that a knight always moves from a square of one color to the other. This means our graph is **bipartite**: all edges go between black and white squares, never within the same color.

For bipartite graphs, the Maximum Independent Set can be found using a Max Flow/Min Cut algorithm. We build a flow network:
- Add a source node and connect it to all "white" squares.
- Connect all "black" squares to the sink node.
- For every valid knight move, add an edge between them.

The value of the maximum flow gives the size of the maximum matching. The answer is simply the total number of available squares minus the maximum matching.

### Code
```cpp
#include<iostream>
#include<vector>
#include<queue>

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
typedef boost::graph_traits<graph>::out_edge_iterator      out_edge_it;

// Create a vector containing all possible moves for a knight
std::vector<std::pair<int, int>> OFFSETS = {
    {-1, -2}, {-1, 2}, {1, -2}, {1, 2},
    {-2, -1}, {-2, 1}, {2, -1}, {2, 1}
};

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

int get_index(int row, int col, int n) {
  return row * n + col;
}

void solve() {
  // ===== READ INPUT =====
  int n; std::cin >> n;
  
  std::vector<std::vector<int>> board(n, std::vector<int>(n));
  for(int row = 0; row < n; ++row) {
    for(int col = 0; col < n; ++col) {
      std::cin >> board[row][col];
    }
  }
  
  // ===== BUILD GRAPH =====
  // Add all nodes, even if there are holes to make indexing easier
  // We can just not connect the "hole nodes" to any other nodes
  graph G(n * n); 
  edge_adder adder(G);
  auto rc_map = boost::get(boost::edge_residual_capacity, G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  for(int row = 0; row < n; ++row) {
    for(int col = 0; col < n; ++col) {
      if(board[row][col] == 0) continue;
      int idx = get_index(row, col, n);

      // Connect to source or sink
      if((row + col) % 2 == 0) {
        adder.add_edge(v_source, idx, 1);
      }
      else { 
        adder.add_edge(idx, v_sink, 1);
        continue;
      }
      
      // Connect to nodes reachable by knight
      for(const std::pair<int, int> &offset : OFFSETS) {
        int adjusted_row = row + offset.first;
        int adjusted_col = col + offset.second;
        
        if((adjusted_row < 0 || adjusted_row >= n) ||  // Row out of bounds
           (adjusted_col < 0 || adjusted_col >= n) ||  // Col out of bounds
           (board[adjusted_row][adjusted_col] == 0)    // Space is a hole
           ){
          continue;
        }
        adder.add_edge(idx, get_index(adjusted_row, adjusted_col, n), 1);
      }
    }
  }
  
  // ===== COMPUTE MAX FLOW AND SET S =====
  boost::push_relabel_max_flow(G, v_source, v_sink);
  
  std::vector<int> vis(n * n + 2, false); // visited flags
  std::queue<int> Q; // BFS queue (from std:: not boost::)
  vis[v_source] = true; // Mark the source as visited
  Q.push(v_source);
  while (!Q.empty()) {
    const int u = Q.front();
    Q.pop();
    out_edge_it ebeg, eend;
    for (boost::tie(ebeg, eend) = boost::out_edges(u, G); ebeg != eend; ++ebeg) {
      const int v = boost::target(*ebeg, G);
      // Only follow edges with spare capacity
      if (rc_map[*ebeg] == 0 || vis[v]) continue;
      vis[v] = true;
      Q.push(v);
    }
  }
  
  // ===== COUNT SIZE OF INDEPENDENT SET =====
  int count = 0;
  for(int row = 0; row < n; ++row) {
    for(int col = 0; col < n; col++) {
      if(board[row][col] == 0) continue;
      int idx = get_index(row, col, n);
      
      if((row + col) % 2 == 0) count += vis[idx];
      else count += 1 - vis[idx];
    }
  }
  
  // ===== OUTPUT =====
  std::cout << count << std::endl;
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
   Test set 1 (30 pts / 0.500 s) : Correct answer      (0.0s)
   Test set 2 (35 pts / 0.500 s) : Correct answer      (0.048s)
   Test set 3 (35 pts / 0.500 s) : Correct answer      (0.155s)

Total score: 100
```