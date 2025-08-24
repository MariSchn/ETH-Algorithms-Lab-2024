# Tiles

## 📝 Problem Description

The central task is to determine whether a given rectangular layout can be completely tiled using 2x1 tiles. You are given the dimensions of the layout, $W$ and $H$. The layout itself is represented as a grid where each space is either designated to be tiled or to be left empty.

Each 2x1 tile must cover exactly two adjacent spaces that are marked for tiling. These adjacent spaces can be either horizontal or vertical neighbors. Every space that needs to be tiled must be covered by exactly one tile, and no tiles can be placed on spaces designated to be left empty. Your program should output "yes" if a valid tiling exists and "no" otherwise.

## 💡 Hints

<details>
<summary>Hint #1</summary>
Before diving into complex algorithms, consider a simple, fundamental property of the tiles. Each tile covers exactly two spaces. What does this imply about the total number of spaces that need to be tiled for a valid solution to even be possible?
</details>
<details>
<summary>Hint #2</summary>
This problem is about pairing up adjacent, tileable spaces. How can you model this? Think about representing each tileable space as an object and the potential placement of a tile between two adjacent spaces as a connection between them. This reframes the problem from laying tiles to finding a perfect set of pairings.
</details>
<details>
<summary>Hint #3</summary>
The problem can be solved by finding a **maximum matching**. To construct the required model, it's incredibly helpful to color the grid like a checkerboard. Notice that any 2x1 tile will always cover one "white" space and one "black" space. This structure allows you to create a **bipartite graph**. Maximum matching in a bipartite graph is a classic problem that can be solved efficiently using a max-flow algorithm.
</details>

## ✨ Solutions

<details> 

<summary>First Solution (Test Set 3, 4)</summary>

This solution correctly identifies the problem as a **bipartite matching** problem and uses **maximum flow** to solve it. The approach models the tileable spaces as vertices in a bipartite graph, where vertices are colored in a checkerboard pattern (based on whether `(r + c)` is even or odd).

The key insight is that any 2×1 tile must cover exactly one "white" space and one "black" space. This creates a bipartite graph where we need to find a perfect matching.

However, this implementation has some issues in its graph construction. It creates **bidirectional edges** between adjacent tileable spaces and attempts to handle both possible checkerboard colorings by running the flow algorithm twice (once with `odd=false` and once with `odd=true`) and checking if either produces a perfect matching.

While this approach demonstrates understanding of the core concepts, the graph construction is not optimal, which limits its effectiveness on certain test cases. Despite these implementation issues, the solution manages to pass some test sets.

### Code
```cpp
///3
#include<iostream>
#include<vector>
#include<string>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/push_relabel_max_flow.hpp>

using index_map = std::unordered_map<std::pair<int, int>, int>;

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

int calculate_flow(int w, int h, std::vector<std::vector<bool>>& garden, bool odd) {
  // Build Graph
  graph G(w * h); // It would make more sense to only add num_nodes nodes, but this makes it easier to code
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);

  for(int r = 0; r < h; ++r) {
    for(int c = 0; c < w; ++c) {
      if(garden[r][c]) {
        int v_idx = r * w + c;

        // Add alternating source and sink connections in checkerboard pattern
        if((r + c) % 2 == odd) {
          adder.add_edge(v_source, v_idx, 1);
        } else {
          adder.add_edge(v_idx, v_sink, 1);
        }
        
        // Add connections to neighbors
        if(r != h-1 && garden[r+1][c]) {
          adder.add_edge(v_idx, (r+1) * w + c, 1);
          adder.add_edge((r+1) * w + c, v_idx, 1);
        }
        if(c != w-1 && garden[r][c+1]) {
          adder.add_edge(v_idx, r * w + (c + 1), 1);
          adder.add_edge(r * w + (c + 1), v_idx, 1);
        }
      }
    }
  }
  
  // ===== CALCULATE MAX FLOW =====
  int flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  return flow;
}

void solve() {
  // ===== READ INPUT =====
  // Read Input
  int w, h; std::cin >> w >> h;
  int num_nodes = 0;
  
  std::vector<std::vector<bool>> garden(h, std::vector<bool>(w, false));
  for(int r = 0; r < h; ++r) {
    std::string row; std::cin >> row;
    
    for(int c = 0; c < w; ++c) {
      if(row[c] == '.') {
        garden[r][c] = true;
        num_nodes++;
      }
    }
  }
  
  if(num_nodes % 2 == 1) {
    // Can't tile an odd numbered of spaces
    std::cout << "no" << std::endl;
    return;
  }
  
  // ===== CALCULATE MAX FLOW =====
  int even_flow = calculate_flow(w, h, garden, false);
  int odd_flow = calculate_flow(w, h, garden, true);
  
  // ===== OUTPUT =====
  if(even_flow * 2 == num_nodes || odd_flow * 2 == num_nodes) {
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


<details>
<summary>Final Solution</summary>
This problem can be elegantly solved by modeling it as a maximum flow problem. The core idea is to determine if all tileable spaces in the garden can be perfectly paired up with an adjacent tileable space. This is equivalent to finding a **perfect matching** in a graph.

### From Tiling to Graphs

First, we can represent the garden layout as a graph. Each space `(r, c)` that can be tiled (marked with a '.') becomes a vertex in our graph. An edge exists between two vertices if their corresponding spaces are adjacent (horizontally or vertically) in the grid. A tiling of the garden corresponds to a **matching** in this graph—a set of edges where no two edges share a vertex. A **perfect matching** is a matching that covers every single vertex in the graph. Our goal is to find out if such a perfect matching exists.

### Bipartite Matching and Max-Flow

Finding a maximum matching in a general graph can be complex. However, our graph has a special property. We can color the grid like a checkerboard. A space at `(r, c)` can be colored "white" if `(r + c)` is even and "black" if `(r + c)` is odd. Crucially, any 2x1 tile placed on the grid will always cover exactly one white space and one black space. This means we can partition our graph's vertices into two sets (white and black) where edges only exist *between* the sets, not within them. This is the definition of a **bipartite graph**.

The problem of finding a maximum matching in a bipartite graph can be transformed into a maximum flow problem using the **Max-Flow Min-Cut Theorem**. We construct a flow network as follows:

1.  **Source and Sink:** Create a source vertex `S` and a sink vertex `T`.
2.  **Source to White Vertices:** For every "white" tileable space `u`, add a directed edge from `S` to `u` with a capacity of 1. This represents that each white space can be the start of one tile.
3.  **Black Vertices to Sink:** For every "black" tileable space `v`, add a directed edge from `v` to `T` with a capacity of 1. This represents that each black space can be the end of one tile.
4.  **White to Black Vertices:** For every pair of adjacent white and black tileable spaces, `u` and `v` respectively, add a directed edge from `u` to `v` with a capacity of 1. This represents the possibility of placing a tile between them.

### Interpreting the Result

The maximum flow from `S` to `T` in this network corresponds to the maximum number of tiles that can be placed, which is the size of the maximum matching. Let `N` be the total number of tileable spaces.

-   A simple initial check: If `N` is odd, a perfect tiling is impossible, as each tile covers two spaces.
-   If `N` is even, a perfect tiling exists if and only if we can place `N / 2` tiles. This means the maximum flow must be equal to `N / 2`.
-   Therefore, the condition for a successful tiling is: **`max_flow * 2 == N`**.

If this condition holds, the layout can be tiled; otherwise, it cannot. The provided C++ code implements this exact logic using the Boost Graph Library to calculate the maximum flow.

**Code**
```cpp
///3
#include<iostream>
#include<vector>
#include<string>

#include<boost/graph/adjacency_list.hpp>
#include<boost/graph/push_relabel_max_flow.hpp>

using index_map = std::unordered_map<std::pair<int, int>, int>;

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
  // ===== READ INPUT =====
  // Read Input
  int w, h; std::cin >> w >> h;
  int num_nodes = 0;
  
  std::vector<std::vector<bool>> garden(h, std::vector<bool>(w, false));
  for(int r = 0; r < h; ++r) {
    std::string row; std::cin >> row;
    
    for(int c = 0; c < w; ++c) {
      if(row[c] == '.') {
        garden[r][c] = true;
        num_nodes++;
      }
    }
  }
  
  if(num_nodes % 2 == 1) {
    // Can't tile an odd numbered of spaces
    std::cout << "no" << std::endl;
    return;
  }
  
  // Build Graph
  graph G(w * h); // It would make more sense to only add num_nodes nodes, but this makes it easier to code
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);

  for(int r = 0; r < h; ++r) {
    for(int c = 0; c < w; ++c) {
      if(garden[r][c]) {
        int v_idx = r * w + c;

        // Add alternating source and sink connections in checkerboard pattern
        if((r + c) % 2 == 0) {
          adder.add_edge(v_source, v_idx, 1);
          if(r != h-1 && garden[r+1][c]) adder.add_edge(v_idx, (r+1) * w + c, 1);
          if(c != w-1 && garden[r][c+1]) adder.add_edge(v_idx, r * w + (c + 1), 1);
        } else {
          adder.add_edge(v_idx, v_sink, 1);
          if(r != h-1 && garden[r+1][c]) adder.add_edge((r+1) * w + c, v_idx, 1);
          if(c != w-1 && garden[r][c+1]) adder.add_edge(r * w + (c + 1), v_idx, 1);
        }
      }
    }
  }
  
  // ===== CALCULATE MAX FLOW =====
  int flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  // ===== OUTPUT =====
  if(flow * 2 == num_nodes) {
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
   Test set 1 (40 pts / 0.200 s) : Correct answer      (0.003s)
   Test set 2 (30 pts / 0.200 s) : Correct answer      (0.025s)
   Test set 3 (20 pts / 0.200 s) : Correct answer      (0.087s)
   Test set 4 (10 pts / 0.200 s) : Correct answer      (0.009s)

Total score: 100
```