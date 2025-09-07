# Astérix the Gaul

## 📝 Problem Description

Consider a set of $N$ available movements, each characterized by a distance $d$ and a time $t$. The task is to select a subset of these movements such that the total distance covered is at least $D$ and the total time required is strictly less than $T$ seconds.

Before starting, it is possible to consume $i$ gulps of a potion, where taking $i$ gulps increases the distance of every chosen movement by $s_i$. The values $s_i$ are provided for $i$ from 1 to $M$. The objective is to determine the minimum number of gulps necessary to satisfy the distance and time requirements.

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

A solution that checks all $2^N$ subsets is too slow when $N=30$, as $2^{30}$ is over a billion operations. When faced with an exponential complexity problem on a set, a common strategy is to split the set into two halves. Can you generate all possible outcomes for each half independently and then combine the results from the two halves efficiently? This technique is known as split and list.

</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Sets 1, 2, 3)</summary>

This problem asks us to select a subset of movements to satisfy certain conditions, which is a variation of the classic **Subset Sum Problem**. Since these problems are generally NP-complete, we expect a solution with exponential time complexity. For the smaller constraints where $N \le 20$, a brute-force approach that checks every possible subset of movements is feasible.

The overall strategy can be broken down into two main phases:

### Phase 1: Precomputation of Maximum Distances

First, a simplified version of the problem is solved: without any potions, what is the maximum distance that can be covered for a given number of moves, say $k$, while staying under the time limit $T$?

This is accomplished by iterating through all $2^N$ subsets of the available movements. For each subset, the following are computed:
1.  The total distance covered (`sum_distance`).
2.  The total time taken (`sum_time`).
3.  The number of movements used (`n_moves`).

If `sum_time` is strictly less than $T$, then this subset is a valid combination of moves. An array, `n_moves_to_best_raw_distance`, is used to store the best distance found so far for each possible number of moves. The value `n_moves_to_best_raw_distance[n_moves]` is updated with `sum_distance` if it is greater than the currently stored value.

After checking all $2^N$ subsets, this array contains the maximum raw distance (without potions) achievable for using $k = 0, 1, \dots, N$ movements.

### Phase 2: Finding the Minimum Potion Gulps

The next step is to determine the minimum number of gulps needed. All possible numbers of moves, from $k=1$ to $N$, are considered. For each $k$:

1.  The maximum raw distance achievable with $k$ moves is retrieved as `n_moves_to_best_raw_distance[k]`. If no valid combination for $k$ moves exists, this value is skipped.
2.  If `n_moves_to_best_raw_distance[k] >= D`, then the destination can be reached with **0 gulps**. In this case, 0 is reported as the answer.
3.  Otherwise, the remaining distance to cover is `D - n_moves_to_best_raw_distance[k]`. With $k$ moves, each gulp of potion adds a certain boost to each move. If a potion provides a boost of `b`, the total extra distance is `k * b`.
4.  Therefore, a per-move boost of at least `ceil((D - n_moves_to_best_raw_distance[k]) / k)` is required. Denote this as `necessary_boost`.
5.  Given a sorted list of potion effects $s_1, s_2, \dots, s_M$, binary search (specifically, `std::lower_bound`) is used to find the smallest index $i$ such that the potion effect $s_i$ is greater than or equal to `necessary_boost`. The number of gulps is then $i+1$ (since the list is 0-indexed).
6.  The minimum number of gulps found across all values of $k$ is tracked.

After checking all possible numbers of moves, the minimum value found is the answer. If no way is found to reach the destination, the task is impossible.

**Complexity:** The first phase dominates the runtime. Iterating through all subsets takes $O(2^N \cdot N)$ time. The second phase takes $O(N \cdot \log M)$ for the binary searches. The total complexity is approximately $O(2^N \cdot N)$, which is acceptable for $N \le 20$.

### Code
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
  // For every number of moves [0, 1, ..., n] calculate the furthest we can reach WITHOUT drinking any potion in the given time T
  std::vector<long> n_moves_to_best_raw_distance(n + 1, MIN_LONG);
  for (int s = 0; s < 1<<n; ++s) { // Iterate through all subsets of moves
    long sum_distance = 0;
    long sum_time = 0;
    int n_moves = 0;
    
    // Calculate total distance, time, and move count of the subset
    for (int i = 0; i < n; ++i) {
      if (s & 1<<i) { // Move i is in the subset
        sum_distance += moves[i].first;
        sum_time += moves[i].second;
        n_moves++;
      }
    }
    
    if(sum_time >= T) { continue; } // Subset takes too long
    
    n_moves_to_best_raw_distance[n_moves] = std::max(n_moves_to_best_raw_distance[n_moves], sum_distance);
  }
  
  // Check for every possible number of moves, the minimum number of potion needed to reach Panoramix with this amount of moves
  // output the minimum number of potions among all possible number of moves
  long min_gulps = MAX_LONG;
  for(int n_moves = 1; n_moves <= n; ++n_moves) {
    if (n_moves_to_best_raw_distance[n_moves] == MIN_LONG) { continue; }
    
    long remaining_distance = D - n_moves_to_best_raw_distance[n_moves];  // Calculate remaining distance to reach Panoramix
    if(remaining_distance <= 0) {
      min_gulps = 0;
      break;
    }
    
    // Calculate how much boost would be required to reach Panoramix with 
    long necessary_boost = std::ceil(((double) remaining_distance) / ((double) n_moves));
    
    // Check if it is possible to gain that boost using the potions
    const auto potion_iter = std::lower_bound(std::begin(boosts), std::end(boosts), necessary_boost);
    if (potion_iter != std::end(boosts)) {
      min_gulps = std::min(min_gulps, potion_iter - std::begin(boosts) + 1);
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
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}
```
</details>

<details>

<summary>Final Solution</summary>

The previous solution's $O(2^N \cdot N)$ complexity is too slow for the full constraints where $N$ can be up to 30. To optimize this, we can use the **split and list** (or **meet-in-the-middle**) technique. This approach reduces the complexity by splitting the problem into two smaller, more manageable subproblems and then combining their results.

### The Split-and-List Strategy

1.  **Split:** Divide the $N$ movements into two halves: the first half with $\lfloor N/2 \rfloor$ moves and the second half with the rest.

2.  **Generate Sub-solutions:** For each half, we generate all possible subsets of movements using a recursive approach (DFS). For each subset, we store a tuple representing its properties: `(number of moves, total time, total distance)`. This gives us two lists of results, one for each half. The size of each list is manageable, around $O(2^{N/2})$.

3.  **Filter Sub-solutions:** Within each list of results, and for a fixed number of moves, many generated states are suboptimal. For instance, if we have two states for $k$ moves, $(t_1, d_1)$ and $(t_2, d_2)$, and if $t_1 \le t_2$ and $d_1 \ge d_2$, the second state is redundant because the first one is at least as good in every aspect. We can filter out these suboptimal states. A standard way to do this is to sort the states by time and then iterate through them, keeping only those that offer a better distance than all preceding states. This leaves us with a list of states where both time and distance are strictly increasing.

4.  **Combine Results:** Now, we combine the filtered results from the two halves. We iterate through every state $(k_1, t_1, d_1)$ from the first half. For each, we need to find the best possible partner state $(k_2, t_2, d_2)$ from the second half. A valid partner must satisfy the time constraint: $t_1 + t_2 < T$. Our goal is to find the partner that maximizes the total distance $d_1 + d_2$.
    For a given $t_1$, we need to find a state from the second half where $t_2 < T - t_1$ and $d_2$ is as large as possible. Since our filtered lists from step 3 are sorted by time (and thus also by distance), we can use binary search (`std::upper_bound`) to efficiently find the best valid partner.
    For each valid combination `(k1+k2, t1+t2, d1+d2)`, we update our `n_moves_to_best_raw_distance` array.

5.  **Calculate Final Answer:** This final step is identical to the one in the first solution. We use the populated `n_moves_to_best_raw_distance` array to calculate the required boost for each number of moves and find the minimum number of potion gulps.

**Implementation Note:** The provided code implements this logic by working with "remaining distance" and "remaining time" instead of total distance and time. It initializes with `(D, T)` and subtracts the move's `(d, t)` at each step. So, minimizing the final "remaining distance" is equivalent to maximizing the total distance covered.

**Complexity:** The generation and filtering steps for each half take roughly $O(N \cdot 2^{N/2})$. The combination step involves iterating through the first half's results and performing a binary search on the second half's results, leading to a complexity around $O(N \cdot 2^{N/2} \log(2^{N/2}))$, which simplifies to $O(N^2 \cdot 2^{N/2})$. This is significantly faster than $O(2^N)$ and is efficient enough for $N \le 30$.

### Code
**Note**: The original solution code comes from [this repo](https://github.com/simon-hrabec/Algolab-2020/tree/main).
```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <limits>

using vi = std::vector<int>;
using vpii = std::vector<std::pair<int, int>>;

vpii moves;
vi potions;
std::array<std::vector<std::pair<int64_t, int64_t>>, 16> first_half, second_half;
std::array<int64_t, 31> best_without_potion;

void dfs_fill(int move, int moves_used, int64_t distance, int64_t time, int max_move, std::array<std::vector<std::pair<int64_t, int64_t>>, 16> &arr) {
  if (time <= 0) return;
  if (move >= max_move) {
    arr[moves_used].emplace_back(time, distance);
    return;
  }
  dfs_fill(move + 1, moves_used, distance, time, max_move, arr);
  dfs_fill(move + 1, moves_used + 1, distance - moves[move].first, time - moves[move].second, max_move, arr);
}

void sort_and_remove_suboptimal(std::array<std::vector<std::pair<int64_t, int64_t>>, 16> &arr) {
  for (auto &row : arr) {
    if (row.size() <= 1) continue;
    std::sort(row.begin(), row.end(), [](const auto &a, const auto &b) {
      return a.first < b.first || (a.first == b.first && a.second > b.second);
    });
    int64_t min_distance = row.back().second;
    for (int i = row.size() - 2; i >= 0; --i) {
      if (row[i].second >= min_distance) row.erase(row.begin() + i);
      else min_distance = row[i].second;
    }
  }
}

void clear() {
  for (auto &v : first_half) v.clear();
  for (auto &v : second_half) v.clear();
}

void solve() {
  clear();
  int nr_moves, nr_potions;
  int64_t distance, time;
  std::cin >> nr_moves >> nr_potions >> distance >> time;

  moves.resize(nr_moves);
  for (auto &m : moves) std::cin >> m.first >> m.second;

  potions.resize(nr_potions);
  for (auto &p : potions) std::cin >> p;
  std::sort(potions.begin(), potions.end());

  best_without_potion.fill(std::numeric_limits<int64_t>::max());
  dfs_fill(0, 0, distance, time, (nr_moves + 1) / 2, first_half);
  dfs_fill((nr_moves + 1) / 2, 0, distance, time, nr_moves, second_half);

  sort_and_remove_suboptimal(first_half);
  sort_and_remove_suboptimal(second_half);

  for (int i = 0; i <= (nr_moves + 1) / 2; i++) {
    for (const auto &elem : first_half[i]) {
      for (int j = 0; j <= nr_moves / 2; j++) {
        const auto &row = second_half[j];
        auto it = std::upper_bound(row.begin(), row.end(), time - elem.first, [](const auto &value, const auto &elem) {
          return value < elem.first;
        });
        if (it == row.end()) break;
        best_without_potion[i + j] = std::min(best_without_potion[i + j], elem.second + it->second - distance);
      }
    }
  }

  int64_t best = std::numeric_limits<int64_t>::max();
  for (int i = 1; i <= nr_moves; i++) {
    if (best_without_potion[i] <= 0) {
      std::cout << "0\n";
      return;
    }
    if (best_without_potion[i] != std::numeric_limits<int64_t>::max()) {
      int64_t needed_gain = (best_without_potion[i] - 1) / i + 1;
      auto it = std::lower_bound(potions.begin(), potions.end(), needed_gain);
      if (it != potions.end()) best = std::min(best, (int64_t)(it - potions.begin() + 1));
    }
  }

  if (best == std::numeric_limits<int64_t>::max()) {
    std::cout << "Panoramix captured" << std::endl;;
  } else {
    std::cout << best << std::endl;;
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

## 🧠 Learnings

<details> 

<summary> Expand to View </summary>

- `std::lower_bound` is a very nice and useful function if you need a quick and simple binary search.
- For problems which require exponential complexity (as they are NP-hard) split-and-list is probably necessary.

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