# Hit?

## 📝 Problem Description

Given a set of $N$ line segments in a 2D plane, the task is to determine if a specified ray intersects with any of these segments.

The ray originates from a starting point $(x, y)$ and is defined by another point $(a, b)$ that lies on its path. Each of the $N$ line segments is defined by its two endpoints, $(r, s)$ and $(t, u)$. The coordinates are provided as integer values. The final output should be "yes" if the ray hits at least one segment and "no" otherwise.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem involves determining whether a ray intersects with any line segments. Consider the precision requirements for intersection tests and whether exact results are necessary for all aspects of the computation. to determine which CGAL kernel to use.

</details>

<details>

<summary>Hint #2</summary>

The **Exact Predicates Inexact Constructions Kernel (EPIC)** from CGAL is well-suited for this problem. It ensures exact results for predicate tests like intersection checks while maintaining efficiency for other operations, as we do not need to calculate actual intersection points.

</details>

<details>

<summary>Hint #3</summary>

To solve the problem, use the `K::Ray_2` class to represent the ray and the `K::Segment_2` class for the line segments. The `CGAL::do_intersect` function can then be used to check for intersections between the ray and each segment.

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

As this is an introductary problem to CGAL the problem is not hard algorithmically, but is ment to familiarize you with CGAL.

### Choice of Kernel

In this solution, we use the **Exact Predicates Inexact Constructions Kernel (EPICK)** from CGAL. This kernel is well-suited for our needs because it provides exact arithmetic for predicate tests (like intersection checks, which we are interested in) while allowing for inexact constructions (like computing intersection points, which we do not need here) using floating-point arithmetic. This balance helps avoid precision issues during intersection tests while maintaining performance by leveraging the efficiency of floating-point computations where exact results are not critical.

### Determining Intersection

To determine if the ray intersects with a segment, we can use use the `K::Ray_2` class for the ray itself and `K::Segment_2` to store the ray and the segments respectively. 
Afterward, we can use the `CGAL::do_intersect` function, which is designed to handle various geometric objects and their interactions. This function will return `true` if the ray and segment intersect, and `false` otherwise.

We can therefore simply iterate over all segments and perform the intersection test, to determine if any segment intersects the ray.

### Code
```cpp
#include<iostream>
#include<vector>

#include<CGAL/Exact_predicates_inexact_constructions_kernel.h>

using K = CGAL::Exact_predicates_inexact_constructions_kernel;

int main() {
  std::ios_base::sync_with_stdio(false);
  
  while(true) {
    int n; std::cin >> n;
    if(!n) break;
    
    long x, y, a, b; std::cin >> x >> y >> a >> b;
    K::Ray_2 ray(K::Point_2(x, y), K::Point_2(a, b));

    bool hit = false;

    for(int j = 0; j < n; ++j) {
      long r, s, t, u; std::cin >> r >> s >> t >> u;
  
      K::Segment_2 segment(K::Point_2(r, s), K::Point_2(t, u));
      
      if(CGAL::do_intersect(ray, segment)) {
        for(; j < n - 1; ++j) std::cin >> r >> s >> t >> u;
        hit = true;
        break;
      }
    }
    
    if (hit) std::cout << "yes" << std::endl;
    else std::cout << "no" << std::endl;
  }
}
```

</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (40 pts / 1.000 s) : Correct answer      (0.001s)
   Test set 2 (20 pts / 1.000 s) : Correct answer      (0.135s)
   Test set 3 (30 pts / 1.000 s) : Correct answer      (0.235s)
   Test set 4 (10 pts / 0.140 s) : Correct answer      (0.135s)

Total score: 100
```