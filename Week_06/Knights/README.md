# Knights

## 📝 Problem Description

Given an $m \times n$ grid of intersections connected by hallways, there are $k$ knights, each initially positioned at a specific intersection $(x, y)$. The goal is to determine the maximum number of knights that can escape the grid under the following constraints:

1. Each hallway segment connecting two adjacent intersections (either horizontally or vertically) may be traversed by at most one knight in total.
2. Each intersection may be used by at most $C$ knights in total, with a knight's starting position counting as one use of that intersection.

A knight is considered to have escaped if it moves from an intersection located on the boundary of the grid (that is, any intersection in row $0$ or $n-1$, or column $0$ or $m-1$) to the outside. The task is to compute the largest possible number of knights that can simultaneously find valid escape routes from their starting positions.

## 💡 Hints

<details>

<summary>Hint #1</summary>

This problem involves moving entities (knights) through a system with capacity limitations (on hallways and at intersections). This structure is characteristic of problems that can be modeled as a network. Think about how you can represent the movement of knights and the given constraints as flows through such a system.

</details>

<details>

<summary>Hint #2</summary>

Consider modeling the cave as a graph. What would the vertices and edges represent? If intersections are vertices and hallways are edges, the constraint on hallways is straightforward to model. What property of an edge can represent that it can only be used once? How would you integrate the knights' starting positions and their escape routes into this graph model?

</details>

<details>

<summary>Hint #3</summary>

A standard graph flow model handles capacities on edges, but this problem also imposes a capacity $C$ on each *node* (intersection). This is a common and important variation. Can you replace each original node with a pair of new nodes connected by an edge, such that the capacity of this new edge enforces the original node capacity?

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

We can model the problem as a **graph**, where each intersection is a node and the hallways are the edges connecting the intersections. This gives us an $n \times m$ grid of nodes.

The problem can be formulated as a **max flow** problem. The path of each individual knight corresponds to a unit of flow. This allows us to ensure that **no two knights walk through the same hallway** by setting the edge capacity for all hallways to 1.

However, we cannot introduce a source or sink within the grid without disrupting its structure. Therefore, we add **additional source and sink nodes**:

- The **source** is connected to each starting position of a knight with an edge of capacity 1, ensuring that each knight can contribute at most one unit of flow.
- The **sink** is connected to all nodes at the border of the grid, representing the possible escape points for the knights. Each connection from a border node to the sink has capacity 1. <br />
**Note**: The capacity of the sink edges does not actually matter as by construction of the entire graph, only one flow/knight can reach each border node.

The flow through the graph represents the path of each knight. Since only one knight can go along each hallway, all hallway capacities are set to one. By connecting the source node to all the knight positions, we ensure that the flow starts at the correct locations. When we calculate the max flow, the knights will automatically “block” each other by saturating the capacity of each path. If no knight can find a path to the border, the max flow algorithm will stop, as there is no way to increase the flow. In this case, no further knight can escape, and our final answer is the value of the flow.

The only remaining constraint is that each intersection also has a maximum capacity $C$ before it collapses. This requires modeling **node capacities**. To achieve this we apply the following trick:

- Each intersection is split into two nodes: one that receives all incoming edges (**in-node**) and one that sends all outgoing edges (**out-node**).
- We add an edge between the in-node and out-node with capacity $C$ to ensure that at most $C$ knights/flow can pass through the node/intersection.

If a knight (flow) wants to go through an intersection, it must enter at the in-node (where all incoming edges point), traverse the edge with capacity $C$ (the only edge available from the in-node), and then proceed to any connected out-node. By forcing each knight/flow through the edge connecting the in-node and out-node, we ensure that at most $C$ knights/flow can pass through this intersection.

The final step is to compute the maximum flow from the source to the sink in this constructed network. The value of this maximum flow is the maximum number of knights that can escape the cave.


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

bool INCOMING = false;
bool OUTGOING = true;

int get_vertex_idx(int row, int col, bool outgoing, int n, int m) {
  return row * m + col + n * m * outgoing;
}

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
  int m, n, k, c; std::cin >> m >> n >> k >> c;
  
  // Build Graph
  graph G(m * n * 2); // * 2 Since per node, we need 2 nodes to model vertex capacity
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_sink = boost::add_vertex(G);
    
  for(int row = 0; row < n; ++row) {
    for(int col = 0; col < m; ++col) {
      int v_incoming = get_vertex_idx(row, col, INCOMING, n, m);
      int v_outgoing = get_vertex_idx(row, col, OUTGOING, n, m);
      
      // Connect incoming to outgoing
      adder.add_edge(v_incoming, v_outgoing, c);
      
      // Connect to top intersection
      if(row != 0) adder.add_edge(v_outgoing, get_vertex_idx(row - 1, col, INCOMING, n, m), 1);
      else adder.add_edge(v_outgoing, v_sink, 1);
      
      // Connect to bottom intersection
      if(row != n-1) adder.add_edge(v_outgoing, get_vertex_idx(row + 1, col, INCOMING, n, m), 1);
      else adder.add_edge(v_outgoing, v_sink, 1);
      
      // Connect to left intersection
      if(col != 0) adder.add_edge(v_outgoing, get_vertex_idx(row, col - 1, INCOMING, n, m), 1);
      else adder.add_edge(v_outgoing, v_sink, 1);
      
      // Connect to right intersection
      if(col != m-1) adder.add_edge(v_outgoing, get_vertex_idx(row, col + 1, INCOMING, n, m), 1);
      else adder.add_edge(v_outgoing, v_sink, 1);
    }
  }
    
  // Connect source to knight positions
  for(int i = 0; i < k; ++i) {
    int x, y; std::cin >> x >> y;
    adder.add_edge(v_source, get_vertex_idx(y, x, INCOMING, n, m), 1);
  }
  
  // ===== CALCULATE MAX FLOW =====
  int flow = boost::push_relabel_max_flow(G, v_source, v_sink);
  
  // ===== OUTPUT =====
  std::cout << flow << std::endl;
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

- You can enforce node capacities by simply splitting the node into two and connecting them with an edge of capacity equal to the original node's capacity. See solution for details.

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