# Rapunzel

## 📝 Problem Description

A set of $N$ hair ties, indexed from $0$ to $N-1$, is given. Each hair tie $i$ has a non-negative brightness value $h_i$. The hair ties are connected by strands of hair in a hierarchical arrangement: hair tie $0$ is the topmost, and for any two connected hair ties $u$ and $v$, $u$ is always closer to the top than $v$. The structure ensures that from hair tie $0$, a unique sequence of connections leads to every other hair tie.

A "rope" refers to a sequence of hair ties $s_0, s_1, \dots, s_{\ell-1}$ such that for each $j$ from $1$ to $\ell-1$, a strand connects $s_{j-1}$ to $s_j$. The objective is to identify all ropes that are "climbable," which requires meeting two conditions:
1. The rope must consist of exactly $m$ hair ties.
2. The difference between the maximum and minimum brightness values among the hair ties in the rope (the "contrast") must not exceed a specified value $k$.

The task is to determine all hair ties that can serve as the starting point of at least one climbable rope. The output should be a single line listing the indices of these starting hair ties in increasing order. If no such hair tie exists, output 'Abort mission'.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem describes connections between hair ties where one is always "higher" than the other, with hair tie 0 at the top and a unique sequence of connections from 0 to every other hair tie. This structure is a **tree** with 0 as the root. Recognizing this tree structure is crucial for understanding how to traverse and find all possible paths.

</details>

<details>

<summary>Hint #2</summary>

To find all "ropes" (paths) of exactly length $m$, you need to systematically explore all possible downward paths in the tree. Use a **Depth-First Search (DFS)** that starts from the root and explores each complete path before backtracking. As you traverse, maintain the current path and check if it forms a valid rope when it reaches the required length.

</details>

<details>

<summary>Hint #3</summary>

During your DFS traversal, you need to efficiently track the minimum and maximum brightness values in the current path window of size $m$. Instead of rescanning the entire window each time, use a **`multiset`** to maintain the brightness values in sorted order. This allows you to find the minimum (`*multiset.begin()`) and maximum (`*multiset.rbegin()`) in $O(\log n)$ time, with $O(\log n)$ insertions and deletions as you slide the window.

</details>

<details>

<summary>Hint #4</summary>

Perform a DFS starting from the root, maintaining a `deque` for the current path and a `multiset` for brightness values. At each node, add it to both structures. If the path exceeds length $m$, remove the oldest element from both. When the path has exactly $m$ elements, check if the contrast (max - min brightness from the multiset) is ≤ $k$, and if so, mark the first node in the path as a valid starting point. After exploring all children, backtrack by removing the current node and restoring any previously removed elements to maintain the sliding window correctly.

</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Set 1, 2)</summary>

The problem describes a collection of hair ties (nodes) and strands of hair (edges). Several key properties allow us to model this as a tree:
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

### Code
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

To generalize the First Solution to the remaining Test Sets we need to achieve 2 things

- Find a **faster way to find the minimum and maximum** in the current window.
- **Generalize** from Linked Lists to Trees

### Using a `multiset`

**Finding a faster way to find the minimum and maximum** is straight forward, as we simply need a **more efficient data structure** than two pointers. For this we will use the fact that **STL Sets are sorted**. Therefore we can simply organize the nodes of the current path in a `multiset` which allows for $\log n$ access to the Min (`*set.begin()`) and Max (`*set.rbegin()`). Additionally it allows for $\log n$ delete and insert, which is necessary, as we are frequently manipulating the set. <br />
**Note**: Using `multiset` and not `set` is necessary, as we can have duplicate brightness values in our set.

A `multiset` is simply exactly what we need as it (under the hood) is implemented as a (sorted) Red Black Tree which allows for essentially everything in $\log n$ time.

### Generalizing to Trees

To generalize from Linked Lists to Trees, we will need to **traverse the Tree one path at a time**. This sparks the idea that we can simply perform a **DFS on the Tree**. The DFS works as follows:

1. Add the current node to the **current** `path` and **add its brightness to the current** `brightnesses`
2. Check if that made the current  `path` **too long** (larger than the path length $m$ we are looking for)
    
    If Yes → Remove the first node from the `path` and `brightnesses` as the path is too long
    
3. Check if the current path is **long enough** (the path length $m$ we are looking for)
    
    If Yes → Check if the contrast (difference between Max and Min Brightness) is low enough ($\leq k$) and mark the first node in the path as a valid start node <br />
    **Note**: This can be done efficiently as described above
    
4. Recurse into all children
5. As now all children are processed, we need to exit the path. Therefore, remove the current node from both the `path` and the `brightnesses`
6. Add the previous node (removed during step 2) back at the front of the `path` <br />
**Note**: This of course only needs to be done, if it is not the root node, as otherwise there is nothing to add

After performing this DFS we have found all the valid start positions for a path and can simply output these

### Code
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

## 🧠 Learnings

<details> 

<summary> Expand to View </summary>

- Don't necessarily just think about what type of data structure you need, but instead think about what operations you need to do and what time complexity you need for them. Then check which data structure does this.

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