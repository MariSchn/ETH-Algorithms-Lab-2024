# San Francisco

## 📝 Problem Description

You are given a game played on a board with $n$ holes and $m$ directed canals. Each canal connects two holes, carries a non-negative point value, and can only be traversed in its specified direction. The game begins at a designated starting hole, numbered 0. A "move" consists of traversing a single canal, which takes one turn and adds the canal's points to your total score. If you reach a hole with no outgoing canals, you are instantly teleported back to the starting hole (hole 0). This teleportation is a "free action," costing no moves and yielding no points.

Your task is to determine the minimum number of moves required to achieve a total score of at least $x$, given that you cannot exceed a maximum of $k$ moves. For a given set of $n, m, x, k$ and canal descriptions, you must output this minimum number of moves. If it's impossible to reach the target score within $k$ moves, you should report that.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem asks for the *minimum* number of moves. A straightforward approach could be to find the maximum score achievable in exactly 1 move, then in exactly 2 moves, and so on, up to $k$ moves. How can we keep track of the game's state, which seems to depend on our current location and the number of moves made?
</details>
<details>
<summary>Hint #2</summary>
We need to find the maximum score for any given number of moves. Let's try to formalize this. If we define $S(i, j)$ as the maximum score we can get by starting at hole $j$ with $i$ moves remaining, can we establish a relationship between $S(i, j)$ and scores achievable with $i-1$ moves? This structure strongly suggests a dynamic programming approach.
</details>
<details>
<summary>Hint #3</summary>
The "free action" from a hole with no outgoing canals (a Weayaya hole) is a critical special case in the recurrence. If you are at a Weayaya hole $u$ with $i$ moves left, the rules state you instantly return to the start (hole 0). This implies that the maximum score you can get from hole $u$ with $i$ moves is exactly the same as the maximum score you can get from hole 0 with $i$ moves. How does this affect your DP state transition?
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>

### Initial Thoughts and Approach

At first glance, this problem might seem solvable with graph algorithms like maximum flow, as the board can be modeled as a graph. However, this approach presents difficulties. The input size for $n$ is larger than what is typically feasible for standard max-flow algorithms. Furthermore, modeling the ability to reuse canals and the "free action" teleportation is non-trivial with flows.

A more direct approach is to focus on the core question: for a given number of moves $i \le k$, what is the maximum score we can achieve? If we can answer this for all $i$ from $0$ to $k$, we can easily find the minimum number of moves to reach the target score $x$. This leads us to a **Dynamic Programming** solution.

### Dynamic Programming Formulation

We will build a solution from the ground up, calculating the maximum score for an increasing number of moves. Let's define our DP state as follows:

$dp[i][j]$ = The maximum score achievable by starting at hole $j$ and making exactly $i$ moves.

Our goal is to compute this table for $i$ from $0$ to $k$ and $j$ from $0$ to $n-1$.

**Base Case:**
With 0 moves ($i=0$), no canals can be traversed, so no points can be scored, regardless of the starting hole.
$dp[0][j] = 0$ for all $j \in [0, n-1]$.

**State Transition (Recurrence):**
To calculate $dp[i][j]$ for $i > 0$, we consider all possible first moves from hole $j$. If we take a canal from $j$ to a neighboring hole $v$ that gives $p$ points, we use one move. We are now at hole $v$ with $i-1$ moves remaining. The maximum score we can obtain from this new state is, by our definition, $dp[i-1][v]$. The total score for this sequence of plays is $p + dp[i-1][v]$. To maximize our score from hole $j$, we should choose the outgoing canal that leads to the best outcome.

Thus, the recurrence relation is:
$dp[i][j] = \max_{(j, v, p) \in \text{Canals}} \{ p + dp[i-1][v] \}$

**Handling the "Weayaya" Holes:**
The rules specify a special case for holes with no outgoing canals (Weayaya holes). From such a hole, we take a "free action" back to the start (hole 0). This action doesn't count as a move. This means the potential to score from a Weayaya hole $j$ with $i$ moves is identical to the potential to score from hole 0 with $i$ moves.

Therefore, if hole $j$ is a Weayaya hole:
$dp[i][j] = dp[i][0]$

### Implementation and Final Answer

We can implement this with a 2D array, `dp[k+1][n]`, filled in a bottom-up manner. We iterate `i` from 1 to $k$, and for each `i`, we compute `dp[i][j]` for all holes `j$. The loop structure ensures that when we compute `dp[i][...]`, all values for `dp[i-1][...]` are already available. The special case for Weayaya holes is also handled correctly by the loop order `j = 0, 1, ..., n-1`, as `dp[i][0]` is computed before it is needed by any other Weayaya hole `j > 0`.

After the DP table is fully populated, the value `dp[i][0]` gives the maximum score starting from the beginning with $i$ moves. To find our answer, we iterate `i` from $0$ to $k$. The first value of `i` for which $dp[i][0] \ge x$ is the minimum number of moves required. If no such `i` is found after checking up to $k$ moves, the target score is impossible to achieve within the given move limit.

Scores can be very large ($x \le 10^{14}$), so it's essential to use 64-bit integers (`long long` in C++) for the DP table and the target score $x$.

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

void solve() {
  long n, m;
  long long x;
  long k;
  std::cin >> n >> m >> x >> k;
  
  // adj[u] stores pairs {v, p} for canals from u to v with p points.
  std::vector<std::vector<std::pair<int, int>>> adj(n);
  for(int i = 0; i < m; ++i) {
    int u, v, p;
    std::cin >> u >> v >> p;
    adj[u].emplace_back(v, p);
  }
  
  // dp[i][j]: max score starting from hole j with exactly i moves.
  std::vector<std::vector<long long>> dp(k + 1, std::vector<long long>(n, 0));
  
  // Fill DP table bottom-up, for 1 to k moves.
  for(int i = 1; i <= k; ++i) {
    for(int j = 0; j < n; ++j) {
      if(adj[j].empty()) {
        // Weayaya hole: free action to return to start. Score is same as starting from hole 0.
        // Since we loop j from 0 to n-1, dp[i][0] is computed before this line is reached for any j > 0.
        dp[i][j] = dp[i][0];
      } else {
        // dp[i][j] is initialized to 0. We find the max over all outgoing canals.
        for(const auto& edge : adj[j]) {
          int v = edge.first;
          int p = edge.second;
          dp[i][j] = std::max(dp[i][j], (long long)p + dp[i-1][v]);
        }
      }
    }
  }
  
  // Find the minimum number of moves 'i' to achieve score x from the start.
  for(int i = 0; i <= k; ++i) {
    if(dp[i][0] >= x) {
      std::cout << i << std::endl;
      return;
    }
  }
  
  std::cout << "Impossible" << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  
  int t;
  std::cin >> t;
  while(t--) {
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
   Test set 1 (30 pts / 4 s) : Correct answer      (0.003s)
   Test set 2 (30 pts / 4 s) : Correct answer      (0.057s)
   Test set 3 (40 pts / 4 s) : Correct answer      (0.79s)

Total score: 100
```