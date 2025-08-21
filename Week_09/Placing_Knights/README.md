# Placing Knights

## 📝 Problem Description

Given a square chessboard of size $N \times N$, some of its squares may be unusable "holes". The task is to determine the maximum number of knights that can be placed on the available squares of the board such that no two knights threaten each other.

The input specifies the board's dimensions and the location of the available squares versus the holes. A knight at position $(r, c)$ threatens all valid positions reachable by a standard knight's move. The output should be a single integer representing the maximum number of knights that can be safely placed.

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
This problem can be effectively modeled using a graph. Let's represent each available square on the chessboard as a vertex in a graph. An edge will connect two vertices if a knight can move between the corresponding squares. With this representation, the problem asks for the maximum number of vertices we can select such that no two selected vertices are connected by an edge. This is the classic definition of the **Maximum Independent Set (MIS)** problem.

### The Bipartite Insight

While finding the MIS is NP-hard for general graphs, our specific graph has a special property. If we color the chessboard squares alternatingly (like a standard black and white board), a knight always moves from a square of one color to a square of the opposite color. For example, if we color a square `(r, c)` "white" if `r + c` is even and "black" if `r + c` is odd, a knight on a white square can only attack black squares, and vice versa.

This means our graph is **bipartite**. We can partition the vertices into two sets, $U$ (white squares) and $V$ (black squares), such that all edges connect a vertex in $U$ to a vertex in $V$. There are no edges within $U$ or within $V$.

### From Independent Set to Max-Flow

For bipartite graphs, there is a powerful connection between the Maximum Independent Set and the Maximum Matching, given by **Konig's Theorem**. The theorem states that the size of a minimum vertex cover equals the size of a maximum matching. The size of the maximum independent set, $\alpha(G)$, is related to the number of vertices $|V|$ and the size of the minimum vertex cover, $\beta(G)$, by the identity $\alpha(G) + \beta(G) = |V|$.

Combining these, we get:
$\alpha(G) = |V| - \beta(G) = |V| - (\text{size of max matching})$

The size of the maximum matching in a bipartite graph can be found by converting the problem into a maximum flow problem. We construct a flow network as follows:
1.  Create a source vertex `s` and a sink vertex `t`.
2.  For every vertex $u$ in the "white" partition $U$, add a directed edge from `s` to $u$ with capacity 1.
3.  For every vertex $v$ in the "black" partition $V$, add a directed edge from $v$ to `t` with capacity 1.
4.  For every edge between a white square $u$ and a black square $v$ in the original graph, add a directed edge from $u$ to $v$ with capacity 1 (or infinity).

The value of the maximum flow from `s` to `t` in this network is equal to the size of the maximum matching in the bipartite graph. The final answer would then be the total number of available squares minus the max-flow value.

However, the provided C++ solution uses an alternative, equivalent method that directly computes the MIS from the min-cut of the flow network. After computing the max-flow, we find all vertices reachable from the source `s` in the residual graph. Let this set of reachable vertices be $S_{res}$. The MIS is then the set of "white" vertices in $S_{res}$ plus the set of "black" vertices *not* in $S_{res}$.

### C++ Implementation

The solution uses the Boost Graph Library to implement the max-flow algorithm.

1.  **Graph Representation**: The board is flattened into a 1D array of vertices. An `n x n` board has `n*n` vertices, indexed from `0` to `n*n - 1`. A source and a sink vertex are added.
2.  **Flow Network Construction**:
    *   The code iterates through each square `(r, c)`.
    *   If `r + c` is even (a "white" square), an edge `source -> vertex` is added with capacity 1. Then, for each valid knight move to a "black" square, an edge `white_vertex -> black_vertex` is added with capacity 1.
    *   If `r + c` is odd (a "black" square), an edge `vertex -> sink` is added with capacity 1.
3.  **Max-Flow Calculation**: `boost::push_relabel_max_flow` is called to compute the maximum flow from source to sink.
4.  **MIS Recovery**:
    *   A Breadth-First Search (BFS) is performed starting from the source on the **residual graph**. The residual graph consists of edges that still have remaining capacity.
    *   The BFS marks all vertices reachable from the source.
    *   Finally, the size of the MIS is calculated by summing up:
        *   The number of "white" squares `(r+c % 2 == 0)` that were visited by the BFS.
        *   The number of "black" squares `(r+c % 2 == 1)` that were **not** visited by the BFS.

This count gives the size of the maximum independent set, which is the maximum number of knights we can place.

```cpp
#include <iostream>
#include <vector>
#include <queue>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

// Type definitions for the Boost Graph Library
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
typedef boost::graph_traits<graph>::out_edge_iterator out_edge_it;

// A helper class to add edges and their reverse counterparts for the flow network
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
      c_map[rev_e] = 0; // Reverse edge has zero initial capacity
      r_map[e] = rev_e;
      r_map[rev_e] = e;
    }
};

// All 8 possible knight moves
const std::vector<std::pair<int, int>> OFFSETS = {
    {-1, -2}, {-1, 2}, {1, -2}, {1, 2},
    {-2, -1}, {-2, 1}, {2, -1}, {2, 1}
};

// Convert 2D board coordinates to a 1D vertex index
int get_index(int row, int col, int n) {
  return row * n + col;
}

void solve() {
  int n;
  std::cin >> n;
  
  std::vector<std::vector<int>> board(n, std::vector<int>(n));
  int valid_squares = 0;
  for(int row = 0; row < n; ++row) {
    for(int col = 0; col < n; ++col) {
      std::cin >> board[row][col];
      if (board[row][col] == 1) {
          valid_squares++;
      }
    }
  }
  
  // Create a graph with n*n vertices for the board, plus a source and sink
  graph G(n * n); 
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
  
  // Build the flow network
  for(int row = 0; row < n; ++row) {
    for(int col = 0; col < n; ++col) {
      if(board[row][col] == 0) continue; // Skip holes
      int idx = get_index(row, col, n);

      // Partition based on board color
      if((row + col) % 2 == 0) { // "White" squares
        adder.add_edge(v_source, idx, 1);
        // Add edges to reachable "black" squares
        for(const std::pair<int, int> &offset : OFFSETS) {
          int next_row = row + offset.first;
          int next_col = col + offset.second;
          
          if (next_row >= 0 && next_row < n && next_col >= 0 && next_col < n && board[next_row][next_col] == 1) {
            adder.add_edge(idx, get_index(next_row, next_col, n), 1);
          }
        }
      } else { // "Black" squares
        adder.add_edge(idx, v_sink, 1);
      }
    }
  }
  
  // Compute max flow
  long flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  // The size of the maximum independent set is the total number of valid squares
  // minus the size of the maximum matching (which equals the max flow).
  std::cout << valid_squares - flow << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int t;
  std::cin >> t;
  while(t--) {
    solve();
  }
  
  return 0;
}
```
</details>

## ⚡ Result

```plaintext

```