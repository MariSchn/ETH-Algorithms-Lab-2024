# The Lernaean Hydra

## 📝 Problem Description
The objective is to determine the minimum number of cuts needed to eradicate a collection of $N$ heads, labeled from $0$ to $N-1$. If eradication of all heads is impossible, this should be reported.

Eradication must occur in a strict order: head $0$ first, followed by head $1$, continuing sequentially up to head $N-1$. To attempt eradication of head $i$, all heads with indices less than $i$ must have already been removed.

A head is eradicated only if the sequence of the last $k$ cuts matches one of the $m$ provided "eradication patterns." Each pattern consists of a sequence of $k$ head indices, with the final index $h_{k-1}$ indicating the head targeted for eradication. Importantly, once a head has been eradicated, it cannot be included in any subsequent cuts.

The task is to compute the length of the shortest sequence of cuts that successfully eradicates all $N$ heads.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem specifies a strict order for eradicating the heads: $0, 1, \dots, N-1$. The process of eradicating head $i$ can only begin after head $i-1$ is gone. This sequential nature suggests that you can build the solution step-by-step, solving for one head at a time. Think about what information you need to carry over from the successful eradication of head $i-1$ to calculate the cost of eradicating head $i$.
</details>

<details>

<summary>Hint #2</summary>

Take a closer look at the input constraints: the problem provides values like the maximum number of times a head may appear in patterns, and the maximum pattern length $k$ is quite small. These "strange" constraints are not typical for most algorithms, and often hint towards a specific approach. What kind of algorithm is well-suited for problems with small pattern sizes and restrictions on repeated elements? Try to connect these observations to a technique that can efficiently handle such state transitions and overlapping subproblems.

</details>

<details>

<summary>Hint #3</summary>

This problem is a perfect fit for Dynamic Programming. Let's define a DP state. A natural choice is $DP(i, p)$, representing the minimum total cuts to eradicate all heads up to and including head $i$, with the eradication of head $i$ being accomplished using pattern $p$.

To compute $DP(i, p_{\text{current}})$, you would transition from a state for the previous head, $(i-1, p_{\text{previous}})$. The number of additional cuts required for this transition depends on the overlap between the end of the cut sequence for $p_{\text{previous}}$ and the beginning of the pattern $p_{\text{current}}$. A larger overlap means fewer new cuts are needed. A critical detail is to always verify that any new, non-overlapping cuts do not involve heads that have already been eradicated.

</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Set 1, 3)</summary>

When reading the problem we can realize that some “**strange**” inputs are given. E.g. usually the “*maximum number of times a head may appear*” $d$ is not something that many algorithms would need. 
After some thinking we can stumble upon **Dynamic Programming**.
(I know, this is not the best way but it works… sometimes….)

When experimenting with different DP Dimensions it is quite clear that one dimension $i$ needs to be **the index of current head we are trying to kill**. <br />
This is especially apparent because of the statement “*… that (and only that!) particular head is eradicated if and only if all heads j < i have been eradicated*”. This means that we have to kill the heads from left to right one by one, which makes it very appealing to choose one of our DP Dimensions for that

Additionally it might be appealing to model the last $k$ cuts we performed as DP Dimensions. However, as $k \leq 10$, this will **explode** if we add $10$ dimensions each of size $n = 10^3$.
Therefore, after some thinking we can come up with the following **DP Dimensions**:

- $i$: The index of the head we are trying to kill
- $p$: The pattern for this head we are trying to use to kill the head

**Note**: We can efficiently get the pattern for each head by accordingly “storing” them in a 3D Vector while we parse the input.

Naturally the **values of this DP will give the number of cuts**, we need to perform to kill the head $i$ (and therefore also all previous heads) using pattern $p$.

For our **Base Case**, we can realize that the first head will **always require** $k$ cuts, as we can not use any previous cuts, as there have not been any previous cuts. Therefore we will initialize our DP as `dp[0][p] = k` for all patterns `p` the first head can be killed with.
**Note**: As we want to minimize the number of cuts, we will initialize all entries with $\infty$ (invalid) and only enter valid values one we reach them

For the **Recursive Case**, we can then calculate the entry `dp[i][p]` as follows.

1. Check if the pattern `p` can be executed **individually** (ignoring any previous cuts). 
This is **not always the case**, as `p` **might contain a head** `h` **we have already killed** `h < i` which is not allowed (”*Once a head has been eradicated, it cannot be cut anymore*”).
If we can execute it individually the previous pattern `p_prev` does not matter, so we simply take the previous pattern and add the length `k` of the current pattern `p` to it: 

    `dp[i][p] = std::min(dp[i][p], dp[i-1][prev_p] + k)`
    
    **Intuition**: We do not have to use the previous cuts we made. We can just ignore them and execute the entire pattern `p` and kill the current head `i` using that (or at least try) in that case we need to perform `k` cuts for the pattern. Therefore we just take the minimum number of cuts from the previous head `dp[i-1][prev_p]` and add `k` (as we do not care about the previous pattern)
    
2. Check if we can **reuse/match** part of the pattern `p` with any of the **previous patterns** `prev_p` from the last head `i-1`.
For this we can will simply compare the current pattern `p` to all previous patterns `prev_p` from the last head `i-1`. At each head we will test if there is any match between them by considering all possible number of matches `n_matches` from `1` to `k-1`. 
    
    We will then simply **loop over the patterns to check if they match** <br />
    This might seem inefficient but as the patterns are relatively small and we only have a few patterns per head this is fine
    
    When a match with `n_matches` is found, we can simply update our DP entry as: 

    `dp[i][p] = std::min(dp[i][p], dp[i-1][prev_p] + k - n_matches)`

    Note the only thing that really changed here in comparison to 1., is that we subtract `n_matches` because we were able to reuse `n_matches` cuts from the previous pattern `p_prev`
    
**Note**: We also always have to check if `p_prev` itself was possible. If not we can not continue of of it

Implementing this will already yield a couple of points but there is still a bug in this code.

### Code
```cpp
#include <iostream>
#include <vector>
#include <unordered_map>
#include <limits>
#include <cmath>

typedef std::vector<int> VI;
typedef std::vector<VI> VVI;
typedef std::vector<VVI> VVVI;

typedef std::vector<long> VL;
typedef std::vector<VL> VVL;


// Each pattern can only be used once
// To kill head i, all heads j < i need to be killed before

// Last Cut as DP dimension -> not feasible as there are n possibilities and in the end we would have to store n^9


const long INF = std::numeric_limits<long>::max();

void solve() {
  // ===== READ INPUT =====
  int n, m, k, d; std::cin >> n >> m >> k >> d;
  
  VVVI head_to_patterns(n);
  VI head_to_num_patterns(n, 0);
  for(int i = 0; i < m; ++i) {
    // Read Pattern i
    std::vector<int> pattern; pattern.reserve(k);
    for(int j = 0; j < k; ++j) {
      int h; std::cin >> h; 
      pattern.push_back(h);
    }
    
    // Store Pattern i for the according head
    head_to_patterns[pattern[k-1]].push_back(pattern);
    head_to_num_patterns[pattern[k-1]]++;
  }

  // ===== SOLVE =====
  VVL dp(n, VL(d, std::numeric_limits<long>::max()));
  
  // Initialize DP table for the first head
  for(int p = 0; p < head_to_num_patterns[0]; ++p) {
    dp[0][p] = k;
  }
  
  for(int i = 1; i < n; ++i) {
    for(int p = 0; p < head_to_num_patterns[i]; ++p) {
      // Check if it would be possible to execute the pattern individually
      bool possible = true;
      for(int h = 0; h < k; ++h) {
        if(head_to_patterns[i][p][h] < i) { 
          possible = false;
          break;
        }
      }
      
      // Check if there is any match between the current pattern p and any previous pattern prev_p
      for(int prev_p = 0; prev_p < head_to_num_patterns[i-1]; ++prev_p) {
        // Check if the previous pattern was even possible
        if(dp[i-1][prev_p] == INF) { continue; }
        
        // Check for 1 matching cut
        if(head_to_patterns[i-1][prev_p][k-1] == head_to_patterns[i][p][0]) {
          dp[i][p] = std::min(dp[i][p], dp[i-1][prev_p] + k - 1);
        }
        
        // Check for 2 matching cuts
        if(k > 2) {
          if(head_to_patterns[i-1][prev_p][k-1] == head_to_patterns[i][p][1] &&
             head_to_patterns[i-1][prev_p][k-2] == head_to_patterns[i][p][0]
          ) {
            dp[i][p] = std::min(dp[i][p], dp[i-1][prev_p] + k - 2);
          }
        }
      }
      
      // If it was possible to kill using only the pattern check if that is better
      if(possible) {
        for(int prev_p = 0; prev_p < head_to_num_patterns[i-1]; ++prev_p) {
          // Check if the previous pattern was even possible
          if(dp[i-1][prev_p] == INF) { continue; }
          
          dp[i][p] = std::min(dp[i][p], dp[i-1][prev_p] + k);
        }
      }
    }
  }
  
  // Find minimum cuts to kill the last head
  long min_cuts = std::numeric_limits<long>::max();
  for(int p = 0; p < head_to_num_patterns[n-1]; ++p) {
    min_cuts = std::min(min_cuts, dp[n-1][p]);
  }
  
  // ===== OUTPUT =====
  if(min_cuts == std::numeric_limits<long>::max()) {
    std::cout << "Impossible!" << std::endl;
  } else {
    std::cout << min_cuts << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}
```

</details>

<details>

<summary>Final Solution</summary>

The main **oversight** in the first solution was that even after matching the first `n_matches` cuts to the previous pattern `prev_p` there **might remain cuts that are not allowed**, because the corresponding heads we try to cut are already killed (`h < i`).

Therefore, **after matching patterns**, we still need to **validate**, if the remaining cuts are possible/valid.

This corrects the code and solves all test cases.

### Code
```cpp
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

typedef std::vector<int> VI;
typedef std::vector<VI> VVI;
typedef std::vector<VVI> VVVI;

typedef std::vector<long> VL;
typedef std::vector<VL> VVL;

const long INF = std::numeric_limits<long>::max();

// Function to calculate overlap between two patterns
int calculate_overlap(const VI &a, const VI &b, int k) {
    for (int o = k; o >= 0; --o) { // Try overlaps of size o from k down to 0
        bool valid = true;
        for (int i = 0; i < o; ++i) {
            if (a[k - o + i] != b[i]) {
                valid = false;
                break;
            }
        }
        if (valid) return o;
    }
    return 0;
}

void solve() {
    // ===== READ INPUT =====
    int n, m, k, d;
    std::cin >> n >> m >> k >> d;

    VVVI head_to_patterns(n);
    VI head_to_num_patterns(n, 0);
    for (int i = 0; i < m; ++i) {
        // Read Pattern i
        std::vector<int> pattern(k);
        for (int j = 0; j < k; ++j) {
            std::cin >> pattern[j];
        }

        // Store Pattern i for the appropriate head
        head_to_patterns[pattern[k - 1]].push_back(pattern);
        head_to_num_patterns[pattern[k - 1]]++;
    }

    // ===== SOLVE =====
    // Current Head i x Current Pattern p to kill Head i
    VVL dp(n, VL(d, INF));

    // Initialize DP table for the first head
    for (int p = 0; p < head_to_num_patterns[0]; ++p) {
        dp[0][p] = k; // All patterns take k cuts initially
    }

    // Fill remaining entries
    for (int i = 1; i < n; ++i) {
        for (int p = 0; p < head_to_num_patterns[i]; ++p) {
            const VI &current_pattern = head_to_patterns[i][p];

            // Check if the pattern can independently eradicate the current head
            bool possible = true;
            for (int h = 0; h < k; ++h) {
                if (current_pattern[h] < i) {
                    possible = false;
                    break;
                }
            }

            // Update entry for the current head when no overlap is considered
            if (possible) {
                for (int prev_p = 0; prev_p < head_to_num_patterns[i - 1]; ++prev_p) {
                    if (dp[i - 1][prev_p] == INF) continue; // Skip unreachable states
                    
                    dp[i][p] = std::min(dp[i][p], dp[i - 1][prev_p] + k);
                }
            }

            // Check overlaps between the current pattern and all patterns of the previous head
            for (int prev_p = 0; prev_p < head_to_num_patterns[i - 1]; ++prev_p) {
                if (dp[i - 1][prev_p] == INF) continue; // Skip unreachable states

                const VI &prev_pattern = head_to_patterns[i - 1][prev_p];
                int o = calculate_overlap(prev_pattern, current_pattern, k);

                // Check if the overlap forms a valid transition
                bool valid = true;
                for (int h = o; h < k; ++h) {
                    if (current_pattern[h] < i) {
                        valid = false;
                        break;
                    }
                }

                // Update DP table for valid transitions
                if (valid) {
                    dp[i][p] = std::min(dp[i][p], dp[i - 1][prev_p] + k - o);
                }
            }
        }
    }

    // Find the minimum cuts to kill the last head
    long min_cuts = INF;
    for (int p = 0; p < head_to_num_patterns[n - 1]; ++p) {
        min_cuts = std::min(min_cuts, dp[n - 1][p]);
    }

    // ===== OUTPUT =====
    if (min_cuts == INF) {
        std::cout << "Impossible!\n";
    } else {
        std::cout << min_cuts << "\n";
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);

    int t;
    std::cin >> t;
    while (t--) {
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
   Test set 1 (25 pts / 2 s) : Correct answer      (0.027s)
   Test set 2 (25 pts / 2 s) : Correct answer      (0.03s)
   Test set 3 (25 pts / 2 s) : Correct answer      (0.28s)
   Test set 4 (25 pts / 2 s) : Correct answer      (0.512s)

Total score: 100
```