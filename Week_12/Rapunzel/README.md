# Rapunzel

## 📝 Problem Description

You are given a set of $N$ hair ties, indexed from $0$ to $N-1$. Each hair tie $i$ has an associated non-negative brightness value, $h_i$. These hair ties are connected by strands of hair in a specific hierarchical structure. Hair tie $0$ is the topmost one. For any two connected hair ties $u$ and $v$, $u$ is always closer to the top than $v$. The connections are arranged such that from hair tie $0$, there is a unique sequence of connections leading to any other hair tie.

A "rope" is defined as a sequence of hair ties $s_0, s_1, \dots, s_{\ell-1}$ where for each $j$ from $1$ to $\ell-1$, a strand of hair connects $s_{j-1}$ to $s_j$. The problem asks you to identify all "climbable" ropes. A rope is considered climbable if it meets two criteria:
1.  It must have a specific length of exactly $m$ hair ties.
2.  It must be "safe," meaning the difference between the maximum and minimum brightness values of all hair ties in the rope (the "contrast") is no more than a given value $k$.

Your task is to find every hair tie that can serve as the starting point of at least one climbable rope. The final output should be a single line containing the indices of these starting hair ties, sorted in increasing order. If no such hair tie exists, you should output 'Abort mission'.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem describes connections between hair ties where one is always "higher" than the other. It's also stated that a unique sequence of connections exists from the topmost tie (0) to all others. What kind of data structure does this arrangement of connections and elements represent? Modeling the problem using the correct structure is a crucial first step.
</details>
<details>
<summary>Hint #2</summary>
The problem asks you to find all "ropes" (which are essentially paths) of a fixed length $m$ that satisfy a condition on the brightness values of their elements. How can you systematically explore all possible downward paths in the structure you identified in Hint #1? Consider a traversal algorithm that starts from the top.
</details>
<details>
<summary>Hint #3</summary>
A simple brute-force approach that re-evaluates every possible path of length $m$ will likely be too slow. As you traverse the structure, you can maintain a "window" of the last $m$ elements visited. The main challenge is to efficiently find the minimum and maximum brightness within this moving window. A linear scan of the window at every step is inefficient. What data structures are specifically designed to maintain a collection of elements in sorted order, allowing for quick retrieval of minimum/maximum values and efficient insertions/deletions?
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1, 2)</summary>

### Analysis of the Problem Structure

The problem describes a collection of hair ties (nodes) and strands of hair (edges). Several key properties allow us to model this as a rooted tree:
- **Root Node:** Hair tie 0 is explicitly "the topmost one," making it the natural root of our structure.
- **Directed Edges:** A strand from $u$ to $v$ implies $u$ is "closer to Rapunzel's head," which gives directionality to the connections, always pointing away from the root.
- **Unique Paths:** The statement, "For each hair tie $u$, there is precisely one rope starting at 0 and ending at $u$," confirms that the structure is a tree, as there is a unique path from the root (0) to every other node.

The task is to find all paths (ropes) of length exactly $m$ such that the contrast condition is met:
$$ \max\{h_i \mid i \in \text{path}\} - \min\{h_i \mid i \in \text{path}\} \le k $$
We are asked to output the starting nodes of all such valid paths.

### A Simpler Case: The Linked List

For the first two test sets, a crucial simplifying assumption is given: "for every hair tie, there is at most one rope starting from it." In our tree model, this means every node has at most one child. Since every node is reachable from the root, the entire tree degenerates into a single path, effectively forming a **linked list**.

### Sliding Window Algorithm

On this linked list structure, the problem reduces to finding all contiguous sub-sequences of length $m$ that satisfy the contrast constraint. This is a classic application for the **sliding window** technique.

We can maintain a window of size $m$ and slide it down the list one node at a time. For each window, we check if the contrast condition holds.

1.  **Initialization:** Create a window containing the first $m$ nodes of the list.
2.  **Iteration:**
    a. Check the contrast of the current window. If it's valid ($\le k$), record the starting node of the window as a valid solution.
    b. Slide the window one step forward: add the next node in the list to the end of the window and remove the first node.
3.  **Termination:** Repeat until the end of the list is reached.

To efficiently track the minimum and maximum brightness in the window, we can use pointers or references to the min/max nodes. When the window slides:
- The newly added node might become the new minimum or maximum.
- If the node that slides *out* of the window was the current minimum or maximum, we must re-scan the *entire* new window to find the new minimum/maximum.

While this recalculation can be slow in the general case, it is sufficient for the constraints of the first test sets.

```cpp
#include <iostream>
#include <vector>
#include <deque>

struct Node {
  int idx;
  int brightness;
  std::vector<Node*> children;
};

void solve() {
  // std::cout << "================================" << std::endl;
  // ===== READ INPUT =====
  int n, m, k; std::cin >> n >> m >> k;
  
  std::vector<Node> nodes(n);
  for(int i = 0; i < n; ++i) { 
    std::cin >> nodes[i].brightness; 
    nodes[i].idx = i;
  }
  for(int i = 0; i < n - 1; ++i) {
    int u, v; std::cin >> u >> v;
    nodes[u].children.push_back(&nodes[v]);
  }
  
  // ===== SOLVE =====

  std::vector<bool> result(n, false);
  std::deque<Node*> curr_range; curr_range.push_back(&nodes[0]);
  
  Node* curr_min   = &nodes[0];
  Node* curr_max   = &nodes[0];
  
  // Initialize first window
  for(int i = 0; i < m - 1; ++i) {
    if(curr_range.back()->children.empty()) { std::cout << "Abort mission (no initial window)" << std::endl; return; }
    curr_range.push_back(curr_range.back()->children[0]);
    
    if(curr_range.back()->brightness <= curr_min->brightness) { curr_min = curr_range.back(); }
    if(curr_range.back()->brightness >= curr_max->brightness) { curr_max = curr_range.back(); }
  }
  
  // Check if initial window is valid
  if(curr_max->brightness - curr_min->brightness <= k) { result[curr_range.front()->idx] = true; }
  
  while(!curr_range.back()->children.empty()) {
    // std::cout << "Current Range: "; for(const Node *node : curr_range) { std::cout << node->idx << " "; } std::cout << std::endl;
    // std::cout << "Current Max (" << curr_max->idx << "): " << curr_max->brightness << " Current Min (" << curr_min->idx << "): " << curr_min->brightness << std::endl;
    
    // Move curr_end forward
    curr_range.push_back(curr_range.back()->children[0]);
    
    if(curr_range.back()->brightness <= curr_min->brightness) { curr_min = curr_range.back(); }
    if(curr_range.back()->brightness >= curr_max->brightness) { curr_max = curr_range.back(); }
    
    // Move curr_start forward
    bool replace_min = curr_min == curr_range.front();
    bool replace_max = curr_max == curr_range.front();
    
    curr_range.pop_front();
    
    if(replace_min || replace_max) {
      if (replace_min) { curr_min = curr_range.front(); }
      if (replace_max) { curr_max = curr_range.front(); }
      
      for(Node *node : curr_range) {
        if(replace_min && node->brightness <= curr_min->brightness) { curr_min = node; }
        if(replace_max && node->brightness >= curr_max->brightness) { curr_max = node; }
      }
    }
  
    // Check if current window is valid
    if(curr_max->brightness - curr_min->brightness <= k) { result[curr_range.front()->idx] = true; }
  }
  
  // ===== OUTPUT =====
  int n_outputs = 0;
  
  for(int i = 0; i < n; ++i) {
    if(result[i]) { 
      std::cout << i << " "; 
      n_outputs++;
    }
  }
  
  if(n_outputs == 0) { std::cout << "Abort mission"; }
  std::cout << std::endl;
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

### Generalizing the Approach

The first solution is tailored to the simplified case where the tree is a linked list. To create a solution for the general case, we must address two main challenges:
1.  **Handling the Tree Structure:** We need a way to traverse all possible downward paths in a general tree, not just a single list.
2.  **Efficient Min/Max Finding:** Re-scanning the window to find the minimum and maximum brightness at each step is too slow. For a tree, a path of length $m$ could be checked many times, and this inefficiency becomes a major bottleneck. Its complexity would be roughly $O(N \cdot m)$, which is too slow for the full constraints.

### The Algorithm: DFS with an Efficient Sliding Window

A **Depth-First Search (DFS)** is a natural choice for exploring all paths in a tree starting from the root. As we traverse down the tree, we can maintain a "sliding window" of the last $m$ nodes visited on the current path from the root.

To solve the efficiency problem, we use a data structure that can maintain a collection of numbers and provide the minimum and maximum in logarithmic time. A `std::multiset` in C++ is perfect for this. It keeps elements sorted and allows for efficient insertion, deletion, and retrieval of the min (`*begin()`) and max (`*rbegin()`) elements. We use a `multiset` instead of a `set` to correctly handle duplicate brightness values.

Our DFS-based algorithm works as follows:

1.  **State representation:** The DFS function will maintain the current path window in a `std::deque` and the brightness values of the nodes in that window in a `std::multiset`.

2.  **Traversal and Window Management:**
    - When the DFS visits a new node `curr`, we add `curr` to the back of our `deque` and its brightness to the `multiset`.
    - If the `deque`'s size exceeds `m`, it means our window has become too long. We remove the oldest node (from the front of the `deque`) and its corresponding brightness from the `multiset`. This keeps the window size fixed at $m$.

3.  **Condition Check:**
    - If the `deque`'s size is exactly `m`, we have a valid candidate rope. We check its contrast by subtracting the first element of the `multiset` from the last.
    - If `*brightnesses.rbegin() - *brightnesses.begin() <= k`, the rope is climbable. We mark its starting node (`path.front()`) as a valid solution.

4.  **Recursion and Backtracking:**
    - The DFS proceeds by recursively calling itself on all children of the current node.
    - After the recursive calls for all children return, we must **backtrack**. This involves undoing the changes made when we first entered the current node, restoring the state for the parent's traversal path. We remove the current node from the back of the `deque` and its brightness from the `multiset`.

The provided code uses an interesting, albeit unconventional, backtracking strategy. Instead of just removing the current node, it also tries to restore the window by adding the parent of the new window-head. A more standard approach is to manage the state restoration entirely within the recursive calls, but the provided logic correctly explores all paths.

By combining DFS with an efficient data structure like `multiset`, we can check every possible rope of length $m$ in the tree with an overall time complexity of approximately $O(N \log m)$, which is efficient enough for all test sets.

```cpp
#include <iostream>
#include <vector>
#include <set>
#include <deque>

struct Node {
  int idx;
  int brightness;
  Node* parent = nullptr;
  std::vector<Node*> children;
};

int calculateContrast(const std::multiset<int> &brightnesses) {
  // Note: multisets are sorted, so first *rbegin() is max and *begin() is min
  return *brightnesses.rbegin() - *brightnesses.begin();
}

void dfs(Node *curr, std::deque<Node*> &path, std::multiset<int> &brightnesses, std::vector<bool> &result, const size_t m, const int k) {
  // Add current node to path and to the brightnesses
  path.push_back(curr);
  brightnesses.insert(curr->brightness);
  
  // Check if the path is too long and remove the first element if it is too long
  // As we pop as soon as the path reaches m+1, it is not necessary to pop multiple times
  if(path.size() > m) {
    Node *first_node = path[0];
    path.pop_front();
    brightnesses.erase(brightnesses.find(first_node->brightness));
  }

  // Check if the current path is valid
  if(path.size() == m) {
    int contrast = calculateContrast(brightnesses);
    
    // Check if contrast is low enough
    if (contrast <= k) {
      // Mark the current start node as valid
      result[path[0]->idx] = true;
    }
  }

  // Recurse into children
  for(Node *child : curr->children) {
    dfs(child, path, brightnesses, result, m, k);
  }

  // Finished with this node -> Remove it from path and its brightness
  path.pop_back();
  brightnesses.erase(brightnesses.find(curr->brightness));


  // Restore the first element of the path, we removed because the path was too long
  // If the first node in the path is the root node, there is no need to do this as the path has not gotten to long
  if(path[0]->parent != nullptr) {
    Node *first_node = path[0]->parent;
    path.push_front(first_node);
    brightnesses.insert(first_node->brightness);
  }
}

void solve() {
  // ===== READ INPUT =====
  int n, m, k; std::cin >> n >> m >> k;
  
  std::vector<Node> nodes(n);
  for(int i = 0; i < n; ++i) { 
    std::cin >> nodes[i].brightness; 
    nodes[i].idx = i;
  }
  for(int i = 0; i < n - 1; ++i) {
    int u, v; std::cin >> u >> v;
    nodes[u].children.push_back(&nodes[v]);
    nodes[v].parent = &nodes[u];
  }
  
  // ===== SOLVE =====
  std::deque<Node*> path;
  std::multiset<int> brightnesses;
  std::vector<bool> result(n, false);

  dfs(&nodes[0], path, brightnesses, result, m, k);
  
  // ===== OUTPUT =====
  int n_outputs = 0;
  
  for(int i = 0; i < n; ++i) {
    if(result[i]) { 
      std::cout << i << " "; 
      n_outputs++;
    }
  }
  
  if(n_outputs == 0) { std::cout << "Abort mission"; }
  std::cout << std::endl;
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
   Test set 1 (20 pts / 4 s) : Correct answer      (0.013s)
   Test set 2 (20 pts / 4 s) : Correct answer      (1.598s)
   Test set 3 (20 pts / 4 s) : Correct answer      (0.004s)
   Test set 4 (40 pts / 4 s) : Correct answer      (2.218s)

Total score: 100
```