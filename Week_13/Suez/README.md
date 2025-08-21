# Suez

## 📝 Problem Description

You are given the locations of $n$ new nails and $m$ old nails on a 2D plane. You need to hang one new rectangular poster, centered on each of the $n$ new nails. The posters already hanging on the $m$ old nails must remain untouched.

Initially, all posters have a standard size of $h$ inches in height and $w$ inches in width. For each of the $n$ new posters, you can choose a scaling factor $a \ge 1$ to create a larger poster of size $(a \cdot h) \times (a \cdot w)$. Each new poster can have its own distinct scaling factor.

The primary constraint is that no two posters on the wall (new or old) can overlap. The goal is to choose the scaling factors for the $n$ new posters such that the sum of their perimeters is maximized. Your task is to calculate this maximum possible sum of perimeters, rounded up to the nearest integer.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem asks you to maximize a certain value (the sum of perimeters) by adjusting a set of variables (the scaling factors $a_i$). What kind of constraints do the non-overlapping conditions impose on these variables? Are these constraints linear?
</details>
<details>
<summary>Hint #2</summary>
This problem can be modeled as a Linear Program (LP). The variables of the LP would be the scaling factors $a_i$ for each new poster. The objective function is a linear combination of these variables. The main challenge is to correctly formulate the non-overlapping conditions as linear constraints.
</details>
<details>
<summary>Hint #3</summary>
Consider two posters, $i$ and $j$, centered at $(x_i, y_i)$ and $(x_j, y_j)$ with scaling factors $a_i$ and $a_j$. For them not to overlap, their projections on the x-axis *or* their projections on the y-axis must not overlap. This means that either $\frac{w}{2}a_i + \frac{w}{2}a_j \le |x_i - x_j|$ or $\frac{h}{2}a_i + \frac{h}{2}a_j \le |y_i - y_j|$ must hold. How can you decide which constraint is the most restrictive one to add to your LP? Think about which dimension will cause an overlap first as the posters grow.
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1)</summary>
This problem involves maximizing a value subject to a set of constraints, which strongly suggests using **Linear Programming (LP)**. The core of this approach is to define the variables, the objective function, and the constraints.

### Model Formulation

1.  **Variables:** The quantities we can control are the scaling factors for each of the $n$ new posters. Let's denote the scaling factor for the $i$-th new poster as $a_i$. Since the posters can only be magnified, we have a lower bound $a_i \ge 1$ for all $i=1, \dots, n$.

2.  **Objective Function:** We want to maximize the sum of the perimeters of the new posters. The perimeter of a poster $i$ with scaling factor $a_i$ is $2(a_i \cdot w + a_i \cdot h) = a_i(2w + 2h)$. Our objective is to maximize:
    $$ \sum_{i=1}^{n} a_i (2w + 2h) $$

3.  **Constraints:** The posters must not overlap. For the first test set, all nails lie on a horizontal line (e.g., $y=0$), and there are no old posters ($m=0$). This simplifies the problem significantly, as we only need to consider overlaps along the x-axis.

    Consider two new posters, $i$ and $j$, centered at $(x_i, 0)$ and $(x_j, 0)$. Let's assume $x_i < x_j$. The posters are scaled by $a_i$ and $a_j$, so their widths are $a_i \cdot w$ and $a_j \cdot w$. Their right and left boundaries are at $x_i \pm \frac{a_i w}{2}$ and $x_j \pm \frac{a_j w}{2}$. To prevent overlap, the right edge of poster $i$ must be to the left of the left edge of poster $j$.
    
    $$ x_i + \frac{a_i w}{2} \le x_j - \frac{a_j w}{2} $$
    
    Rearranging this gives us a linear constraint on our variables $a_i$ and $a_j$:
    
    $$ \frac{w}{2} a_i + \frac{w}{2} a_j \le x_j - x_i $$
    
    We must add such a constraint for every pair of new posters $(i, j)$.

By setting up this LP with the CGAL library and solving it, we can find the optimal values for $a_i$ and thus the maximum total perimeter. Remember that CGAL's LP solver minimizes, so we must maximize $\sum a_i(2w+2h)$ by minimizing $-\sum a_i(2w+2h)$.

```cpp
#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpq.h>

// Define types for the LP solver
typedef double IT;
typedef CGAL::Gmpq ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

// Helper function to round up to the nearest integer
double ceil_to_double(const CGAL::Quotient<ET> &x) {
  double a = std::ceil(CGAL::to_double(x));
  while (a < x) a += 1;
  while (a-1 >= x) a -= 1;
  return a;
}

void solve() {
  int n, m, h, w;
  std::cin >> n >> m >> h >> w;

  std::vector<long> x_coords(n);
  for (int i = 0; i < n; ++i) {
    long x, y;
    std::cin >> x >> y;
    x_coords[i] = x;
  }

  // Linear program setup: variables a_i >= 1
  Program lp(CGAL::SMALLER, true, 1, false, 0); 
  
  // Add constraints for every pair of new posters
  int constraint_idx = 0;
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      long dist_x = std::abs(x_coords[i] - x_coords[j]);
      
      // Constraint: a_i * w/2 + a_j * w/2 <= dist_x
      lp.set_a(i, constraint_idx, (double)w / 2.0);
      lp.set_a(j, constraint_idx, (double)w / 2.0);
      lp.set_b(constraint_idx, dist_x);
      constraint_idx++;
    }
  }
  
  // Set the objective function to MINIMIZE: -sum(a_i * (2w + 2h))
  for (int i = 0; i < n; ++i) {
    lp.set_c(i, -(2.0 * w + 2.0 * h));
  }
  
  // Solve the LP
  Solution s = CGAL::solve_linear_program(lp, ET());
  
  // Output the result, which is -objective_value, rounded up
  std::cout << std::fixed << std::setprecision(0);
  std::cout << ceil_to_double(-s.objective_value()) << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  int t;
  std::cin >> t;
  while (t--) {
    solve();
  }
  return 0;
}
```
</details>

<details>
<summary>Second Solution (Test Set 1, 2)</summary>
To handle the general 2D case without old posters, we must extend our constraints. A naive approach of adding constraints for both x and y dimensions for every pair of posters is too restrictive. For instance, if two posters have the same x-coordinate but different y-coordinates, an x-constraint would force their widths to be zero, which is incorrect.

### The Key Insight
Two posters overlap if and only if their projections on **both** the x-axis and the y-axis overlap. Therefore, to prevent overlap, we only need to ensure that their projections do not overlap on *at least one* of the axes. We should enforce the constraint on the dimension that is "tighter" or, in other words, the dimension where an overlap would occur first as the posters scale up.

How do we determine which dimension is tighter? We can't simply compare the absolute distances $|x_i - x_j|$ and $|y_i - y_j|$, because the posters might not be squares ($w \neq h$). A poster might be very wide and short, making it more prone to horizontal overlap.

The correct approach is to compare the distances *normalized* by the poster dimensions. For a pair of posters $i$ and $j$, we compare:
$$ \frac{|x_i - x_j|}{w} \quad \text{and} \quad \frac{|y_i - y_j|}{h} $$

This comparison tells us how many base-widths or base-heights "fit" between the poster centers. The smaller of these two values corresponds to the more restrictive dimension.
*   If $\frac{|x_i - x_j|}{w} < \frac{|y_i - y_j|}{h}$, the posters are "closer" horizontally. We must add the x-dimension constraint: $\frac{w}{2}a_i + \frac{w}{2}a_j \le |x_i - x_j|$.
*   If $\frac{|y_i - y_j|}{h} < \frac{|x_i - x_j|}{w}$, the posters are "closer" vertically. We add the y-dimension constraint: $\frac{h}{2}a_i + \frac{h}{2}a_j \le |y_i - y_j|$.
*   If they are equal, we must add both constraints to be safe.

This logic ensures that for every pair of new posters, we add the constraint that will become active first, correctly modeling the non-overlap condition.

```cpp
#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpq.h>

typedef double IT;
typedef CGAL::Gmpq ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point;

double ceil_to_double(const CGAL::Quotient<ET> &x) {
  double a = std::ceil(CGAL::to_double(x));
  while (a < x) a += 1;
  while (a-1 >= x) a -= 1;
  return a;
}

void solve() {
  int n, m, h, w;
  std::cin >> n >> m >> h >> w;

  std::vector<Point> new_nails(n);
  for (int i = 0; i < n; ++i) {
    int x, y;
    std::cin >> x >> y;
    new_nails[i] = Point(x, y);
  }

  Program lp(CGAL::SMALLER, true, 1, false, 0); 
  
  int constraint_idx = 0;
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      Point p1 = new_nails[i];
      Point p2 = new_nails[j];
      
      double norm_dist_x = std::abs(p1.x() - p2.x()) / (double)w;
      double norm_dist_y = std::abs(p1.y() - p2.y()) / (double)h;

      if (norm_dist_x < norm_dist_y) {
        // Y constraint is tighter
        lp.set_a(i, constraint_idx, (double)h / 2.0);
        lp.set_a(j, constraint_idx, (double)h / 2.0);
        lp.set_b(constraint_idx, std::abs(p1.y() - p2.y()));
        constraint_idx++;
      } else {
        // X constraint is tighter or they are equal
        lp.set_a(i, constraint_idx, (double)w / 2.0);
        lp.set_a(j, constraint_idx, (double)w / 2.0);
        lp.set_b(constraint_idx, std::abs(p1.x() - p2.x()));
        constraint_idx++;
      }
    }
  }
  
  for (int i = 0; i < n; ++i) {
    lp.set_c(i, -(2.0 * w + 2.0 * h));
  }
  
  Solution s = CGAL::solve_linear_program(lp, ET());
  
  std::cout << std::fixed << std::setprecision(0);
  std::cout << ceil_to_double(-s.objective_value()) << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  int t;
  std::cin >> t;
  while (t--) {
    solve();
  }
  return 0;
}
```
</details>
<details>
<summary>Final Solution</summary>
The final step is to incorporate the $m$ old posters, which are already on the wall. These posters have a fixed scaling factor of 1.

### Constraints with Old Posters
For each new poster $i$ (with variable scaling factor $a_i$) and each old poster $k$ (with fixed scaling factor 1), we must ensure they do not overlap. The constraint logic from the previous solution still applies. For the x-dimension, the constraint would be:
$$ \frac{w}{2}a_i + \frac{w}{2}(1) \le |x_i - x_k| $$
This can be rearranged to isolate the variable $a_i$:
$$ \frac{w}{2}a_i \le |x_i - x_k| - \frac{w}{2} $$
A similar constraint exists for the y-dimension. As before, we only need to add the constraint for the dimension that is more restrictive, determined by comparing the normalized distances $|x_i - x_k|/w$ and $|y_i - y_k|/h$.

### Performance Optimization
A naive implementation would add constraints between each new poster and *every* old poster. With $n \le 30$ and $m \le 10^3$, this would result in approximately $n \times m = 30000$ new constraints, which is too slow.

We need to be smarter. For a given new poster $i$, which of the $m$ old posters imposes the tightest restriction on $a_i$? The tightest restriction comes from the old poster that is "closest" in the normalized distance metric. That is, for each new poster $i$, we should find the old poster $k$ that minimizes:
$$ \max\left(\frac{|x_i - x_k|}{w}, \frac{|y_i - y_k|}{h}\right) $$
This expression represents the scaling factor at which poster $i$ would first touch poster $k$. By finding the old poster $k$ that minimizes this value, we find the one that limits the growth of poster $i$ the most. We only need to add the non-overlapping constraint(s) for this single, most restrictive old poster. This reduces the number of additional constraints from $O(n \cdot m)$ to $O(n)$, making the LP feasible to solve.

Note: Using a Delaunay triangulation to find the "closest" old poster is tempting but incorrect. Delaunay triangulation is based on Euclidean (L2) distance, whereas our problem's geometry is defined by the L-infinity norm of the normalized coordinates, i.e., $\max(\Delta x/w, \Delta y/h)$. We must therefore iterate through all old posters for each new one to find the correct closest one.

To improve precision and potentially speed up the solver with integer arithmetic, we can multiply our constraints by 2, avoiding floating-point coefficients. For example, $\frac{w}{2}a_i + \frac{w}{2}a_j \le |x_i - x_j|$ becomes $w \cdot a_i + w \cdot a_j \le 2|x_i - x_j|$.

```cpp
#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <limits>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.hh>
#include <CGAL/Gmpz.h>

// Use integer coefficients for precision
typedef int IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point;

double ceil_to_double(const CGAL::Quotient<ET> &x) {
  double a = std::ceil(CGAL::to_double(x));
  while (a < x) a += 1;
  while (a-1 >= x) a -= 1;
  return a;
}

void solve() {
  int n, m, h, w;
  std::cin >> n >> m >> h >> w;

  std::vector<Point> new_nails(n);
  for (int i = 0; i < n; ++i) {
    long x, y; std::cin >> x >> y;
    new_nails[i] = Point(x, y);
  }

  std::vector<Point> old_nails(m);
  for (int i = 0; i < m; ++i) {
    long x, y; std::cin >> x >> y;
    old_nails[i] = Point(x, y);
  }

  Program lp(CGAL::SMALLER, true, 1, false, 0); 
  int constraint_idx = 0;

  // Constraints between pairs of new posters
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      double norm_dist_x = std::abs(new_nails[i].x() - new_nails[j].x()) / (double)w;
      double norm_dist_y = std::abs(new_nails[i].y() - new_nails[j].y()) / (double)h;

      if (norm_dist_x < norm_dist_y) {
        lp.set_a(i, constraint_idx, h); lp.set_a(j, constraint_idx, h);
        lp.set_b(constraint_idx, 2 * std::abs(new_nails[i].y() - new_nails[j].y()));
        constraint_idx++;
      } else {
        lp.set_a(i, constraint_idx, w); lp.set_a(j, constraint_idx, w);
        lp.set_b(constraint_idx, 2 * std::abs(new_nails[i].x() - new_nails[j].x()));
        constraint_idx++;
      }
    }
  }

  // Constraints between new posters and the CLOSEST old poster
  if (m > 0) {
    for (int i = 0; i < n; ++i) {
      double min_max_norm_dist = std::numeric_limits<double>::max();
      Point closest_old_nail = old_nails[0];

      for (int k = 0; k < m; ++k) {
        double norm_dist_x = std::abs(new_nails[i].x() - old_nails[k].x()) / (double)w;
        double norm_dist_y = std::abs(new_nails[i].y() - old_nails[k].y()) / (double)h;
        double max_norm_dist = std::max(norm_dist_x, norm_dist_y);
        if (max_norm_dist < min_max_norm_dist) {
          min_max_norm_dist = max_norm_dist;
          closest_old_nail = old_nails[k];
        }
      }
      
      double norm_dist_x = std::abs(new_nails[i].x() - closest_old_nail.x()) / (double)w;
      double norm_dist_y = std::abs(new_nails[i].y() - closest_old_nail.y()) / (double)h;

      if (norm_dist_x < norm_dist_y) {
        lp.set_a(i, constraint_idx, h);
        lp.set_b(constraint_idx, 2 * std::abs(new_nails[i].y() - closest_old_nail.y()) - h);
        constraint_idx++;
      } else {
        lp.set_a(i, constraint_idx, w);
        lp.set_b(constraint_idx, 2 * std::abs(new_nails[i].x() - closest_old_nail.x()) - w);
        constraint_idx++;
      }
    }
  }

  // Objective function
  for (int i = 0; i < n; ++i) {
    lp.set_c(i, -(2 * w + 2 * h));
  }

  Solution s = CGAL::solve_linear_program(lp, ET());
  
  std::cout << std::fixed << std::setprecision(0);
  std::cout << ceil_to_double(-s.objective_value()) << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  int t;
  std::cin >> t;
  while (t--) {
    solve();
  }
  return 0;
}
```
</details>

## ⚡ Result

```plaintext

```