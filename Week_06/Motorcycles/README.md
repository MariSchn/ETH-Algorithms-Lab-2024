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
Each biker's path can be described by a linear equation of the form $y = mx + b$. The y-intercept, $b$, is simply the biker's starting y-coordinate. The slope, $m$, can be calculated from the given direction vector. The key question is: if two bikers' paths intersect, which one gets eliminated? The biker who reaches the intersection point first continues. How does the slope $m$ relate to how quickly a biker reaches an intersection point at some $x > 0$?
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
This problem can be solved by modeling the bikers' paths and applying a geometric insight to determine eliminations efficiently. The core idea is to process bikers in sorted order of their starting positions and track potential eliminators in two separate passes.

### Modeling the Bikers

Each biker starts at $(0, y_0)$ and rides in the direction of $(x_1, y_1)$. This defines a ray in the positive x-halfplane, which is part of a line with the equation $y = mx + b$.
- The **y-intercept $b$** is simply the starting coordinate $y_0$.
- The **slope $m$** is calculated as $\frac{\Delta y}{\Delta x} = \frac{y_1 - y_0}{x_1}$.

Since the coordinates can be large, using standard floating-point types like `double` for the slope can lead to precision errors. It is crucial to use a data structure that can represent fractions exactly, such as `CGAL::Gmpq` or a custom rational number class.

### The Key Insight: Elimination by Slope

All bikers travel at the same constant speed. Let this speed be $v$. The velocity vector for a biker with slope $m$ is proportional to $(1, m)$. The speed is the magnitude of this vector, $v = k \sqrt{1^2 + m^2}$ for some constant $k$. This implies that the horizontal component of the velocity is $v_x = \frac{v}{\sqrt{1+m^2}}$.

From this, we can see that a **smaller absolute slope $|m|$ results in a larger horizontal speed $v_x$**. When two bikers' paths intersect at some point $(x_{int}, y_{int})$ with $x_{int} > 0$, the biker with the larger horizontal speed will reach that x-coordinate first, and thus the intersection point first.

This leads to our fundamental elimination rule:
> At an intersection, the biker with the **smaller absolute slope `|m|`** wins and eliminates the biker with the larger absolute slope.

The problem's tie-breaking rule (biker from the right wins) applies when two bikers have equal absolute slopes. If bikers $i$ and $j$ have $|m_i| = |m_j|$, the one starting lower on the y-axis will be "to the right" of the other at any vertical line, so the lower biker wins the tie.

### The Two-Pass Algorithm

A naive $O(N^2)$ approach checking every pair of bikers is too slow for the given constraints. We can devise a more efficient, linear-time algorithm (after sorting) by processing the bikers in order of their starting y-coordinates. A biker can be eliminated by someone starting *below* them or someone starting *above* them. We handle these two scenarios in two separate passes.

First, we sort all bikers in ascending order based on their y-intercept $b$ (their starting $y_0$). We use a boolean array, `rides_forever`, initialized to `true` for all bikers.

#### Pass 1: Elimination from Below (Bottom-to-Top)

In this pass, we iterate through the sorted bikers from bottom to top and determine which ones are eliminated by bikers starting below them. We maintain a "champion" from below—the biker seen so far who is the most effective at eliminating others above them.

- **Who can eliminate whom?** For a biker $j$ starting below a biker $i$ ($y_{0,j} < y_{0,i}$), their paths can only intersect in the positive x-halfplane if $j$'s path converges towards $i$'s path. This happens if $m_j > m_i$.
- **Who is the champion?** The most dangerous eliminator from below is the one who wins all its encounters. This is the one with the minimum absolute slope among all potential eliminators.

Our algorithm iterates from the bottom-most biker upwards, maintaining the slope of the biker with the current minimum absolute slope encountered so far. Let's call this `min_abs_slope_below`.

For each biker `i`, we check if they can be eliminated by the current "champion" represented by `min_abs_slope_below`.
1.  **Intersection Condition**: An intersection with a biker below is possible only if the current biker's slope `m_i` is less than the champion's slope.
2.  **Elimination Condition**: If they intersect, we compare their absolute slopes. Biker `i` is eliminated if `abs(min_abs_slope_below) <= abs(m_i)`. The equality handles the tie-break, where the biker from below (the champion) wins.
3.  **Updating the Champion**: After checking biker `i`, we see if `i` can become a new champion. Biker `i` becomes the new reference if its absolute slope is smaller than the current `min_abs_slope_below`. This new champion will be used to test subsequent bikers further up the y-axis.

#### Pass 2: Elimination from Above (Top-to-Bottom)

This pass is symmetric to the first. We iterate through the sorted bikers from top to bottom to find eliminations caused by bikers starting above. We maintain a `min_abs_slope_above`.

For each biker `i`, we check for elimination by the champion from above.
1.  **Intersection Condition**: For a biker `j` above `i` ($y_{0,j} > y_{0,i}$), their paths intersect for $x>0$ if $m_j < m_i$.
2.  **Elimination Condition**: If they intersect, biker `i` is eliminated if `abs(min_abs_slope_above) < abs(m_i)`. Note the strict inequality: in a tie of absolute slopes, biker `i` (who is below) wins, so they are *not* eliminated.
3.  **Updating the Champion**: Biker `i` becomes the new champion from above if its absolute slope is smaller than the current `min_abs_slope_above`.

After both passes, any biker `i` for whom `rides_forever[i]` is still `true` is a survivor. We then print their original indices in sorted order.

### C++ Implementation
```cpp
#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <CGAL/Gmpq.h>

// A Biker is represented by their y-intercept, slope, and original index.
// Using CGAL::Gmpq for the slope is crucial to avoid floating-point precision issues.
typedef std::tuple<int64_t, CGAL::Gmpq, int> Biker;

void solve() {
    int n;
    std::cin >> n;

    std::vector<Biker> bikers;
    bikers.reserve(n);
    for (int i = 0; i < n; ++i) {
        int64_t y0, x1, y1;
        std::cin >> y0 >> x1 >> y1;
        CGAL::Gmpq slope(y1 - y0, x1);
        bikers.emplace_back(y0, slope, i);
    }

    // Sort bikers by their starting y-coordinate (y-intercept) in ascending order.
    std::sort(bikers.begin(), bikers.end());

    std::vector<bool> rides_forever(n, true);

    // Pass 1: Elimination from below (bottom to top)
    if (n > 0) {
        CGAL::Gmpq champ_slope = std::get<1>(bikers[0]);
        for (int i = 1; i < n; ++i) {
            CGAL::Gmpq current_slope = std::get<1>(bikers[i]);
            int original_index = std::get<2>(bikers[i]);

            // Paths intersect for x > 0 only if current biker's slope is less than champion's.
            if (champ_slope > current_slope) {
                // If they intersect, check who wins based on absolute slope.
                // Champion from below wins ties (<=).
                if (CGAL::abs(champ_slope) <= CGAL::abs(current_slope)) {
                    rides_forever[original_index] = false;
                } else {
                    champ_slope = current_slope; // Current biker becomes the new champion.
                }
            } else {
                // No intersection, but current biker might become a better champion
                // for bikers further up due to a smaller absolute slope.
                if (CGAL::abs(champ_slope) >= CGAL::abs(current_slope)) {
                    champ_slope = current_slope;
                }
            }
        }
    }
    
    // Pass 2: Elimination from above (top to bottom)
    if (n > 0) {
        CGAL::Gmpq champ_slope = std::get<1>(bikers[n - 1]);
        for (int i = n - 2; i >= 0; --i) {
            CGAL::Gmpq current_slope = std::get<1>(bikers[i]);
            int original_index = std::get<2>(bikers[i]);

            // Paths intersect for x > 0 only if current biker's slope is greater than champion's.
            if (champ_slope < current_slope) {
                // If they intersect, check who wins.
                // Champion from above loses ties (<).
                if (CGAL::abs(champ_slope) < CGAL::abs(current_slope)) {
                    rides_forever[original_index] = false;
                } else {
                    champ_slope = current_slope; // Current biker becomes new champion.
                }
            } else {
                // No intersection, but update champion if current biker is better.
                if (CGAL::abs(champ_slope) >= CGAL::abs(current_slope)) {
                    champ_slope = current_slope;
                }
            }
        }
    }

    // Output the original indices of surviving bikers.
    for (int i = 0; i < n; ++i) {
        if (rides_forever[i]) {
            std::cout << i << " ";
        }
    }
    std::cout << std::endl;
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