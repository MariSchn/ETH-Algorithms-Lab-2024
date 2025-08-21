# Germs

## 📝 Problem Description

You are asked to simulate the life of bacteria in a rectangular dish. Initially, you are given the coordinates for the centers of $N$ bacteria. All bacteria start as disks with a radius of 0.5 at time $t=0$.

The radius of each bacterium grows over time according to the function $\varrho(t) = t^2 + \frac{1}{2}$. A bacterium dies the moment it touches another bacterium or one of the four boundaries of the rectangular dish. For this simulation, we use a simplified model where germs are assumed to continue growing even after they die, potentially overlapping with other germs or the boundaries.

Your task is to determine three key moments in this simulation:
1.  The time when the **first** bacterium dies.
2.  The time when the number of living bacteria drops **below 50%** of the initial count for the first time.
3.  The time when the **last** bacterium dies.

The input provides the number of bacteria $N$, the coordinates $(l, b, r, t)$ defining the rectangular dish (from $x=l$ to $x=r$ and $y=b$ to $y=t$), and the initial center coordinates $(x, y)$ for each of the $N$ bacteria. All times reported in the output must be rounded up to the nearest integer.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem asks for specific times, but the events (deaths) are triggered by distances. A bacterium dies when its radius reaches a certain critical value. This value is determined by either the distance to a boundary or the distance to another bacterium. Can you establish a mathematical relationship between the death radius and the time of death?
</details>

<details>
<summary>Hint #2</summary>
Instead of trying to find the three required times directly, consider a different approach. What if you could calculate the exact time of death for *every single bacterium*? If you had a list of all $N$ death times, you could simply sort them. The first, median, and last death events would then correspond to the minimum, median, and maximum values in this sorted list.
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
$$ R_{\text{death}, i} = \min(R_{\text{boundary}}, \min_{j \neq i} \frac{\|p_i - p_j\|}{2}) $$

### Efficiently Finding Nearest Neighbors with Delaunay Triangulation

Calculating the nearest neighbor for each bacterium by checking all other $N-1$ bacteria results in an $O(N^2)$ algorithm, which is too slow. This is where **Delaunay triangulation** becomes essential. A fundamental property of Delaunay triangulations is that for any vertex $v$, its nearest neighbor in the point set is one of the vertices connected to $v$ by an edge in the triangulation. This reduces the search for the nearest neighbor of a point from $N-1$ candidates to just its handful of adjacent vertices in the triangulation, leading to a much faster overall solution.

### Algorithm and Implementation Details

The final algorithm is as follows:

1.  **Construct Delaunay Triangulation:** Read all $N$ bacterium center coordinates and insert them into a Delaunay triangulation.
2.  **Calculate Death Radii:** For each vertex (bacterium) `v` in the triangulation:
    a. Calculate the distance to the nearest boundary, $R_{\text{boundary}}$.
    b. Find the minimum distance to an adjacent vertex in the triangulation. Let this be $\|p_v - p_{neighbor}\|$. The corresponding collision radius is $R_{\text{cell}} = \frac{\|p_v - p_{neighbor}\|}{2}$.
    c. The death radius for `v` is $R_{\text{death}} = \min(R_{\text{boundary}}, R_{\text{cell}})$.
3.  **Use Squared Distances:** To avoid costly `sqrt` operations, we work with squared radii and squared distances.
    *   $R_{\text{death}}^2 = \min(R_{\text{boundary}}^2, (R_{\text{cell}})^2) = \min(( \min(\dots))^2, \frac{\|p_v - p_{neighbor}\|^2}{4})$.
    *   We compute this squared death radius for each of the $N$ bacteria and store them in a vector.
4.  **Find Event Times:**
    a. Sort the vector of squared death radii.
    b. The first death corresponds to the smallest squared radius: `radii[0]`.
    c. The "median" death (when the count of living bacteria drops below 50%) corresponds to the element at index `floor(N/2)`.
    d. The last death corresponds to the largest squared radius: `radii[N-1]`.
5.  **Convert to Time:** For each of the three selected squared death radii, $R_{\text{death}}^2$, convert it back to time using the formula $t = \sqrt{\sqrt{R_{\text{death}}^2} - 0.5}$ and round the result up to the nearest integer.

```cpp
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <cmath>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Triangulation;

typedef Triangulation::Finite_vertices_iterator VertexIterator;
typedef Triangulation::Edge_circulator EdgeCirculator;

typedef K::Point_2 Point;

// Converts a squared death radius to the corresponding death time, rounded up.
int squared_radius_to_time(K::FT squared_radius) {
  // The radius R is given by R = t^2 + 0.5.
  // So, t = sqrt(R - 0.5).
  // We have the squared radius, so R = sqrt(squared_radius).
  // Thus, t = sqrt(sqrt(squared_radius) - 0.5).
  // If the argument to the outer sqrt is negative, the event is impossible
  // or happens at t=0. We can treat this time as 0.
  double r = std::sqrt(CGAL::to_double(squared_radius));
  if (r < 0.5) return 0;
  
  double t = std::sqrt(r - 0.5);
  
  // Round up to the next integer.
  return static_cast<int>(std::ceil(t));
}

void solve() {
  int n;
  std::cin >> n;
  if (n == 0) exit(0);
  
  long l, b, r, t;
  std::cin >> l >> b >> r >> t;
  
  std::vector<Point> points;
  points.reserve(n);
  for (int i = 0; i < n; ++i) {
    int x, y;
    std::cin >> x >> y;
    points.emplace_back(x, y);
  }
  
  Triangulation tri;
  tri.insert(points.begin(), points.end());
  
  std::vector<K::FT> squared_death_radii;
  squared_death_radii.reserve(n);
  
  for (VertexIterator v = tri.finite_vertices_begin(); v != tri.finite_vertices_end(); ++v) {
    Point p = v->point();
    
    // Calculate squared distance to the nearest boundary.
    K::FT dx = std::min(p.x() - l, r - p.x());
    K::FT dy = std::min(p.y() - b, t - p.y());
    K::FT min_dist_to_border = std::min(dx, dy);
    K::FT squared_radius_boundary = min_dist_to_border * min_dist_to_border;
    
    // Find the squared distance to the nearest neighbor in the triangulation, then divide by 4.
    K::FT squared_radius_cell = -1; // Sentinel for no neighbors
    
    EdgeCirculator e_start = tri.incident_edges(v);
    if (e_start != nullptr) {
      EdgeCirculator e = e_start;
      do {
        if (!tri.is_infinite(e)) {
          K::FT current_sq_dist = tri.segment(e).squared_length();
          if (squared_radius_cell == -1 || current_sq_dist < squared_radius_cell) {
            squared_radius_cell = current_sq_dist;
          }
        }
      } while (++e != e_start);
    }
    
    // If a neighbor was found, the radius is half the distance.
    // So the squared radius is (dist/2)^2 = dist^2 / 4.
    if (squared_radius_cell != -1) {
      squared_radius_cell /= 4;
    } else {
      // If a point has no finite neighbors, it can only die by hitting the boundary.
      // Set its cell collision radius to infinity.
      squared_radius_cell = std::numeric_limits<double>::max();
    }
    
    squared_death_radii.push_back(std::min(squared_radius_boundary, squared_radius_cell));
  }
  
  std::sort(squared_death_radii.begin(), squared_death_radii.end());
  
  // The time for the first death.
  int first_time = squared_radius_to_time(squared_death_radii[0]);
  
  // The time when the number of living bacteria goes below 50%.
  // This happens at the death time of the ceil(n/2)-th bacterium to die.
  // In a 0-indexed sorted array, this is at index floor(n/2).
  int median_time = squared_radius_to_time(squared_death_radii[n / 2]);
  
  // The time for the last death.
  int last_time = squared_radius_to_time(squared_death_radii[n - 1]);
  
  std::cout << first_time << " " << median_time << " " << last_time << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  while (true) {
    solve();
  }
  return 0;
}
```
</details>

## ⚡ Result

```plaintext

```