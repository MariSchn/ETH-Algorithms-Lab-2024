# Hiking Maps

## 📝 Problem Description

The objective is to find the minimum cost to purchase a set of map parts that collectively cover a given hiking path. The path is defined as a sequence of $m-1$ straight line segments, connecting a series of $m$ coordinate points $p_0, p_1, \dots, p_{m-1}$.

There are $n$ available map parts, $t_0, t_1, \dots, t_{n-1}$, each represented by a triangular region. These map parts can only be purchased in a single contiguous block. That is, for some starting index $b$ and ending index $e$, one can buy the set of maps $\{t_b, t_{b+1}, \dots, t_{e-1}\}$. The cost of this purchase is the number of maps bought, which is $e-b$.

A set of purchased maps is considered valid if it covers the entire hiking path. This means that for every single leg of the hike (the segment from $p_i$ to $p_{i+1}$), there must be at least one purchased map part $t_j$ that completely contains this leg. The map parts themselves are described not by their vertices, but by six points, where two distinct points lie on the interior of each of the triangle's three edges.

The goal is to find the smallest possible cost, i.e., the length of the shortest contiguous sequence of map parts that provides full coverage for the hiking path.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem asks for the shortest <strong>contiguous</strong> sub-sequence of map parts that satisfies a certain property (covering the entire path). What is a common algorithmic technique for finding optimal contiguous sub-arrays or sub-sequences?
</details>
<details>
<summary>Hint #2</summary>
Consider a "sliding window" approach, where a window `[left, right]` represents the map parts $t_{left}, \dots, t_{right}$. A window is "valid" if all path legs are covered. As you expand the window by incrementing `right` or shrink it by incrementing `left`, you need a way to efficiently check if the window remains valid. How can you track which legs are covered without re-scanning all maps in the window each time?
</details>
<details>
<summary>Hint #3</summary>
The main geometric challenge is to determine if a leg $(p_i, p_{i+1})$ is contained within a triangle $t_j$. A key property of triangles is that they are <em>convex</em> shapes. What does this imply about containing a line segment? If a convex shape contains the two endpoints of a line segment, it must also contain the entire segment. This simplifies the problem from segment-in-triangle to point-in-triangle.
</details>
<details>
<summary>Hint #4</summary>
How can we efficiently check if a point is inside a triangle when we are only given six points on its edges, not its vertices? Calculating the vertices by intersecting lines is possible but can be slow and complex. A better approach is to use orientation tests. A point is inside a triangle if it lies on the "same side" of all three of the triangle's edges. To make this work, you first need to orient the edges consistently (e.g., all clockwise). You can achieve this by using a point on a third edge to determine the orientation of the other two.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
This problem combines a geometric component with a search algorithm. The overall task is to find the shortest contiguous sub-sequence of map parts, $t_{b}, \dots, t_{e-1}$, that covers an entire hiking path.

### High-Level Strategy

We can break the problem into two main parts:
1.  **Geometric Check:** An efficient method to determine if a given triangular map part covers a specific leg of the hike.
2.  **Search Algorithm:** A way to find the shortest contiguous block of maps that satisfies the covering condition for all legs.

The requirement of a *contiguous* block strongly suggests a **sliding window** algorithm.

### Geometric Containment Check

The first challenge is to check if a leg, which is a line segment, is contained within a triangle.
A crucial observation is that triangles are **convex**. For any convex shape, if it contains the two endpoints of a line segment, it must also contain the entire segment. Therefore, the problem of checking if leg $(p_i, p_{i+1})$ is contained in triangle $t_j$ reduces to two simpler checks:
1. Is point $p_i$ contained in triangle $t_j$?
2. Is point $p_{i+1}$ contained in triangle $t_j$?

The next challenge is performing this point-in-triangle test. The input format is unusual: each triangle is defined by six points lying on its edges, not its vertices. Calculating the triangle's vertices by intersecting the lines defined by these points is computationally expensive and can introduce precision issues.

A more robust method is to use **orientation tests**. A point $P$ is inside a triangle if and only if it lies on the same side of all three of the triangle's edges. For example, if we traverse the triangle's boundary in a clockwise direction, point $P$ must always be to our right.

To implement this, we first need to establish a consistent orientation for the edges of each triangle. The input gives us pairs of points for each edge, like $(q_0, q_1)$, but the direction from $q_0$ to $q_1$ might not be consistent with a clockwise traversal. We can enforce a consistent orientation (e.g., clockwise) as a preprocessing step. For a triangle with edges $A, B, C$, we can orient edge $A$ (defined by points $a_1, a_2$) by checking its orientation relative to a point on edge $B$ (say, $b_1$). If `CGAL::left_turn(a_1, a_2, b_1)` is true, it means traversing from $a_1$ to $a_2$ results in a left turn towards the rest of the triangle, implying a counter-clockwise orientation. To enforce a clockwise orientation, we would swap the points to $(a_2, a_1)$. We repeat this process for all three edges to ensure they are all oriented consistently.

Once all edges are oriented clockwise, a point $P$ is inside or on the boundary of the triangle if it is *not* to the left of any edge. This can be checked with `!CGAL::left_turn(edge_p1, edge_p2, P)`. Using "not left" instead of "right" correctly handles cases where $P$ is collinear with an edge.

### Sliding Window Algorithm

With the geometric check in place, we can find the shortest valid sequence of maps.

1.  **Preprocessing:** For each map part $t_i$ and each path leg $l_j$, we precompute whether $t_i$ covers $l_j$. We can store this information in a structure like `std::vector<std::vector<int>> covers`, where `covers[i]` contains the indices of all legs covered by map $t_i$. This step takes $O(n \cdot m)$ time.

2.  **Sliding Window:** We use a window `[left, right]` over the array of map parts. We also need two auxiliary data structures:
    *   `times_covered[m-1]`: An array where `times_covered[j]` stores how many maps currently in our window cover leg $l_j$.
    *   `n_uncovered`: A counter for the number of legs that are covered zero times by the maps in the window.

The algorithm proceeds as follows:
*   Initialize `left = 0`, `right = 0`, and `n_uncovered = m - 1`.
*   Expand the window by incrementing `right`. For the new map $t_{right}$, iterate through all legs it covers (using our precomputed table). For each such leg $j$, increment `times_covered[j]`. If `times_covered[j]` becomes 1, it means this leg just went from uncovered to covered, so we decrement `n_uncovered`.
*   Once `n_uncovered` reaches 0, the window `[left, right]` is valid and covers the entire path. We record its length, `right - left + 1`, and update our minimum answer.
*   Now, we shrink the window from the left by incrementing `left`. For the old map $t_{left}$, we iterate through the legs it covered. For each such leg $j$, we decrement `times_covered[j]`. If `times_covered[j]` becomes 0, it means the leg is now uncovered, so we increment `n_uncovered`.
*   We continue shrinking the window until it is no longer valid (`n_uncovered > 0`). Then, we go back to expanding with `right`.

This process continues until `right` reaches the end of the map list. The overall time complexity of the sliding window part is $O(n)$, as both `left` and `right` pointers only move forward. The total complexity is dominated by the preprocessing step, resulting in $O(n \cdot m)$.

**Code**
```cpp
#include <iostream>
#include <vector>
#include <limits>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point;

// Checks if a point is inside of a triangle, by checking if it is on the right side of every edge of the triangle
// This assumes that the individual edges are oriented such that right is inside and left is outside
bool contains(const std::vector<Point> &triangle, const Point &point) {
  return (!CGAL::left_turn(triangle[0], triangle[1], point) &&
          !CGAL::left_turn(triangle[2], triangle[3], point) &&
          !CGAL::left_turn(triangle[4], triangle[5], point));
}

void solve() {
  // ===== READ INPUT =====
  int m, n; std::cin >> m >> n;
  
  // Read hiking path
  std::vector<Point> path; path.reserve(m);
  for(int i = 0; i < m; ++i) {
    int x, y; std::cin >> x >> y;
    path.emplace_back(x, y);
  }
  
  // Read map parts
  std::vector<std::vector<Point>> triangles(n);
  for(int i = 0; i < n; ++i) {
    // Read all points
    triangles[i].reserve(6);
    for(int j = 0; j < 6; ++j) {
      int x, y; std::cin >> x >> y;
      triangles[i].emplace_back(x, y);
    }
  }
  
  // ===== SOLVE =====
  // Ensure correct orientation for all lines/point pairs (Right Side -> Inside, Left Side -> Outside)
  for(int triangle_idx = 0; triangle_idx < n; ++triangle_idx) {
    // Check for line a and swap if orientation is wrong
    if(!CGAL::right_turn(triangles[triangle_idx][0], triangles[triangle_idx][1], triangles[triangle_idx][2])) {
      std::swap(triangles[triangle_idx][0], triangles[triangle_idx][1]);
    } 
    // Check for line b and swap if orientation is wrong
    if(!CGAL::right_turn(triangles[triangle_idx][2], triangles[triangle_idx][3], triangles[triangle_idx][4])) {
      std::swap(triangles[triangle_idx][2], triangles[triangle_idx][3]);
    } 
    // Check for line b and swap if orientation is wrong
    if(!CGAL::right_turn(triangles[triangle_idx][4], triangles[triangle_idx][5], triangles[triangle_idx][0])) {
      std::swap(triangles[triangle_idx][4], triangles[triangle_idx][5]);
    } 
  }
  
  // Pre compute, which map part contains covers which part of the hiking path
  std::vector<std::vector<int>> covers(n);
  for(int triangle_idx = 0; triangle_idx < n; ++triangle_idx) {
    for(int path_idx = 0; path_idx < m - 1; ++path_idx) {
      // Check if the leg (path_idx, path_idx + 1) is covered by the triangle
      if(contains(triangles[triangle_idx], path[path_idx]) && contains(triangles[triangle_idx], path[path_idx + 1])) {
        covers[triangle_idx].push_back(path_idx);
      }
    }
  }
    
  // Sliding window over map parts to find smallest
  std::vector<int> times_covered(m - 1, 0);  // Stores for each leg of the path how often it is covered by the current window
  int n_uncovered = m - 1;                   // Number of legs that are currently not covered by the current window
  int left = 0;
  int min_size = std::numeric_limits<int>::max();
  
  for(int right = 0; right < n; ++right) {
    // Add the triangle[right] to the current window
    for(const int &leg_idx : covers[right]) {
      times_covered[leg_idx]++;
      
      // If the leg was previously not covered, decrease the n_uncovered counter
      if(times_covered[leg_idx] == 1) { n_uncovered--; } 
    }

    // Move left pointer until the not the entire path is covered
    while(n_uncovered == 0) {
      min_size = std::min(min_size, right - left + 1);
      
      // Remove the triangle[left] from the current window
      for(const int &leg_idx : covers[left]) {
        times_covered[leg_idx]--;
        
        // If the leg was is no longer covered, increase the n_uncovered counter
        if(times_covered[leg_idx] == 0) { n_uncovered++; } 
      }
      
      left++;
    }
  }
  
  // ===== OUTPUT =====
  std::cout << min_size << std::endl;
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
   Test set 1 (20 pts / 0.400 s) : Correct answer      (0.004s)
   Test set 2 (20 pts / 0.400 s) : Correct answer      (0.007s)
   Test set 3 (20 pts / 0.400 s) : Correct answer      (0.064s)
   Test set 4 (20 pts / 0.400 s) : Correct answer      (0.18s)
   Test set 5 (20 pts / 0.400 s) : Correct answer      (0.114s)

Total score: 100
```