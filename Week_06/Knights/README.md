# Knights

## 📝 Problem Description

You are given an $m \times n$ grid of intersections connected by hallways. There are $k$ knights, each starting at a specific intersection coordinate $(x, y)$. The objective is to find the maximum number of knights that can escape the grid.

The movement of knights is constrained by two primary rules:
1.  A hallway segment connecting two adjacent intersections (horizontally or vertically) can be used by at most one knight in total.
2.  Any given intersection can be passed through by at most $C$ knights in total. A knight's starting position counts as one use of that intersection.

A knight escapes by moving from an intersection at the boundary of the grid (i.e., in row 0 or $n-1$, or column 0 or $m-1$) to the "outside". Your task is to calculate the maximum number of knights that can simultaneously find a valid path from their starting position to an escape route.

## 💡 Hints

<details>
<summary>Hint #1</summary>
This problem involves moving entities (knights) through a system with capacity limitations (on hallways and at intersections). This structure is characteristic of problems that can be modeled as a network. Think about how you can represent the movement of knights and the given constraints as flows through such a system.
</details>
<details>
<summary>Hint #2</summary>
Consider modeling the cave as a graph. What would the vertices and edges represent? If intersections are vertices and hallways are edges, the constraint on hallways is straightforward to model. What property of an edge can represent that it can only be used once? This naturally leads to the concept of edge capacities. How would you integrate the knights' starting positions and their escape routes into this graph model? Think about adding special vertices, like a source and a sink.
</details>
<details>
<summary>Hint #3</summary>
A standard graph flow model handles capacities on edges, but this problem also imposes a capacity $C$ on each *vertex* (intersection). This is a common and important variation. A powerful technique to handle vertex capacities is **vertex splitting**. Can you replace each original vertex with a pair of new vertices connected by an edge, such that the capacity of this new edge enforces the original vertex capacity?
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
This problem can be elegantly solved by modeling it as a **maximum flow** problem on a carefully constructed graph. The "flow" in our network will represent the paths of the knights. The maximum flow will then correspond to the maximum number of knights that can find valid escape routes.

### Graph Construction

The core of the solution is to build a flow network that accurately represents all the problem's constraints.

1.  **Source and Sink:** We introduce a single **super-source** vertex, $s$, and a single **super-sink** vertex, $t$. The source $s$ will be the origin of all flow (knights), and the sink $t$ will be the final destination (the outside world).

2.  **Vertex Splitting for Intersection Capacities:** A standard flow network has capacities on edges, not on vertices. To model the constraint that each intersection can be used by at most $C$ knights, we use a technique called **vertex splitting**. For each intersection at grid coordinates $(r, c)$, we create two nodes in our graph:
    *   An **in-node**, $v_{in}(r, c)$.
    *   An **out-node**, $v_{out}(r, c)$.

    We then add a directed edge from the in-node to the out-node, $v_{in}(r, c) \to v_{out}(r, c)$, with a capacity of $C$. Any flow representing a knight passing through this intersection *must* traverse this edge. This construction elegantly ensures that the total flow through the intersection $(r, c)$ cannot exceed $C$.

3.  **Hallway Edges:** Hallways connect adjacent intersections. Since a hallway can be used by only one knight, we model them as edges with capacity 1. For any two adjacent intersections $(r_1, c_1)$ and $(r_2, c_2)$, we add two directed edges to our network:
    *   $v_{out}(r_1, c_1) \to v_{in}(r_2, c_2)$ with capacity 1.
    *   $v_{out}(r_2, c_2) \to v_{in}(r_1, c_1)$ with capacity 1.

    Notice that a knight "leaves" an intersection via its out-node and "arrives" at the next intersection via its in-node.

4.  **Knight Starting Positions:** To place the knights into the network, we connect the super-source $s$ to the in-node of each knight's starting intersection. For each knight starting at $(r, c)$, we add an edge $s \to v_{in}(r, c)$ with a capacity of 1. This injects one unit of flow into the network for each knight, representing their potential to escape.

5.  **Escape Routes:** Knights escape when they move from a boundary intersection to the outside. In our model, this corresponds to flow reaching the super-sink $t$. For any move from an intersection $(r, c)$ that would go off the grid, we instead direct that path to the sink. For example, from an intersection $(0, c)$ in the top row, a move "up" leads to an escape. We model this by adding an edge from its out-node to the sink: $v_{out}(0, c) \to t$ with capacity 1. We do this for all four boundaries.

### Finding the Solution

After constructing this network, the problem is reduced to finding the maximum possible flow from the source $s$ to the sink $t$. The value of this maximum flow is precisely the maximum number of knights that can escape the cave. Any standard max-flow algorithm, such as Edmonds-Karp or Push-Relabel, can be used to find this value. This single, general model correctly handles all subtasks, including the specific cases for $C=1$ and $C=2$.

```cpp
#include <iostream>
#include <vector>

// We use the Boost Graph Library for a robust implementation of the max-flow algorithm.
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>

// Type definitions for the Boost graph
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, 
                              boost::vecS, 
                              boost::directedS, 
                              boost::no_property,
                              boost::property<boost::edge_capacity_t, long,
                                              boost::property<boost::edge_residual_capacity_t, long,
                                                              boost::property<boost::edge_reverse_t, traits::edge_descriptor>>>> graph;
typedef traits::vertex_descriptor vertex_desc;

// Helper function to map a grid coordinate (row, col) and type (in/out) to a unique vertex index.
// In-nodes are indexed from 0 to n*m-1. Out-nodes are indexed from n*m to 2*n*m-1.
int get_vertex_idx(int row, int col, bool is_outgoing_node, int n, int m) {
  return row * m + col + n * m * is_outgoing_node;
}

// An edge adder helper class to simplify adding edges and their reverse edges for the BGL.
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
      c_map[rev_e] = 0; // Reverse edge has 0 capacity in a directed graph
      r_map[e] = rev_e;
      r_map[rev_e] = e;
    }
};

void solve() {
  int m, n, k, c;
  std::cin >> m >> n >> k >> c;
  
  // Create a graph with 2*n*m nodes for intersections, plus source and sink.
  graph G(n * m * 2);
  edge_adder adder(G);
  
  // Add source and sink vertices. Their indices will be 2*n*m and 2*n*m + 1.
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
    
  for (int row = 0; row < n; ++row) {
    for (int col = 0; col < m; ++col) {
      int v_incoming = get_vertex_idx(row, col, false, n, m);
      int v_outgoing = get_vertex_idx(row, col, true, n, m);
      
      // 1. Add edge from in-node to out-node to model vertex capacity C.
      adder.add_edge(v_incoming, v_outgoing, c);
      
      // 2. Connect to adjacent intersections or to the sink if on the boundary.
      // Move Up
      if (row > 0) adder.add_edge(v_outgoing, get_vertex_idx(row - 1, col, false, n, m), 1);
      else adder.add_edge(v_outgoing, v_sink, 1);
      
      // Move Down
      if (row < n - 1) adder.add_edge(v_outgoing, get_vertex_idx(row + 1, col, false, n, m), 1);
      else adder.add_edge(v_outgoing, v_sink, 1);
      
      // Move Left
      if (col > 0) adder.add_edge(v_outgoing, get_vertex_idx(row, col - 1, false, n, m), 1);
      else adder.add_edge(v_outgoing, v_sink, 1);
      
      // Move Right
      if (col < m - 1) adder.add_edge(v_outgoing, get_vertex_idx(row, col + 1, false, n, m), 1);
      else adder.add_edge(v_outgoing, v_sink, 1);
    }
  }
    
  // 3. Connect the source to each knight's starting position.
  for (int i = 0; i < k; ++i) {
    int x, y; // x is col, y is row
    std::cin >> x >> y;
    adder.add_edge(v_source, get_vertex_idx(y, x, false, n, m), 1);
  }
  
  // Calculate the maximum flow from source to sink.
  long flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  std::cout << flow << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
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
   Test set 1 (40 pts / 1 s) : Correct answer      (0.123s)
   Test set 2 (40 pts / 1 s) : Correct answer      (0.105s)
   Test set 3 (20 pts / 1 s) : Correct answer      (0.109s)

Total score: 100
```