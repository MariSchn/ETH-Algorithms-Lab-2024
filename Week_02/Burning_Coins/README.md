# Burning Coins

## 📝 Problem Description

You are given a row of $n$ coins, each with a specific value $v_i$. You and an opponent play a game by taking turns. On each turn, the current player can take either the leftmost or the rightmost coin from the remaining row. This process continues until no coins are left.

Your task is to determine the maximum total value of coins you can guarantee to collect. You are the first player and you aim to maximize your score. The output should be this guaranteed maximum value for your collection.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem asks for the amount you are *guaranteed* to win. This means you must account for an opponent who is also playing optimally. Your optimal strategy should maximize your score, assuming your opponent will always make a move that maximizes their own score (which, from your perspective, might seem like they are trying to minimize your potential gain). This is a characteristic of game theory problems and often suggests a recursive approach.

</details>

<details>

<summary>Hint #2</summary>

This problem exhibits optimal substructure. The state of the game at any point can be uniquely described by the subarray of coins that are still available. Let's say coins from index $s$ to $e$ are left. Can you define a function, say `f(s, e)`, that computes the best possible outcome for the player whose turn it is for this specific subproblem?

</details>

<details>

<summary>Hint #3</summary>

Let's define `f(s, e)` as the maximum value the **current player** can obtain from the coins in the subarray from index $s$ to $e$. If it's your turn with coins `[s...e]`, you have two choices:

1.  **Take coin `v_s`**: You get `v_s`. The remaining coins are `[s+1...e]`. Now it's your opponent's turn. They will play optimally on this subproblem. What does this mean for you? After your opponent makes their move (taking either `v_{s+1}` or `v_e}`), you will be left with either the subproblem `[s+2...e]` or `[s+1...e-1]`. Since the opponent is smart, they will leave you with the subproblem that gives you a smaller future gain.
2.  **Take coin `v_e`**: Similarly, you get `v_e`. The remaining coins are `[s...e-1]`. Your opponent will make a move to leave you with the worse of the two possible resulting subproblems.

Your recurrence relation must capture this minimax nature. You want to `max`imize your choice, while your opponent's move forces you to take the `min`imum of the subsequent outcomes.

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

Based on the problem description we need to realize two key aspects:

- The problem can be nicely formulated through a **recursion**. This is because once we and our opponent take a coin, we are left with essentially the same problem but with fewer coins. I.e. we are left with the same problem just with 2 fewer coins.

- We need to assume that **our opponent is also playing optimally**. This is because we are interested in finding the maximum amount we are **guaranteed** to win, so among all possible outcomes, we are only concerned with the worst-case scenario where our opponent plays optimally to minimize our winnings.

These two observations (especially the recursion) lead us to a **dynamic programming** solution, as it allows us to efficiently compute the maximum winnings for each subproblem without recalculating them multiple times.

### Recursion

Lets define a function $f(s, e)$ that calculates the maximum value we can win when it is our turn and the coins $\{v_s, \dots, v_e\}$ are left. 

#### Base Case

The base case for this recursion is simply when there are either 1 or 2 coins left. In this case, we will only have one more turn to take a coin, so we can simply return the maximum value of the coin(s) left:

$$f(s, e) = \max(v_s, v_e)$$

#### Recursive Case

The recursive case is more complex, as the maximum value we can win now **not only depends on the leftmost and rightmost coins themselves**, but also the remaining coins after we take one of them. Regardless, we can realize that we only have two choices:

- Take the leftmost coin $v_s$. Afterwards the coins $\{v_{s+1}, \dots, v_e\}$ are left.
- Take the rightmost coin $v_e$. Afterwards the coins $\{v_s, \dots, v_{e-1}\}$ are left.

Since we are interested in the maximum winning we would always want to take the option that will yield us the most value (also considering the rest of the coins left). This means that we can express the maximum winnings we can get as follows:

$$f(s, e) = \max( \quad \underbrace{v_s + f(s+1, e)}_{\text{Take Left}}, \quad \underbrace{v_e + f(s, e-1)}_{\text{Take Right}}\quad)$$

However, now we have the problem that in both $f(s+1, e)$ and $f(s, e-1)$ its our **opponent's turn**. Since $f(s, e)$ is the maximum winnings we can get when it is **our turn**, we need to go one step further and consider that our opponent has already played and it is again our turn, essentially skipping two turns ahead.

Assuming we took the leftmost coin $v_s$/$f(s+1, e)$, our opponent now has two choices aswell:

- Take the leftmost coin $v_{s+1}$, leaving us with the coins $\{v_{s+2}, \dots, v_e\}$.
- Take the rightmost coin $v_e$, leaving us with the coins $\{v_{s+1}, \dots, v_{e-1}\}$.

However, since our opponent is also playing optimally, they will choose the option that **minimizes our winnings**. This means that we can express the maximum winnings we can get after our opponent has played as follows:

$$f(s+1, e) = \min( \quad \underbrace{f(s+2, e)}_{\text{Opponent takes Left}}, \quad \underbrace{f(s+1, e-1)}_{\text{Opponent takes Right}}\quad)$$

Note that we do not have any $v_i$ in this case, as this is our opponent's turn so we are not taking any coins, just calculating the maximum winnings we can get after our opponent has played.

Combining these two observations, we can express the maximum winnings we can get when it is our turn as follows:

$$
\begin{align*}
f(s, e) = \max( \quad &\min( \quad v_{s\phantom{+1}} + f(s+2, e\phantom{-2}), \quad v_{s\phantom{+1}} + f(s+1, e-1) \quad ), \\
                \quad &\min( \quad v_{s+1}           + f(s\phantom{+2}, e-2), \quad v_{e-1}          + f(s+1, e-1) \quad ) \quad )
\end{align*}
$$

### Memoization

Writing this out as a recursive function would lead to a lot of repeated calculations, as we would be calculating the same subproblems multiple times. To avoid this, we can use **memoization** to store the results of already computed subproblems. This means that in our code we will use a 2D vector `memo` to store the results of the subproblems, where `memo[s][e]` will store the value of $f(s, e)$.

If we encounter a subproblem that has already been computed, we can simply return the stored value from `memo[s][e]` instead of recalculating it.

### Code
```cpp
#include<iostream>
#include<vector>
#include<limits>


int calcMaxWinnings(const std::vector<int>& values, std::vector<std::vector<int>>& memo, int start, int end) {
  // ===== BASE CASES =====
  // Only one coin left -> No choice but to take it
  if (end - start < 2) return std::max(values[start], values[end]);
  // Already computed this subproblem -> Return the stored result from the memo
  if (memo[start][end] != -1) return memo[start][end];
  
  // ===== RECURSIVE CASE =====
  // Calculate the maximum winnings obtainable by taking either the left or right coin (values[start] or values[end])
  // and the maximum winnings obtainable from the remaining coins (values[start + 1] to values[end - 1] or values[start + 2] to values[end])
  int max_winnings_left = values[start] + std::min(calcMaxWinnings(values, memo, start + 2, end    ),
                                                   calcMaxWinnings(values, memo, start + 1, end - 1));
  int max_winnings_right = values[end] +  std::min(calcMaxWinnings(values, memo, start + 1, end - 1),
                                                   calcMaxWinnings(values, memo, start    , end - 2));

  memo[start][end] = std::max(max_winnings_left, max_winnings_right);

  return memo[start][end];
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    
    std::vector<int> values;
    for(int i = 0; i < n; i++) {
      int v; std::cin >> v;
      values.push_back(v);
    }
    
    // ===== SOLVE =====
    std::vector<std::vector<int>> memo(n, std::vector<int>(n, -1));
    std::cout << calcMaxWinnings(values, memo, 0, n - 1) << std::endl;
    
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Testing solution >>>>
   Test set 1 (30 pts / 2 s) : Correct answer      (0.0s)
   Test set 2 (40 pts / 2 s) : Correct answer      (0.021s)
   Test set 3 (30 pts / 2 s) : Correct answer      (0.633s)

Total score: 100
```