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

How can we efficiently check if a point is inside a triangle when we are only given six points on its edges, not its vertices? Calculating the vertices by intersecting lines is possible but can be slow and complex, especially since it would require exact constructions. 

A better approach is to use orientation tests. A point is inside a triangle if it lies on the "same side" of all three of the triangle's edges. To make this work, you first need to orient the edges consistently (e.g., all clockwise). You can achieve this by using a point on a third edge to determine the orientation of the other two.

</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>

We can easily see, that this problem involves some **Geometry/CGAL**, as the Map Parts are represented by **Triangles** and the Path is a combination of **Segments**.

Breaking it down, the problem requires us to find the **smallest contiguous subsequence of triangles** $t_{\text{left}}, \dots, t_{\text{right}}$ such that all the legs of the path $l_0, \dots, l_{m-1}$, $l_i = (p_i, p_{i + 1})$ are covered. 
To find the smallest contiguous subsequence we usually use a **Sliding Window**. However, the Sliding Window approach is **not quite straight forward**, as it is not clear how our window updates when we increase or decrease its size.

### Sliding Window

A window is **valid**, if the triangles $t_{\text{left}}, \dots, t_{\text{right}}$ in it cover all the legs $l_0, \dots, l_{m - 1}$.
To check this we will introduce a vector **counting how often each leg $l_i$ is covered** in the window. This also provides an intuitive update rule when the `left` or `right` pointer of the window is updated:

- If the `right` pointer is updated, we add a new triangle to the window, so for every leg $l_i$  that is covered by the new triangle $t_{\text{right}}$ we increase the counter for $l_i$ by $1$
- If the `left` pointer is updated, we remove the triangle $t_{\text{left}}$ from the window, so for every leg $l_i$ that was covered by the triangle $t_{\text{left}}$ we decrese the counter for $l_i$ by $1$

We can easily check if a window is valid, by checking if all legs $l_i$ are currently covered at least once.
To **speed this up**, we can also introduce a `n_uncovered` counter, which counts the number of legs $l_i$ that are not currently covered. 

- If a new triangle is added (update of `right`) and increments the counter from $0$ to $1$ for some leg $l_i$, `n_uncovered` is decremented, as we have just covered a leg that was previously uncovered
- If an old triangle is removed (update of `left`) and decrements the counter from $1$ to $0$ for some leg $l_i$, `n_uncovered` is incremented, as we have just removed the only triangle that covered $l_i$

Using this we only need to check if `n_uncovered == 0` to determine whether a window is valid or not, instead of having to loop over the entire vector.

### Containment

The **main challenge** of the problem, however, still remains. We need to find a way to efficiently determine wheter a leg $l_j$ is contained in a triangle $t_i$.

Given that each triangle is represented by 6 points $a_1, a_2, b_1, b_2, c_1, c_2$ on its edges instead of its vertices it is not trivial to use the regular CGAL contains/intersects function to check whether the segment $l_i$ is contained in the triangle $t_i$. This is because to use that we would have to explicitly create the triangle from the 6 points. This would require intersecting the lines defined by $(a_1, a_2), (b_1, b_2), (c_1, c_2)$ to determine its vertices which required Constructions and consequently will be too slow.

First, we can observe, that as triangles are convex, a leg $l_i$ is contained in $t_i$ if both its starting point $p_i$ and its ending point $p_{i + 1}$ are contained in the triangle $t_i$. Therefore, it is sufficient to check if $p_i, p_{i+1}$ are contained in $t_i$.

To efficiently check if a point $p$ is contained in the triangle $t_i$ we will use **Orientation Tests**, which do not require constructions.

Given a new point $p$, we can check if it is on the **same side** (left or right) of each edge $e_a, e_b, e_c$ using `CGAL::left_turn`/`CGAL::right_turn`. 
**If the point $p$ is inside the triangle, it has to be on the same side for all edges**.
(If this is not clear try drawing some examples, or rather counter examples for this)

However, for this we need to ensure that all the edges $e_a,e_b,e_c$ are oriented in the same way. To avoid that for some edge, the inside is left while for the others inside is right. This depends on the **order** of $(a_1, a_2)$. We can ensure that this is consistent across all edgegs $e_a, e_b, e_c$ by **pre-processing** them such that for every edge, **right is inside** and **left is outside**. For this we use the fact that the triangle is convex. That means that at every vertex is either a left or a right turn, but they need to be all the same. Therefore we can just check if witht the current configuration $(a_1, a_2)$ of an edge every vertex is a right turn or left turn, and if it is a left turn, we can simply swap the points of the edge $(a_2, a_1)$ to make it a right turn. <br />
With this consistent orientation for each triangle, we can then check if a point $p$ is contained in the triangle by checking if it is **NOT on the left side** for any edge $e_a, e_b, e_c$

**Note**: It would be more intuitive to check wheter it is on the right side for all edges, but this causes some problems if $p$ is colinear with any edge. Therefore we instead check if it is not left of any edge.

We can then simply precompute for every triangle $t_i$, which legs $l_j$  it covers and then apply the above described sliding window apprach

### Code
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
    // Check for line c and swap if orientation is wrong
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

## 🧠 Learnings

<details> 

<summary> Expand to View </summary>

- For CGAL problems really think about whether you really need (exact) constructions or if you can find any way around them
- 

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