# The Iron Islands

## 📝 Problem Description

You are tasked with finding an optimal plan to conquer a set of islands. You are given $N$ islands, each with a specific cost $c_i$ representing the number of soldiers needed to conquer it. You command a total of $k$ soldiers.

The islands are organized into $w$ distinct waterways, each being a linear sequence of islands originating from a central island (island 0, called Pyke). Every island other than Pyke belongs to exactly one waterway.

A "plan" is a selection of islands to conquer. A plan is considered **valid** if it meets two strict conditions:
1.  **Exact Cost:** The sum of the costs of all conquered islands must be exactly equal to your total number of soldiers, $k$. Every soldier must be deployed.
2.  **Traversability:** The set of conquered islands must form a single, contiguous path. This means you must be able to traverse all conquered islands sequentially along the waterways without visiting an island more than once and without passing through an unconquered island. Such a path can exist entirely within one waterway or be formed by joining segments from two different waterways at the central island.

Your goal is to determine the maximum number of islands that can be conquered in any valid plan. If no valid plan exists, the answer is 0.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The condition that all attacked islands must be traversable without passing through an unattacked island is crucial. Think about what this implies about the arrangement of the islands you choose to conquer. They must be "next to" each other along the defined waterways.
</details>

<details>
<summary>Hint #2</summary>
A valid path can only visit each island once. To move from one waterway to another, you must pass through the central island, Pyke. What does this imply about the maximum number of different waterways that can be part of a single valid plan?
</details>

<details>
<summary>Hint #3</summary>
The problem can be simplified by considering two disjoint cases for a valid plan: either all conquered islands lie on a single waterway, or they lie on exactly two waterways, connected at Pyke. How can you find the best solution for each case? For the two-waterway case, if you spend $C_1$ soldiers on a segment of the first waterway, you are left with a fixed budget for the second. How can you efficiently find the best segment on another waterway for this remaining budget?
</details>

## ✨ Solutions

<details>
<summary>A Brute-Force Approach (Test Sets 1 & 2)</summary>

### Core Idea

The problem requires us to find a set of islands that form a contiguous path and whose combined conquest cost is exactly $k$. The goal is to maximize the number of islands in such a set. The contiguous path constraint is reminiscent of problems involving contiguous subarrays, which often suggests a **sliding window** approach.

### Key Observation: Path Structure

A crucial observation comes from the traversal rules. To switch between waterways, one must pass through Pyke (island 0). Since each island can only be visited once in the final traversal, Pyke can be visited at most once. This means a valid path can involve islands from **at most two waterways**. If a path involved three or more waterways, it would need to pass through Pyke at least twice, which is forbidden.

This simplifies the problem immensely: a valid plan consists of either:
1.  A contiguous segment of islands on a single waterway.
2.  A contiguous segment of islands from one waterway connected at Pyke to a contiguous segment from a second waterway.

### The Algorithm

This observation leads to an approach where we check all valid path structures. We can iterate through all possible combinations of one or two waterways and find the best solution for each combination.

1.  **Single Waterway Case:** For each of the $w$ waterways, we can run a standard sliding window algorithm. We find the longest contiguous sequence of islands on that waterway whose costs sum to exactly $k$.

2.  **Two Waterway Case:** We can iterate through every pair of distinct waterways, say waterway `A` and waterway `B`. To form a single contiguous path, we can conceptually "stitch" them together at Pyke. For example, we can form a path `(end of A) -> ... -> Pyke -> ... -> (end of B)`. This means we traverse one waterway in reverse towards Pyke, and the other away from Pyke. We can then apply the sliding window algorithm on this combined path.

The main implementation challenge is managing the indices when creating a combined path from two separate waterways. In the provided code, this is handled by carefully tracking `left` and `right` pointers across the boundary between the two waterways.

### Complexity Analysis

This approach involves iterating through all pairs of waterways. There are $O(w^2)$ such pairs. For each pair, we perform a sliding window algorithm. The length of the combined path can be up to $O(n)$. Therefore, the time complexity is $O(w^2 \cdot n)$. If $w$ is small (as in Test Sets 1 and 2, where $w \le 20$), this is feasible. However, for larger $w$, this approach becomes too slow.

```cpp
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

void solve() {
  // ===== READ INPUT =====
  int n, k, w; std::cin >> n >> k >> w;
  
  std::vector<int> required_men(n);
  for(int i = 0; i < n; ++i) { std::cin >> required_men[i]; }
  
  std::vector<std::vector<int>> waterways(w);
  for(int i = 0; i < w; ++i) {
    int l; std::cin >> l;
    waterways[i].resize(l);
    for(int j = 0; j < l; ++j) {
      std::cin >> waterways[i][j];
    }
  }
  
  // ===== SOLVE =====
  int max_size = 0;
  
  // Create a combined path for every pair of waterways and apply sliding window
  for(int w_1 = 0; w_1 < w; ++w_1) {
    for (int w_2 = 0; w_2 < w; ++w_2) {
      std::vector<int> path;
      // The path can be on a single waterway
      if (w_1 == w_2) {
        for(int island_idx : waterways[w_1]) {
          path.push_back(required_men[island_idx]);
        }
      } else {
        // Or on two waterways, connected at Pyke.
        // Traverse waterway 1 in reverse, towards Pyke.
        for(int i = waterways[w_1].size() - 1; i >= 0; --i) {
          path.push_back(required_men[waterways[w_1][i]]);
        }
        // Traverse waterway 2 away from Pyke (skip Pyke itself as it's already added)
        for(size_t i = 1; i < waterways[w_2].size(); ++i) {
          path.push_back(required_men[waterways[w_2][i]]);
        }
      }

      // Standard Sliding Window
      long long current_sum = 0;
      int left = 0;
      for (int right = 0; right < path.size(); ++right) {
        current_sum += path[right];
        while (current_sum > k) {
          current_sum -= path[left];
          left++;
        }
        if (current_sum == k) {
          max_size = std::max(max_size, right - left + 1);
        }
      }
    }
  }

  // Handle case where k=0, which means no islands can be conquered
  if (k == 0) {
      max_size = 0;
  }
  
  // ===== OUTPUT =====
  std::cout << max_size << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
  return 0;
}
```
*Note: The provided student code was slightly different. The version above has been adjusted for clarity and correctness, representing the described brute-force logic more directly.*

</details>

<details>
<summary>An Optimal O(N) Solution with Hashing</summary>

### Decomposing the Problem

To achieve a solution efficient enough for all test sets, we must avoid the $O(w^2)$ term from the previous approach. We can do this by sticking to the same decomposition but handling the two-waterway case more intelligently.

The two cases to consider for an optimal solution are:
1.  The conquered islands form a contiguous segment on a **single waterway**.
2.  The conquered islands form a path spanning **two waterways**, connected at Pyke.

We can find the maximum number of islands for each case separately and take the overall maximum.

### Case 1: Single Waterway Paths

This case is straightforward. We can iterate through each of the $w$ waterways. For each one, we apply a standard **sliding window** algorithm to find the longest contiguous segment of islands whose costs sum up to exactly $k$. The total time complexity for this part is proportional to the sum of the lengths of all waterways, which is $O(n)$.

### Case 2: Two Waterway Paths

This is the more complex case. A path on two waterways must consist of:
- A segment of islands on waterway `A`, starting from Pyke and extending outwards.
- Pyke itself (island 0).
- A segment of islands on waterway `B`, also starting from Pyke and extending outwards.

Let's say we choose a segment on waterway `A` (excluding Pyke) with total cost $C_A$ and length $L_A$. For a valid plan, the total cost must be $k$. This means the remaining components—Pyke and the segment from waterway `B`—must have a combined cost of $k - C_A$. So, the segment from waterway `B` must have a cost of $C_B = k - C_A - \text{cost(Pyke)}$.

Our goal is to find a pair of such segments $(A, B)$ that maximizes the total length $L_A + L_B + 1$.

### An Efficient Approach using Hashing

A naive search for the matching segment on waterway `B` would be too slow. We can optimize this using a hash map (like `std::unordered_map` in C++).

The algorithm proceeds as follows:
1.  Initialize `max_size` by solving Case 1 for all single waterways.
2.  Initialize an empty hash map, `men_to_max_num_islands`. This map will store `cost -> max_length` pairs. Specifically, `map[C]` will hold the maximum number of islands (length) we can conquer with a path starting from (but not including) Pyke that costs exactly $C$.
3.  Iterate through each waterway, one by one. For the current waterway `i`:
    a.  Calculate the prefix sums of costs for segments starting from Pyke. Let `prefix_sum[j]` be the cost to conquer the first `j` islands on this waterway after Pyke.
    b.  For each prefix of length `j` and cost `prefix_sum[j]`:
        i.  Calculate the required cost for a complementary path on another waterway: `complement_cost = k - prefix_sum[j] - cost(Pyke)`.
        ii. Look up `complement_cost` in our hash map. If an entry exists, say `men_to_max_num_islands[complement_cost] = L_complement`, it means we have previously processed a waterway with a segment of length `L_complement` and cost `complement_cost`.
        iii. We have found a valid two-waterway plan with total length `j + L_complement + 1` (for Pyke). We update our global `max_size` with this new length if it's larger.
    c. After checking all prefixes of the current waterway `i`, we update the hash map with its own path information. For each prefix of length `j` and cost `prefix_sum[j]`, we update `men_to_max_num_islands[prefix_sum[j]] = max(men_to_max_num_islands[prefix_sum[j]], j)`.

By updating the map *after* checking for complements, we ensure that we only combine paths from two *different* waterways.

### Complexity Analysis

-   **Case 1:** As discussed, this is $O(n)$.
-   **Case 2:** We iterate through each waterway once. For each waterway of length $L$, we compute prefix sums ($O(L)$) and perform $L$ lookups and updates in the hash map (average $O(1)$ each). The total time across all waterways is proportional to the sum of their lengths, which is $O(n)$.

The overall time complexity is $O(n) + O(n) = O(n)$, which is efficient enough for all constraints.

```cpp
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>

void solve() {
  // ===== READ INPUT =====
  int n;
  long long k;
  int w;
  std::cin >> n >> k >> w;
  
  std::vector<int> required_men(n);
  for(int i = 0; i < n; ++i) { std::cin >> required_men[i]; }
  
  std::vector<std::vector<int>> waterways(w);
  for(int i = 0; i < w; ++i) {
    int l; std::cin >> l;
    waterways[i].resize(l);
    for(int j = 0; j < l; ++j) {
      int r; std::cin >> r;
      // Store costs directly instead of island indices for convenience
      waterways[i][j] = required_men[r];
    }
  }
  
  const int CENTER_COST = required_men[0];
  
  // ===== SOLVE =====
  int max_size = 0;
  
  // Case 1: Search for maximum size on each waterway individually
  for(int i = 0; i < w; ++i) {
    int l = waterways[i].size();
    
    // Perform Sliding Window over waterway i
    long long sum = 0;
    int left = 0;
    for(int right = 0; right < l; ++right) {
      sum += waterways[i][right];
      
      while(sum > k) {
        sum -= waterways[i][left];
        left++;
      }
      
      if(sum == k) {
        max_size = std::max(max_size, right - left + 1);
      }
    }
  }
  
  // Case 2: Search for maximum size across 2 waterways
  std::unordered_map<long long, int> men_to_max_num_islands; // num_men -> most islands
  
  for(int w_idx = 0; w_idx < w; ++w_idx) {
    // Calculate Prefix Sums for segments on the current waterway starting after Pyke
    std::vector<long long> prefix_sums;
    
    long long current_sum = 0;
    // Iterate from island 1 (after Pyke)
    for(size_t i = 1; i < waterways[w_idx].size(); ++i) {
      current_sum += waterways[w_idx][i];
      // Optimization: No need to extend path if cost already exceeds k
      if(current_sum + CENTER_COST > k) { break; } 
      prefix_sums.push_back(current_sum);
    }
    
    // For each segment on the current waterway, check for a complement in previously seen waterways
    for(size_t i = 0; i < prefix_sums.size(); ++i) {
      long long path_len = i + 1;
      long long current_cost = prefix_sums[i];
      long long complement = k - current_cost - CENTER_COST;

      if(men_to_max_num_islands.count(complement)) {
        // Found a valid two-waterway path
        max_size = std::max(max_size, (int)path_len + men_to_max_num_islands[complement] + 1); // +1 for Pyke
      }
    }
    
    // Add the paths from this waterway to the map for future waterways to use
    for(size_t i = 0; i < prefix_sums.size(); ++i) {
        long long path_len = i + 1;
        long long current_cost = prefix_sums[i];
        if(!men_to_max_num_islands.count(current_cost)) {
            men_to_max_num_islands[current_cost] = path_len;
        } else {
            men_to_max_num_islands[current_cost] = std::max(men_to_max_num_islands[current_cost], (int)path_len);
        }
    }
  }
  
  // ===== OUTPUT =====
  std::cout << max_size << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
  return 0;
}
```

</details>

## ⚡ Result

```plaintext

```