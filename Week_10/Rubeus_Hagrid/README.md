# Rubeus Hagrid

## 📝 Problem Description
A system consists of a surface entry point and $N$ underground chambers, connected by a network of two-way tunnels with specified lengths. The configuration ensures that from any chamber, a unique path leads back to the surface entry point.

Each chamber $i$ initially contains $g_i$ galleons. A traversal begins at the surface entry point, with travel time through each tunnel equal to its length. As time progresses, one galleon disappears from every chamber that has not yet been visited for each second elapsed since the start of the traversal.

Upon reaching chamber $i$ at time $t$, all remaining galleons in that chamber are collected instantly, yielding $g_i - t$ galleons. It is guaranteed that at least one galleon remains in every chamber when it is visited for the first time.

The challenge is to devise a traversal strategy that maximizes the total number of galleons collected. The traversal must start and end at the surface entry point, visit every chamber exactly once, and traverse each tunnel at most twice (once in each direction).

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem describes a set of locations (chambers and an entry point) connected by tunnels, with the specific property that there is a unique path from any chamber back to the entry point. This structure is a fundamental concept in algorithmics. What is it called? Consider how a traversal that must visit every location might work within this structure, especially given the constraint that tunnels can be used at most twice.

</details>

<details>

<summary>Hint #2</summary>

The requirement to visit every chamber, combined with the constraint that each tunnel is traversed at most once in each direction, strongly suggests a full traversal of the structure. A Depth-First Search (DFS) is a natural fit for this pattern of exploration. However, a standard DFS explores branches in an arbitrary order. Here, the order matters. The core of the problem is to find the *optimal* order to explore the different branches from any given junction to maximize the total galleons.

</details>

<details>

<summary>Hint #3</summary>

At every second, you lose a total number of galleons equal to the number of chambers you haven't visited yet. To minimize this loss, you should aim to visit chambers as quickly as possible, thereby reducing the count of "unvisited" chambers. When you are at a chamber with several sub-networks to explore, which one should you choose first? A good greedy heuristic would be to prioritize the sub-network that is most "efficient" to clear. Think about how to define this efficiency. It should likely involve a trade-off between how many chambers a sub-network contains and how long it takes to fully explore it.

</details>

<details>

<summary>Hint #4</summary>

Let's formalize the greedy choice. Suppose from your current location, you can explore two sub-networks, $A$ and $B$. Sub-network $A$ has $N_A$ chambers and takes $T_A$ time to fully traverse (go in, visit everything, and return). Sub-network $B$ has $N_B$ chambers and takes $T_B$ time.
If you visit $A$ then $B$, the $N_B$ chambers in $B$ will each lose an additional $T_A$ galleons while you are busy in $A$. The "cross-loss" is $N_B \times T_A$.
If you visit $B$ then $A$, the "cross-loss" is $N_A \times T_B$.
To minimize loss, you should visit $A$ first if $N_B \times T_A < N_A \times T_B$, which is equivalent to $\frac{T_A}{N_A} < \frac{T_B}{N_B}$. This gives you the greedy criterion: always explore the sub-network with the smallest (Traversal Time / Number of Chambers) ratio first.

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

This problem can be modeled using a tree data structure. The surface entry point is the root of the tree, the chambers are the nodes, and the tunnels are the edges. The property that "from every chamber there is a unique sequence of tunnels leading up to the surface" confirms that the structure is indeed a tree.

The niffler's journey requires visiting every chamber, which corresponds to a full traversal of the tree. The constraint that each tunnel is used at most twice (once down, once up) perfectly describes a Depth-First Search (DFS) traversal that explores a subtree completely before backtracking.

### The Greedy Strategy

The challenge is that not all DFS traversals are equal. The order in which we explore the subtrees from any node affects the total galleons collected. At each second, we lose one galleon from every unvisited chamber. To maximize our collection, we must minimize this loss. This means we should try to reduce the number of unvisited chambers as quickly as possible.

This observation leads to a greedy approach. Whenever we are at a node with multiple children (and thus multiple subtrees to explore), we should prioritize the one that is most "efficient" to clear. We derived the efficiency metric in the hints: we should visit the subtree with the minimum ratio of $\frac{\text{Traversal Time}}{\text{Number of Nodes}}$. By always picking the subtree with the lowest time-per-node cost, we minimize the time that other, larger subtrees have to wait, thus minimizing the overall loss of galleons.

### Two-Pass DFS Implementation

To implement this greedy strategy, we need two key pieces of information for each subtree: its size (number of nodes) and the total time required to traverse it. We can compute these using a two-pass DFS approach.

**1. First Pass: Information Gathering (`fill_fields`)**

We perform a post-order traversal (a type of DFS) starting from the root to compute the required statistics for each node. For each node `u`, we need:
-   `n_nodes`: The total number of nodes in the subtree rooted at `u`. This is simply $1$ (for `u` itself) plus the sum of `n_nodes` of all its children.
-   `traverse_time`: The time required to fully explore the subtree rooted at `u` and return to `u`. For a leaf, this is 0. For an internal node `u`, this is the sum of `(2 * length_to_child_v + traverse_time_of_v)` over all its children `v`. To simplify calculations and avoid large numbers, we *half* of this traversal time. Let's call it `half_traversal_time`. For a node `u`, `half_traversal_time[u] = sum(half_traversal_time[v] + length_to_child_v)`.

**2. Second Pass: Value Calculation (`calculate_values`)**

With the precomputed values, we perform a second DFS to calculate the maximum number of galleons.
-   This traversal also proceeds from the root. At each node `u`, we sort its children `v` in ascending order based on our greedy criterion: $$\frac{\text{half\_traversal\_time}[v] + \text{length}(u,v)}{\text{n\_nodes}[v]}$$ 
To avoid floating-point arithmetic, we use integer cross-multiplication for the comparison: 
$$\frac{T_A}{N_A} < \frac{T_B}{N_B} \iff T_A \cdot N_B < T_B \cdot N_A$$
-   We keep track of the `elapsed_time` since the start of the journey. When we move from a parent `u` to a child `v` through a tunnel of length `l`, the elapsed time increases by `l`.
-   We recursively call the function on the children in the sorted order. After returning from a child `v`'s subtree, the time spent was `2 * (half_traversal_time[v] + l)`. This duration is added to `elapsed_time` before proceeding to the next sibling.
-   The final value for a subtree at `u` is the sum of galleons collected at `u` (`raw_value - elapsed_time_at_u`) and the total galleons collected from all its child subtrees.

The final answer is the total value calculated for the root node (the surface entry point).

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

struct Node {
  int idx;                // Used for Debugging (set during input reading)
  long n_nodes = 1;       // Number of Nodes in the (sub)tree which has this node as root (set in fill_fields)
  long traverse_time = 0; // Time it takes from the root to traverse the (sub)tree which has this node as root (set in fill_fields)
  long raw_value;         // Initial amount of gold in the chamber/node (set during input reading)
  long value;             // Amount of gold obtained from this (sub)tree, respecting the elapsed time (set in calculate_values)
  std::vector<std::pair<Node*, int>> children;
};

void fill_fields(Node *root) {
  // ===== LEAF NODE =====
  if(root->children.empty()) {
    // Leaf nodes already have the fields set either by default or while reading the input
    return;
  }
  
  // ===== INNER NODE =====
  for(std::pair<Node*, int> child_pair : root->children) {
    Node *child = child_pair.first;
    int distance = child_pair.second;
    
    fill_fields(child);
    
    root->n_nodes += child->n_nodes;
    root->traverse_time += child->traverse_time + distance;
  }
}

void calculate_values(Node *root, long elapsed_time) {
  root->value = root->raw_value - elapsed_time;

  // ===== LEAF NODE =====
  if(root->children.empty()) {
    return;
  }
  
  // ===== INNER NODE =====
  // Sort children based on n_nodes / (traverse_time + distance)
  std::sort(root->children.begin(), root->children.end(), [](const std::pair<Node*, int> &a, const std::pair<Node*, int> &b){
    return b.first->n_nodes * (a.first->traverse_time + a.second) < a.first->n_nodes * (b.first->traverse_time + b.second);
  });
  
  // Calculate value for "root"
  for(std::pair<Node*, int> child_pair : root->children) {
    Node *child = child_pair.first;
    int distance = child_pair.second;
 
    calculate_values(child, elapsed_time + distance);
      
    root->value += child->value;
    elapsed_time += 2 * (child->traverse_time + distance); // * 2 as we need to pass all edges twice once on the way in once on the way out
  }
}


void solve() {
  // ===== READ INPUT =====
  int n; std::cin >> n;
  
  std::vector<Node> nodes(n + 1);
  nodes[0].idx = 0;
  for(int i = 1; i < n + 1; ++i) {
    int g; std::cin >> g;
    nodes[i].raw_value = g;
    nodes[i].idx = i;
  }
  
  for(int i = 0; i < n; ++i) {
    int u, v, l; std::cin >> u >> v >> l;
    nodes[u].children.emplace_back(&nodes[v], l);
  }
  
  // ===== CALCULATE VALUES FOR EACH NODE =====
  fill_fields(&nodes[0]);
  calculate_values(&nodes[0], 0);
  
  // ===== OUTPUT =====
  std::cout << nodes[0].value << std::endl;
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
   Test set 1 (30 pts / 3 s) : Correct answer      (0.012s)
   Test set 2 (30 pts / 3 s) : Correct answer      (0.368s)
   Test set 3 (30 pts / 3 s) : Correct answer      (0.35s)
   Test set 4 (10 pts / 3 s) : Correct answer      (0.431s)

Total score: 100
```