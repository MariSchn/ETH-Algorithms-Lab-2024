# Motorcycles

## 📝 Problem Description

This problem involves a set of $n$ bikers, each starting at a unique position on the y-axis. All bikers begin simultaneously, travel at the same constant speed, and move in a straight line into the positive x-halfplane. The path of each biker is defined by their starting point $(0, y_0)$ and a direction specified by a second point $(x_1, y_1)$, where $x_1 > 0$.

The core of the challenge lies in the rules of elimination. A biker's journey ends if they reach a point that has been previously traveled by another biker. This is described as hitting another's "tracks". In the special case where two bikers arrive at the same location at the exact same time, a tie-breaking rule applies: the biker who approaches the intersection point from the right gets to continue, while the other stops.

The goal is to determine which bikers are never eliminated and thus "ride forever". The output should be a space-separated list of the original indices of these surviving bikers, sorted in ascending order.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The path of each biker is a straight line starting from the y-axis. How can you represent these paths mathematically? Since all bikers travel at the same constant total speed, consider what this implies for their individual speed components in the horizontal and vertical directions. A biker whose path is more horizontal will cover ground in the x-direction faster than a biker whose path is more vertical.

</details>

<details>

<summary>Hint #2</summary>

Each biker's path can be described by a linear equation of the form $y = mx + b$. The y-intercept, $b$, is simply the biker's starting y-coordinate. The slope, $m$, can be calculated from the given direction vector. However, also consider what data type we might need for the slope.
The key question is: if two bikers' paths intersect, which one gets eliminated? The biker who reaches the intersection point first continues. How does the slope $m$ relate to how quickly a biker reaches an intersection point at some $x > 0$?

</details>

<details>

<summary>Hint #3</summary>

A biker with a smaller absolute slope, $|m|$, has a larger horizontal velocity component. This means they will always reach any given x-coordinate faster than a biker with a larger absolute slope. Consequently, at any intersection point, **the biker with the smaller absolute slope eliminates the one with the larger absolute slope.** This is the primary rule of elimination. The tie-breaking rule for bikers arriving at the same time applies only when their absolute slopes are equal. How can you use this insight to build an efficient algorithm?

</details>

<details>

<summary>Hint #4</summary>

A brute-force comparison of all pairs of bikers would be too slow ($O(N^2)$). To optimize, we can process the bikers in a structured manner. Try sorting all bikers based on their starting y-coordinate. A biker can be eliminated either by someone starting below them or by someone starting above them. This suggests that we might need to handle these two cases. Consider two separate passes over the sorted bikers: one from bottom-to-top to handle eliminations from below, and another from top-to-bottom to handle eliminations from above. In each pass, how can you keep track of the "most dangerous" biker seen so far?

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>
This problem is solved by modeling the bikers' paths as lines and using geometric reasoning to efficiently determine which bikers survive.
### 1. Representing Biker Paths

Each biker starts at $(0, y_0)$ and rides in the direction of $(x_1, y_1)$. This defines a ray in the positive x-halfplane, which can be described by the line $y = mx + b$:
- The **y-intercept $b$** is the starting $y_0$.
- The **slope $m$** is $\frac{y_1 - y_0}{x_1}$.

To avoid precision issues with large coordinates, we need to use `CGAL::Gmpq` to store slopes as exact rational numbers.

### The Key Insight: Elimination by Slope

All bikers move at the same speed, but the biker with the smaller absolute slope $|m|$ has a greater horizontal velocity and will reach any intersection point first. Thus, at any intersection, the biker with the smaller $|m|$ eliminates the other. If two bikers have the same $|m|$, the one starting lower on the y-axis (smaller $y_0$) wins the tie.

### The Two-Pass Algorithm


Checking all pairs would be too slow. Instead, the solution sorts all bikers by their starting $y_0$ (ascending) and uses two passes to check for eliminations. Here is a more detailed breakdown:

#### Pass 1: Top-to-Bottom (Elimination from Above)

**Goal:** Identify bikers eliminated by someone starting above them.

**Process:**
1. Start with the top-most biker (largest $y_0$) and move down.
2. Maintain the current "champion from above": the biker with the lowest absolute slope $|m|$ seen so far, which eliminates all bikers with higher absolute slope.
3. For each biker $i$:
  - If the champion's slope is greater than $m_i$, their paths may intersect in the positive $x$-halfplane.
  - If $|m_{\text{champ}}| \leq |m_i|$, biker $i$ is eliminated (the champion reaches any intersection first, or wins the tie).
  - If not eliminated, update the champion if $|m_i|$ is smaller or equal to the current champion's $|m|$.
4. If the champion's slope is not greater, update the champion if $|m_i|$ is smaller or equal.

**Why this works:**
Only bikers above can eliminate those below, and only if their path is "converging" (i.e., the champion's slope is greater than the current biker's). The lowest $|m|$ always wins at intersections.

#### Pass 2: Bottom-to-Top (Elimination from Below)

**Goal:** Identify bikers eliminated by someone starting below them.

**Process:**
1. Start with the bottom-most biker (smallest $y_0$) and move up.
2. Maintain the current "champion from below": the biker with the lowest absolute slope $|m|$ seen so far.
3. For each biker $i$:
  - If the champion's slope is less than $m_i$, their paths may intersect.
  - If $|m_{champ}| < |m_i|$, biker $i$ is eliminated (the champion reaches the intersection first; note the strict inequality, so ties are not eliminated in this pass).
  - If not eliminated, update the champion if $|m_i|$ is smaller or equal to the current champion's $|m|$.
4. If the champion's slope is not less, update the champion if $|m_i|$ is smaller or equal.

**Why this works:**
Only bikers below can eliminate those above, and only if their path is "converging" (i.e., the champion's slope is less than the current biker's). The strict inequality ensures that in a tie, the biker above survives (matching the problem's tie-breaking rule).

### Code
```cpp
#include<iostream>
#include<vector>
#include <tuple>
#include <algorithm>
#include <CGAL/Gmpq.h>

// Intercept, Slope, Index
typedef std::tuple<int64_t, CGAL::Gmpq, int> Biker;

void solve() {
  // ===== READ INPUT =====
  int n; std::cin >> n;
  
  std::vector<Biker> bikers; bikers.reserve(n);
  for(int i = 0; i < n; ++i) {
    int64_t y_0, x_1, y_1; std::cin >> y_0 >> x_1 >> y_1;
    bikers.emplace_back(y_0, CGAL::Gmpq(y_1 - y_0, x_1), i);
  }
  
  // ===== SOLVE ======
  // Sort bikers descendingly by their intercept
  std::sort(bikers.begin(), bikers.end(), [](const Biker &a, const Biker &b){
    return std::get<0>(a) < std::get<0>(b);
  });
  
  std::vector<bool> rides_forever(n, true);
  // Iterate from the top and eliminate all riders that can not ride forever
  CGAL::Gmpq lowest_abs_slope = std::get<1>(bikers[0]);
  for(int i = 1; i < n; ++i) {
    CGAL::Gmpq curr_slope = std::get<1>(bikers[i]);
    
    // Check if the riders will intersect (either both drive up or both drive down)
    if(lowest_abs_slope > curr_slope) {
      // The bikers will cross -> Check who will continue by determining which has the lower abs slope
      if(CGAL::abs(lowest_abs_slope) <= CGAL::abs(curr_slope)) {
        rides_forever[std::get<2>(bikers[i])] = false;
      } else {
        lowest_abs_slope = curr_slope;
      }
    } else {
      // The biker[i] has a lower slope, so it either drives furhter down or is a new `lowest_abs_slope`
      if(CGAL::abs(lowest_abs_slope) >= CGAL::abs(curr_slope)) {
        lowest_abs_slope = curr_slope;
      }
    }
  }
  
  // Iterate from the bottom and eliminate all riders that can not ride forever
  // Basically the exact same as the previous loop just from the bottom
  lowest_abs_slope = std::get<1>(bikers[n-1]);
  for(int i = n - 2; i >= 0; --i) {
    CGAL::Gmpq curr_slope = std::get<1>(bikers[i]);

    if(lowest_abs_slope < curr_slope) {
      if(CGAL::abs(lowest_abs_slope) < CGAL::abs(curr_slope)) {
        rides_forever[std::get<2>(bikers[i])] = false;
      } else {
        lowest_abs_slope = curr_slope;
      }
    } else {
      if(CGAL::abs(lowest_abs_slope) >= CGAL::abs(curr_slope)) {
        lowest_abs_slope = curr_slope;
      }
    }
  }
  
  // ===== OUTPUT =====
  for(int i = 0; i < n; ++i) {
    if(rides_forever[i]) {
      std::cout << i << " ";
    }
  }
  std::cout << std::endl;
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
   Test set 1        (20 pts / 4 s) : Correct answer      (0.013s)
   Hidden test set 1 (05 pts / 4 s) : Correct answer      (0.013s)
   Test set 2        (20 pts / 4 s) : Correct answer      (0.04s)
   Hidden test set 2 (05 pts / 4 s) : Correct answer      (0.04s)
   Test set 3        (20 pts / 4 s) : Correct answer      (0.145s)
   Hidden test set 3 (05 pts / 4 s) : Correct answer      (0.145s)
   Test set 4        (20 pts / 4 s) : Correct answer      (1.007s)
   Hidden test set 4 (05 pts / 4 s) : Correct answer      (1.007s)

Total score: 100
```