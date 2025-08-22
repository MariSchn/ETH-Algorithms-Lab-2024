# The Great Game

## 📝 Problem Description

This problem describes a turn-based game played by two opponents, Sherlock and Moriarty, on a board with $N$ positions labeled from 1 to $N$. The board has a set of one-way transitions, where each transition connects a position with a lower label to one with a higher label. The position labeled $N$ is the target.

There are two meeples on the board: one red and one black. The game starts with the red meeple at position $r$ and the black meeple at position $b$. The players take turns moving one of the meeples along a single transition. The turns are made as follows:

- If Sherlock has played an **even** amount of turns so far, he moves the red meeple.
- If Sherlock has played an **odd** amount of turns so far, he moves the black meeple.
- If Moriarty has played an **even** amount of turns so far, he moves the black meeple.
- If Moriarty has played an **odd** amount of turns so far, he moves the red meeple.

The game concludes as soon as one meeple reaches the target position $N$. If the red meeple arrives at $N$, Sherlock wins. If the black meeple arrives at $N$, Moriarty wins. We are guaranteed that the target is always reachable, so the game never ends in a draw. 

## 💡 Hints

<details>

<summary>Hint #1</summary>

The game ends as soon as a single meeple reaches the target. The position of the red meeple does not restrict the movement of the black meeple, and vice-versa. This suggests that you can analyze the journey of each meeple to the target as a separate, independent subproblem.

</details>

<details>

<summary>Hint #2</summary>

Consider the subproblem for a single meeple, for instance, the red one. Sherlock wants this meeple to reach the target in the fewest possible moves. Moriarty, when it's his turn to move the red meeple, will try to do the opposite: he will choose a move that makes the path to the target as long as possible. Consider this when calculating how long it takes for a meeple to reach the target

</details>

<details>

<summary>Hint #3</summary>

The structure of the board (all transitions go from a lower to a higher position label) means there are no cycles. This structure is ideal for dynamic programming or memoization. You can determine the outcome from any position by working backward from the target position $N$.

Try to define a recursive function, say `get_moves(position, ...)`, that calculates the number of moves to reach the target from any given `position`. 
Note that some more args are necessary to actually calculate this.

</details>

<details>

<summary>Hint #4</summary>

After you calculate the optimal number of moves for the red meeple to win ($R_{\text{moves}}$) and the black meeple to win ($B_{\text{moves}}$), you need to compare them. If $R_{\text{moves}} < B_{\text{moves}}$, Sherlock clearly wins. If $B_{\text{moves}} < R_{\text{moves}}$, Moriarty wins.

What if $R_{\text{moves}} = B_{\text{moves}} = k$? In this case, the winner is determined by who can complete their $k$-move sequence first. Carefully analyze the turn order to see whose meeple is moved on the final game turn deciding who wins.

</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>

This problem can be modeled as a game on a Directed Acyclic Graph (DAG), where positions are vertices and transitions are edges. For each meeple, we need to find the number of moves it will take to reach the target, assuming both players play optimally.
It is important to realize here that the meeples do not interact with each other directly. This allows us to analyze their paths to the target position $N$ independently, which makes this much more efficient.

### Minimax Approach

Let's focus on one meeple, for example, the red one. Sherlock "owns" this meeple and wants it to reach the target in the minimum number of moves. Moriarty, when he gets to move the red meeple, wants to maximize the number of moves to delay Sherlock's victory. This is a classic minimax problem.

We can define the state by `(pos, minimize)`, where `minimize` indicates if the current player for this meeple is trying to *minimize* or *maximize* the moves to the goal.

*   A **minimizing** player, from a position `p`, will choose a transition `p -> q` that leads to the minimum possible total moves to get to the goal as quickly as possible.
*   A **maximizing** player, from `p`, will choose a transition `p -> q` that leads to the maximum possible total moves to delay the opponent's victory.

### Dynamic Programming with Memoization

Since the board is a DAG, we can solve this with recursion and memoization. For this we define a function `recursion` which returns the number of moves to reach the goal $N$ from `pos`, given the current player for this meeple is a minimizer or a maximizer.

*   **Base Case:** If `pos == N`, the meeple has reached the target. The number of moves from here is 0.`.

*   **Recursive Step:**
    *   If `minimize` is true: The player wants to find the best next step. This means he wants to make the move that minimizes the number of moves to the goal. I.e. The number of moves will be `1 + min(recursion(... next_pos, false))` for all available transitions `pos -> next_pos`. The player takes one move and puts the opponent (a maximizer) in the best possible situation for the minimizer.
    *   If `minimize` is false: The player wants to find the "worst" next step for the owner. This means he wants to make the move that maximizes the number of moves to the goal. The number of moves will be `1 + max(recursion(... next_pos, true))` for all available transitions `pos -> next_pos`.

### Determining Winner

To determine the winner, we can simply call the recursion function for both meeples, to determine the number of moves required for each to reach the goal, under the assumption that both players play optimally.

```cpp
int r_n_steps = recursion(transitions, r_memo, n, r, true);
int b_n_steps = recursion(transitions, b_memo, n, b, true);
```

Note that we set `minimize` to `true` for both calls as the first turn made for the red meeple is made by Sherlock (minimizing for red meeple) and the first move for the black meeple is made by Moriarty (minimizing for black meeple).

Once we have $R_{\text{moves}}$ and $B_{\text{moves}}$, we can determine the winner:
*   If $R_{\text{moves}} < B_{\text{moves}}$: Sherlock has a faster path to victory. He can win in $R_{\text{moves}}$ turns with the red meeple, and Moriarty cannot win with the black meeple in time to stop him. **Sherlock wins.**
*   If $B_{\text{moves}} < R_{\text{moves}}$: Moriarty has a faster path and will win with the black meeple. **Moriarty wins.**
*   If $R_{\text{moves}} = B_{\text{moves}} = k$: Both meeples require the same number of moves. The winner is whoever can complete their $k$ moves in fewer *game turns*. This depends mainly on which meeple Sherlock moved last, as Sherlock always turns before Moriarty:
    * If $k$ is **odd**, Sherlock has moved the red meeple last (as the number of previous turns is even). I.e. the red meeple moves first and Sherlock wins.
    * If $k$ is **even**, Sherlock has moved the black meeple last (as the number of previous turns is odd). I.e. the black meeple moves first and Moriarty wins.

Combining these conditions, Sherlock wins if `(r_n_steps < b_n_steps)` or `(r_n_steps == b_n_steps && r_n_steps % 2 != 0)`. Otherwise, Moriarty wins.

---

### Code

```cpp
#include<iostream>
#include<vector>
#include<cmath>
#include<limits>

using Memo = std::vector<std::vector<int>>;
using Transitions = std::vector<std::vector<int>>;


int recursion(const Transitions& transitions, Memo& memo, int n, int pos, bool minimize) {
  if(pos == n) return 0; // Base Case
  if(memo[pos][minimize] != -1) return memo[pos][minimize]; // Memo
    
  // Recursion
  if(minimize) {
    // Minimize
    int min_steps = std::numeric_limits<int>::max();
    for(int next_pos : transitions[pos]) {
      min_steps = std::min(min_steps, 1 + recursion(transitions, memo, n, next_pos, false));
    }
    memo[pos][minimize] = min_steps;
  } else {
    // Maximize
    int max_steps = std::numeric_limits<int>::min();
    for(int next_pos : transitions[pos]) {
      max_steps = std::max(max_steps, 1 + recursion(transitions, memo, n, next_pos, true));
    }
    memo[pos][minimize] = max_steps;
  }
  
  return memo[pos][minimize];
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    int n, m, r, b; std::cin >> n >> m >> r >> b;
  
    Transitions transitions(n, std::vector<int>());
    for(int i = 0; i < m; i++) {
      int u, v; std::cin >> u >> v;
      transitions[u].push_back(v);
    }
    
    Memo r_memo(n, std::vector<int>(2, -1));
    Memo b_memo(n, std::vector<int>(2, -1));
  
    int r_n_steps = recursion(transitions, r_memo, n, r, true); r_memo.clear();
    int b_n_steps = recursion(transitions, b_memo, n, b, true); b_memo.clear();
    
    std::cout << (((r_n_steps < b_n_steps) || (r_n_steps == b_n_steps && r_n_steps % 2 != 0)) ? '0' : '1') << std::endl;
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (25 pts / 3.500 s) : Correct answer      (0.0s)
   Test set 2 (25 pts / 3.500 s) : Correct answer      (0.002s)
   Test set 3 (25 pts / 3.500 s) : Correct answer      (0.049s)
   Test set 4 (25 pts / 3.500 s) : Correct answer      (3.116s)

Total score: 100
```