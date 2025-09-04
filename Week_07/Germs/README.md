# Germs

## 📝 Problem Description

The task is to simulate the life cycle of bacteria in a rectangular dish. The centers of $N$ bacteria are provided, each starting as a disk with radius $0.5$ at time $t=0$. The radius of each bacterium evolves over time according to $\varrho(t) = t^2 + \frac{1}{2}$. A bacterium is considered dead at the moment it touches another bacterium or one of the four boundaries of the dish. In this model, bacteria continue to grow after death, potentially overlapping with other bacteria or the boundaries.

The objective is to determine three specific moments during the simulation:
1. The time at which the **first** bacterium dies.
2. The time when the number of living bacteria falls **below 50%** of the initial count for the first time.
3. The time at which the **last** bacterium dies.

The input specifies the number of bacteria $N$, the coordinates $(l, b, r, t)$ that define the rectangular dish (with $x$ ranging from $l$ to $r$ and $y$ from $b$ to $t$), and the initial center coordinates $(x, y)$ for each bacterium. All reported times must be rounded up to the nearest integer.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem asks for specific times, but the events (deaths) are triggered by distances. A bacterium dies when its radius reaches a certain critical value. This value is determined by either the distance to a boundary or the distance to another bacterium. Can you establish a mathematical relationship between the death radius and the time of death?

</details>

<details>

<summary>Hint #2</summary>

Instead of trying to find the three required times directly, consider a different approach. What if you could calculate the exact time of death for *every single bacterium* given its radius when it dies? If you had a list of all $N$ death times, you could simply sort them. The first, median, and last death events would then correspond to the minimum, median, and maximum values in this sorted list.

</details>

<details>

<summary>Hint #3</summary>

To find the death time for each bacterium, you need to find its closest obstacle. This obstacle is either a wall or another bacterium. Finding the closest bacterium for every single bacterium by checking all pairs would take $O(N^2)$ time, which is too slow for the given constraints. This is a classic nearest-neighbor search problem. Consider using a spatial data structure to speed up this search. A Delaunay triangulation has a crucial property: the nearest neighbor of any point is guaranteed to be one of its adjacent vertices in the triangulation. This dramatically reduces the number of distance calculations needed.

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

This problem asks us to find the times of three specific events: the first death, the point where half the bacteria are dead, and the last death. The core of the problem lies in determining when each individual bacterium dies.

### From Time to Distance (and back)

The radius of a bacterium at time $t$ is given by $\varrho(t) = t^2 + \frac{1}{2}$. Instead of working with time directly, it's more convenient to work with distances and radii, which are natural in a geometric setting. We can determine the critical radius at which a bacterium dies and then convert this radius back to a time.

By rearranging the given formula, we can express time $t$ as a function of a given radius $R$:
$$ R = t^2 + \frac{1}{2} \quad \implies \quad t^2 = R - \frac{1}{2} \quad \implies \quad t = \sqrt{R - \frac{1}{2}} $$
This formula allows us to calculate the time of death if we know the radius of the bacterium at that moment.

Our problem is now transformed: for each bacterium, what is the radius at which it dies? Once we have these "death radii" for all $N$ bacteria, we can sort them. The first, median, and last death times correspond to the minimum, median, and maximum death radii.

### Calculating the Death Radius

A bacterium dies for one of two reasons:
1.  **It hits a boundary:** A bacterium centered at $(x, y)$ is contained within a dish defined by $l \le x' \le r$ and $b \le y' \le t$. The distances to the four boundaries are $x-l$, $r-x$, $y-b$, and $t-y$. The bacterium will touch the nearest boundary first. Therefore, the radius at which it would die from hitting a boundary is:
    $R_{\text{boundary}} = \min(x-l, r-x, y-b, t-y)$.

2.  **It hits another bacterium:** When two bacteria touch, their centers are separated by a distance equal to the sum of their radii. Since all bacteria grow at the same rate, they will have the same radius $R$ at any given time $t$. If two bacteria with centers $p_i$ and $p_j$ touch, the distance between them is $\|p_i - p_j\| = R + R = 2R$. Thus, the radius at which they touch is $R = \frac{\|p_i - p_j\|}{2}$. A bacterium `i` will die from the first bacterium it touches, which is its nearest neighbor.

The actual death radius for a bacterium $i$, let's call it $R_{\text{death}, i}$, is the minimum of the radii required to hit any obstacle:
$$ R_{\text{death}, i} = \min\left(R_{\text{boundary}}, \min_{j \neq i} \frac{\|p_i - p_j\|}{2}\right) $$

### Efficiently Finding Nearest Neighbors with Delaunay Triangulation

Calculating the nearest neighbor for each bacterium by checking all other $N-1$ bacteria results in an $O(N^2)$ algorithm, which is too slow. This is where **Delaunay triangulation** becomes essential. A fundamental property of Delaunay triangulations is that for any vertex $v$, its nearest neighbor in the point set is one of the vertices connected to $v$ by an edge in the triangulation. This reduces the search for the nearest neighbor of a point from $N-1$ candidates to just its handful of adjacent vertices in the triangulation, leading to a much faster overall solution.

### Final Algorithm

The final algorithm is as follows:

1.  **Construct Delaunay Triangulation:** Read all $N$ bacterium center coordinates and insert them into a Delaunay triangulation.
2.  **Calculate Death Radii:** For each vertex (bacterium) `v` in the triangulation:
    - Calculate the distance to the nearest boundary, $R_{\text{boundary}}$.
    - Find the minimum distance to an adjacent vertex in the triangulation. Let this be $\|p_v - p_{neighbor}\|$. The corresponding collision radius is $R_{\text{cell}} = \frac{\|p_v - p_{neighbor}\|}{2}$.
    - The death radius for `v` is $R_{\text{death}} = \min(R_{\text{boundary}}, R_{\text{cell}})$.
3.  **Use Squared Distances:** To avoid costly `sqrt` operations, we work with squared radii and squared distances.
    *   $R_{\text{death}}^2 = \min(R_{\text{boundary}}^2, (R_{\text{cell}})^2) = \min(( \min(\dots))^2, \frac{\|p_v - p_{neighbor}\|^2}{4})$.
    *   We compute this squared death radius for each of the $N$ bacteria and store them in a vector.
4.  **Find Event Times:**
    - Sort the vector of squared death radii.
    - The first death corresponds to the smallest squared radius: `death_distances[0]`.
    - The "median" death (when the count of living bacteria drops below 50%) corresponds to the element at index `floor(N/2)`.
    - The last death corresponds to the largest squared radius: `death_distances[N-1]`.
5.  **Convert to Time:** For each of the three selected squared death radii, $R_{\text{death}}^2$, convert it back to time using the formula $t = \sqrt{\sqrt{R_{\text{death}}^2} - 0.5}$ and round the result up to the nearest integer.

### Code
```cpp
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;

typedef Triangulation::Finite_vertices_iterator VertexIterator;
typedef Triangulation::Edge_circulator EdgeCirculator;

typedef K::Point_2 Point;

int squared_distance_to_time(K::FT d) {
  // Calculate exact time
  K::FT t = std::sqrt(std::sqrt(d) - 0.5);
  
  // Round up to next integer;
  int out = std::ceil(CGAL::to_double(t));
  while (out < t) out += 1;
  while (out-1 >= t) out -= 1;
  
  return out;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  while(true) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    if(n == 0) { break; } // Terminate if end of input is reached
    
    int l, b, r, t; std::cin >> l >> b >> r >> t;
    
    std::vector<Point> points; points.reserve(n);
    for(int i = 0; i < n; ++i) {
      int x, y; std::cin >> x >> y;
      points.emplace_back(x, y);
    }
    
    // ===== TRIANGULATION =====
    Triangulation tri;
    tri.insert(points.begin(), points.end());
    
    // ===== DETERMINE DEATH DISTANCES =====
    std::vector<double> death_distances; death_distances.reserve(n);
    for(VertexIterator v = tri.finite_vertices_begin(); v != tri.finite_vertices_end(); ++v) {
      Point p = v->point();
      
      // Calculate distance to border
      double x_dist = std::min(p.x() - l, r - p.x());
      double y_dist = std::min(p.y() - b, t - p.y());
      double border_dist = std::min(x_dist, y_dist);
      double squared_border_dist = std::pow(border_dist, 2);
      
      // Find the shortest edge distance
      double shortest_edge = std::numeric_limits<double>::max();
      
      EdgeCirculator e = tri.incident_edges(v);
      if(e != 0) {  // Check if the circulator is valid
        do {
          if (!tri.is_infinite(e)) {  
            // Note, we have to divide by 4 here as both germs grow towards each other
            // Therefore we have to divide by 2, but since we are considering squared distances
            // we divide by 4
            shortest_edge = std::min(shortest_edge, tri.segment(e).squared_length() / 4);
          }
        } while (++e != tri.incident_edges(v));
      }
      
      // Add the death distance to the vector
      death_distances.push_back(std::min(squared_border_dist, shortest_edge));
    }
    
    // ==== CALCULATE QUERY TIMES =====
    std::sort(death_distances.begin(), death_distances.end());
    
    std::cout << squared_distance_to_time(death_distances[0]) << " " <<
                squared_distance_to_time(death_distances[std::floor(death_distances.size() / 2)]) << " " <<
                 squared_distance_to_time(death_distances[death_distances.size() - 1]) << std::endl;
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (20 pts / 0.600 s) : Correct answer      (0.002s)
   Test set 2 (20 pts / 0.600 s) : Correct answer      (0.054s)
   Test set 3 (30 pts / 0.600 s) : Correct answer      (0.201s)
   Test set 4 (30 pts / 0.600 s) : Correct answer      (0.178s)

Total score: 100
```