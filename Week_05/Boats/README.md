# Boats

## 📝 Problem Description

The problem asks for the maximum number of boats that can be tied to a series of rings along a river bank without any boats overlapping.

We are given $N$ boats. For each boat $i$, we know its length, $l_i$, and the position of its assigned ring, $p_i$. A boat must be tied such that the ring is located somewhere along the boat's length, inclusive of its endpoints. This means if a boat of length $l_i$ is placed occupying the interval of coordinates $[s, s+l_i]$, its ring at position $p_i$ must satisfy $s \le p_i \le s+l_i$.

The key constraint is that no two boats can overlap. Their occupied intervals must be disjoint, although their endpoints are allowed to touch. The objective is to select the largest possible subset of boats and a valid, non-overlapping placement for each, then output the size of this subset.

## 💡 Hints

<details>
<summary>Hint #1</summary>
When dealing with problems involving intervals or objects positioned along a line, it's often beneficial to process them in a specific order rather than randomly. What property of the boats could we sort by to create a more structured approach to the problem?
</details>
<details>
<summary>Hint #2</summary>
Sorting the boats by their ring positions ($p_i$) from smallest to largest is a very effective strategy. This allows you to process the boats in a natural "left-to-right" order along the river bank. Now, as you iterate through the sorted boats, what greedy decision can you make at each step?
</details>
<details>
<summary>Hint #3</summary>
The core of a greedy strategy is to make a locally optimal choice that hopefully leads to a globally optimal solution. As you process boats sorted by their ring positions, your goal should be to place them in a way that leaves the most possible space for subsequent boats. This means you should always try to make the rightmost endpoint of your placed boats as far to the left as possible. What happens if a boat doesn't fit? Perhaps it can *replace* a previously placed boat if doing so results in a better (i.e., smaller) rightmost endpoint for your set of chosen boats.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
This problem can be solved efficiently using a greedy algorithm. The main idea is to process the boats in a specific order and make a locally optimal choice at each step. The choice should be guided by the heuristic of keeping the rightmost endpoint of all placed boats as far to the left as possible, thereby maximizing the available space for future boats.

### Algorithm Breakdown

1.  **Sorting:** First, we sort all the boats based on their ring position, $p$, in ascending order. This creates a natural left-to-right processing order.

2.  **Greedy Iteration:** We iterate through the sorted boats, maintaining a count of placed boats (`n_boats`) and the coordinates of the two most recent boat endpoints: `right_end` (the rightmost endpoint of the last placed boat) and `prev_right_end` (the rightmost endpoint of the second-to-last placed boat).

3.  **Decision Logic:** For each boat with length $l$ and ring position $p$, we have two main cases:

    *   **Case 1: The boat can be added.** This occurs if the boat's ring is not covered by the last boat we placed, i.e., `p >= right_end`. If so, we can add this boat to our set. We increment `n_boats`. To maintain our greedy strategy, we must place this new boat as far to the left as possible. Its starting position `s` must be at least `right_end` (to avoid collision) and also at least `p - l` (to reach its ring). Thus, the optimal start is `s = max(right_end, p - l)`. The new `right_end` becomes `s + l`. We also update `prev_right_end` to the old `right_end`.

    *   **Case 2: The boat cannot be added directly.** This occurs if `p < right_end`, meaning the current boat's ring is underneath the last boat we placed. We cannot simply add it. However, we can check if it's beneficial to **replace** the last boat with the current one. A replacement is beneficial if the resulting arrangement has a rightmost endpoint that is smaller than the current `right_end`. If we swap out the last boat, the available space starts after `prev_right_end`. The new boat would be placed with a start position `s = max(prev_right_end, p - l)`, leading to a new endpoint `e_new = s + l`. If `e_new < right_end`, we perform the replacement by updating `right_end` to `e_new`. The number of boats, `n_boats`, remains unchanged in a replacement.

By following this strategy, we ensure that at each step, we maintain an optimal configuration for the number of boats chosen so far, characterized by the smallest possible rightmost endpoint. This greedy approach is guaranteed to find the maximal number of boats.

### Implementation Details

The implementation uses a `std::vector` of pairs to store the boats, which is then sorted by the ring position. We then loop through the sorted vector, applying the logic described above. We initialize `right_end` and `prev_right_end` to a very small number (negative infinity) to handle the placement of the first boat correctly.

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

void solve() {
    // Read input for one test case
    int n;
    std::cin >> n;

    std::vector<std::pair<int, int>> boats;
    boats.reserve(n);
    for (int i = 0; i < n; ++i) {
        int l, p;
        std::cin >> l >> p;
        boats.emplace_back(l, p);
    }

    // Sort boats by ring position (the second element of the pair)
    std::sort(boats.begin(), boats.end(), [](const std::pair<int, int> &a, const std::pair<int, int> &b) {
        return a.second < b.second;
    });

    int n_boats = 0;
    // Use long long for endpoints to avoid overflow with large coordinates and lengths
    long long right_end = std::numeric_limits<long long>::min();
    long long prev_right_end = std::numeric_limits<long long>::min();

    for (const auto& boat : boats) {
        long long length = boat.first;
        long long ring_pos = boat.second;

        // Case 1: Ring is not covered, so we can potentially add this boat.
        if (ring_pos >= right_end) {
            n_boats++;
            prev_right_end = right_end;
            
            // Place the new boat as far left as possible.
            // Its start must be >= right_end and >= ring_pos - length.
            // New endpoint is max(right_end, ring_pos - length) + length.
            right_end = std::max(right_end, ring_pos - length) + length;

        } else {
            // Case 2: Ring is covered. Check if replacing the last boat is better.
            // A replacement is better if the new right_end is smaller.
            long long potential_new_right_end = std::max(prev_right_end, ring_pos - length) + length;
            
            if (potential_new_right_end < right_end) {
                // The replacement is beneficial. Update right_end.
                // n_boats does not change.
                right_end = potential_new_right_end;
            }
        }
    }

    std::cout << n_boats << std::endl;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

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
   Test set 1 (30 pts / 2 s) : Correct answer      (0.002s)
   Test set 2 (30 pts / 2 s) : Correct answer      (0.005s)
   Test set 3 (40 pts / 2 s) : Correct answer      (0.674s)

Total score: 100
```