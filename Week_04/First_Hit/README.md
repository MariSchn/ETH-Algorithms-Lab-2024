# First hit

## 📝 Problem Description

You are given a set of $N$ obstacle line segments in a 2D plane. You are also given a ray, defined by its starting point $S$ and another point $P$ that lies on the ray.

Your task is to find the first point where the ray intersects with any of the obstacle segments. An intersection point is considered "first" if it is the closest one to the ray's starting point $S$.

If an intersection exists, you should output the coordinates of this first intersection point, with each coordinate rounded down to the nearest integer. If the ray does not intersect any of the segments, you should report that there is no intersection.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The fundamental operation in this problem is determining if and where a ray intersects a line segment. A straightforward approach would be to iterate through all the obstacle segments, calculate the intersection point for each, and then identify which of these intersection points is closest to the ray's origin.
</details>
<details>
<summary>Hint #2</summary>
The coordinate values can be extremely large. Standard floating-point data types like `double` may not have enough precision to represent these values and the results of intermediate calculations accurately. This can lead to incorrect intersection results. Consider using a computational geometry library that supports exact arithmetic to avoid these precision issues.
</details>
<details>
<summary>Hint #3</summary>
The brute-force approach of checking every segment has a time complexity of $O(N)$. Given the constraints, this is likely efficient enough. To improve the practical running time, consider this: once you find an intersection, you are only interested in other intersections that are even closer. You can maintain the "closest hit found so far" and shorten your ray for subsequent checks. Shuffling the input segments randomly can increase the probability of finding a close intersection early, potentially speeding up the average-case performance.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
This problem requires finding the closest intersection point between a ray and a set of line segments. The main challenges are the large coordinate values, which can cause precision errors with standard floating-point arithmetic, and handling the geometric calculations correctly.

### Approach

The solution uses the **CGAL (Computational Geometry Algorithms Library)**, which is specifically designed for robust geometric computations. By using an exact kernel, we can avoid precision issues entirely.

The overall strategy is as follows:
1.  **Initialization:** Read the ray and all obstacle segments. The core of the algorithm is to iterate through each segment and keep track of the closest intersection point found so far.
2.  **Iterative Search:**
    *   We start with the original, infinite ray.
    *   We iterate through the obstacle segments. The first time we find an intersection, we store that point. This point now becomes the endpoint of our "search segment", which starts at the ray's origin. We are no longer interested in any intersection that is further away.
    *   For all subsequent obstacle segments, we check for an intersection with our current (and progressively shorter) search segment.
    *   If a new, closer intersection is found, we update the endpoint of our search segment to this new point.
3.  **Randomization for Performance:** Before iterating, we shuffle the list of obstacle segments randomly. While the worst-case complexity remains $O(N)$, this is a powerful heuristic. If a segment close to the ray's origin is processed early, our search segment shrinks quickly. Subsequent intersection tests against this shorter segment are often faster (e.g., their bounding boxes may not overlap), leading to better average-case performance.

### Implementation Details

*   **Kernel:** We use `CGAL::Exact_predicates_exact_constructions_kernel` (or `EPECK`). This kernel uses number types that can represent rational numbers with arbitrary precision, guaranteeing that all geometric predicates (like `do_intersect`) and constructions (like `intersection`) are exact.
*   **Intersection Handling:** The `CGAL::intersection()` function can return different types. It might be a `Point` (for a simple intersection) or a `Segment` (if the ray and an obstacle are collinear and overlap). Our code must handle both cases correctly. If the intersection is an overlapping segment, we must identify which of its endpoints is closer to the ray's origin.
*   **Distance Comparison:** To find the closest point, we compare distances. Comparing squared distances (`CGAL::squared_distance`) is more efficient as it avoids computing square roots, and it's sufficient for determining which point is closer.
*   **Output:** The problem requires flooring the final coordinates. The provided `floor_to_double` function correctly converts CGAL's exact number type (`K::FT`) to a `double` and then applies the floor operation, carefully handling potential floating-point inaccuracies near integer boundaries.

This approach is both robust due to the use of an exact geometry kernel and efficient enough to pass within the time limits.

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <stdexcept>

// CGAL headers for exact geometric computations
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

// Define type aliases for convenience
using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Point = K::Point_2;
using Segment = K::Segment_2;
using Ray = K::Ray_2;

// Custom function to correctly floor a coordinate from CGAL's exact number type.
// Standard std::floor on a converted double might be inaccurate.
double floor_to_double(const K::FT& x) {
  double a = std::floor(CGAL::to_double(x));
  while (a > x) a -= 1;
  while (a + 1 <= x) a += 1;
  return a;
}

void solve() {
  int n;
  std::cin >> n;
  if (n == 0) {
    exit(0);
  }

  // ===== READ INPUT =====
  long x, y, a, b;
  std::cin >> x >> y >> a >> b;
  Ray ray(Point(x, y), Point(a, b));

  std::vector<Segment> segments(n);
  for (int i = 0; i < n; ++i) {
    long r, s, t, u;
    std::cin >> r >> s >> t >> u;
    segments[i] = Segment(Point(r, s), Point(t, u));
  }

  // Randomly shuffle segments for better average-case performance
  std::random_shuffle(segments.begin(), segments.end());

  // ===== FIND FIRST HIT =====
  bool hit_found = false;
  Segment min_hit_segment; // Represents segment from ray origin to closest hit

  for (const Segment& s : segments) {
    if (!hit_found) {
      // First phase: Intersect with the infinite ray
      if (CGAL::do_intersect(ray, s)) {
        hit_found = true;
        auto intersection_obj = CGAL::intersection(ray, s);
        if (const Point* p = boost::get<Point>(&*intersection_obj)) {
          min_hit_segment = Segment(ray.source(), *p);
        } else if (const Segment* seg = boost::get<Segment>(&*intersection_obj)) {
          // If overlap, find the closer endpoint of the overlap
          Point p1 = seg->source();
          Point p2 = seg->target();
          min_hit_segment = (CGAL::squared_distance(ray.source(), p1) < CGAL::squared_distance(ray.source(), p2))
                           ? Segment(ray.source(), p1)
                           : Segment(ray.source(), p2);
        }
      }
    } else {
      // Second phase: Intersect with the current shortest hit segment
      if (CGAL::do_intersect(min_hit_segment, s)) {
        auto intersection_obj = CGAL::intersection(min_hit_segment, s);
        if (const Point* p = boost::get<Point>(&*intersection_obj)) {
          // Update the shortest hit segment
          min_hit_segment = Segment(ray.source(), *p);
        } else if (const Segment* seg = boost::get<Segment>(&*intersection_obj)) {
          // If overlap, find the closer endpoint and update
          Point p1 = seg->source();
          Point p2 = seg->target();
          Point closer_point = (CGAL::squared_distance(ray.source(), p1) < CGAL::squared_distance(ray.source(), p2)) ? p1 : p2;
          min_hit_segment = Segment(ray.source(), closer_point);
        }
      }
    }
  }

  // ===== OUTPUT =====
  if (hit_found) {
    Point hit_point = min_hit_segment.target();
    std::cout << std::setprecision(0) << std::fixed
              << floor_to_double(hit_point.x()) << " "
              << floor_to_double(hit_point.y()) << std::endl;
  } else {
    std::cout << "no" << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
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