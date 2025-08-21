# World Cup

## 📝 Problem Description

The goal is to maximize the total profit from selling beer during a sporting event. We are given a set of $N$ warehouses and $M$ stadiums, each with a specific location.

Each warehouse stores a single brand of beer, characterized by its available supply and alcohol percentage. Each stadium has a required demand for beer (in liters) that must be met exactly. Additionally, for each stadium, there is a maximum permissible limit on the total amount of pure alcohol that can be sold.

Beer can be transported from any warehouse to any stadium. For each warehouse-stadium pair, there is a base revenue per liter. This base revenue is reduced by a transportation cost which depends on crossing geographical contour lines, modeled as circles. The cost is a fixed amount per liter for each contour line crossed. A delivery from a warehouse to a stadium is assumed to cross a circular contour line if and only if one is inside the circle and the other is outside.

The task is to determine the amount of beer to ship from each warehouse to each stadium to maximize the total profit. The total profit is the sum of revenues minus transportation costs over all shipments. If it's impossible to satisfy all constraints (i.e., meet all demands while respecting supply and alcohol limits), we should report that. Otherwise, we should output the maximum possible profit.

## 💡 Hints

<details>
<summary>Hint #1</summary>
This problem involves maximizing a certain value (profit) subject to a collection of linear constraints (supply limits, demand requirements, alcohol limits). This structure is characteristic of a particular class of optimization problems. Can you identify which one?
</details>
<details>
<summary>Hint #2</summary>
The problem can be modeled as a Linear Program (LP). The variables of your LP should represent the quantity of beer transported from each warehouse to each stadium. How would you formulate the objective function and the constraints using these variables?
</details>
<details>
<summary>Hint #3</summary>
A major challenge is calculating the transportation cost, which involves checking intersections with up to a million contour lines. A naive check for every warehouse-stadium pair against every contour line will be too slow. Can you find a way to quickly filter out the contour lines that are "irrelevant" for any transport? Also, be mindful of potential floating-point precision issues when setting up the LP. It's often safer to work with integer coefficients by scaling the equations appropriately.
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1, 2)</summary>

### Introduction

This problem, with its goal of maximizing a value under several linear constraints, is a classic application of **Linear Programming (LP)**. The initial thought might be to model it as a min-cost max-flow problem, but an LP formulation is more direct and versatile for the given constraints.

This first solution addresses the simpler versions of the problem (Test Sets 1 and 2) where there are no contour lines ($c=0$). This simplifies the profit calculation significantly, as there are no transportation costs to subtract.

### LP Formulation

Let's define our variables and constraints for the linear program. The core decision we need to make is how many liters of beer to transport from each warehouse $w$ to each stadium $s$.

**Variables:**
Let $x_{w,s}$ be the amount of beer (in liters) transported from warehouse $w$ to stadium $s$. These are the variables we want our LP solver to determine. Since we cannot transport a negative amount of beer, we have the implicit constraint $x_{w,s} \ge 0$ for all pairs $(w, s)$.

**Objective Function:**
Our goal is to maximize the total profit. For test sets with $c=0$, the profit is simply the sum of revenues.
Let $r_{w,s}$ be the revenue per liter for transporting beer from warehouse $w$ to stadium $s$.
The objective is to maximize:
$$ \sum_{w=1}^{N} \sum_{s=1}^{M} x_{w,s} \cdot r_{w,s} $$

**Constraints:**
The problem statement imposes several conditions that must be translated into linear constraints:

1.  **Supply Constraint:** The total amount of beer shipped from a warehouse cannot exceed its supply. Let $S_w$ be the supply at warehouse $w$.
    $$ \forall w \in \{1, \dots, N\}: \sum_{s=1}^{M} x_{w,s} \le S_w $$

2.  **Demand Constraint:** The total amount of beer received by a stadium must exactly match its demand. Let $D_s$ be the demand at stadium $s$.
    $$ \forall s \in \{1, \dots, M\}: \sum_{w=1}^{N} x_{w,s} = D_s $$
    *Note: An equality constraint $\sum x = D$ is typically implemented in LP solvers as two inequality constraints: $\sum x \le D$ and $\sum x \ge D$ (which is equivalent to $-\sum x \le -D$).*

3.  **Alcohol Limit Constraint:** The total amount of pure alcohol delivered to a stadium must not exceed its upper limit. Let $A_w$ be the alcohol percentage of the beer in warehouse $w$, and $U_s$ be the alcohol limit (in liters) at stadium $s$.
    $$ \forall s \in \{1, \dots, M\}: \sum_{w=1}^{N} x_{w,s} \cdot \frac{A_w}{100} \le U_s $$

### Implementation Details

A common issue with LP solvers is floating-point precision. To ensure accuracy, it's a good practice to work with integer coefficients. We can achieve this by changing our units. For instance, instead of liters, we can work with milliliters (ml).

*   **Amounts:** Convert all supplies, demands, and alcohol limits from liters to milliliters by multiplying by 1000.
*   **Alcohol Content:** The alcohol percentage $A_w$ means that 1 liter (1000 ml) of beer contains $1000 \cdot (A_w/100) = 10 \cdot A_w$ ml of pure alcohol. This gives us a convenient integer factor.

With these unit changes, our constraints can be written with integer coefficients, making the solution robust. The LP solver will find the optimal values for $x_{w,s}$ (in liters). The final maximum profit is calculated from the objective function's value. If the solver reports that the problem is infeasible, it means no solution exists that satisfies all constraints.

```cpp
#include <iostream>
#include <vector>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

// Use Gmpz for exact representation of numbers
typedef CGAL::Gmpz ET;
// Use doubles for input type, as it's sufficient for the coefficients
typedef double IT;

// Setup the linear program and solution types
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

// Struct to hold warehouse data
struct Warehouse {
  int supply; // Supply of Beer in ml
  int alcohol_per_liter; // Alcohol content in ml per liter of beer
  
  Warehouse(int s, int a) : supply(s * 1000), alcohol_per_liter(a * 10) {}
};

// Struct to hold stadium data
struct Stadium {
  int demand; // Demand of Beer in ml
  int alcohol_limit; // Upper Limit on the pure alcohol in ml
  
  Stadium(int d, int u) : demand(d * 1000), alcohol_limit(u * 1000) {}
};

// Custom floor function for CGAL's exact number type
long floor_to_long(const CGAL::Quotient<ET>& x) {
  double a = std::floor(CGAL::to_double(x));
  while (a > x) a -= 1;
  while (a + 1 <= x) a += 1;
  return static_cast<long>(a);
}

void solve() {
  int n, m, c;
  std::cin >> n >> m >> c;

  std::vector<Warehouse> warehouses;
  warehouses.reserve(n);
  for (int i = 0; i < n; ++i) {
    int x, y, s, a;
    std::cin >> x >> y >> s >> a;
    warehouses.emplace_back(s, a);
  }

  std::vector<Stadium> stadiums;
  stadiums.reserve(m);
  for (int i = 0; i < m; ++i) {
    int x, y, d, u;
    std::cin >> x >> y >> d >> u;
    stadiums.emplace_back(d, u);
  }

  std::vector<std::vector<int>> revenues(n, std::vector<int>(m));
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      std::cin >> revenues[i][j];
    }
  }

  // For Test Sets 1 & 2, we ignore contour lines (variable c is read but not used)
  
  // Create a linear program: variables x >= 0, no upper bound
  Program lp(CGAL::SMALLER, true, 0, false, 0);
  int constraint_idx = 0;

  // Set up the objective function and constraints
  for (int w = 0; w < n; ++w) {
    for (int s = 0; s < m; ++s) {
      int var_idx = w * m + s;
      // LP minimizes, so we negate the revenue to maximize it
      lp.set_c(var_idx, -revenues[w][s]);
    }
  }

  // Constraint 1: Supply limits for each warehouse
  for (int w = 0; w < n; ++w) {
    for (int s = 0; s < m; ++s) {
      // x_ws is in liters, so multiply by 1000 for ml
      lp.set_a(w * m + s, constraint_idx, 1000);
    }
    lp.set_b(constraint_idx, warehouses[w].supply);
    constraint_idx++;
  }

  // Constraints 2 & 3: Demand and alcohol limits for each stadium
  for (int s = 0; s < m; ++s) {
    // Constraint 2: Alcohol Limit
    for (int w = 0; w < n; ++w) {
      // x_ws (liters) * alcohol_per_liter (ml/liter) <= alcohol_limit (ml)
      lp.set_a(w * m + s, constraint_idx, warehouses[s].alcohol_per_liter);
    }
    lp.set_b(constraint_idx, stadiums[s].alcohol_limit);
    constraint_idx++;

    // Constraint 3: Demand Equality
    // We model sum(x_ws) = D_s with two inequalities:
    // sum(x_ws) <= D_s  and  sum(x_ws) >= D_s
    for (int w = 0; w < n; ++w) {
        // sum(1000 * x_ws) <= demand_in_ml
        lp.set_a(w * m + s, constraint_idx, 1000);
        // -sum(1000 * x_ws) <= -demand_in_ml
        lp.set_a(w * m + s, constraint_idx + 1, -1000);
    }
    lp.set_b(constraint_idx, stadiums[s].demand);
    lp.set_b(constraint_idx + 1, -stadiums[s].demand);
    constraint_idx += 2;
  }

  // Solve the LP
  Solution sol = CGAL::solve_linear_program(lp, ET());
  
  if (sol.is_infeasible()) {
    std::cout << "no" << std::endl;
  } else {
    // The result is a fraction; convert to double and take the floor
    // Negate again because we minimized -profit
    std::cout << floor_to_long(-sol.objective_value()) << std::endl;
  }
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

### Generalizing the Solution

To solve the problem for all test sets, we must incorporate the cost associated with crossing contour lines. This cost directly affects our profit, and therefore modifies the objective function of our Linear Program. The constraints remain identical to the first solution.

The actual profit for transporting one liter of beer from warehouse $w$ to stadium $s$ is $r_{w,s} - \frac{t_{w,s}}{100}$, where $t_{w,s}$ is the number of contour lines separating $w$ and $s$.

Our new objective function is to maximize:
$$ \sum_{w=1}^{N} \sum_{s=1}^{M} x_{w,s} \cdot \left(r_{w,s} - \frac{t_{w,s}}{100}\right) $$

To maintain integer coefficients in our LP, we can multiply the entire expression by 100. The objective then becomes maximizing:
$$ \sum_{w=1}^{N} \sum_{s=1}^{M} x_{w,s} \cdot (100 \cdot r_{w,s} - t_{w,s}) $$
The final objective value returned by the solver will be 100 times the actual profit, so we must remember to divide it by 100 before outputting the result.

### The Challenge of Counting Intersections

The main difficulty is computing $t_{w,s}$ for all pairs $(w, s)$ efficiently. A brute-force approach would be:
```
for each warehouse w:
  for each stadium s:
    t_ws = 0
    for each contour line c:
      if w and s are on opposite sides of c:
        t_ws++
```
With up to $N=200$, $M=20$, and $C=10^6$, this $O(N \cdot M \cdot C)$ approach is far too slow.

However, the problem statement provides a crucial hint for Test Set 4: at most 100 contour lines contain at least one warehouse or stadium. A contour line that contains no locations in its interior cannot separate any pair of locations. This suggests a filtering strategy.

### Efficiently Filtering Contour Lines

We can significantly reduce the number of contour lines we need to check by filtering out those that are "irrelevant". A contour line is only relevant if it has at least one warehouse or stadium inside it.

To check this efficiently for all $C$ contour lines:
1.  Collect the coordinates of all $N$ warehouses and $M$ stadiums.
2.  Build a **Delaunay Triangulation** from these $N+M$ points. A key property of Delaunay triangulations is that they provide a fast way to find the nearest neighbor to any query point.
3.  For each of the $C$ contour lines, find the vertex in the triangulation that is nearest to the circle's center.
4.  If the squared distance from the circle's center to this nearest point is less than or equal to the circle's squared radius, it means the point is inside or on the circle. We mark this contour line as "relevant".
5.  This filtering step takes roughly $O(C \log(N+M))$ time.

After filtering, we are left with a much smaller set of relevant contour lines (at most 100). We can then use the brute-force method on this reduced set to calculate all $t_{w,s}$ values in $O(N \cdot M \cdot |\text{relevant_contours}|)$, which is fast enough.

With the $t_{w,s}$ values pre-calculated, we set up and solve the LP just as before, but with the modified objective function coefficients.

```cpp
#include <iostream>
#include <vector>
#include <iomanip>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Triangulation;
typedef K::Point_2 Point;
typedef K::Circle_2 Circle;

// Use Gmpz for exact representation of numbers
typedef CGAL::Gmpz ET;
// Use integers for input type in the LP
typedef int IT;

typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

// Struct to hold warehouse data
struct Warehouse {
  Point pos;
  int supply; // Supply of Beer in ml
  int alcohol_per_liter; // Alcohol content in ml per liter of beer
  
  Warehouse(int x, int y, int s, int a) : pos(x, y), supply(s * 1000), alcohol_per_liter(a * 10) {}
};

// Struct to hold stadium data
struct Stadium {
  Point pos;
  int demand; // Demand of Beer in ml
  int alcohol_limit; // Upper Limit on the pure alcohol in ml
  
  Stadium(int x, int y, int d, int u) : pos(x, y), demand(d * 1000), alcohol_limit(u * 1000) {}
};

// Custom floor function for CGAL's exact number type
long floor_to_long(const CGAL::Quotient<ET>& x) {
  double a = std::floor(CGAL::to_double(x));
  while (a > x) a -= 1;
  while (a + 1 <= x) a += 1;
  return static_cast<long>(a);
}

void solve() {
  int n, m, c;
  std::cin >> n >> m >> c;

  std::vector<Point> locations;
  locations.reserve(n + m);
  std::vector<Warehouse> warehouses;
  warehouses.reserve(n);
  for (int i = 0; i < n; ++i) {
    long x, y;
    int s, a;
    std::cin >> x >> y >> s >> a;
    warehouses.emplace_back(x, y, s, a);
    locations.push_back(Point(x, y));
  }

  std::vector<Stadium> stadiums;
  stadiums.reserve(m);
  for (int i = 0; i < m; ++i) {
    long x, y;
    int d, u;
    std::cin >> x >> y >> d >> u;
    stadiums.emplace_back(x, y, d, u);
    locations.push_back(Point(x, y));
  }

  std::vector<std::vector<int>> revenues(n, std::vector<int>(m));
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      std::cin >> revenues[i][j];
    }
  }

  std::vector<Circle> contours;
  contours.reserve(c);
  for (int i = 0; i < c; ++i) {
    long x, y, r;
    std::cin >> x >> y >> r;
    contours.emplace_back(Point(x, y), (K::FT)r * r);
  }

  // Filter relevant contour lines
  Triangulation t;
  t.insert(locations.begin(), locations.end());

  std::vector<Circle> relevant_contours;
  for (const auto& contour : contours) {
    Point center = contour.center();
    Point nearest_loc = t.nearest_vertex(center)->point();
    if (CGAL::squared_distance(center, nearest_loc) < contour.squared_radius()) {
      relevant_contours.push_back(contour);
    }
  }
  
  // Count intersections for each warehouse-stadium pair
  std::vector<std::vector<int>> intersections(n, std::vector<int>(m, 0));
  for (int w = 0; w < n; ++w) {
    for (int s = 0; s < m; ++s) {
      for (const auto& contour : relevant_contours) {
        bool w_inside = contour.has_on_bounded_side(warehouses[w].pos);
        bool s_inside = contour.has_on_bounded_side(stadiums[s].pos);
        if (w_inside != s_inside) {
          intersections[w][s]++;
        }
      }
    }
  }

  // Create a linear program
  Program lp(CGAL::SMALLER, true, 0, false, 0);
  int constraint_idx = 0;

  // Set up the objective function (multiplied by 100 for integer coefficients)
  for (int w = 0; w < n; ++w) {
    for (int s = 0; s < m; ++s) {
      int var_idx = w * m + s;
      int cost = 100 * revenues[w][s] - intersections[w][s];
      lp.set_c(var_idx, -cost); // Negate to maximize
    }
  }

  // Constraint 1: Supply limits
  for (int w = 0; w < n; ++w) {
    for (int s = 0; s < m; ++s) {
      lp.set_a(w * m + s, constraint_idx, 1000);
    }
    lp.set_b(constraint_idx, warehouses[w].supply);
    constraint_idx++;
  }

  // Constraints 2 & 3: Demand and alcohol limits
  for (int s = 0; s < m; ++s) {
    for (int w = 0; w < n; ++w) {
      // Alcohol limit
      lp.set_a(w * m + s, constraint_idx, warehouses[w].alcohol_per_liter);
      // Demand equality
      lp.set_a(w * m + s, constraint_idx + 1, 1000);
      lp.set_a(w * m + s, constraint_idx + 2, -1000);
    }
    lp.set_b(constraint_idx, stadiums[s].alcohol_limit);
    lp.set_b(constraint_idx + 1, stadiums[s].demand);
    lp.set_b(constraint_idx + 2, -stadiums[s].demand);
    constraint_idx += 3;
  }

  Solution sol = CGAL::solve_linear_program(lp, ET());

  if (sol.is_infeasible()) {
    std::cout << "no" << std::endl;
  } else {
    // Objective value is 100*profit. Divide by 100 and floor.
    std::cout << floor_to_long(-sol.objective_value() / 100) << std::endl;
  }
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