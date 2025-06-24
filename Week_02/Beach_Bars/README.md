# Beach Bars

## 📝 Problem Description

The problem involves a set of parasols positioned along a beach, represented as a one-dimensional coordinate system. Each parasol has a coordinate $x_i$, which is a distance from the parking lot, and can be positive or negative. <br />
Given the coordinates of $n$ parasols, the task is to **find the optimal location(s) for a beach bar**. A location is considered **optimal** if it **maximizes the number of parasols** within a distance of 100 meters. <br />
If multiple locations maximize the number of parasols, the location(s) with the **smallest maximum distance** to a parasol within 100 meters are chosen. 

## 💡 Hints

<details><summary>Hint #1</summary>

The parasol positions are given in a random order. This makes it very hard to process efficiently, since we can not exploit any structure in the input. <br />
To solve the problem efficiently, you should first **sort the parasol positions**. This will allow you to easily determine which parasols are within 100 meters of a given location. <br />

</details>

<details><summary>Hint #2</summary>

Iterating through all possible locations, meter-by-meter, is not efficient enough. However, moving one meter at a time is not really necessary, as we have a range/window of 200 meters to consider. Try to find a way to **efficiently enumerate all possible locations** that are relevant.

</details>

<details><summary>Hint #3</summary>

To efficiently enumerate all possible (valid, relevant) locations, you can use a **sliding window** approach. Then you only need to find the optimal location(s) among the valid windows.

</details>

<details><summary>Hint #4</summary>

For every window there are 4 cases to consider:
1. The current window has **more parasols** than the best found so far.
2. The current window has the **same number of parasols**, but a **smaller maximum distance** to a parasol.
3. The current window has the **same number of parasols** and the **same maximum distance** to a parasol. <br />
4. The current window has **less parasols** than the best found so far. <br />

</details>

<details><summary>Hint #5</summary>

Have you handled odd-sized windows correctly? In all cases where it is necessary to consider?

</details>

## ✨ Solutions

<details><summary>Final Solution</summary>

The **core idea** behind the solution is to use a **sliding window** to efficiently go through all the parasol positions and determine the optimal location(s).

To be able to apply a sliding window approach, the parasol positions first need to be sorted, to ensure that they appear in *increasing order*. <br />
Using a sliding window approach, we use two pointers `left` and `right` to define a range of parasols. For this range/window we can easily calculate:

* The **number of parasols in the window**, which corresponds to the size of the window. (`amount = right - left + 1;`)
* The **size of the window**, which is the distance between the leftmost and rightmost parasol in the window. (`size = std::abs(parasols[right] - parasols[left]);`)
* The **maximum distance to a parasol within the window**, which is half the size of the window, rounded up. (`max_dist = std::ceil(static_cast<float>(size) / 2);`). <br />
This is because the optimal location for a given window is in the middle of it. We only need to round up, for the case of an odd-sized window, to ensure that the maximum distance is correctly calculated.

Based on these values, we can then determine how to update our optimal locations:

* If the **current window has more parasols** than the best found so far, we need to store this new maximum and clear the list of all previous optimal locations, as we have a new maximum.
* If the **current window has the same number of parasols as the best found so far, but a smaller maximum distance**, we need to do the same, as we found a better optimum.
* If the **current window has the same number of parasols and the same maximum distance**, we need to add the current location to our list of optimal locations (`optimal_x`). <br />
The optimal location is either the midpoint of the window (if the size is even) or two

**Note**: We also need to make sure that all invalid windows are skipped, i.e., if the size of the window exceeds 200 meters, we can immediately advance the `left` pointer to reduce the window size. 

If we iterate over the entire (sorted) list of parasols, this will ensure that we find all optimal locations, as we have considered all valid windows, and determined the largest number of parasols within 100 meters, as well as the smallest maximum distance to a parasol, among all the valid windows.


```c++
#include<iostream>
#include<vector>
#include<cmath>
#include<algorithm>


int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    
    std::vector<int> parasols(n);
    for(int i = 0; i < n; i++) {
      std::cin >> parasols[i];
    }
    
    // ===== SOLVE =====
    std::sort(parasols.begin(), parasols.end());
  
    // Initialize variables to track the best solution
    int max_n = 0;              // Maximum number of parasols within 100 meters
    int min_dist = 100;         // Minimum maximum distance to a parasol within 100 meters
    std::vector<int> optimal_x; // Vector to store optimal locations
    
    // Setup sliding window
    int left = 0;
    int right = 0;
    
    while(left < n && right < n) {
      int amount = right - left + 1;                          // Number of parasols in the current window
      int size = std::abs(parasols[right] - parasols[left]);  // Size of the current window
      int max_dist = std::ceil(static_cast<float>(size) / 2); // Maximum distance to a parasol within the current window
      
      // If window is too big, instantly skip and reduce window size by advancing left
      if(size >  200) {
        left++;
        continue;
      }
      
      // Clear optimal_x vector if new optimum is found
      // Found more parasols
      if(amount > max_n) {
        optimal_x.clear();
        
        max_n = amount;
        min_dist = max_dist;
      }
      // Found same amount of parasols, but smaller maximum distance
      if(amount == max_n && max_dist < min_dist) {
        optimal_x.clear();
        
        min_dist = max_dist;
      }
      
      // Add current window if it is optimal
      if(amount == max_n && max_dist == min_dist) {
        // dist is even -> Unique midpoint (optimum)
        // dist is odd  -> Two midpoints (optimums)
        if(size % 2 == 0) {
          optimal_x.push_back(static_cast<float>(parasols[right] + parasols[left]) / 2);
        } else {
          optimal_x.push_back(std::floor(static_cast<float>(parasols[right] + parasols[left]) / 2));
          optimal_x.push_back(std::ceil(static_cast<float>(parasols[right] + parasols[left]) / 2));
        }
      }

      // Advance window
      if(right == n - 1) left++;
      else right++;
    }
    
    // ===== OUTPUT =====
    std::cout << max_n << " " << min_dist << std::endl;
    for(const int& x : optimal_x) {
      std::cout << x << " ";
    }
    std::cout << std::endl;
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Testing solution >>>>
   Test set 1 (20 pts / 1 s) : Correct answer      (0.001s)
   Test set 2 (20 pts / 1 s) : Correct answer      (0.002s)
   Test set 3 (60 pts / 1 s) : Correct answer      (0.393s)

Total score: 100
```