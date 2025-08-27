# James Bond's Sovereigns

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

A key challenge is handling the two adversarial aspects. First, whenever it is your turn, you must assume that in the intervening $m-1$ turns, the other players will collaborate to leave you with the worst possible options for your *next* turn. Second, for the very first $k$ turns of the game (before you, passenger $p_k$, get to play), the players $p_0$ through $p_{k-1}$ will make moves to leave you in the worst possible starting position. Your solution must account for both of these worst-case scenarios, which involves minimizing over the opponents' choices.

</details>

<details>

<summary>Hint #4</summary>

You can simplify the problem by focusing solely on your own turns and the possible outcomes for those turns. Essentially, you can skip over the opponents' turns and only consider the resulting states that could occur due to their adversarial moves. This reduces the complexity of the problem and allows you to focus on maximizing your guaranteed outcome.

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

This problem describes a game where we want to find the best possible outcome in a worst-case scenario. We want to maximize our score, while all other players act as adversaries trying to minimize it. This is a classic **minimax** problem, which can be solved efficiently using **dynamic programming (DP)** with memoization.

### Overall Strategy

Our strategy is divided into two main parts:
1.  **Handling the Initial Moves:** Before we (passenger $p_k$) get to make our first move, players $p_0, \dots, p_{k-1}$ will have taken a total of $k$ coins. Since they are adversaries, they will choose these $k$ coins in a way that leaves us in the worst possible starting position. They can take $i$ coins from the left and $k-i$ coins from the right, for any $i \in \{0, \dots, k\}$. We must find our guaranteed score for each of these possible starting boards and take the **minimum** among them.

2.  **Solving the Subgame:** For a given board state (a subarray of coins), we need a function that calculates the maximum score we can guarantee if it's our turn to pick. Let's call this function `solve(start, end)` (In the solution it is simply referred to as `recursion(...)`).

### DP Formulation

Let `solve(start, end)` be a function that computes the maximum winnings a player can guarantee if it's their turn and the remaining coins are in the subarray from `values[start]` to `values[end]`.

**Recursive Step:**
When it's our turn to choose from the subarray `[start, end]`, we have two options:

1.  **Take the leftmost coin (`values[start]`):** Our immediate gain is `values[start]`. The remaining subarray is `[start+1, end]`. Before our next turn, the other $m-1$ players will each take a coin. They will remove a total of $m-1$ coins from the board `[start+1, end]`. To minimize our future winnings, they will leave us with the worst possible subproblem. They can take $i$ coins from the left and $(m-1)-i$ from the right. Our guaranteed score from the rest of the game will be the minimum over all their choices:  $$\min_{0 \le i \leq m - 1} { \text{solve}(\text{start}+1+i, \text{end} - (m-1-i)) }$$
Thus, our total guaranteed score if we take `values[start]` is:
$$\text{values}[\text{start}] + \min_{0 \leq i \leq m - 1} { \text{solve}(\text{start}+1+i, \text{end} - (m-1-i)) }$$

2.  **Take the rightmost coin (`values[end]`):** Similarly, our gain is `values[end]`, and the remaining subarray is `[start, end-1]`. Our guaranteed score from the rest of the game is $$\min_{0 \leq i \leq m - 1} { \text{solve}(\text{start}+i, \text{end}-1 - (m-1-i)) }$$
Thus, our total guaranteed score if we take `values[end]` is:
$$\text{values}[\text{end}] + \min_{0 \leq i \leq m - 1} { \text{solve}(\text{start}+i, \text{end}-1 - (m-1-i)) }$$

Since we want to maximize our score, we will choose the better of these two options:
$$\text{solve}(\text{start}, \text{end}) = \max(\text{Option 1}, \text{Option 2})$$

**Base Case:**
The recursion terminates when there are not enough coins left for a full round of $m$ players. If `end - start + 1 < m`, it means that if we pick a coin now, we will not get another turn. Therefore, we should simply pick the more valuable of the two available coins:

$$
\text{solve}(\text{start}, \text{end}) = \max(\text{values}[\text{start}],\ \text{values}[\text{end}])
$$

**Memoization:**
This recursive approach involves many overlapping subproblems, especially, when iterating over all possible game states that can arise from the adversarial moves. We can use a 2D array, `memo[start][end]`, to store the result of `solve(start, end)` and avoid recomputing it.

### Implementation

```cpp
#include<iostream>
#include<vector>
#include<cmath>
#include<limits>

using Memo = std::vector<std::vector<int>>;

int recursion(const std::vector<int>& values, Memo& memo, int m, int start, int end) {
  // Memo
  if(memo[start][end] != -1) {
    return memo[start][end];
  }

  if(end - start < m) {  
    // Base Case
    memo[start][end] = std::max(values[start], values[end]);
  } else {
    // Recursion
    
    // Calculate max winnings you can get when choosing the start-coin or end-coin
    int start_min_winnings = std::numeric_limits<int>::max();
    int end_min_winnings = std::numeric_limits<int>::max();
    for(int i = 0; i < m; i++) {
      start_min_winnings = std::min(start_min_winnings, recursion(values, memo, m, start + i + 1, end - (m - 1 - i)));
      end_min_winnings   = std::min(end_min_winnings,   recursion(values, memo, m, start + i,     end - (m - 1 - i) - 1));
    }
    start_min_winnings += values[start];
    end_min_winnings += values[end];
    
    memo[start][end] = std::max(start_min_winnings, end_min_winnings);
  }
  
  return memo[start][end];
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    int n, m, k; std::cin >> n >> m >> k;

    std::vector<int> values;
    for(int i = 0; i < n; i++) {
      int v; std::cin >> v;
      values.push_back(v);
    }
    
    int start = 0;
    int end = n - 1;
    Memo memo(n, std::vector<int>(n, -1));
    
    int res = std::numeric_limits<int>::max();
    for(int i = 0; i <= k; i++) {
      res = std::min(res, recursion(values, memo, m, start + i, end - (k - i)));
    }
    
    std::cout << res << std::endl;
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1        (20 pts / 2 s) : Correct answer      (0.414s)
   Test set 2        (40 pts / 2 s) : Correct answer      (0.809s)
   Test set 3        (20 pts / 2 s) : Correct answer      (1.113s)
   Hidden test set 1 (05 pts / 2 s) : Correct answer      (0.414s)
   Hidden test set 2 (10 pts / 2 s) : Correct answer      (0.809s)
   Hidden test set 3 (05 pts / 2 s) : Correct answer      (1.097s)

Total score: 100
```