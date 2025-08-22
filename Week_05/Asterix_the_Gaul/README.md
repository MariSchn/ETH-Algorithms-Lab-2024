# Astérix the Gaul

## 📝 Problem Description

Given a set of $N$ available movements, where each movement is defined by a distance $d$ it covers and a time $t$ it takes, the goal is to select a subset of these movements to travel a total distance of at least $D$ in strictly less than $T$ seconds.

Additionally, we can drink $i$ gulps of a potion before starting. Taking $i$ gulps increases the distance of *every* chosen movement by an amount $s_i$. We are given the values of $s_i$ for $i$ from 1 to $M$. The objective is to find the minimum number of gulps required to meet the distance and time criteria. If it's impossible to succeed even with the maximum of $M$ gulps, we should report that.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The number of available movements, $N$, is relatively small (up to 30). This often suggests that solutions with exponential complexity in $N$, such as those that explore all subsets of movements, might be feasible, at least for smaller values of $N$.
</details>
<details>
<summary>Hint #2</summary>
Let's first simplify the problem by ignoring the potion. How can you determine the maximum distance achievable for a *fixed number of moves*, say $k$, while keeping the total time under the limit $T$? This subproblem seems related to the classic subset sum problem. If you can solve this for all possible $k$, how can you use that information to find the answer?
</details>
<details>
<summary>Hint #3</summary>
A solution that checks all $2^N$ subsets is too slow when $N=30$, as $2^{30}$ is over a billion operations. When faced with an exponential complexity problem on a set, a common strategy is to split the set into two halves. Can you generate all possible outcomes for each half independently and then combine the results from the two halves efficiently? This technique is known as **meet-in-the-middle**.
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Sets 1-3)</summary>
This problem asks us to select a subset of movements to satisfy certain conditions, which is a variation of the classic **Subset Sum Problem**. Since these problems are generally NP-complete, we expect a solution with exponential time complexity. For the smaller constraints where $N \le 20$, a brute-force approach that checks every possible subset of movements is feasible.

The overall strategy can be broken down into two main phases:

### Phase 1: Precomputation of Maximum Distances
First, we solve a simplified version of the problem: without any potions, what is the maximum distance we can cover for a given number of moves, say $k$, while staying under the time limit $T$?

To do this, we can iterate through all $2^N$ subsets of the available movements. For each subset, we calculate:
1.  The total distance covered (`sum_distance`).
2.  The total time taken (`sum_time`).
3.  The number of movements used (`n_moves`).

If `sum_time` is strictly less than $T$, then this subset is a valid combination of moves. We use an array, let's call it `max_dist_for_k_moves`, to store the best distance found so far for each possible number of moves. We update `max_dist_for_k_moves[n_moves]` with `sum_distance` if it's greater than the currently stored value.

After checking all $2^N$ subsets, this array will contain the maximum raw distance (without potions) achievable for using $k = 0, 1, \dots, N$ movements.

### Phase 2: Finding the Minimum Potion Gulps
Now, we can determine the minimum number of gulps needed. We iterate through all possible numbers of moves, from $k=1$ to $N$. For each $k$:

1.  We retrieve the maximum raw distance achievable with $k$ moves, `dist_raw = max_dist_for_k_moves[k]`. If no valid combination for $k$ moves exists, we skip it.
2.  We check if we can already reach the destination: if `dist_raw >= D`, it means we can succeed with **0 gulps**. We can stop and report 0.
3.  Otherwise, we need to cover a remaining distance of `D - dist_raw`. With $k$ moves, each gulp of potion adds a certain boost to *each* move. If we take a potion that gives a boost of `b`, the total extra distance is `k * b`.
4.  Therefore, we need a per-move boost of at least `ceil((D - dist_raw) / k)`. Let's call this `necessary_boost`.
5.  We are given a sorted list of potion effects $s_1, s_2, \dots, s_M$. We can use binary search (specifically, `std::lower_bound` in C++) to find the smallest index $i$ such that the potion effect $s_i$ is greater than or equal to `necessary_boost`. The number of gulps would then be $i$.
6.  We keep track of the minimum number of gulps found across all values of $k$.

After checking all possible numbers of moves, the minimum value we found is our answer. If we never find a way to reach the destination, it's impossible.

**Complexity:** The first phase dominates the runtime. Iterating through all subsets takes $O(2^N \cdot N)$ time. The second phase takes $O(N \cdot \log M)$ for the binary searches. The total complexity is approximately $O(2^N \cdot N)$, which is acceptable for $N \le 20$.

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>

const long MIN_LONG = std::numeric_limits<long>::min();
const long MAX_LONG = std::numeric_limits<long>::max();

void solve() {
  // ===== READ INPUT =====
  int n, m; long D, T; std::cin >> n >> m >> D >> T;
  
  std::vector<std::pair<long, long>> moves; moves.reserve(n);
  for(int i = 0; i < n; ++i) {
    long d, t; std::cin >> d >> t;
    moves.emplace_back(d, t);  
  }
  
  std::vector<long> boosts(m);
  for(int i = 0; i < m; ++i) {
    std::cin >> boosts[i];
  }
  
  // ===== SOLVE =====
  // For every number of moves k in [0, n], find the max distance achievable in less than T seconds without potions.
  std::vector<long> n_moves_to_best_raw_distance(n + 1, MIN_LONG);
  for (int s = 0; s < (1 << n); ++s) { // Iterate through all 2^n subsets of moves
    long sum_distance = 0;
    long sum_time = 0;
    int n_moves = 0;
    
    // Calculate total distance, time, and move count for the current subset
    for (int i = 0; i < n; ++i) {
      if (s & (1 << i)) { // Check if the i-th move is in the subset
        sum_distance += moves[i].first;
        sum_time += moves[i].second;
        n_moves++;
      }
    }
    
    // If the time is valid, update the max distance for this number of moves
    if (sum_time < T) {
      n_moves_to_best_raw_distance[n_moves] = std::max(n_moves_to_best_raw_distance[n_moves], sum_distance);
    }
  }
  
  long min_gulps = MAX_LONG;
  // If we can reach Panoramix with 0 moves (D<=0), 0 gulps are needed.
  if (D <= 0) {
    min_gulps = 0;
  }

  // For each possible number of moves, find the minimum gulps needed.
  for (int n_moves = 1; n_moves <= n; ++n_moves) {
    // Skip if there's no way to make n_moves in time.
    if (n_moves_to_best_raw_distance[n_moves] == MIN_LONG) {
      continue;
    }
    
    long remaining_distance = D - n_moves_to_best_raw_distance[n_moves];
    if (remaining_distance <= 0) {
      min_gulps = 0; // Reachable without any potion
      break; // 0 is the best possible, so we can stop.
    }
    
    // Calculate the required boost per move. Use ceiling division.
    long necessary_boost = (remaining_distance + n_moves - 1) / n_moves;
    
    // Find the first potion that provides at least the necessary boost.
    auto potion_iter = std::lower_bound(boosts.begin(), boosts.end(), necessary_boost);
    if (potion_iter != boosts.end()) {
      // The number of gulps is the 1-based index of the potion.
      long gulps_needed = std::distance(boosts.begin(), potion_iter) + 1;
      min_gulps = std::min(min_gulps, gulps_needed);
    }
  }
  
  // ===== OUTPUT =====
  if (min_gulps == MAX_LONG) {
    std::cout << "Panoramix captured" << std::endl;
  } else {
    std::cout << min_gulps << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}
```
</details>

<details>
<summary>Final Solution</summary>
The previous solution's $O(2^N \cdot N)$ complexity is too slow for the full constraints where $N$ can be up to 30. To optimize this, we can use the **meet-in-the-middle** (or **split and list**) technique. This approach reduces the complexity by splitting the problem into two smaller, more manageable subproblems and then combining their results.

### The Meet-in-the-Middle Strategy

1.  **Split:** Divide the $N$ movements into two halves: the first half with $\lfloor N/2 \rfloor$ moves and the second half with the rest.

2.  **Generate Sub-solutions:** For each half, we generate all possible subsets of movements using a recursive approach (DFS). For each subset, we store a tuple representing its properties: `(number of moves, total time, total distance)`. This gives us two lists of results, one for each half. The size of each list is manageable, around $O(2^{N/2})$.

3.  **Filter Sub-solutions:** Within each list of results, and for a fixed number of moves, many generated states are suboptimal. For instance, if we have two states for $k$ moves, $(t_1, d_1)$ and $(t_2, d_2)$, and if $t_1 \le t_2$ and $d_1 \ge d_2$, the second state is redundant because the first one is at least as good in every aspect. We can filter out these suboptimal states. A standard way to do this is to sort the states by time and then iterate through them, keeping only those that offer a better distance than all preceding states. This leaves us with a list of states where both time and distance are strictly increasing.

4.  **Combine Results:** Now, we combine the filtered results from the two halves. We iterate through every state $(k_1, t_1, d_1)$ from the first half. For each, we need to find the best possible partner state $(k_2, t_2, d_2)$ from the second half. A valid partner must satisfy the time constraint: $t_1 + t_2 < T$. Our goal is to find the partner that maximizes the total distance $d_1 + d_2$.
    For a given $t_1$, we need to find a state from the second half where $t_2 < T - t_1$ and $d_2$ is as large as possible. Since our filtered lists from step 3 are sorted by time (and thus also by distance), we can use binary search (`std::upper_bound`) to efficiently find the best valid partner.
    For each valid combination `(k1+k2, t1+t2, d1+d2)`, we update our `max_dist_for_k_moves` array.

5.  **Calculate Final Answer:** This final step is identical to the one in the first solution. We use the populated `max_dist_for_k_moves` array to calculate the required boost for each number of moves and find the minimum number of potion gulps.

**Implementation Note:** The provided code implements this logic by working with "remaining distance" and "remaining time" instead of total distance and time. It initializes with `(D, T)` and subtracts the move's `(d, t)` at each step. So, minimizing the final "remaining distance" is equivalent to maximizing the total distance covered.

**Complexity:** The generation and filtering steps for each half take roughly $O(N \cdot 2^{N/2})$. The combination step involves iterating through the first half's results and performing a binary search on the second half's results, leading to a complexity around $O(N \cdot 2^{N/2} \log(2^{N/2}))$, which simplifies to $O(N^2 \cdot 2^{N/2})$. This is significantly faster than $O(2^N)$ and is efficient enough for $N \le 30$.

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <limits>

using vpii = std::vector<std::pair<int, int>>;

// These are global to be accessible by the recursive helper and solve function.
vpii moves;
std::vector<int> potions;
// Arrays to store results for the two halves, indexed by number of moves.
// Each element is a pair of (remaining_time, remaining_distance).
std::array<std::vector<std::pair<int64_t, int64_t>>, 16> first_half;
std::array<std::vector<std::pair<int64_t, int64_t>>, 16> second_half;
// Final best results after combining halves.
std::array<int64_t, 31> best_rem_dist;

// Recursive function to generate all subsets for a half.
// It calculates remaining time and distance from the initial T and D.
void dfs_fill(int move_idx, int moves_used, int64_t rem_dist, int64_t rem_time, int max_move_idx, auto &arr) {
  if (rem_time <= 0) return; // Path is already too long
  if (move_idx >= max_move_idx) {
    arr[moves_used].emplace_back(rem_time, rem_dist);
    return;
  }
  // Recurse without taking the current move
  dfs_fill(move_idx + 1, moves_used, rem_dist, rem_time, max_move_idx, arr);
  // Recurse taking the current move
  dfs_fill(move_idx + 1, moves_used + 1, rem_dist - moves[move_idx].first, rem_time - moves[move_idx].second, max_move_idx, arr);
}

// Filters out suboptimal pairs.
// For a given time, we only need the one that minimizes remaining distance.
// For two pairs (t1, d1) and (t2, d2), if t1 > t2, we need d1 < d2 for it to be optimal.
void sort_and_remove_suboptimal(auto &arr) {
  for (auto &row : arr) {
    if (row.size() <= 1) continue;
    // Sort by rem_time ascending, then rem_dist ascending.
    std::sort(row.begin(), row.end());
    // Remove duplicates and entries that are worse than previous ones.
    std::vector<std::pair<int64_t, int64_t>> optimal_row;
    optimal_row.push_back(row[0]);
    for (size_t i = 1; i < row.size(); ++i) {
        // Keep a point if it has a better (smaller) remaining distance
        // than the last kept optimal point.
        if (row[i].second < optimal_row.back().second) {
            optimal_row.push_back(row[i]);
        }
    }
    row = optimal_row;
  }
}

void clear_globals() {
  for (auto &v : first_half) v.clear();
  for (auto &v : second_half) v.clear();
}

void solve() {
  clear_globals();
  int n, m;
  int64_t D, T;
  std::cin >> n >> m >> D >> T;

  moves.resize(n);
  for (auto &move : moves) std::cin >> move.first >> move.second;

  potions.resize(m);
  for (auto &p : potions) std::cin >> p;
  // Potions are not guaranteed to be sorted in the input.
  std::sort(potions.begin(), potions.end());

  best_rem_dist.fill(std::numeric_limits<int64_t>::max());
  
  int n_half = n / 2;
  dfs_fill(0, 0, D, T, n_half, first_half);
  dfs_fill(n_half, 0, 0, T, n, second_half); // Note: rem_dist starts at 0 for the second half
  
  sort_and_remove_suboptimal(first_half);
  sort_and_remove_suboptimal(second_half);

  // Combine results from both halves
  for (int i = 0; i <= n_half; ++i) {
    for (const auto &p1 : first_half[i]) {
      for (int j = 0; j <= n - n_half; ++j) {
        if(second_half[j].empty()) continue;

        // Find partner in second half s.t. p1.rem_time + p2.rem_time > T
        // This is equivalent to p2.rem_time > T - p1.rem_time
        // We want the partner with the minimum remaining distance.
        auto it = std::lower_bound(second_half[j].begin(), second_half[j].end(), std::make_pair(T - p1.first, (int64_t) -1));
        if (it != second_half[j].begin()) {
            --it; // The last element that satisfies the time condition
            int64_t combined_rem_dist = p1.second + it->second;
            best_rem_dist[i + j] = std::min(best_rem_dist[i + j], combined_rem_dist);
        }
      }
    }
  }

  int64_t min_gulps = std::numeric_limits<int64_t>::max();
  if (D <= 0) min_gulps = 0;

  for (int k = 1; k <= n; ++k) {
    if (best_rem_dist[k] <= 0) {
      min_gulps = 0;
      break;
    }
    if (best_rem_dist[k] != std::numeric_limits<int64_t>::max()) {
      int64_t needed_boost = (best_rem_dist[k] + k - 1) / k;
      auto it = std::lower_bound(potions.begin(), potions.end(), needed_boost);
      if (it != potions.end()) {
        min_gulps = std::min(min_gulps, (int64_t)(it - potions.begin() + 1));
      }
    }
  }
  
  if (min_gulps == std::numeric_limits<int64_t>::max()) {
    std::cout << "Panoramix captured" << std::endl;
  } else {
    std::cout << min_gulps << std::endl;
  }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n_tests; std::cin >> n_tests;
    while (n_tests--) { solve(); }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (20 pts / 5 s) : Correct answer      (0.003s)
   Test set 2 (20 pts / 5 s) : Correct answer      (0.003s)
   Test set 3 (20 pts / 5 s) : Correct answer      (0.452s)
   Test set 4 (40 pts / 5 s) : Correct answer      (0.637s)

Total score: 100
```