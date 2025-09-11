# San Francisco

## 📝 Problem Description

A game is played on a board with $n$ holes and $m$ directed canals. Each canal connects two holes, carries a non-negative point value, and can only be traversed in its specified direction. The game begins at a designated starting hole, numbered 0. A move consists of traversing a single canal, which takes one turn and adds the canal's points to the total score. Upon reaching a hole with no outgoing canals, instant teleportation back to the starting hole (hole 0) occurs as a free action, costing no moves and yielding no points.

The objective is to determine the minimum number of moves required to achieve a total score of at least $x$, without exceeding a maximum of $k$ moves. For given values of $n, m, x, k$ and canal descriptions, the minimum number of moves must be reported. If the target score cannot be reached within $k$ moves, the answer should indicate impossibility.

## 💡 Hints

<details>

<summary>Hint #1</summary>

At first glance, the problem may resemble a flow problem, since it involves moving along directed connections and accumulating points. However, the constraints and the "teleportation" mechanic make traditional flow-based approaches unsuitable. Consider alternative methods for modeling the game's progression.

</details>

<details>

<summary>Hint #2</summary>

The problem asks for the *minimum* number of moves. A straightforward approach could be to find the maximum score achievable in exactly 1 move, then in exactly 2 moves, and so on, up to $k$ moves. How can we keep track of the game's state, which seems to depend on our current location and the number of moves made?

</details>

<details>

<summary>Hint #3</summary>

We need to find the maximum score for any given number of moves. Let's try to formalize this. If we define $S(i, j)$ as the maximum score we can get by starting at hole $j$ with $i$ moves remaining, can we establish a relationship between $S(i, j)$ and scores achievable with $i-1$ moves? This structure strongly suggests a dynamic programming approach.

</details>

<details>

<summary>Hint #4</summary>

The "free action" from a hole with no outgoing canals (a Weayaya hole) is a critical special case in the recurrence. If you are at a Weayaya hole $u$ with $i$ moves left, the rules state you instantly return to the start (hole 0). This implies that the maximum score you can get from hole $u$ with $i$ moves is exactly the same as the maximum score you can get from hole 0 with $i$ moves. How does this affect your DP state transition?

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

### First Intuition

As a first thought solving this problem using a (Min Cost) Max Flow might occur, as the problem can be modeled very nicely as a **Graph.**
However, many **problems** occur with this approach:

- The input $n \leq 10^3$ is larger than usual for Flow Problems (~$10^2$)
- Modeling the “ciruclar” aspect of the Game is non-trivial using Flows

### Technique

Ultimately we are interested in the following: What is the **minimum number of steps** $k' \leq k$ that is required to **achieve a score $s \geq x$** starting from the Hole **$0$**

While Flows are not really applicable here, we will use the fact that the **inputs are still relatively small**, as we have $10^3$ Holes and a Maximum of $4 \cdot 10^3$ possible turns. This makes it feasible to calculate the **maximum achievable score for every number of turns $k'$.**

For this we will use a **Bottom-Up Dynamic Programming Approach**, to calculate the maximum score $f(k', h)$, where $k'$ is the **number of turns** we can make and $h$ is the **hole where we start**. <br />
It might seem unintutive why we include the hole we start at $h$ in the DP if we are only interested in the maximum score for hole $0$. But in the design of the DP it is necessary, to model the individual subproblems for which its necessary to consider different starting holes.

### Defining the Recursion

#### Base Case

Naturally, the **Base Case** will be if $k' = 0$, which corresponds to no turns being left, so $f(0, h) = 0$

#### Recursive Case

The recursive case is defined as:

$$
f(k', u) = \begin{cases}
 \max\{ f(k'-1, v) + s_{(u, v)} \mid (u, v) \in E\} &\text{if $u$ has edges} \\
f(k', 0) &\text{else }
\end{cases}
$$

To unpack/understand this, lets start with the simpler $\text{else}$ case. Here the hole $u$ does not have any outgoing edges. Therefore the only possible action is to return to the starting hole $0$, which does not count as a move. Therefore the maximum achievable score $f(k', u)$ for node $u$ is the same as for the node $0$, $f(k', 0)$ with $k'$ turns left <br />
In the $\text{if}$ case the node $u$ has outgoing edges. What we are essentially doing there is that we consider the score we can get when taking one of these edges. This is first the score $s_{(u, v)}$ we immediately get when going from $u$ to $v$ and the remaining score we can get starting from the new node $v$ with one lesss turn $f(k' -1 ,v)$. By calculating this for every Edge, we can then simply take the maximum among these


For the final output we can simply go over all turns $0 \dots k$ and check at which turn $f(k', 0) \geq x$ and output that turn $k'$

### Code
```cpp
#include <iostream>
#include <vector>

void solve() {
  // ===== READ INPUT =====
  long n, m, x, k; std::cin >> n >> m >> x >> k;
  
  std::vector<std::vector<std::pair<int, int>>> edges(n);
  for(int i = 0; i < m; ++i) {
    int u, v, p; std::cin >> u >> v >> p;
    edges[u].emplace_back(v, p);
  }
  
  // ===== SOLVE =====
  std::vector<std::vector<long>> dp(k + 1, std::vector<long>(n, 0));  // Number of Turns left x Hole -> Max Achievable Score
  
  // Fill DP table bottom up
  for(int turn = 1; turn < k + 1; ++turn) {
    for(int hole = 0; hole < n; ++hole) {
      if(edges[hole].empty()) {
        // If the node has no edges, the "free action" of going back to the beginning is performed
        dp[turn][hole] = dp[turn][0];
      } else {
        // If the node has edges the maximum achievable score is the maximum of the score achievable from its children with one move less + the score of going to that child
        for(const std::pair<int, int> &edge : edges[hole]) {
          dp[turn][hole] = std::max(dp[turn][hole], dp[turn - 1][edge.first] + edge.second);
        }
      }
    }
  }
  
  // ===== OUTPUT =====
  // Search for first turn, where score x is reached
  for(int turn = 0; turn < k + 1; ++turn) {
    if(dp[turn][0] >= x) {
      std::cout << turn << std::endl;
      return;
    }
  }
  std::cout << "Impossible" << std::endl;
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

- Even if think you got the right technique/algorithm for the problem, be open to changing your mind and explore alternatives.

</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (30 pts / 4 s) : Correct answer      (0.003s)
   Test set 2 (30 pts / 4 s) : Correct answer      (0.057s)
   Test set 3 (40 pts / 4 s) : Correct answer      (0.79s)

Total score: 100
```