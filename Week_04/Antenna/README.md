# Antenna

## 📝 Problem Description

Given a set of $N$ locations in a 2D plane, represented by their coordinates $(x_i, y_i)$, the goal is to determine a single central point and a common radius that defines a coverage area containing all $N$ locations. The objective is to find the smallest possible radius for which such a coverage area exists. The final output should be this minimum radius, rounded up to the nearest integer.

## 💡 Hints

<details>

<summary>Hint #1</summary>

For this problem, you need a kernel that supports exact arithmetic for geometric predicates and constructions. The **Exact_predicates_exact_constructions_kernel_with_sqrt** is the ideal choice, as it ensures precision even when calculating square roots, which is necessary for determining the exact radius of the circle.

</details>

<details>

<summary>Hint #2</summary>

CGAL provides built-in functionality for solving this problem. Use the `Min_circle_2` class along with its associated traits, `Min_circle_2_traits_2`. These tools allow you to compute the smallest enclosing circle for a set of points efficiently and robustly.

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

This problem is a classic computational geometry task: finding the **Smallest Enclosing Circle** for a given set of points. The center of the radio transmitter corresponds to the center of this circle, and the transmission radius is the circle's radius.

### Choice of Kernel

For this problem, we need a kernel that can handle exact arithmetic, especially for geometric predicates and constructions (such as the exact radius of the circle). The most suitable choice is the **Exact_predicates_exact_constructions_kernel_with_sqrt**. The important aspect here is that we also need exact square roots, as we are asked to give the actual radius and not the squared radius, which involves a root.

### Approach

CGAL already offers everything we need for this problem. We can simply read in all points as `K::Point_2` objects and store them in a vector. 
The `mc` function already provides us with the minimum enclosing circle for all the points. We simply need to get its radius by taking the square root of `c.squared_radius()`.

Note that for the output in this problem to be counted correctly, you need to set the output precision to 0 decimal places.

### Code
```cpp
#include<iostream>
#include<cmath>

#include<CGAL/Exact_predicates_exact_constructions_kernel_with_sqrt.h>
#include<CGAL/Min_circle_2.h>
#include<CGAL/Min_circle_2_traits_2.h>

using K = CGAL::Exact_predicates_exact_constructions_kernel_with_sqrt;
using Traits = CGAL::Min_circle_2_traits_2<K>;
using MinCircle = CGAL::Min_circle_2<Traits>;
using Point = K::Point_2;

double ceil_to_double(const K::FT& x) {
  double a = std::ceil(CGAL::to_double(x));
  while (a < x) a += 1;
  while (a-1 >= x) a -= 1;
  return a;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  while(true) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    if(!n) break;
    
    std::vector<Point> points(n);
    for(int i = 0; i < n; i++) {
      long x, y; std::cin >> x >> y;
      points[i] = Point(x, y);
    }
    
    // ===== FIND MINIMUM ENCLOSING RADIUS =====
    MinCircle mc(points.begin(), points.end(), true);
    Traits::Circle c = mc.circle();
    
    // ===== OUTPUT =====  
    std::cout << std::setprecision(0) << std::fixed;
    std::cout << ceil_to_double(CGAL::sqrt(c.squared_radius())) << std::endl;
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (10 pts / 1 s) : Correct answer      (0.001s)
   Test set 2 (10 pts / 1 s) : Correct answer      (0.003s)
   Test set 3 (20 pts / 1 s) : Correct answer      (0.019s)
   Test set 4 (20 pts / 1 s) : Correct answer      (0.265s)
   Test set 5 (10 pts / 1 s) : Correct answer      (0.575s)
   Test set 6 (10 pts / 1 s) : Correct answer      (0.0s)
   Test set 7 (10 pts / 1 s) : Correct answer      (0.404s)
   Test set 8 (10 pts / 1 s) : Correct answer      (0.416s)

Total score: 100
```