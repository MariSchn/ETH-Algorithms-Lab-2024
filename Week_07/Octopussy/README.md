# Octopussy

## 📝 Problem Description

You are given $N$ bombs, indexed from $0$ to $N-1$. Each bomb $i$ has an associated timer $t_i$, indicating that it must be deactivated at or before the end of minute $t_i$. It takes exactly one minute to deactivate a single bomb.

The bombs have a dependency structure. For any bomb $j$, it can only be deactivated if the bombs at indices $2j+1$ and $2j+2$ (if they exist within the index range) have already been deactivated. Bombs with indices $j$ such that $2j+1 \ge N$ have no dependencies and are considered to be on the ground.

The task is to determine if a valid deactivation sequence for all $N$ bombs exists. A sequence is valid if every bomb is deactivated before its timer runs out and all dependency constraints are respected. If such a sequence is possible, the output is 'yes'; otherwise, the output is 'no'.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The dependency rule, where item $j$ depends on items $2j+1$ and $2j+2$, defines a specific, structured relationship between the bombs. What kind of hierarchical data structure does this arrangement resemble? Consider how this structure constrains the order in which you can deactivate the bombs.
</details>
<details>
<summary>Hint #2</summary>
The dependencies form a binary tree, where bomb $j$ is the parent of bombs $2j+1$ and $2j+2$. The rule that a parent can only be defused after its children implies that any valid deactivation sequence must process subtrees in a post-order traversal fashion. The main question is, in what order should we select bombs or subtrees to process? A greedy approach might be effective. Which bomb is the most "urgent" to deal with at any given time?
</details>
<details>
<summary>Hint #3</summary>
A common and powerful greedy strategy for scheduling problems is to prioritize the task with the earliest deadline. In this problem, this translates to focusing on the bomb with the smallest explosion timer $t_i$. However, we can't simply deactivate this bomb; we must first handle all of its dependencies. This suggests an algorithm: identify the bomb `b` with the earliest deadline among all those not yet handled. Then, perform the necessary deactivations for `b`'s entire dependency chain (i.e., its subtree) before finally deactivating `b`. If this can be done without any bomb exploding, repeat the process for the bomb with the next-earliest deadline.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>

### 1. Problem Modeling

First, let's analyze the dependency structure. A bomb at index $j$ depends on bombs at indices $2j+1$ and $2j+2$. This is the standard indexing scheme for a **binary tree** stored in an array, where bomb $j$ is the parent node of its children, $2j+1$ and $2j+2$. The condition that a bomb can only be defused after the ones it "stands on" means that a parent node can only be processed after all nodes in its subtree have been processed. This is a classic **post-order traversal** constraint.

The core challenge is to find a single, valid permutation of deactivations (a specific post-order traversal of the entire tree) that satisfies every bomb's individual deadline.

### 2. A Greedy Strategy

When faced with multiple tasks and deadlines, a strong heuristic is to prioritize the most constrained task. In our case, the most constrained bomb is the one with the **earliest explosion time**. This forms the basis of our greedy strategy: we should always try to defuse the bomb that is closest to exploding.

This leads to the following algorithm:
1.  Create a list of all bombs.
2.  Sort this list primarily by explosion time $t_i$ in ascending order. This gives us a priority list of which bombs to *target*.
3.  To handle cases where multiple bombs have the same deadline, we use a secondary sorting criterion as a tie-breaker. The provided solution sorts by the bomb's index $i$ in descending order. This ensures a deterministic processing order.
4.  Iterate through this sorted list of bombs. For each bomb $b$ in the list:
    *   If $b$ has already been defused (as a dependency of a previously targeted bomb), we can skip it.
    *   Otherwise, we must initiate the process to defuse $b$. This requires us to first defuse its entire subtree in a post-order manner.

### 3. Implementation with a Stack

A stack is a natural fit for performing an iterative post-order traversal. The process for defusing a target bomb `b` and its dependencies is as follows:

1.  Initialize a stack and push the target bomb `b` onto it.
2.  Maintain a global `elapsed_time` counter, initialized to 0.
3.  While the stack is not empty, inspect the bomb `c` at the top of the stack:
    *   Check if `c` has children that have not yet been defused. If so, push these undisposed children onto the stack. This ensures they are processed before `c`.
    *   If all of `c`'s children have been defused (or if `c` is a leaf), it's now safe to defuse `c`.
        *   Increment `elapsed_time`.
        *   Check if the deadline is met: `elapsed_time <= t_c`. If this condition fails, it means bomb `c` would have exploded. No solution is possible, so we can immediately terminate and report 'no'.
        *   If the deadline is met, mark `c` as defused and pop it from the stack.

If we successfully process all bombs from our initial sorted list without any deadline violations, it means a valid deactivation sequence exists, and the answer is 'yes'.

### 4. Code

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <tuple>

// Define a pair to store a bomb's time and original index
using BombInfo = std::pair<int, int>;

// Helper function to find the children of a bomb
std::pair<int, int> get_children(int ball_idx, int n_balls) {
  // Bombs in the latter half of the array are leaves (on the ground)
  if (ball_idx >= (n_balls - 1) / 2) {
    return {-1, -1}; // No children
  } else {
    return {2 * ball_idx + 1, 2 * ball_idx + 2};
  }
}

void solve() {
  // ===== READ INPUT =====
  int n_balls;
  std::cin >> n_balls;
  
  std::vector<int> explosion_times(n_balls);
  std::vector<BombInfo> sorted_bombs(n_balls);
  
  for (int i = 0; i < n_balls; i++) {
    int t;
    std::cin >> t;
    explosion_times[i] = t;
    sorted_bombs[i] = {t, i}; // Pair the time with the index
  }
  
  // ===== SOLVE =====
  
  // Sort bombs by explosion time (ascending) and then by index (descending) as a tie-breaker.
  std::sort(sorted_bombs.begin(), sorted_bombs.end(), [](const BombInfo &a, const BombInfo &b) {
    if (a.first != b.first) {
      return a.first < b.first;
    } else {
      return a.second > b.second;
    }
  });
  
  std::vector<bool> diffused(n_balls, false);
  int elapsed_time = 0;
  
  // Iterate through the greedily sorted bombs
  for (int i = 0; i < n_balls; ++i) {
    int target_bomb_idx = sorted_bombs[i].second;
    
    // If this bomb was already defused as part of another subtree, skip it.
    if (diffused[target_bomb_idx]) {
      continue;
    }
    
    // Use a stack to perform post-order traversal for the target bomb's subtree.
    std::stack<int> to_diffuse_stack;
    to_diffuse_stack.push(target_bomb_idx);
    
    while (!to_diffuse_stack.empty()) {
      int current_bomb_idx = to_diffuse_stack.top();
      
      int child1, child2;
      std::tie(child1, child2) = get_children(current_bomb_idx, n_balls);
      
      bool children_done = true;
      if (child1 != -1 && !diffused[child1]) {
        to_diffuse_stack.push(child1);
        children_done = false;
      }
      if (child2 != -1 && !diffused[child2]) {
        to_diffuse_stack.push(child2);
        children_done = false;
      }
      
      // If children are not done, the loop will continue to process them first.
      // If children are done, we can defuse the current bomb.
      if (children_done) {
        // Now we can defuse the current bomb
        elapsed_time++;
        
        // Check if we missed the deadline
        if (explosion_times[current_bomb_idx] < elapsed_time) {
          std::cout << "no" << std::endl;
          return;
        }
        
        diffused[current_bomb_idx] = true;
        to_diffuse_stack.pop();
      }
    }
  }
  
  std::cout << "yes" << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  
  int n_tests;
  std::cin >> n_tests;
  while (n_tests--) {
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
   Test set 1        (20 pts / 1 s) : Correct answer      (0.0s)
   Test set 2        (20 pts / 1 s) : Correct answer      (0.008s)
   Test set 3        (20 pts / 1 s) : Correct answer      (0.017s)
   Test set 4        (20 pts / 1 s) : Correct answer      (0.153s)
   Hidden test set 1 (05 pts / 1 s) : Correct answer      (0.0s)
   Hidden test set 2 (05 pts / 1 s) : Correct answer      (0.008s)
   Hidden test set 3 (05 pts / 1 s) : Correct answer      (0.017s)
   Hidden test set 4 (05 pts / 1 s) : Correct answer      (0.153s)

Total score: 100
```