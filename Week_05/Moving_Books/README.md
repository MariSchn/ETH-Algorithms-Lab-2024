# Moving books

## 📝 Problem Description

You are given a set of $N$ friends and $M$ boxes. Each friend $i$ has an associated strength $s_i$, and each box $j$ has an associated weight $w_j$. A friend can carry a box only if their strength is greater than or equal to the box's weight.

The task is to move all $M$ boxes from an apartment to a moving van. Each friend can carry at most one box at a time. The time it takes for any friend to carry a box to the van is 2 minutes, and the time to return to the apartment is 1 minute. All friends start at the same time, and the process is complete once the last box reaches the van.

Your goal is to determine the minimum amount of time required to move all $M$ boxes. If it's not possible for the given friends to move all the boxes (i.e., some box is heavier than the strongest friend), you should indicate that the task is impossible.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The total time for the entire operation is determined by the friend who makes the most trips. If a friend needs to make $k$ trips, what is the total time they will spend? Remember that they don't need to return to the apartment after their last delivery.
</details>

<details>
<summary>Hint #2</summary>
Consider the relationship between the number of trips and feasibility. If all boxes can be moved when each friend makes at most $k$ trips, can they also be moved if each friend is allowed to make $k+1$ trips? This monotonic property often suggests a powerful search algorithm. Instead of searching for the optimal time directly, perhaps you can search for the optimal number of trips.
</details>

<details>
<summary>Hint #3</summary>
To check if a plan is feasible for a given maximum number of trips, say $k$, you need to assign boxes to friends effectively. To maximize your chances, who should carry the heaviest boxes? Sorting the friends by strength and the boxes by weight is a very good first step. Once sorted, how can you quickly verify if an assignment is possible for the $k$ trips limit?
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>

This problem asks for the minimum time to move $M$ boxes using $N$ friends. The core of the problem is to find an optimal assignment of boxes to friends that minimizes the total duration.

### From Time to Trips

First, let's analyze the time calculation. The total time is determined by the friend who performs the most trips. A single trip consists of carrying a box down (2 minutes) and returning up (1 minute), totaling 3 minutes. However, after delivering their final box, a friend does not need to return.

If a friend makes a maximum of $k$ trips, the total time elapsed will be:
$$(k-1) \times 3 \text{ minutes (for the first k-1 trips)} + 2 \text{ minutes (for the final trip)}$$
This simplifies to a total time of $3k - 1$ minutes. Therefore, **minimizing the total time is equivalent to minimizing the maximum number of trips, $k$, that any friend has to make.**

### Binary Searching on the Answer

The problem now becomes finding the smallest integer $k$ such that it's possible to move all $M$ boxes with no friend making more than $k$ trips.

Let's define a function, `is_possible(k)`, that returns `true` if all boxes can be moved with a trip limit of $k$, and `false` otherwise. Notice that this function has a monotonic property: if `is_possible(k)` is `true`, then `is_possible(k+1)` must also be `true`. This structure is a perfect fit for **binary search on the answer**, $k$.

We can search for the optimal $k$ within a range.
*   A lower bound for $k$ is $\lceil M/N \rceil$, which is the number of trips required if the work is distributed as evenly as possible among all friends.
*   An upper bound is $M$, which corresponds to the case where a single friend moves all the boxes.

### The `is_possible(k)` Check

The crucial part is to implement the `is_possible(k)` check efficiently. To give ourselves the best chance of success for a given $k$, we should use our strongest friends to carry the heaviest boxes. This suggests a **greedy approach**.

1.  **Sort:** Sort the friend strengths and box weights in descending order. This lets us easily access the strongest friends and heaviest boxes.
2.  **Check for Impossibility:** As a preliminary step, if the strongest friend cannot lift the heaviest box (`strengths[0] < weights[0]`), the task is impossible. We can report this and terminate.
3.  **Simulate Assignment:** For a given $k$, each friend can carry at most $k$ boxes. This means we will need at least $f = \lceil M/k \rceil$ friends to move all the boxes. If $f > N$, it's impossible with trip limit $k$. Otherwise, we must use our $f$ strongest friends.
    To verify if these $f$ friends are strong enough, we can assign the boxes greedily. We can think of this as giving the $k$ heaviest boxes to the strongest friend, the next $k$ heaviest to the second-strongest, and so on.
    
    A simpler way to check this is to realize that the $i$-th strongest friend (at index `i-1` in our sorted array) will be tasked with carrying boxes from a batch that is "easier" than the batch for the $(i-1)$-th strongest friend. The most difficult box the $i$-th friend would ever have to carry in this scheme is the $i \times k$-th heaviest box overall (using 0-based indexing).
    
    Therefore, our check is: for each required friend `i` from `0` to $f-1$, we must ensure that `strengths[i] >= weights[i * k]`. If this condition fails for any `i`, then `is_possible(k)` is `false`. If it holds for all required friends, it's `true`.

By combining the binary search with this greedy check, we can efficiently find the minimum required number of trips, $k^*$, and the final answer will be $3k^* - 1$.

```cpp
#include<iostream>
#include<vector>
#include<algorithm>
#include<cmath>

void solve() {
    // ===== READ INPUT =====
    int n_friends, n_books; std::cin >> n_friends >> n_books;
    
    std::vector<int> strengths(n_friends), weights(n_books);
    
    for(int i = 0; i < n_friends; ++i) std::cin >> strengths[i];
    for(int i = 0; i < n_books; ++i) std::cin >> weights[i];

    
    // ===== CALCULATE SOLUTION =====
    std::sort(strengths.begin(), strengths.end(), std::greater<int>());
    std::sort(weights.begin(), weights.end(), std::greater<int>());

    if (strengths[0] < weights[0]) {
      std::cout << "impossible" << std::endl;
      return;
    }
    
    int low = std::ceil(static_cast<double>(n_books) / static_cast<double>(n_friends));
    int high = n_books;
    
    while(low < high) {
      int middle = std::floor((low + high) / 2);
      int used_friends = std::ceil(static_cast<double>(n_books) / static_cast<double>(middle));
      
      bool can_carry = true;
      
      for(int i = 0; i < used_friends; i++) {
        if(weights[i * middle] > strengths[i]) {
          can_carry = false;
          break;
        }
      }
      
      if(can_carry) {
        high = middle;
      } else {
        low = middle+1;
      }
    }

    // ===== OUTPUT =====
    std::cout << low * 3 - 1 << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (20 pts / 1.500 s) : Correct answer      (0.004s)
   Test set 2 (40 pts / 1.500 s) : Correct answer      (0.242s)
   Test set 3 (40 pts / 1.500 s) : Correct answer      (0.439s)

Total score: 100
```