# First hit

## 📝 Problem Description

A set of $N$ obstacle line segments is given in a 2D plane, along with a ray defined by its starting point $(x, y)$ and another point $(a, b)$ that lies on the ray.

The task is to determine the first point where the ray intersects with any of the obstacle segments. An intersection point is considered "first" if it is the closest one to the ray's starting point $(x, y)$.

If an intersection exists, the coordinates of this first intersection point should be output, with each coordinate rounded down to the nearest integer.

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

### Choice of Kernel

For this problem, we need a kernel that can handle not only exact predicated but also exact constructions to determine where the intersection points lies. Therefore, the most suitable kernel for our needs is the **Exact Predicates Exact Constructions Kernel (EPECK)**. This kernel uses exact number types for all geometric predicates and constructions, ensuring that we can perform our calculations without worrying about floating-point inaccuracies.

### Approach

The strategy involves starting with the original ray and iterating through the obstacle segments. When an intersection is found, the ray is **replaced with a shorter segment** ending at the closest intersection point. This ensures that only closer intersections are considered in subsequent checks. 

We can additionally first check if the ray (or segment) actually intersects the obstacle segment before computing the intersection point to avoid unnecessary calculations.

To improve performance, the input segments are randomly shuffled before processing, increasing the likelihood of finding a close intersection early and preventing adversarial test cases/worst-case.

### Implementation Details

*   **Intersection Handling:** The `CGAL::intersection()` function can return different types. It might be a `Point` (for a simple intersection) or a `Segment` (if the ray and an obstacle are collinear and overlap). Our code must handle both cases correctly. If the intersection is an overlapping segment, we must identify which of its endpoints is closer to the ray's origin.
*   **Distance Comparison:** To find the closest point, we compare distances. Comparing squared distances (`CGAL::squared_distance`) is more efficient as it avoids computing square roots, and it's sufficient for determining which point is closer.
*   **Output:** The problem requires flooring the final coordinates. The provided `floor_to_double` function correctly converts CGAL's exact number type (`K::FT`) to a `double` and then applies the floor operation, carefully handling potential floating-point inaccuracies near integer boundaries.

### Code
```cpp
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

## 🧠 Learnings

<details> 

<summary> Expand to View </summary>

- Sometimes we need to shuffle the input to account for adversarial test cases
- How long it takes to determine intersections can heavily depend on the geometric primitives we use. Always try to take the "simplest" possible.
- How to work with the CGAL `intersection` function

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