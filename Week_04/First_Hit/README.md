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
///1
#include<iostream>
#include<type_traits>
#include<limits>
#include<algorithm>

#include<CGAL/Exact_predicates_exact_constructions_kernel.h>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Point = K::Point_2;
using Segment = K::Segment_2;
using Ray = K::Ray_2;

double floor_to_double(const K::FT& x) {
  double a = std::floor(CGAL::to_double(x));
  while (a > x) a -= 1;
  while (a+1 <= x) a += 1;
  return a;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  while(true) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    if(!n) break;
    
    long x, y, a, b; std::cin >> x >> y >> a >> b;
    Point o(x, y), d(a, b);
    Ray ray(o, d);
    
    std::vector<Segment> segments(n);
    for(int i = 0; i < n; ++i) {
      long r, s, t, u; std::cin >> r >> s >> t >> u;
      segments[i] = Segment(Point(r, s), Point(t, u));
    }
    std::random_shuffle(segments.begin(), segments.end());
    
    // ===== FIND FIRST HIT =====
    bool found = false;
    Segment curr_segment;
    
    for(const Segment& segment : segments) {
      if(!found) {
        if(CGAL::do_intersect(ray, segment)) {
          std::result_of<K::Intersect_2(Ray, Segment)>::type its = CGAL::intersection(ray, segment);
          found = true;
          
          if(const Point* its_p = boost::get<Point>(&*its)) {
            curr_segment = Segment(ray.source(), *its_p);
          } else if (const Segment* its_s = boost::get<Segment>(&*its)) {
            Point source = its_s->source();
            Point target = its_s->target();
            
            curr_segment = Segment(ray.source(),
                                   CGAL::squared_distance(o, source) < CGAL::squared_distance(o, target) ? source : target);
          } else {
            throw std::runtime_error("Undefined intersection");
          }
        }
      } else {
        if(CGAL::do_intersect(curr_segment, segment)) {
          std::result_of<K::Intersect_2(Segment, Segment)>::type its = CGAL::intersection(curr_segment, segment);
          
          if(const Point* its_p = boost::get<Point>(&*its)) {
            curr_segment = Segment(curr_segment.source(), *its_p);
          } else if (const Segment* its_s = boost::get<Segment>(&*its)) {
            Point source = its_s->source();
            Point target = its_s->target();
            
            curr_segment = Segment(o,
                                   CGAL::squared_distance(o, source) < CGAL::squared_distance(o, target) ? source : target);
          } else {
            throw std::runtime_error("Undefined intersection");
          }
        }
      }
    }
    
    // ===== OUTPUT =====
    if(found) {
      std::cout << std::setprecision(0) << std::fixed;
      std::cout << floor_to_double(curr_segment.target().x()) << " " << floor_to_double(curr_segment.target().y()) << std::endl;
    } else {
      std::cout << "no" << std::endl;
    }
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (15 pts / 1.000 s) : Correct answer      (0.006s)
   Test set 2 (15 pts / 1.000 s) : Correct answer      (0.055s)
   Test set 3 (15 pts / 1.000 s) : Correct answer      (0.068s)
   Test set 4 (15 pts / 1.000 s) : Correct answer      (0.054s)
   Test set 5 (10 pts / 1.000 s) : Correct answer      (0.177s)
   Test set 6 (10 pts / 1.000 s) : Correct answer      (0.062s)
   Test set 7 (10 pts / 1.000 s) : Correct answer      (0.054s)
   Test set 8 (10 pts / 0.200 s) : Correct answer      (0.054s)

Total score: 100
```