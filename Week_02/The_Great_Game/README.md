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

<summary>First Solution (Test Set 1, 2)</summary>

This first solution attempts to solve the problem by directly simulating the game using a **recursive approach with memoization**. The state of the game is captured by the tuple `(r, b, player, turn)`, representing the positions of the red and black meeples, the current player, and the turn number.

The core of the solution is the `recursion` function, which determines the winner from a given game state. The logic follows a minimax structure: it checks if the current `player` can make a move that forces a win. A player is considered to have a winning move if they can transition to a new state from which the *other* player is guaranteed to lose. The function explores all legal moves for the current player. If any of these moves leads to a state where the opponent loses, the current player is marked as the winner for the original state. If all moves lead to states where the opponent wins, the current player loses.

However, this approach has a critical flaw: the state space is too large. The `turn` number can grow, making the memoization table effectively unbounded and thus impractical. The state should ideally not include the turn number. Because of this, the solution is not efficient enough and fails on larger test cases where the number of turns can be high.

### Code
```cpp
#include<iostream>
#include<vector>
#include<unordered_map>

enum Player {
  HOLMES,
  MORIARTY
};

// [r][b][player][turn]
using MemoVector = std::vector<std::vector<std::vector<std::vector<int>>>>;
using Transitions = std::unordered_map<int, std::vector<int>>;

int recursion(Transitions& transitions, MemoVector& memo, int n, int r, int b, Player player, int turn) {
  // Base cases
  if(r == n) return HOLMES;
  else if(b == n) return MORIARTY;
  
  // Use already calculated value if available
  if(memo[r][b][player][turn] != -1) {
    return memo[r][b][player][turn];
  }
  
  // Recursion
  if(player == HOLMES && turn % 2 == 0) {
    // Holmes moves Black
    memo[r][b][player][turn] = MORIARTY;
    
    for(int x : transitions[b]) {
      if(recursion(transitions, memo, n, r, x, MORIARTY, turn) == HOLMES) {
        memo[r][b][player][turn] = HOLMES;
        break;
      }
    }
  }
  else if(player == HOLMES && turn % 2 == 1) {
    // Holmes moves Red
    memo[r][b][player][turn] = MORIARTY;
    
    for(int x : transitions[r]) {
      if(recursion(transitions, memo, n, x, b, MORIARTY, turn) == HOLMES) {
        memo[r][b][player][turn] = HOLMES;
        break;
      }
    }
  }
  else if(player == MORIARTY && turn % 2 == 0) {
    // Moriarty moves Red
    memo[r][b][player][turn] = HOLMES;
    
    for(int x : transitions[r]) {
      if(recursion(transitions, memo, n, x, b, HOLMES, turn + 1) == MORIARTY) {
        memo[r][b][player][turn] = MORIARTY;
        break;
      }
    }
  }
  else if(player == MORIARTY && turn % 2 == 1) {
    // Moriarty moves Black
    memo[r][b][player][turn] = HOLMES;
    
    for(int x : transitions[b]) {
      if(recursion(transitions, memo, n, r, x, HOLMES, turn + 1) == MORIARTY) {
        memo[r][b][player][turn] = MORIARTY;
        break;
      }
    }
  }
  
  return memo[r][b][player][turn];
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    int n, m, r, b; std::cin >> n >> m >> r >> b;
    
    MemoVector memo(n, std::vector<std::vector<std::vector<int>>>(
                           n, std::vector<std::vector<int>>(
                                  2, std::vector<int>(n, -1))));
    std::unordered_map<int, std::vector<int>> transitions;
    for(int i = 0; i < m; i++) {
      int u, v; std::cin >> u >> v;
      transitions[u].push_back(v);
    }

    int winner = recursion(transitions, memo, n, r, b, HOLMES, 1);
    std::cout << winner << std::endl;
  }
}
```

</details>

<details>

<summary>Second Solution (Test Set 1, 2, 3) </summary>

This second solution builds upon the first by refining the state representation. It correctly identifies that the specific turn number is not necessary; only its **parity** matters for determining which player moves which meeple.

The state is now `(r, b, player, turn % 2)`, effectively reducing the fourth dimension of the state space from a potentially large number to just two values (0 or 1). This significantly shrinks the number of states that need to be memoized.

To handle the potentially sparse state space more efficiently, this solution also switches from a 4D vector to an `unordered_map` for memoization, using a custom hash for the tuple key.

This optimization makes the solution much more efficient, allowing it to pass more test cases. However, the state representation still considers the positions of both meeples simultaneously. This results in a state space that is quadratic in the number of board positions ($O(N^2)$), which is still too large for the given constraints and causes the solution to time out on the final test set.

### Code
```cpp
#include<iostream>
#include<vector>
#include<unordered_map>

enum Player {
  HOLMES,
  MORIARTY
};

struct tuple_hash {
    std::size_t operator()(const std::tuple<int, int, int, int>& key) const {
        int a = std::get<0>(key);
        int b = std::get<1>(key);
        int c = std::get<2>(key);
        int d = std::get<3>(key);
        
        std::size_t hash = std::hash<int>()(a);
        hash ^= std::hash<int>()(b) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<int>()(c) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<int>()(d) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};
struct tuple_equal {
    bool operator()(const std::tuple<int, int, int, int>& lhs, const std::tuple<int, int, int, int>& rhs) const {
        return lhs == rhs;
    }
};

// [r][b][player][turn]
// using MemoVector = std::vector<std::vector<std::vector<std::vector<MemoPrimitive>>>>;
using MemoPrimitive = int;
using MemoKey = std::tuple<MemoPrimitive, MemoPrimitive, MemoPrimitive, MemoPrimitive>;
using MemoMap = std::unordered_map<std::tuple<int, int, int, int>, int, tuple_hash>;
using Transitions = std::unordered_map<int, std::vector<int>>;

int recursion(const Transitions& transitions, MemoMap& memo, int n, int r, int b, Player player, int turn) {
  // Base cases
  if(r == n) return HOLMES;
  else if(b == n) return MORIARTY;
  
  std::tuple<int, int, int, int> input_tuple = std::make_tuple(r, b, player, turn);
  
  // Use already calculated value if available
  if(memo.find(input_tuple) != memo.end()) {
    return memo[input_tuple];
  }
  
  // Recursion
  if(player == HOLMES && turn % 2 == 0) {
    // Holmes moves Black
    memo[input_tuple] = MORIARTY;
    
    for(int x : transitions.at(b)) {
      if(recursion(transitions, memo, n, r, x, MORIARTY, turn) == HOLMES) {
        memo[input_tuple] = HOLMES;
        break;
      }
    }
  }
  else if(player == HOLMES && turn % 2 == 1) {
    // Holmes moves Red
    memo[input_tuple] = MORIARTY;
    
    for(int x : transitions.at(r)) {
      if(recursion(transitions, memo, n, x, b, MORIARTY, turn) == HOLMES) {
        memo[input_tuple] = HOLMES;
        break;
      }
    }
  }
  else if(player == MORIARTY && turn % 2 == 0) {
    // Moriarty moves Red
    memo[input_tuple] = HOLMES;
    
    for(int x : transitions.at(r)) {
      if(recursion(transitions, memo, n, x, b, HOLMES, (turn + 1) % 2) == MORIARTY) {
        memo[input_tuple] = MORIARTY;
        break;
      }
    }
  }
  else if(player == MORIARTY && turn % 2 == 1) {
    // Moriarty moves Black
    memo[input_tuple] = HOLMES;
    
    for(int x : transitions.at(b)) {
      if(recursion(transitions, memo, n, r, x, HOLMES, (turn + 1) % 2) == MORIARTY) {
        memo[input_tuple] = MORIARTY;
        break;
      }
    }
  }
  
  return memo[input_tuple];
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    int n, m, r, b; std::cin >> n >> m >> r >> b;
  
    std::unordered_map<int, std::vector<int>> transitions;
    for(int i = 0; i < m; i++) {
      int u, v; std::cin >> u >> v;
      transitions[u].push_back(v);
    }

    MemoMap memo;
    
    int winner = recursion(transitions, memo, n, r, b, HOLMES, 1);
    std::cout << winner << std::endl;
  }
}
```

</details>

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