# James Bond's sovereigns

## 📝 Problem Description

You are given $n$ coins arranged in a line, each with a specific value. There are $m$ passengers, indexed $p_0, p_1, \dots, p_{m-1}$, who take turns picking a coin.

The selection process is as follows: The passengers take turns in a fixed cyclical order, starting with $p_0$, then $p_1$, and so on, up to $p_{m-1}$. After $p_{m-1}$'s turn, the cycle repeats with $p_0$. In each turn, the current passenger must choose to take either the leftmost or the rightmost coin from the line of remaining coins. This continues until all coins have been collected.

Your task is to calculate the largest possible total value of coins that a specific passenger, $p_k$, can guarantee collecting. This guarantee must hold true regardless of the choices made by the other $m-1$ passengers, who may not necessarily play to maximize their own winnings but could play to minimize yours.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem asks you to find a guaranteed outcome in a game of sequential choices. This suggests that you need to consider the best move at each step, while also accounting for the worst possible moves your opponents can make. Think about how the state of the game can be represented at any point. What information is essential to decide on a move?
</details>
<details>
<summary>Hint #2</summary>
Since you must guarantee a win against any of your opponents' strategies, this suggests a **minimax** approach. The problem has optimal substructure (the solution to a larger problem depends on solutions to smaller subproblems) and overlapping subproblems (the same subproblems are solved multiple times). This structure makes it a good candidate for **dynamic programming** or recursion with memoization.
</details>
<details>
<summary>Hint #3</summary>
A key challenge is handling the two adversarial aspects. First, whenever it is your turn, you must assume that in the intervening `m-1` turns, the other players will collaborate to leave you with the worst possible options for your *next* turn. Second, for the very first `k` turns of the game (before you, passenger `p_k`, get to play), the players `p_0` through `p_{k-1}` will make moves to leave you in the worst possible starting position. Your solution must account for both of these worst-case scenarios, which involves minimizing over the opponents' choices.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
This problem describes a game where we want to find the best possible outcome in a worst-case scenario. We want to maximize our score, while all other players act as adversaries trying to minimize it. This is a classic **minimax** problem, which can be solved efficiently using **dynamic programming (DP)** with memoization.

### Overall Strategy

Our strategy is divided into two main parts:
1.  **Handling the Initial Moves:** Before we (passenger $p_k$) get to make our first move, players $p_0, \dots, p_{k-1}$ will have taken a total of $k$ coins. Since they are adversaries, they will choose these $k$ coins in a way that leaves us in the worst possible starting position. They can take $i$ coins from the left and $k-i$ coins from the right, for any $i \in \{0, \dots, k\}$. We must find our guaranteed score for each of these possible starting boards and take the **minimum** among them.

2.  **Solving the Subgame:** For a given board state (a subarray of coins), we need a function that calculates the maximum score we can guarantee if it's our turn to pick. Let's call this function `solve(start, end)`.

### DP Formulation

Let `solve(start, end)` be a function that computes the maximum winnings a player can guarantee if it's their turn and the remaining coins are in the subarray from `values[start]` to `values[end]`.

**Recursive Step:**
When it's our turn to choose from the subarray `[start, end]`, we have two options:

1.  **Take the leftmost coin (`values[start]`):** Our immediate gain is `values[start]`. The remaining subarray is `[start+1, end]`. Before our next turn, the other $m-1$ players will each take a coin. They will remove a total of $m-1$ coins from the board `[start+1, end]`. To minimize our future winnings, they will leave us with the worst possible subproblem. They can take $i$ coins from the left and $(m-1)-i$ from the right. Our guaranteed score from the rest of the game will be the minimum over all their choices: `min_{0 \le i < m} { solve(start+1+i, end - (m-1-i)) }`.
    Thus, our total guaranteed score if we take `values[start]` is:
    `values[start] + min_{0 \le i < m} { solve(start+1+i, end - (m-1-i)) }`.

2.  **Take the rightmost coin (`values[end]`):** Similarly, our gain is `values[end]`, and the remaining subarray is `[start, end-1]`. Our guaranteed score from the rest of the game is `min_{0 \le i < m} { solve(start+i, end-1 - (m-1-i)) }`.
    Thus, our total guaranteed score if we take `values[end]` is:
    `values[end] + min_{0 \le i < m} { solve(start+i, end-1 - (m-1-i)) }`.

Since we want to maximize our score, we will choose the better of these two options:
`solve(start, end) = max(Option 1, Option 2)`.

**Base Case:**
The recursion terminates when there are not enough coins left for a full round of $m$ players. If `end - start + 1 < m`, it means that if we pick a coin now, we will not get another turn. Therefore, we should simply pick the more valuable of the two available coins.
`solve(start, end) = max(values[start], values[end])`.

**Memoization:**
This recursive approach involves many overlapping subproblems. We can use a 2D array, `memo[start][end]`, to store the result of `solve(start, end)` and avoid recomputing it.

### Implementation

The main function first determines all possible board states after the initial $k$ adversarial moves. For each state `[i, n-1 - (k-i)]`, it calls our DP function `solve(i, n-1 - (k-i))`. The final answer is the minimum of these results.

```cpp
#include <iostream>
#include <vector>
#include <algorithm> // For std::min and std::max
#include <limits>    // For std::numeric_limits

// Memoization table: memo[start][end] stores the result for the subarray [start, end]
using Memo = std::vector<std::vector<int>>;

/**
 * @brief Calculates max winnings for the current player on the subarray [start, end].
 * Assumes it's the player's turn to pick from this subarray.
 * @param values The values of all coins.
 * @param memo The memoization table.
 * @param m The number of players.
 * @param start The starting index of the current subarray.
 * @param end The ending index of the current subarray.
 * @return The maximum guaranteed winnings from this subgame.
 */
int solve(const std::vector<int>& values, Memo& memo, int m, int start, int end) {
  // If the result is already computed, return it.
  if (memo[start][end] != -1) {
    return memo[start][end];
  }

  // Base Case: If fewer than 'm' coins are left, we get only one more turn.
  // We should pick the best available coin.
  if (end - start + 1 < m) {
    return memo[start][end] = std::max(values[start], values[end]);
  }

  // Recursive Step: It's our turn. We choose to take 'start' or 'end'.
  // We assume the other m-1 players will then conspire to minimize our future winnings.

  // Case 1: We take the 'start' coin.
  // The opponents will leave us with the subproblem that yields the minimum score for us.
  int winnings_if_take_start = std::numeric_limits<int>::max();
  for (int i = 0; i < m; ++i) {
    // Opponents take 'i' from the left and 'm-1-i' from the right.
    winnings_if_take_start = std::min(winnings_if_take_start, solve(values, memo, m, start + 1 + i, end - (m - 1 - i)));
  }
  winnings_if_take_start += values[start];

  // Case 2: We take the 'end' coin.
  // The opponents will again leave us with the subproblem that minimizes our score.
  int winnings_if_take_end = std::numeric_limits<int>::max();
  for (int i = 0; i < m; ++i) {
    // Opponents take 'i' from the left and 'm-1-i' from the right.
    winnings_if_take_end = std::min(winnings_if_take_end, solve(values, memo, m, start + i, end - 1 - (m - 1 - i)));
  }
  winnings_if_take_end += values[end];

  // We choose the option that maximizes our guaranteed winnings.
  return memo[start][end] = std::max(winnings_if_take_start, winnings_if_take_end);
}

void testcase() {
  int n, m, k;
  std::cin >> n >> m >> k;

  std::vector<int> values(n);
  for (int i = 0; i < n; ++i) {
    std::cin >> values[i];
  }

  // memo[i][j] stores the result of solve(i, j)
  Memo memo(n, std::vector<int>(n, -1));

  int guaranteed_winnings = std::numeric_limits<int>::max();

  // Before our first turn (as player 'k'), players 0 to k-1 take 'k' coins.
  // They can take 'i' from the left and 'k-i' from the right.
  // They will choose 'i' to minimize our final score.
  for (int i = 0; i <= k; ++i) {
    int start_node = i;
    int end_node = n - 1 - (k - i);
    guaranteed_winnings = std::min(guaranteed_winnings, solve(values, memo, m, start_node, end_node));
  }

  std::cout << guaranteed_winnings << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  int t;
  std::cin >> t;
  while (t--) {
    testcase();
  }
  return 0;
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Testing solution >>>>
   Test set 1        (20 pts / 2 s) : Correct answer      (0.414s)
   Test set 2        (40 pts / 2 s) : Correct answer      (0.809s)
   Test set 3        (20 pts / 2 s) : Correct answer      (1.113s)
   Hidden test set 1 (05 pts / 2 s) : Correct answer      (0.414s)
   Hidden test set 2 (10 pts / 2 s) : Correct answer      (0.809s)
   Hidden test set 3 (05 pts / 2 s) : Correct answer      (1.097s)

Total score: 100
```