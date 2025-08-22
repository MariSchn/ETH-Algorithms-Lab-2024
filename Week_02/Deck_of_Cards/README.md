# Deck of Cards

## 📝 Problem Description

We are given a sequence of $n$ non-negative card values, $v_0, v_1, \dots, v_{n-1}$, and a target integer $k$. The task is to find a pair of indices $(i, j)$ with $i \le j$ that defines a contiguous block of cards. The goal is to select the pair $(i, j)$ such that the sum of the values in this block, $\sum_{\ell=i}^{j} v_\ell$, is as close as possible to $k$. I.e. find:

$$\min_{i \leq j} \left|k - \sum_{\ell=i}^{j} v_\ell\right|$$

If multiple pairs yield the same minimum difference, we must choose the pair that is lexicographically smallest.


## 💡 Hints

<details>

<summary>Hint #1</summary>

A straightforward approach is to check every possible contiguous subarray, calculate its sum, and see how close it is to $k$. This involves iterating through all possible start indices $i$ and end indices $j$. <br />
This solves the first few test sets. However, since the last test set has $n \leq 10^5$, a more efficient approach is needed. Try to think of a way to better go through the array without having to check every possible subarray explicitly. To be able to pass $n \leq 10^5$, you will need to find a solution with linear time complexity, $O(N)$.

</details>

<details>

<summary>Hint #2</summary>

 Consider maintaining a "window" (a subarray) defined by a `start` and `end` pointer. As you expand the window by incrementing `end`, the sum increases.

 - If the sum is less than $k$, you continue expanding as you need to get closer to $k$. <br />
 - If the sum exceeds $k$, you should shrink the window from the left by incrementing `start`. 
 
 This "sliding window" or "two-pointers" technique allows you to explore all relevant subarrays in a single pass.

</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Set 1, 2) </summary>

This solution uses a brute-force approach. We iterate through all possible start and end indices of a contiguous block of cards. For each block, we compute the sum of its card values and determine how close this sum is to the target value $k$. We keep track of the block that gives the minimum difference.

This approach has a time complexity of $O(N^2)$ due to the nested loops, which is sufficient for the first two test sets.

### Code
```cpp
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>

int main() {
  std::ios_base::sync_with_stdio(false);
  int n_tests; std::cin >> n_tests;
  
  for(int n_test = 0; n_test < n_tests; n_test++) {
    int n, k; std::cin >> n >> k;

    // Read cards
    std::vector<int> cards;
    for(int i = 0; i < n; i++) {
      int card; std::cin >> card;
      cards.push_back(card);
    }
    
    int i = 0;
    int j = 0;
    long long min_diff = std::numeric_limits<long long>::max();
    
    long long sum, diff, abs_diff;
    
    for(int start = 0; start < n; start++) {
      sum = 0;
      
      for(int end = start; end < n; end++) {
        sum += cards[end];
        diff = k - sum;
        abs_diff = std::abs(diff);
        
        if(abs_diff < min_diff) {
          i = start;
          j = end;
          min_diff = abs_diff;
        }
        
        if(diff < 0) {
          // Continute with the next starting position 
          break;
        }
      }
      
      if(min_diff == 0) {
        break;
      }
    }
    
    std::cout << i << " " << j << std::endl;
  }
}
```
</details>

<details>
<summary>Final Solution</summary>

Based on the input size, $n \leq 10^5$, we can already see that a brute-force solution with $O(N^2)$ complexity would be **too slow**. However, as we are looking for a contiguous subarray, we can utiliize the **sliding window**/**two pointers** technique to achieve go through the array in linear time, $O(N)$ while checking all possible subarrays. <br />
**Note**: We can only use a sliding window approach because all card values are non-negative, which guarantees that increasing the size of the window will never decrease the sum. If we had negative values, we would need to be more careful about how we expand and contract the window.

We use two pointers/variables, `start` and `end`, to define the boundaries of our current subarray. Using these two pointers, we iterate through the array and keep track of the `current_sum` between `start` and `end`. The iteration works as follows:

1. Start with both `start` and `end` at the beginning of the array.
2. Move `end` to the right, adding the value at `cards[end]` to `current_sum`.
3. Update `start` accordingly:
  - If `current_sum` is less than or equal to `k`, we need to increase the size of the window, so we move `end` to the right.
  - If `current_sum` is greater than `k`, we need to shrink the window, so we move `start` to the right and subtract `cards[start]` from `current_sum`.
4. After each adjustment, we check if the absolute difference between `current_sum` and `k` is smaller than the previously recorded minimum difference. If it is, we update our best indices.

By iterating through the input array in this way, we can ensure that we check all possible contiguous subarrays while maintaining an efficient time complexity of $O(N)$.

### Code
```cpp
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>


void solve() {       
  // ===== READ INPUT =====
  int n, k; std::cin >> n >> k;

  std::vector<int> cards(n);
  for(int i = 0; i < n; i++) {
    std::cin >> cards[i];
  }
  
  // ===== SOLVE =====
  // Sliding window approach
  int start = 0;
  long long current_sum = 0;

  // Track which window is the best so far
  int best_start = 0;
  int best_end = 0;
  long long min_diff = std::numeric_limits<long long>::max();
  
  for(int end = 0; end < n; end++) {
    // Add the new card to our window
    current_sum += cards[end];
    
    // Try to shrink the window from the left while maintaining or improving the result
    while(start <= end) {
      long long diff = std::abs(k - current_sum);
      
      // Check if current window is better
      if(diff < min_diff) {
        best_start = start;
        best_end = end;
        min_diff = diff;
        
        if(diff == 0) {
          std::cout << best_start << " " << best_end << std::endl;
          return;
        }
      }
      
      // If the current sum is smaller than k our window is too small -> Increment `end`
      if(current_sum <= k) {
        break;
      }
      // If the current sum is larger than k we need to shrink the window -> Increment `start`
      else {
        current_sum -= cards[start];
        start++;
      }
    }
  }

  std::cout << best_start << " " << best_end << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  int n_tests; std::cin >> n_tests;
  
  for(int n_test = 0; n_test < n_tests; n_test++) {
    solve();
  }
}
```

</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1        (20 pts / 1 s) : Correct answer      (0.0s)
   Test set 2        (40 pts / 1 s) : Correct answer      (0.007s)
   Test set 3        (20 pts / 1 s) : Correct answer      (0.133s)
   Hidden test set 1 (05 pts / 1 s) : Correct answer      (0.0s)
   Hidden test set 2 (10 pts / 1 s) : Correct answer      (0.007s)
   Hidden test set 3 (05 pts / 1 s) : Correct answer      (0.133s)

Total score: 100
```