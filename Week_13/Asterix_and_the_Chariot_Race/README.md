# Astérix and the Chariot Race

## 📝 Problem Description

A set of $n$ cities, labeled from $0$ to $n-1$, is connected by $n-1$ one-way stages, ensuring that every city can be reached from city $0$. Each city $i$ has a repair cost $c_i$. A city is considered "saved from disgrace" if it is repaired, or if at least one directly connected neighboring city is repaired. The goal is to select a subset of cities to repair so that every city is saved from disgrace, while minimizing the total repair cost.

Given the number of cities $n$, the $n-1$ connections between them, and the repair costs $c_i$ for each city, determine the minimum total cost required to save all cities from disgrace.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The structure of cities and stages is a **tree**, with city 0 as the root. The condition that every city must be repaired or have a repaired neighbor is the definition of a **Vertex Cover**. The problem is asking for a **Minimum Weight Vertex Cover** on this tree. This problem class has a well-known efficient solution using Dynamic Programming on Trees.

</details>

<details>

<summary>Hint #2</summary>

To solve this with Dynamic Programming, you can use a Depth First Search (DFS) from the root. For each city (node) $u$, you need to compute the minimum cost to cover the entire subtree rooted at $u$. This cost, however, depends on the state of $u$. Consider two main states for each node $u$

</details>

<details>

<summary>Hint #3</summary>

For each node $u$ in the tree, you need to calculate **three different costs** representing different scenarios: the minimum cost to cover the subtree ...

- when $u$ is repaired by itself
- when $u$ is covered by its parent
- when $u$ is covered by one of its children. 

These three states capture all possible ways a node can be "saved from disgrace" in the vertex cover problem.

</details>

<details>

<summary>Hint #4</summary>

To calculate each of the 3 costs: 
- **Self cost** = node's repair cost + sum of children's "parent cost". 
- **Parent cost** = sum of min(child's self cost, child's child cost) for each child. 
- **Child cost** = One of the children needs to be repaired. Try each child as the designated repairer, compute the cost when that child must be repaired while others choose optimally, then take the minimum over all choices.

</details>

<details>

<summary>Hint #5</summary>

The most complex case is when a node must be covered by one of its children. In this case, one of its children needs to be selected to be safed. The naive approach tries each child as the designated repairer, but this leads to quadratic complexity. 
However, you can optimize this: if any child would prefer to be repaired anyway (because repairing itself is cheaper than being covered by its own children), then it can "volunteer" at no extra cost. Only when no child volunteers do you need to force one to be repaired, incurring a penalty.
Use this optimization to speed up the calculation of the cost when a node is covered by its child.

</details>



## ✨ Solutions

<details>

<summary>First Solution (Test Set 1, 2, 3)</summary>

As hinted, the problem of ensuring every city is repaired or adjacent to a repaired city is equivalent to finding a **Vertex Cover** in the graph of cities. Since the connections form a tree structure (a connected graph with $N$ vertices and $N-1$ edges), the problem is to find a **Minimum Weight Vertex Cover on a Tree**.

This can be solved efficiently using **Dynamic Programming on a Tree**. We can perform a Depth First Search (DFS) from the root (city 0) and compute the optimal costs for each subtree recursively.

### DP States

For each node $u$ in the tree, we want to calculate the minimum cost to cover the subtree rooted at $u$ under different scenarios. A node $u$ can be covered in three ways:
1.  **By itself:** We pay the cost to repair city $u$.
2.  **By its parent:** Its parent is repaired, covering the edge between them.
3.  **By one of its children:** One of its children is repaired.

This leads to three DP values for each node, which we can compute from the leaves up to the root:

*   `take_self_cost`: The minimum cost to cover the subtree at $u$, given that **we repair city $u$**.
*   `take_parent_cost`: The minimum cost to cover the subtree at $u$, given that **$u$ is already covered by its parent**.
*   `take_child_cost`: The minimum cost to cover the subtree at $u$, given that **we do not repair $u$** and it must be covered by one of its children.

### Recursive Calculation (DFS)

We use a post-order traversal (DFS) to compute these values.

**Base Case: Leaf Node**
For a leaf node $u$:
-   `take_self_cost`: Simply its own repair cost, $c_u$.
-   `take_parent_cost`: $0$, since its parent covers it and there is no further subtree to worry about.
-   `take_child_cost`: $\infty$, as a leaf has no children to cover it. This state must be avoided.

**Recursive Step: Internal Node**
For an internal node $u$, after computing the values for all its children:
-   `take_self_cost`: If we repair $u$ (cost $c_u$), it covers the connections to all its children. Each child $v$ is now covered by its parent ($u$). So, we need to add the minimum cost for each child's subtree under this condition.
    $$ \text{take\_self\_cost}(u) = c_u + \sum_{v \in \text{children}(u)} \text{take\_parent\_cost}(v) $$

-   `take_parent_cost`: If $u$ is covered by its parent, we don't need to repair $u$. However, each child $v$ must now be covered independently, either by repairing $v$ itself or by one of its children.
    
    $$ \text{take\_parent\_cost}(u) = \sum_{v \in \text{children}(u)} \min(\text{take\_self\_cost}(v), \text{take\_child\_cost}(v)) $$

-   `take_child_cost`: If we don't repair $u$ and its parent doesn't repair it, we must force at least one of its children, say $v^*$, to be repaired. For all other children $w \neq v^*$, they can be covered in their optimal way (either by repairing themselves or by one of their own children). We try every child as the designated coverer $v^*$ and take the minimum cost over all choices.

    $$ \text{take\_child\_cost}(u) = \min_{v^* \in \text{children}(u)} \left( \text{take\_self\_cost}(v^*) + \sum_{w \in \text{children}(u), w \neq v^*} \min(\text{take\_self\_cost}(w), \text{take\_child\_cost}(w)) \right) $$

**Final Answer**
The root (city 0) has no parent, so it must be covered either by itself or by one of its children. The final answer is $\min(\text{take\_self\_cost}(0), \text{take\_child\_cost}(0))$.

### Code
```cpp
#include <iostream>
#include <vector>
#include <limits>

const int MAX_COST = std::numeric_limits<int>::max();

struct Node {
  int idx; // DEBUG
  int raw_cost;
  int take_child_cost;
  int take_parent_cost;
  int take_self_cost;
  Node* parent;
  std::vector<Node*> children;
};

void dfs(Node *root) {
  // ===== BASE CASE =====
  if(root->children.empty()) {
    root->take_self_cost = root->raw_cost;
    root->take_child_cost = MAX_COST;
    root->take_parent_cost = 0;
    
    return;
  }
  
  // ===== RECURSIVE CASE =====
  // Calculate necessary values at children
  for(Node *child : root->children) {
    dfs(child);
  }
  
  // Calculate `take_self_cost`
  root->take_self_cost = root->raw_cost;
  for(Node *child : root->children) {
    root->take_self_cost += child->take_parent_cost;
  }
  
  // Calculate `take_child_cost`
  root->take_child_cost = MAX_COST;
  for(Node *take_child : root->children) {
    int cost = take_child->take_self_cost;
    
    for(Node *other_child : root->children) {
      if(other_child == take_child) { continue; }
      cost += std::min(other_child->take_self_cost, other_child->take_child_cost);
    }
    
    root->take_child_cost = std::min(root->take_child_cost, cost);
  }
  
  // Calculate `take_parent_cost`
  root->take_parent_cost = 0;
  for(Node *child : root->children) {
    root->take_parent_cost += std::min(child->take_self_cost, child->take_child_cost);
  }
  root->take_parent_cost = std::min(root->take_parent_cost, root->take_self_cost);
  
}

void solve() {
  // ===== READ INPUT =====
  int n; std::cin >> n;
  std::vector<Node> nodes(n);
  
  for(int i = 0; i < n - 1; ++i) {
    int u, v; std::cin >> u >> v;
    nodes[u].children.push_back(&nodes[v]);
    nodes[v].parent = &nodes[u];
  }
  for(int i = 0; i < n; ++i) {
    int c; std::cin >> c;
    nodes[i].raw_cost = c;
    nodes[i].idx = i; // DEBUG
  }
  
  // ===== SOLVE =====
  dfs(&nodes[0]);
   
  // ===== OUTPUT =====
  std::cout << std::min(nodes[0].take_self_cost, nodes[0].take_child_cost) << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}
```
</details>

<details>

<summary>Final Solution</summary>

The first solution is logically correct but too slow for the largest test cases. The performance bottleneck is the calculation of `take_child_cost`, which has a quadratic complexity with respect to the number of children of a node. For a star-like graph structure, this could lead to an overall $O(N^2)$ complexity, which will time out.

### The Optimization

We need to optimize the calculation of `take_child_cost`. Let's re-examine the formula:

$$ \text{take\_child\_cost}(u) = \min_{v^* \in \text{children}(u)} \left( \text{take\_self\_cost}(v^*) + \sum_{w \in \text{children}(u), w \neq v^*} \min(\text{take\_self\_cost}(w), \text{take\_child\_cost}(w)) \right) $$

We can rewrite the sum inside the `min` by first taking the sum over all children and then adjusting for the chosen child $v^*$:

$$ \sum_{w \neq v^*} \min(\dots) = \left(\sum_{w \in \text{children}(u)} \min(\dots)\right) - \min(\text{take\_self\_cost}(v^*), \text{take\_child\_cost}(v^*)) $$

Let 

$$ S = \sum_{w \in \text{children}(u)} \min(\text{take\_self\_cost}(w), \text{take\_child\_cost}(w)) $$

Then the total cost when forcing child $v^*$ to be taken is:

$$ \text{Cost}(v^*) = \text{take\_self\_cost}(v^*) + S - \min(\text{take\_self\_cost}(v^*), \text{take\_child\_cost}(v^*)) $$

$$ \text{take\_child\_cost}(u) = \min_{v^* \in \text{children}(u)} \text{Cost}(v^*) = S + \min_{v^* \in \text{children}(u)} \left( \text{take\_self\_cost}(v^*) - \min(\text{take\_self\_cost}(v^*), \text{take\_child\_cost}(v^*)) \right) $$

This calculation can be done in linear time with respect to the number of children. However, there's an even simpler insight that the provided code uses.

### The "Volunteer" Insight

Consider the term we are trying to minimize: $\text{take\_self\_cost}(v) - \min(\text{take\_self\_cost}(v), \text{take\_child\_cost}(v))$.
-   If `take_self_cost(v) <= take_child_cost(v)`, this difference is $0$.
-   If `take_self_cost(v) > take_child_cost(v)`, this difference is positive.

This means if there is any child $v$ that "volunteers" to be taken (i.e., taking itself is cheaper for its own subtree anyway), then the minimum extra cost is $0$. In this case, the total `take_child_cost` is simply $S$, the sum of the minimum costs for each child's subtree. We can find such a volunteer with a single pass through the children.

If **no** child volunteers (i.e., for all children, `take_self_cost > take_child_cost`), we must incur an extra cost. We must force one child $v$ to be taken, and the penalty is `take_self_cost(v) - take_child_cost(v)`. We should choose the child with the minimum penalty.

The solution code implements the first part of this optimization. It checks if a volunteer exists. If so, it calculates the cost in linear time. If not, it falls back to the original quadratic loop, which is sufficient to pass all tests

### Code
```cpp
#include <iostream>
#include <vector>
#include <limits>

const int MAX_COST = std::numeric_limits<int>::max();

struct Node {
  int idx; // DEBUG
  int raw_cost;
  int take_child_cost;
  int take_parent_cost;
  int take_self_cost;
  Node* parent;
  std::vector<Node*> children;
};

void dfs(Node *root) {
  // ===== BASE CASE =====
  if(root->children.empty()) {
    root->take_self_cost = root->raw_cost;
    root->take_child_cost = MAX_COST;
    root->take_parent_cost = 0;
    
    return;
  }
  
  // ===== RECURSIVE CASE =====
  // Calculate necessary values at children
  for(Node *child : root->children) {
    dfs(child);
  }
  
  // Calculate `take_self_cost`
  root->take_self_cost = root->raw_cost;
  for(Node *child : root->children) {
    root->take_self_cost += child->take_parent_cost;
  }
  
  // Calculate `take_child_cost`
  root->take_child_cost = 0;
  bool found_volunteer = false;;
  for(Node *child : root->children) {
    if(child->take_self_cost <= child->take_child_cost) { found_volunteer = true; }
    root->take_child_cost += std::min(child->take_self_cost, child->take_child_cost);
  }
  
  if(!found_volunteer) {
    root->take_child_cost = MAX_COST;
      
    for(Node *take_child : root->children) {
      int cost = take_child->take_self_cost;
      
      for(Node *other_child : root->children) {
        if(other_child == take_child) { continue; }
        cost += std::min(other_child->take_self_cost, other_child->take_child_cost);
      }
      
      root->take_child_cost = std::min(root->take_child_cost, cost);
    }
  }
  
  // Calculate `take_parent_cost`
  root->take_parent_cost = 0;
  for(Node *child : root->children) {
    root->take_parent_cost += std::min(child->take_self_cost, child->take_child_cost);
  }
  root->take_parent_cost = std::min(root->take_parent_cost, root->take_self_cost);
  
}

void solve() {
  // ===== READ INPUT =====
  int n; std::cin >> n;
  std::vector<Node> nodes(n);
  
  for(int i = 0; i < n - 1; ++i) {
    int u, v; std::cin >> u >> v;
    nodes[u].children.push_back(&nodes[v]);
    nodes[v].parent = &nodes[u];
  }
  for(int i = 0; i < n; ++i) {
    int c; std::cin >> c;
    nodes[i].raw_cost = c;
    nodes[i].idx = i; // DEBUG
  }
  
  // ===== SOLVE =====
  dfs(&nodes[0]);
  
  // ===== OUTPUT =====
  std::cout << std::min(nodes[0].take_self_cost, nodes[0].take_child_cost) << std::endl;
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
Compiling: successful

Judging solution >>>>
   Test set 1 (25 pts / 3 s) : Correct answer      (0.088s)
   Test set 2 (25 pts / 3 s) : Correct answer      (0.091s)
   Test set 3 (25 pts / 3 s) : Correct answer      (0.091s)
   Test set 4 (25 pts / 3 s) : Correct answer      (1.373s)

Total score: 100
```