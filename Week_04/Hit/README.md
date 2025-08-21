# Hit?

## 📝 Problem Description

Given a set of $N$ line segments in a 2D plane, the task is to determine if a specified ray intersects with any of these segments.

The ray originates from a starting point $(x, y)$ and is defined by another point $(a, b)$ that lies on its path. Each of the $N$ line segments is defined by its two endpoints, $(r, s)$ and $(t, u)$. The coordinates are provided as integer values. The final output should be "yes" if the ray hits at least one segment and "no" otherwise.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem requires checking a single ray against a collection of line segments. The core of the problem is to determine for each segment whether it intersects the ray. If any such intersection is found, the answer is "yes". If the ray does not intersect any of the segments after checking all of them, the answer is "no".
</details>
<details>
<summary>Hint #2</summary>
The coordinate values can be very large, potentially exceeding the capacity of a standard 32-bit integer. When performing geometric calculations, such as line intersection tests, using standard floating-point types like `double` can introduce precision errors. These errors can lead to incorrect results for edge cases (e.g., when the ray passes very close to an endpoint). Consider how to perform these geometric tests in a way that is robust and exact.
</details>
<details>
<summary>Hint #3</summary>
A straightforward approach is to iterate through each obstacle and perform an intersection test. The main challenge is not the algorithmic complexity but the implementation's correctness. To handle the large coordinates and avoid precision issues, using a specialized computational geometry library is highly recommended. The **Computational Geometry Algorithms Library (CGAL)** is a powerful C++ library that provides data types and functions designed for robust geometric computations. It can handle arbitrary-precision numbers, ensuring that intersection tests are always correct.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
The problem asks us to determine if a given ray intersects any of $N$ line segments. A direct approach is to check each segment one by one. If we find an intersection, we can immediately conclude the answer is "yes". If we check all segments and find no intersections, the answer is "no".

### Core Challenge: Precision and Robustness

A significant challenge in computational geometry problems is floating-point precision. The input coordinates are large integers, which can cause overflow if not handled with appropriate data types (like a 64-bit `long` in C++). Furthermore, calculating intersections often involves division, which can lead to non-integer results. Using standard `double`s for these calculations can introduce small precision errors that lead to incorrect answers, especially in tricky cases where the ray might be collinear with a segment or pass through an endpoint.

### The CGAL-based Solution

To overcome these issues, we can leverage the **Computational Geometry Algorithms Library (CGAL)**. CGAL is designed for robust geometric computing. We will use the `Exact_predicates_inexact_constructions_kernel` (`K`).

-   **Exact Predicates:** This means that decision-making functions (predicates), like `do_intersect()`, are computed using exact arithmetic. This guarantees that the answer to a question like "Do this ray and this segment intersect?" is always correct, free from floating-point errors.
-   **Inexact Constructions:** This part means that functions that construct new geometric objects (like the intersection point itself) might use floating-point arithmetic for performance. Since we only need to know *if* an intersection exists, not *where*, this kernel is a perfect fit.

### Implementation Steps

1.  **Setup:** We include the necessary CGAL headers and define our kernel `K`.
2.  **Input Loop:** The program reads test cases until it encounters an input of $N=0$.
3.  **Ray Representation:** For each test case, we read the two points defining the ray and construct a `K::Ray_2` object. This object correctly represents an infinite ray starting at the first point and passing through the second.
4.  **Segment Iteration:** We loop $N$ times, once for each obstacle segment.
    -   Inside the loop, we read the endpoints of the current segment and create a `K::Segment_2` object.
    -   We use the core CGAL function `CGAL::do_intersect(ray, segment)`. This function handles all the complex geometric logic and returns `true` if they intersect and `false` otherwise.
5.  **Early Exit:** If an intersection is found, we set a `hit` flag to `true`. Critically, we must then read the remaining input lines for the current test case to avoid corrupting the input stream for the next test case. After that, we can `break` from the loop, as we have already found our answer.
6.  **Output:** After the loop finishes (either by checking all segments or by breaking early), we print "yes" or "no" based on the `hit` flag.

This approach effectively delegates the difficult and error-prone geometric calculations to a specialized, well-tested library, leading to a simple and correct solution.

```cpp
#include <iostream>
#include <vector>

// Include the CGAL kernel for exact predicates and inexact constructions.
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

// Define a type alias for the kernel for convenience.
using K = CGAL::Exact_predicates_inexact_constructions_kernel;

int main() {
  // Fast I/O
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);

  while (true) {
    int n;
    std::cin >> n;
    if (!n) {
      break; // Terminate on n=0
    }

    // Read the points defining the ray. Use long for large coordinates.
    long x, y, a, b;
    std::cin >> x >> y >> a >> b;
    // Create a CGAL Ray object.
    K::Ray_2 ray(K::Point_2(x, y), K::Point_2(a, b));

    bool hit = false;

    // Iterate through each of the n obstacle segments.
    for (int j = 0; j < n; ++j) {
      long r, s, t, u;
      std::cin >> r >> s >> t >> u;

      // If we've already found a hit, we just need to consume the remaining
      // input for this test case without processing it.
      if (hit) continue;
      
      // Create a CGAL Segment object.
      K::Segment_2 segment(K::Point_2(r, s), K::Point_2(t, u));

      // Use CGAL's robust intersection test.
      if (CGAL::do_intersect(ray, segment)) {
        hit = true;
      }
    }

    if (hit) {
      std::cout << "yes\n";
    } else {
      std::cout << "no\n";
    }
  }
  return 0;
}
```

</details>

## ⚡ Result

```plaintext

```