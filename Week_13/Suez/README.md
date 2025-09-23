# Suez

## 📝 Problem Description

Given the locations of $n$ new nails and $m$ old nails on a 2D plane, the task is to hang one new rectangular poster, centered on each of the $n$ new nails. The posters already hanging on the $m$ old nails must remain untouched.

Initially, all posters have a standard size of $h$ inches in height and $w$ inches in width. For each of the $n$ new posters, a scaling factor $a \ge 1$ may be chosen to create a larger poster of size $(a \cdot h) \times (a \cdot w)$. Each new poster can have its own distinct scaling factor.

The primary constraint is that no two posters on the wall (new or old) may overlap. The objective is to select the scaling factors for the $n$ new posters so that the sum of their perimeters is maximized. The maximum possible sum of perimeters, rounded up to the nearest integer, must be computed.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem asks you to maximize a linear objective (sum of perimeters) subject to certain constraints (non-overlapping posters). The scaling factors are continuous variables with linear relationships. This structure strongly suggests using **Linear Programming (LP)** to find the optimal solution.

</details>

<details>

<summary>Hint #2</summary>

For each pair of posters that must not overlap, you need to ensure their projections don't overlap in at least one dimension (x or y). This translates to linear constraints of the form: for posters $i$ and $j$, either $\frac{w}{2}a_i + \frac{w}{2}a_j \leq |x_i - x_j|$ (x-dimension) or $\frac{h}{2}a_i + \frac{h}{2}a_j \leq |y_i - y_j|$ (y-dimension) must hold. The key is determining which constraint to actually enforce.

</details>

<details>

<summary>Hint #3</summary>

You cannot add constraints for both x and y dimensions for every pair of posters, as this would be overly restrictive. Two rectangles don't overlap if they don't overlap in **at least one** dimension. Therefore, for each pair of posters, you only need to enforce the constraint in **one dimension** - the dimension where overlap would occur first as the posters scale up.

</details>

<details>

<summary>Hint #4</summary>

To determine which dimension constraint to apply for each pair of posters, compare the **normalized distances**: $\frac{|x_i - x_j|}{w}$ versus $\frac{|y_i - y_j|}{h}$. The smaller of these two values corresponds to the more restrictive dimension - the one where overlap would happen first. This tells you whether to enforce the x-constraint or y-constraint for that pair.

</details>

<details>

<summary>Hint #5</summary>

**Important:** Do NOT use Delaunay triangulation for this problem! While the problem involves 2D geometry, it uses the $L_\infty$ norm (Manhattan/Chebyshev distance) for rectangular shapes, not the $L_2$ norm (Euclidean distance) that Delaunay triangulation is designed for. The rectangular poster overlap conditions require a different approach than typical point-distance problems.

</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Set 1)</summary>

This problem involves maximizing a value subject to a set of constraints, which strongly suggests using **Linear Programming (LP)**. The core of this approach is to define the variables, the objective function, and the constraints.

### Variables
The quantities we can control are the scaling factors for each of the $n$ new posters. Let's denote the scaling factor for the $i$-th new poster as $a_i$. Since the posters can only be magnified, we have a lower bound $a_i \ge 1$ for all $i=1, \dots, n$.

### Objective Function
We want to maximize the sum of the perimeters of the new posters. The perimeter of a poster $i$ with scaling factor $a_i$ is $2(a_i \cdot w + a_i \cdot h) = a_i(2w + 2h)$. Our objective is to maximize:
  $$ \sum_{i=1}^{n} a_i (2w + 2h) $$

### Constraints
The posters must not overlap. For the first test set, all nails lie on a horizontal line (e.g., $y=0$), and there are no old posters ($m=0$). This simplifies the problem significantly, as we only need to consider overlaps along the x-axis.

Consider two new posters, $i$ and $j$, centered at $(x_i, 0)$ and $(x_j, 0)$. Let's assume $x_i < x_j$. The posters are scaled by $a_i$ and $a_j$, so their widths are $a_i \cdot w$ and $a_j \cdot w$. Their right and left boundaries are at $x_i \pm \frac{a_i w}{2}$ and $x_j \pm \frac{a_j w}{2}$. To prevent overlap, the right edge of poster $i$ must be to the left of the left edge of poster $j$. <br />
**Note**: We can always easily check which points lies on the left and which on the right using a simple if statement. This is necessary to ensure that the distance $d$ is positive and correctly calculated

$$ x_i + \frac{a_i w}{2} \le x_j - \frac{a_j w}{2} $$

Rearranging this gives us a linear constraint on our variables $a_i$ and $a_j$:

$$ \frac{w}{2} a_i + \frac{w}{2} a_j \le x_j - x_i $$

We must add such a constraint for every pair of new posters $(i, j)$.

By setting up this LP with the CGAL library and solving it, we can find the optimal values for $a_i$ and thus the maximum total perimeter.

### Code
```cpp
#include <iostream>
#include <vector>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef double IT;
typedef CGAL::Gmpq ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;
typedef Solution::Variable_value_iterator SVI;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;
typedef Triangulation::Edge_iterator  Edge_iterator;

typedef K::Point_2 Point;

double ceil_to_double(const CGAL::Quotient<ET> &x) {
  double a = std::ceil(CGAL::to_double(x));
  while (a < x) a += 1;
  while (a-1 >= x) a -= 1;
  return a;
}

void solve() {
  // ===== READ INPUT =====
  int n, m, h, w; std::cin >> n >> m >> h >> w;
  
  // Read new nails
  std::vector<Point> new_nails; new_nails.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    new_nails.emplace_back(x, y);
  }
  
  // Read old nails
  std::vector<Point> old_nails; old_nails.reserve(m);
  for(int i = 0; i < m; ++i) {
    int x, y; std::cin >> x >> y;
    old_nails.emplace_back(x, y);
  }
  
  // ===== SOLVE =====
  Program lp (CGAL::SMALLER, true, 1, false, 0); 
  double h_halves = ((double) h) / 2.0;
  double w_halves = ((double) w) / 2.0;
  int n_constraints = 0;
  
  // Set constraints that the distance between all new posters needs to be at least 0
  for(int i = 0; i < n; ++i) {
    const Point p_1 = new_nails[i];
    
    for(int j = i + 1; j < n; ++j) {
      const Point p_2 = new_nails[j];
      
      // Determine which of the two points is left and which is right
      const Point *p_left, *p_right;
      if(p_1.x() < p_2.x()) {
        p_left = &p_1;
        p_right = &p_2;
      } else {
        p_left = &p_2;
        p_right = &p_1;
      }
      
      // Add constraint
      lp.set_a(i, n_constraints, w_halves);
      lp.set_a(j, n_constraints, w_halves);
      lp.set_b(n_constraints, p_right->x() - p_left->x());
      
      n_constraints++;
    }
  }
  
  // Set objective
  for(int i = 0; i < n; ++i) {
    lp.set_c(i, -2*w - 2*h);
  }
  
  // Solve LP and calculate sum of perimeters
  Solution s = CGAL::solve_linear_program(lp, ET());
  if(!s.is_optimal()) { std::cout << "ERROR: SOLUTION NOT OPTIMAL" << std::endl; return; }
  
  // ===== OUTPUT =====
  std::cout << std::fixed << std::setprecision(0);
  std::cout << ceil_to_double(-s.objective_value()) << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
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

### Code
```cpp
#include <iostream>
#include <vector>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
// #include <CGAL/Delaunay_triangulation_2.h>

typedef double IT;
typedef CGAL::Gmpq ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;
typedef Solution::Variable_value_iterator SVI;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;
// typedef Triangulation::Edge_iterator  Edge_iterator;

typedef K::Point_2 Point;

double ceil_to_double(const CGAL::Quotient<ET> &x) {
  double a = std::ceil(CGAL::to_double(x));
  while (a < x) a += 1;
  while (a-1 >= x) a -= 1;
  return a;
}

void solve() {
  // std::cout << "==========================================================================" << std::endl;
  // ===== READ INPUT =====
  int n, m, h, w; std::cin >> n >> m >> h >> w;
  
  // Read new nails
  std::vector<Point> new_nails; new_nails.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    new_nails.emplace_back(x, y);
  }
  
  // Read old nails
  std::vector<Point> old_nails; old_nails.reserve(m);
  for(int i = 0; i < m; ++i) {
    int x, y; std::cin >> x >> y;
    old_nails.emplace_back(x, y);
  }
  
  // ===== SOLVE =====
  Program lp (CGAL::SMALLER, true, 1, false, 0); 
  double h_halves = ((double) h) / 2.0;
  double w_halves = ((double) w) / 2.0;
  int n_constraints = 0;
  
  // Set constraints that the distance between all new posters needs to be at least 0
  for(int i = 0; i < n; ++i) {
    const Point p_1 = new_nails[i];
    
    for(int j = i + 1; j < n; ++j) {
      const Point p_2 = new_nails[j];
      
      // Calculate distance in x and y direction
      double x_dist = std::abs(p_1.x() - p_2.x()) / ((double) w);
      double y_dist = std::abs(p_1.y() - p_2.y()) / ((double) h);
      
      // Determine in which dimensions to add constraints
      bool add_x_constraint = false;
      bool add_y_constraint = false;
      if(x_dist < y_dist) { add_y_constraint = true; }
      else if(x_dist > y_dist) { add_x_constraint = true; }
      else { add_x_constraint = add_y_constraint = true; }

      // Add constraints
      if(add_x_constraint) {
        const Point *p_left, *p_right;
        if(p_1.x() < p_2.x()) {
          p_left = &p_1;
          p_right = &p_2;
        } else {
          p_left = &p_2;
          p_right = &p_1;
        }
        
        // Add constraint
        lp.set_a(i, n_constraints, w_halves);
        lp.set_a(j, n_constraints, w_halves);
        lp.set_b(n_constraints, p_right->x() - p_left->x());
        
        n_constraints++;
      }
      if(add_y_constraint) {
        const Point *p_bot, *p_top;
        if(p_1.y() < p_2.y()) {
          p_bot = &p_1;
          p_top = &p_2;
        } else {
          p_bot = &p_2;
          p_top = &p_1;
        }
        
        // Add constraint
        lp.set_a(i, n_constraints, h_halves);
        lp.set_a(j, n_constraints, h_halves);
        lp.set_b(n_constraints, p_top->y() - p_bot->y());
        
        n_constraints++;
      }
    }
  }
  
  // Set objective
  for(int i = 0; i < n; ++i) {
    lp.set_c(i, -2*w - 2*h);
  }
  
  // Solve LP and calculate sum of perimeters
  Solution s = CGAL::solve_linear_program(lp, ET());
  if(!s.is_optimal()) { std::cout << "ERROR: SOLUTION NOT OPTIMAL" << std::endl; return; }
  
  // ===== OUTPUT =====
  std::cout << std::fixed << std::setprecision(0);
  std::cout << ceil_to_double(-s.objective_value()) << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
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

**Note**: Using a Delaunay triangulation to find the "closest" old poster is tempting but incorrect. Delaunay triangulation is based on Euclidean (L2) distance, whereas our problem's geometry is defined by the L-infinity norm of the normalized coordinates, i.e., $\max(\Delta x/w, \Delta y/h)$. We must therefore iterate through all old posters for each new one to find the correct closest one.

Additionally, to improve precision and potentially speed up the solver with integer arithmetic, we can multiply our constraints by 2, avoiding floating-point coefficients. For example, $\frac{w}{2}a_i + \frac{w}{2}a_j \le |x_i - x_j|$ becomes $w \cdot a_i + w \cdot a_j \le 2|x_i - x_j|$.

### Code
```cpp
#include <iostream>
#include <vector>
#include <limits>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef int IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;
typedef Solution::Variable_value_iterator SVI;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;

typedef K::Point_2 Point;

double ceil_to_double(const CGAL::Quotient<ET> &x) {
  double a = std::ceil(CGAL::to_double(x));
  while (a < x) a += 1;
  while (a-1 >= x) a -= 1;
  return a;
}

void solve() {
  // ===== READ INPUT =====
  int n, m, h, w; std::cin >> n >> m >> h >> w;
  
  // Read new nails
  std::vector<Point> new_nails; new_nails.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    new_nails.emplace_back(x, y);
  }
  
  // Read old nails
  std::vector<Point> old_nails; old_nails.reserve(m);
  for(int i = 0; i < m; ++i) {
    int x, y; std::cin >> x >> y;
    old_nails.emplace_back(x, y);
  }
  
  // ===== SOLVE =====
  // Triangulate old nails
  Triangulation t;
  t.insert(old_nails.begin(), old_nails.end());
  
  // Setup Linear Program
  Program lp (CGAL::SMALLER, true, 1, false, 0); 
  int n_constraints = 0;
  
  // Set constraints 
  for(int i = 0; i < n; ++i) {
    const Point p_1 = new_nails[i];
    
    // Set constraints for new nails
    for(int j = i + 1; j < n; ++j) {
      const Point p_2 = new_nails[j];
      
      // Calculate distance in x and y direction
      double x_dist = std::abs(p_1.x() - p_2.x()) / ((double) w);
      double y_dist = std::abs(p_1.y() - p_2.y()) / ((double) h);
      
      // Determine in which dimensions to add constraints
      bool add_x_constraint = false;
      bool add_y_constraint = false;
      if(x_dist < y_dist) { add_y_constraint = true; }
      else if(x_dist > y_dist) { add_x_constraint = true; }
      else { add_x_constraint = add_y_constraint = true; }

      // Add constraints
      if(add_x_constraint) {
        const Point *p_left, *p_right;
        if(p_1.x() < p_2.x()) {
          p_left = &p_1;
          p_right = &p_2;
        } else {
          p_left = &p_2;
          p_right = &p_1;
        }
        
        // Add constraint
        lp.set_a(i, n_constraints, w);
        lp.set_a(j, n_constraints, w);
        lp.set_b(n_constraints, 2 * p_right->x() - 2 * p_left->x());
        
        n_constraints++;
      }
      if(add_y_constraint) {
        const Point *p_bot, *p_top;
        if(p_1.y() < p_2.y()) {
          p_bot = &p_1;
          p_top = &p_2;
        } else {
          p_bot = &p_2;
          p_top = &p_1;
        }
        
        // Add constraint
        lp.set_a(i, n_constraints, h);
        lp.set_a(j, n_constraints, h);
        lp.set_b(n_constraints, 2 * p_top->y() - 2 * p_bot->y());
        
        n_constraints++;
      }
    }
    
    // Set constraint for old nails
    if(m > 0) {
      // Find closes old nail in terms of L1 Norm
      int min_dist = std::numeric_limits<int>::max();
      int min_idx = -1;
      for(int j = 0; j < m; j++) {
        const Point p_2 = old_nails[j];
        
        // Calculate distance in x and y direction
        double x_dist = std::abs(p_1.x() - p_2.x()) / ((double) w);
        double y_dist = std::abs(p_1.y() - p_2.y()) / ((double) h);
        
        if(std::max(x_dist, y_dist) < min_dist) {
          min_dist = std::max(x_dist, y_dist);
          min_idx = j;
        }
      }
      
      // Calculate distance in x and y direction
      const Point p_2 = old_nails[min_idx];
      
      double x_dist = std::abs(p_1.x() - p_2.x()) / ((double) w);
      double y_dist = std::abs(p_1.y() - p_2.y()) / ((double) h);
      
      // Determine in which dimensions to add constraints
      bool add_x_constraint = false;
      bool add_y_constraint = false;
      if(x_dist < y_dist) { add_y_constraint = true; }
      else if(x_dist > y_dist) { add_x_constraint = true; }
      else { add_x_constraint = add_y_constraint = true; }
  
      // Add constraints
      if(add_x_constraint) {
        const Point *p_left, *p_right;
        if(p_1.x() < p_2.x()) {
          p_left = &p_1;
          p_right = &p_2;
        } else {
          p_left = &p_2;
          p_right = &p_1;
        }
        
        // Add constraint
        lp.set_a(i, n_constraints, w);
        lp.set_b(n_constraints, 2 * p_right->x() - 2 * p_left->x() - w);
        
        n_constraints++;
      }
      if(add_y_constraint) {
        const Point *p_bot, *p_top;
        if(p_1.y() < p_2.y()) {
          p_bot = &p_1;
          p_top = &p_2;
        } else {
          p_bot = &p_2;
          p_top = &p_1;
        }
        
        // Add constraint
        lp.set_a(i, n_constraints, h);
        lp.set_b(n_constraints, 2 * p_top->y() - 2 * p_bot->y() - h);
        
        n_constraints++;
      }
    }
  }
  
  // Set objective
  for(int i = 0; i < n; ++i) {
    lp.set_c(i, -2*w - 2*h);
  }
  
  // Solve LP and calculate sum of perimeters
  Solution s = CGAL::solve_linear_program(lp, ET());
  if(!s.is_optimal()) { std::cout << "ERROR: SOLUTION NOT OPTIMAL" << std::endl; return; }
  
  // ===== OUTPUT =====
  std::cout << std::fixed << std::setprecision(0);
  std::cout << ceil_to_double(-s.objective_value()) << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (25 pts / 1.500 s) : Correct answer      (0.081s)
   Test set 2 (25 pts / 1.500 s) : Correct answer      (0.238s)
   Test set 3 (25 pts / 1.500 s) : Correct answer      (0.431s)
   Test set 4 (25 pts / 1.500 s) : Correct answer      (0.55s)

Total score: 100
```