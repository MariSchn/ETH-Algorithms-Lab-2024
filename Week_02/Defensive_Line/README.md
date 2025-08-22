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

<summary>First Solution (Test Set 1, 2)</summary>

This initial attempt uses a **brute-force recursive** approach to find the optimal assignment. The `calcMaxValue` function explores all possible ways to place `m` attackers on disjoint segments that sum to `k`.

The function works by using a sliding window (`start`, `end`) to find the first available valid segment. Once a valid segment is found, it makes a recursive call to solve the subproblem for the remaining defenders (everything after the found segment) with one less attacker. It then continues to slide the window to find other possible first segments to see if they yield a better total result.

The main issue with this solution is its inefficiency. It does not use memoization, meaning it repeatedly solves the same subproblems. For example, the optimal way to assign `m-1` attackers to a specific suffix of the defender line is calculated multiple times. This leads to a very high time complexity, which is why it only passes the smallest test cases.

### Code

```cpp
#include<iostream>
#include<vector>
#include<cmath>

int calcMaxValue(std::vector<int>& d_sums, int n, int k, int start, int end, int r) {
  if(r == 0) return 0;
  if(end >= n) return -1;  // "fail"
  
  int max_value = -1;
  
  while(end != n) {
    int sum = d_sums[end] - d_sums[start - 1];

    if(sum == k) {
      int remainder_value = calcMaxValue(d_sums, n, k, end+1, end+1, r-1);
      
      if(remainder_value != -1) {
        max_value = std::max(max_value, end - (start - 1) + remainder_value);
        // break because if we are not able to fit r - 1 ranges in to the current interval,
        // we will not be able to fit them into an even smaller interval
        // break;
      }
    }
    
    // Advance sliding window
    if (sum < k || start == end) end++;
    else start++;
  }
  
  return max_value;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    int n, m, k; std::cin >> n >> m >> k;
    
    int sum = 0;
    std::vector<int> d_sums = {0};
    for(int i = 0; i < n; i++) {
      int v; std::cin >> v;
      sum += v;
      d_sums.push_back(sum);
    }

    int max_value = calcMaxValue(d_sums, n + 1, k, 1, 1, m);
    
    if(max_value == -1) std::cout << "fail" << std::endl;
    else std::cout << max_value << std::endl;
  }
}
```

</details>

<details>

<summary>Second Solution (Test Set 1, 2, 4)</summary>

This initial solution employs a **top-down dynamic programming** approach with **memoization**. The core of this method is a recursive function, `calcMaxValue(start, r)`, which determines the maximum number of defenders that can be engaged using `r` attackers, starting from the defender at index `start`.

The state for the dynamic programming is defined by `(start, r)`, which corresponds to the subproblem for the suffix of defenders beginning at `start` with `r` attackers still available. To prevent redundant computations for identical subproblems, a memoization table, `memo[start][r]`, is utilized.

Within the recursive function, a **sliding window** technique is used to identify all valid segments that begin at or after the `start` index. For each segment that meets the criteria (i.e., a contiguous group of defenders whose values sum to `k`), the function recursively calls itself to solve the subproblem for the remaining defenders and attackers: `calcMaxValue(end_of_segment + 1, r - 1)`.

The function then selects the maximum value derived from all potential valid segments at the current stage. If no valid assignment is possible, it returns a "fail" indicator (-1). The recursion's base cases are when no attackers are left (`r = 0`) or when there are no more defenders to process.

This strategy systematically examines all valid combinations of disjoint segments and, with the aid of memoization, operates with sufficient efficiency to pass the initial test sets.

The primary drawback of this approach, and what distinguishes it from the more optimized final solution, is its computational efficiency. By integrating the sliding window search for valid segments inside the recursive calls, the same sub-arrays are scanned multiple times. This leads to a higher time complexity, making it too slow for larger test cases.

### Code

```cpp
#include<iostream>
#include<vector>
#include<cmath>

// memo[start][r] = max_value
using Memo = std::vector<std::vector<int>>;

int calcMaxValue(std::vector<int>& d_sums, Memo& memo, int n, int k, int start, int r) {
  if(r == 0) return 0;
  if(start >= n || (start == n - 1 && d_sums[start] - d_sums[start - 1] < k)) return -1; // fail
  
  if(memo[start][r] != -2) return memo[start][r];

  // Setup sliding window
  int i = start;
  int j = start;
  
  int max_value = -1;
  
  while(i < n && j < n) {
    int sum = d_sums[j] - d_sums[i-1];
    int value = j - i + 1;
    
    if(sum > k) {
      i++;
    } else if(sum == k) {
      int remainder_value = calcMaxValue(d_sums, memo, n, k, j + 1, r- 1);
      
      // If we aren't able to find r-1 intervals in [j+1, n], we won't be able to find them in an even smaller interval.
      // So we can break out of the loop
      if(remainder_value == -1) {
        break;
      } else {
        max_value = std::max(max_value, value + remainder_value);
      }
      
      j++;
    } else { // sum < k
      j++;
    }
  }
  
  memo[start][r] = max_value;
  return max_value;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    int n, m, k; std::cin >> n >> m >> k;
    
    int sum = 0;
    std::vector<int> d_sums = {0};
    for(int i = 0; i < n; i++) {
      int v; std::cin >> v;
      sum += v;
      d_sums.push_back(sum);
    }
    
    Memo memo(n + 2, std::vector<int>(m+1, -2));

    int res = calcMaxValue(d_sums, memo, n+1, k, 1, m);
    if(res == -1) std::cout << "fail" << std::endl;
    else std::cout << res << std::endl;
  }
}
```

</details>

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

Judging solution >>>>
   Test set 1 (20 pts / 0.500 s) : Correct answer      (0.0s)
   Test set 2 (20 pts / 0.500 s) : Correct answer      (0.006s)
   Test set 3 (20 pts / 0.500 s) : Correct answer      (0.07s)
   Test set 4 (20 pts / 2.000 s) : Correct answer      (0.007s)
   Test set 5 (20 pts / 0.500 s) : Correct answer      (0.08s)

Total score: 100
```