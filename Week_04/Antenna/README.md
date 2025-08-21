# Antenna

## 📝 Problem Description

Given a set of $N$ locations in a 2D plane, represented by their coordinates $(x_i, y_i)$, the goal is to determine a single central point and a common radius that defines a coverage area containing all $N$ locations. The objective is to find the smallest possible radius for which such a coverage area exists. The final output should be this minimum radius, rounded up to the nearest integer.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem asks you to find an optimal location for a transmitter, which will become the center of a circular coverage area. The size of this area, its radius, is determined by the point or points that are farthest from this optimal center. Think about which of the given locations are most likely to constrain the size of this minimal coverage area.
</details>
<details>
<summary>Hint #2</summary>
This problem can be modeled geometrically. The task is to find the smallest circle that encloses all given points. A key property of this minimal circle is that it is uniquely determined by either two of the points forming a diameter, or three of the points lying on its circumference. This is a classic problem in the field of computational geometry.
</details>
<details>
<summary>Hint #3</summary>
This is a well-known problem called the **Smallest Enclosing Circle** (or Minimum Enclosing Circle). Implementing an algorithm for it from scratch (like Welzl's algorithm) can be quite complex, especially when dealing with geometric edge cases and numerical precision. The large coordinate values given in the problem statement can easily lead to floating-point errors with standard `double` or `long double` types. For a robust solution, consider using a specialized library designed for computational geometry, such as the **Computational Geometry Algorithms Library (CGAL)**.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
This problem is a classic computational geometry task: finding the **Smallest Enclosing Circle** for a given set of points. The center of the radio transmitter corresponds to the center of this circle, and the transmission radius is the circle's radius.

### Approach: Using a Computational Geometry Library (CGAL)

Implementing an algorithm for the smallest enclosing circle from scratch is challenging. It requires handling various geometric configurations and, more importantly, dealing with numerical precision. The input coordinates can be large, making standard floating-point arithmetic prone to errors.

A more practical and robust approach, especially in an academic or competitive setting, is to use a specialized library like the **Computational Geometry Algorithms Library (CGAL)**. CGAL provides highly optimized and numerically stable implementations for a vast range of geometric problems.

### C++ Implementation with CGAL

The solution uses CGAL's `Min_circle_2` component to solve the problem directly.

1.  **Kernel Selection**: We use `Exact_predicates_exact_constructions_kernel_with_sqrt`. This is a crucial choice. This "kernel" tells CGAL to use number types that can represent calculations (including square roots) *exactly*, without any loss of precision. This avoids the floating-point errors that would likely arise from using standard `double` types with large coordinates.

2.  **Algorithm Usage**:
    *   We read all input coordinates and store them as `Point` objects in a `std::vector`.
    *   The core of the solution is the line: `MinCircle mc(points.begin(), points.end(), true);`. This creates an instance of the `Min_circle_2` algorithm, passing the collection of points. The algorithm automatically computes the smallest enclosing circle. The `true` parameter enables a randomized processing of points, which leads to an excellent expected runtime.
    *   The resulting circle object is retrieved via `mc.circle()`.

3.  **Outputting the Radius**:
    *   The problem requires the smallest *integral* transmission radius.
    *   We get the squared radius from the circle object using `c.squared_radius()`.
    *   We then compute the actual radius by taking the square root using `CGAL::sqrt()`. This operation is also performed with exact precision by the kernel.
    *   Finally, we need to find the ceiling of this exact radius value. The helper function `ceil_to_double` handles the conversion from CGAL's exact number type to a `double` and computes the ceiling carefully to avoid floating-point inaccuracies, ensuring the correct integer output.

```cpp
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

// CGAL headers for the smallest enclosing circle problem
#include <CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>
#include <CGAL/Min_circle_2.h>
#include <CGAL/Min_circle_2_traits_2.h>

// Define the kernel and other necessary types for convenience.
// Using a kernel with exact constructions and square root support is vital for precision.
using K = CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt;
using Traits = CGAL::Min_circle_2_traits_2<K>;
using MinCircle = CGAL::Min_circle_2<Traits>;
using Point = K::Point_2;

// A helper function to safely compute the ceiling of a CGAL number type.
// It converts to a double and performs checks to correct for potential
// floating-point representation errors near integers.
double ceil_to_double(const K::FT& x) {
  double a = std::ceil(CGAL::to_double(x));
  while (a < x) a += 1;
  while (a - 1 >= x) a -= 1;
  return a;
}

void solve() {
  while (true) {
    int n;
    std::cin >> n;
    if (n == 0) break;

    std::vector<Point> points(n);
    for (int i = 0; i < n; ++i) {
      long x, y;
      std::cin >> x >> y;
      points[i] = Point(x, y);
    }

    // Create a Min_circle_2 object. The constructor computes the smallest
    // enclosing circle for the points in the given range.
    // The `true` argument enables randomization for better expected performance.
    MinCircle mc(points.begin(), points.end(), true);
    Traits::Circle c = mc.circle();

    // Calculate the radius by taking the square root of the squared radius.
    // This calculation is exact thanks to the chosen kernel.
    K::FT radius_exact = CGAL::sqrt(c.squared_radius());

    // Set output precision and print the ceiling of the radius as an integer.
    std::cout << std::setprecision(0) << std::fixed;
    std::cout << ceil_to_double(radius_exact) << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  solve();
  return 0;
}
```
</details>

## ⚡ Result

```plaintext

```