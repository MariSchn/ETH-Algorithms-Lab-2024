# Defensive Line

## 📝 Problem Description

You are given a sequence of $n$ defenders, each with a specific defense value $v_i$. There are $m$ attackers available, each with an identical attack strength of $k$.

Every attacker has to be assigned to a valid, non-empty, contiguous segment of defenders. A valid assignment requires that the sum of the defense values of the defenders in the segment is exactly equal to the attacker's strength $k$. Furthermore, each defender can be part of at most one such segment, meaning the chosen segments for all attackers must be disjoint.

Your task is to determine a strategy for assigning attackers to segments to maximize the total number of defenders being attacked. The value of a strategy is the sum of the lengths of all chosen segments. You should output this maximum possible value. If it's impossible to assign even a single attacker legally, the output should be "fail".

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem asks for a maximum value, which often points towards optimization techniques. The core decision is which segments to pick. Since the segments must be disjoint, a choice to include one segment might prevent you from choosing another overlapping one. This suggests breaking the problem into subproblems: think about optimal solutions for smaller prefixes of defenders with fewer attackers, and build up to the full solution. What choices do you have at each position, and how do previous decisions affect future possibilities?

</details>

<details>
    
<summary>Hint #2</summary>

This problem can be modeled as finding an optimal selection of items (the valid segments) under certain constraints (disjointness). This structure is a classic indicator for Dynamic Programming.

</details>

<details>
    
<summary>Hint #3</summary>

Before trying to select the best combination of segments, it would be extremely helpful to first identify all possible segments that are "valid" candidates. That is, all contiguous subsegments whose defense values sum to exactly $k$.

How can you find all such contiguous segments efficiently? A naive approach of checking every possible start and end point would be too slow ($O(n^2)$). Can you think of a more efficient method, perhaps one that processes the defenders in a single pass? 

</details>

<details>

<summary>Hint #4</summary>

To first identify all valid segments before maximizing the total number of defenders attacked, you can use a sliding window technique. This allows you to efficiently find all contiguous subsegments whose sum equals $k$ without recalculating sums for overlapping segments.

</details>

<details>

<summary>Hint #5</summary>

A natural way to break down the problem into subproblems is by considering prefixes of the defender line and a certain number of available attackers. What is the maximum number of defenders we can engage using $i$ attackers, considering only the first $j$ defenders?

Let $DP(i, j)$ represent this value. When considering the state $DP(i, j)$, you have two main choices regarding the $j$-th defender: either they are not part of the last segment in your solution, or they are. This line of reasoning will lead you to a recurrence relation.

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

This problem can be effectively solved using **dynamic programming**. The goal is to find a selection of up to $m$ disjoint, continuous subsegments of the defenders' values, where each segment sums to $k$, such that the total length of these segments is maximized.

The approach consists of two steps:

1.  **Sliding Window:** First, we identify **all valid segments** that sum up to $k$. To do this efficiently, we can use a **sliding window** technique that processes the defenders in a single pass.
2.  **Dynamic Programming:** Then, we use a DP formulation to find the optimal combination of these segments.

### Sliding Window

To avoid recomputing sums for all possible $O(n^2)$ subsegments, we can use a **sliding window** approach. We iterate through the defenders with an `end` pointer, expanding a window and adding the current defender's value to a running `sum`. If the `sum` exceeds $k$, we shrink the window from the left by moving a `start` pointer forward and subtracting the corresponding defender's value.

Whenever the `sum` equals exactly $k$, we have found a valid segment. We can store this information for our DP. A convenient way to do this is to use an array, let's call it `segments`. <br />
`segments[i]` will store the **length of a valid segment ending at index $i$**. If no such segment ends at $i$, we store 0.

Afterwards we know at which indices a valid segment ends, and how long it is. This information will be crucial for our dynamic programming solution.

---

### Dynamic Programming

Now, we build our DP solution bottom-up, based on the precomputed segment information. For this we create a 2D DP table $DP[i][j]$, where:

-   $i$ represents the number of attackers used (from 0 to $m$).
-   $j$ represents the number of defenders that are still left/we can still attack (from 0 to $n$). I.e. $j = 8$ means we consider the first 8 defenders (from index 0 to 7).

Let $DP[i][j]$ be the maximum number of defenders that can be attacked using exactly $i$ attackers, considering only the first $j$ defenders (from index 0 to $j-1$).

---

#### Base Case
-   $DP[0][j] = \phantom{-}0$ for all $j \in [0, n]$: With 0 attackers, we can attack 0 defenders.
-   $DP[i][0] = -1$ for all $i \in [1, m]$: With $i$ attackers but 0 defenders, a solution is impossible.

---

#### Recursive Case
To compute $DP[i][j]$, we need to decide what to do with the last defender in our current consideration (the defender at index $j-1$). We have two main choices:

**Choice 1: Skip the current position**
We don't use any segment that ends at position $j-1$. In this case, our optimal solution remains the same as if we only considered the first $j-1$ defenders with $i$ attackers:
$$DP[i][j] = DP[i][j-1]$$

**Choice 2: Use a segment ending at the current position**
We check if there's a valid segment that ends exactly at position $j-1$. From our preprocessing, `segments[j-1]` tells us the length of such a segment (or 0 if none exists).

If `len = segments[j-1] > 0`, then:
- We can assign one attacker to this segment of length `len`
- This segment covers defenders from position $(j-len)$ to $(j-1)$
- We now have $(i-1)$ attackers remaining to optimally assign to the first $(j-len)$ defenders. The $-len$ because we just attacked the segment of length $len$
- The total defenders attacked would be: $DP[i-1][j-len] + len$

**Making the optimal choice:**
We want to maximize the number of defenders attacked, so we take the better of these two options:

$$DP[i][j] = \max(\underbrace{DP[i][j-1]}_{\text{Skip}}, \underbrace{DP[i-1][j-len] + len}_{\text{Use Segment}})$$

**Important constraint:** We can only use Choice 2 if:
1. A valid segment exists at position $j-1$ (i.e., `len > 0`)
2. The previous state $DP[i-1][j-len]$ is valid (not equal to -1)
3. We have enough defenders to accommodate the segment (i.e., $j \geq len$)

If any of these conditions fail, we can only use Choice 1.

---

### Code
```cpp
#include<iostream>
#include<vector>

// Allocate a dynamic programming table for the maximum number of attackers (100) and the maximum number of defenders (100000).
int dp[101][100001];

void solve() {
  // ===== READ INPUT =====
  int n, m, k; std::cin >> n >> m >> k;
    
  std::vector<int> defenders(n);
  for(int i = 0; i < n; i++) {
    std::cin >> defenders[i];
  }
    
  // ===== SOLVE =====

  // ===== SLIDING WINDOW =====
  // For each index i find the length of a segment with sum k that ends at i using a sliding window approach.
  std::vector<int> segments(n);
  int start = 0;
  int sum = 0;
  
  for(int end = 0; end < n; ++end) {
    sum += defenders[end];
    
    // Move start until sum is no longer larger than k
    while(sum > k) {
      sum -= defenders[start];
      ++start;
    }
    
    // If segment sum is equal to k store it
    if(sum == k) {
      segments[end] = end - start + 1;
    }
  }
  
  // ===== DYNAMIC PROGRAMMING =====
  // Fill entries with 0 defenders left with 0
  for(int i = 0; i <= n; i++) {
    dp[0][i] = 0;
  }
  
  // Fill entries with defenders left but no more segments with -1 ("fail")
  for(int i = 1; i <= m; i++) {
    dp[i][0] = -1;
  }
  
  // Bottom-Up DP
  for(int i = 1; i <= m; i++) {
    for(int j = 1; j <= n; j++) {
      const int len = segments[j-1];
      
      if (len && dp[i-1][j-len] != -1) {
        // The current segment is valid, and with the remaining i-1 fighters and j-len segments we can form a valid solution
        // Take the max of choosing this segment for the fighter "dp[i-1][j-len] + len" and not choosing it "dp[i][j-1]"
        dp[i][j] = std::max(dp[i-1][j-len] + len, dp[i][j-1]);
      } else {
        // No valid segment found, move to next segment
        dp[i][j] = dp[i][j-1];  
      }
    }
  }
  
  // ===== OUTPUT =====
  if (dp[m][n] == -1) {
    std::cout << "fail" << std::endl;
  } else {
    std::cout << dp[m][n] << std::endl;
  }
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

Testing solution >>>>
   Test set 1 (20 pts / 0.500 s) : Correct answer      (0.0s)
   Test set 2 (20 pts / 0.500 s) : Correct answer      (0.006s)
   Test set 3 (20 pts / 0.500 s) : Correct answer      (0.074s)
   Test set 4 (20 pts / 2.000 s) : Correct answer      (0.007s)
   Test set 5 (20 pts / 0.500 s) : Correct answer      (0.085s)

Total score: 100
```