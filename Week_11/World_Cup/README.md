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

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point;
typedef K::Segment_2 Segment;
typedef K::Circle_2 Circle;

typedef double IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

bool DEBUG = false;
bool PRINT_CONSTRAINTS = true;

struct Warehouse {
  Point pos;
  int s;  // Supply of Beer in ml
  int a;  // Alcohol per Liter of Beer in ml
  
  Warehouse(int x, int y, int s, int a) : pos(x, y), s(s * 1000), a(a * 10) {}
};

struct Stadium {
  Point pos;
  int d;  // Demand of Beer in ml
  int u;  // Upper Limit on the pure alcohol in ml
  
  Stadium(int x, int y, int d, int u) : pos(x, y), d(d * 1000), u(u * 1000) {}
};

long floor_to_double(const CGAL::Quotient<ET>& x)
{
  double a = std::floor(CGAL::to_double(x));
  while (a > x) a -= 1;
  while (a+1 <= x) a += 1;
  return a;
}

void solve() {
  if (DEBUG) { std::cout << "================================================================" << std::endl; }
  // ===== READ INPUT =====
  int n, m, c; std::cin >> n >> m >> c;
  
  std::vector<Warehouse> warehouses; warehouses.reserve(n);
  std::vector<Stadium> stadiums; stadiums.reserve(m);
  std::vector<Circle> contour_lines; contour_lines.reserve(c); 
  std::vector<std::vector<int>> revenues(n, std::vector<int>(m)); 
  
  // Read warehouses
  for(int i = 0; i < n; ++i) {
    int x, y, s, a; std::cin >> x >> y >> s >> a;
    warehouses.emplace_back(x, y, s, a);
  }
  
  // Read stadiums
  for(int i = 0; i < m; ++i) {
    int x, y, d, u; std::cin >> x >> y >> d >> u;
    stadiums.emplace_back(x, y, d, u);
  }
  
  // Read revenues
  for(int i = 0; i < n; ++i) {
    for(int j = 0; j < m; ++j) {
      std::cin >> revenues[i][j];
    }
  }
  
  // Read contour lines
  for(int i = 0; i < c; ++i) {
    int x, y, r; std::cin >> x >> y >> r;
    contour_lines.emplace_back(Point(x, y), std::pow(r, 2)); // ? Does r need to be squared ?
  }
  
  // ===== CONSTRUCT LINEAR PROGRAM =====
  // Variables are numbered by warehouse_idx * stadium_idx (n * m)
  Program lp(CGAL::SMALLER, true, 0, false, 0); 
  int n_constraints = 0;
  
  // Define Objective Function
  for(int w = 0; w < n; ++w) {
    for(int s = 0; s < m; ++s) {
      lp.set_c(w * n + s, -revenues[w][s]);  // TODO: Respect Contour Lines
    }
  }
  
  // Add constraints to ensure that the amount of Beer sent does not exceed supply
  for(int w = 0; w < n; ++w) {
    for(int s = 0; s < m; ++s) {
      if(DEBUG && PRINT_CONSTRAINTS) { std::cout << "a_" << w << "," << s << " * " << 1000 << " "; }
      lp.set_a(w * n + s, n_constraints, 1000);
    }
    if(DEBUG && PRINT_CONSTRAINTS) { std::cout << "<= " << warehouses[w].s << std::endl; }
    lp.set_b(n_constraints, warehouses[w].s);
    
    n_constraints += 1;
  }
  
  // Add constraints that the pure alocohol for each stadium does not exceed upper limit and
  // Add constraints to ensure the the Beer delivered is equal to the demand for each stadium
  if (DEBUG && PRINT_CONSTRAINTS) {
    for(int s = 0; s < m; ++s) {
      // Constraint to respect Alcohol limit
      for(int w = 0; w < n; ++w) {
        std::cout << "a_" << w << "," << s << " * " << warehouses[w].a << " ";
        lp.set_a(w * n + s, n_constraints, warehouses[w].a);
      }
      std::cout << "<= " << stadiums[s].u << std::endl;
      lp.set_b(n_constraints, stadiums[s].u);
      
      // Constraints to exactly meet Beer demand
      for(int w = 0; w < n; ++w) {
        std::cout << "a_" << w << "," << s << " * " << 1000 << " ";
        lp.set_a(w * n + s, n_constraints + 1, 1000);
      }
      std::cout << "<= " << stadiums[s].d << std::endl;
      lp.set_b(n_constraints + 1, stadiums[s].d);
      
      for(int w = 0; w < n; ++w) {        
        std::cout << "a_" << w << "," << s << " * " << -1000 << " ";
        lp.set_a(w * n + s, n_constraints + 2, -1000);
      }
      std::cout << "<= " << -stadiums[s].d << std::endl;
      lp.set_b(n_constraints + 2, -stadiums[s].d);
      
      n_constraints += 3;
    }
  } else {
    for(int s = 0; s < m; ++s) {
      for(int w = 0; w < n; ++w) {
        // Constraint to respect Alcohol limit
        lp.set_a(w * n + s, n_constraints, warehouses[w].a);
        
        // Constraints to exactly meet Beer demand
        lp.set_a(w * n + s, n_constraints + 1, 1000);
        lp.set_a(w * n + s, n_constraints + 2, -1000);
      }
      lp.set_b(n_constraints, stadiums[s].u);
      lp.set_b(n_constraints + 1, stadiums[s].d);
      lp.set_b(n_constraints + 2, -stadiums[s].d);
      
      n_constraints += 3;
    }
  }
  
  if (DEBUG) { std::cout << "Total number of constraints " << n_constraints << std::endl; }
  
  // ===== SOLVE LP AND OUTPUT =====
  Solution s = CGAL::solve_linear_program(lp, ET());
  if (DEBUG) { std::cout << s << std::endl; }
  
  if (s.is_infeasible()) { 
    std::cout << "no" << std::endl;
  } else {
    std::cout << floor_to_double(-s.objective_value()) << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}
```
</details>

<details>

<summary>Second Solution (Test Set 1, 2, 3)</summary>

This solution extends the first approach to handle contour lines and transportation costs. It maintains the same **Linear Programming** formulation but now incorporates the cost calculation that was missing in the first solution.

### Key Additions

**Transportation Cost Calculation:**
The solution correctly implements the logic for determining when a contour line is crossed. A delivery from warehouse $w$ to stadium $s$ crosses a contour line if one location is inside the circle and the other is outside. This is checked using CGAL's `has_on_bounded_side()` method.

**Modified Objective Function:**
The profit per liter for shipping from warehouse $w$ to stadium $s$ becomes:
$$\text{profit}_{w,s} = \text{revenue}_{w,s} - \frac{\text{intersections}_{w,s}}{100}$$

To maintain integer coefficients in the LP, the objective function is scaled by 100:
$$\text{coefficient}_{w,s} = 100 \times \text{revenue}_{w,s} - \text{intersections}_{w,s}$$

### Implementation Approach

The solution uses a **brute force method** to count contour line intersections:
```cpp
for (int w = 0; w < n; ++w) {
  for (int s = 0; s < m; ++s) {
    for (const auto& contour : contour_lines) {
      bool warehouse_inside = contour.has_on_bounded_side(warehouses[w].pos);
      bool stadium_inside = contour.has_on_bounded_side(stadiums[s].pos);
      if (warehouse_inside != stadium_inside) {
        n_intersections[w][s]++;
      }
    }
  }
}
```

This has a time complexity of $O(N \times M \times C)$, which works for Test Sets 1-3 where $C$ is relatively small, but becomes too slow for Test Set 4 where $C$ can be up to $10^6$.

### Limitations

While this solution correctly handles the transportation costs and passes more test sets than the first solution, it doesn't scale to the largest test cases due to the inefficient contour line processing. The brute force approach becomes a bottleneck when the number of contour lines is very large.

### Code
```cpp
#include <iostream>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Triangulation;

typedef K::Point_2 Point;
typedef K::Circle_2 Circle;

typedef int IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

#define ASSERT(c) do { if (!(c)) throw std::runtime_error("Assertion failed: " #c); } while (0)

struct Warehouse {
  Point pos;
  int s;  // Supply of Beer in ml
  int a;  // Alcohol per Liter of Beer in ml
  
  Warehouse(int x, int y, int s, int a) : pos(x, y), s(s * 1000), a(a * 10) {}
};

struct Stadium {
  Point pos;
  int d;  // Demand of Beer in ml
  int u;  // Upper Limit on the pure alcohol in ml
  
  Stadium(int x, int y, int d, int u) : pos(x, y), d(d * 1000), u(u * 1000) {}
};

long floor_to_double(const CGAL::Quotient<ET>& x) {
  double a = std::floor(CGAL::to_double(x));
  while (a > x) a -= 1;
  while (a+1 <= x) a += 1;
  return a;
}

void solve() {
  int n, m, c; std::cin >> n >> m >> c;
  
  std::vector<Warehouse> warehouses;
  std::vector<Stadium> stadiums;
  std::vector<Circle> contour_lines;
  std::vector<std::vector<int>> revenues(n, std::vector<int>(m));
  
  // Read warehouses
  for (int i = 0; i < n; ++i) {
    int x, y, s, a;
    std::cin >> x >> y >> s >> a;
    warehouses.emplace_back(x, y, s, a);
  }
  
  // Read stadiums
  for (int i = 0; i < m; ++i) {
    int x, y, d, u;
    std::cin >> x >> y >> d >> u;
    stadiums.emplace_back(x, y, d, u);
  }
  
  // Read revenues
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      std::cin >> revenues[i][j];
    }
  }
  
  // Read contour lines
  for (int i = 0; i < c; ++i) {
    int x, y, r;
    std::cin >> x >> y >> r;
    contour_lines.emplace_back(Point(x, y), r * r);
  }
  
  // ===== COUNT CONTOUR LINE INTERSECTIONS =====
  std::vector<std::vector<int>> n_intersections(n, std::vector<int>(m, 0));
  for (int w = 0; w < n; ++w) {
    for (int s = 0; s < m; ++s) {
      for (const auto& contour : contour_lines) {
        bool warehouse_inside = contour.has_on_bounded_side(warehouses[w].pos);
        bool stadium_inside = contour.has_on_bounded_side(stadiums[s].pos);
        if (warehouse_inside != stadium_inside) {
          n_intersections[w][s]++;
        }
      }
    }
  }
  
  // ===== CONSTRUCT LINEAR PROGRAM =====
  Program lp(CGAL::SMALLER, true, 0, false, 0); 
  int n_constraints = 0;
  
  // Objective Function
  for (int w = 0; w < n; ++w) {
    for (int s = 0; s < m; ++s) {
      lp.set_c(w * m + s, -(revenues[w][s] * 100 - n_intersections[w][s]));
    }
  }
  
  // Supply Constraints
  for (int w = 0; w < n; ++w) {
    for (int s = 0; s < m; ++s) {
      lp.set_a(w * m + s, n_constraints, 1000);
    }
    lp.set_b(n_constraints, warehouses[w].s);
    n_constraints++;
  }
  
  // Demand and Alcohol Constraints
  for (int s = 0; s < m; ++s) {
    for (int w = 0; w < n; ++w) {
      lp.set_a(w * m + s, n_constraints, warehouses[w].a); // Alcohol limit
      lp.set_a(w * m + s, n_constraints + 1, 1000);        // Demand equality
      lp.set_a(w * m + s, n_constraints + 2, -1000);       // Demand equality (negative)
    }
    lp.set_b(n_constraints, stadiums[s].u);   // Upper limit on alcohol
    lp.set_b(n_constraints + 1, stadiums[s].d); // Exact demand
    lp.set_b(n_constraints + 2, -stadiums[s].d); // Exact demand (negative)
    n_constraints += 3;
  }
  
  // Solve LP
  Solution solution = CGAL::solve_linear_program(lp, ET());
  std::cout << std::fixed << std::setprecision(0);
  
  if (solution.is_infeasible()) {
    std::cout << "no" << std::endl;
  } else {
    std::cout << floor_to_double(-solution.objective_value() / 100) << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  int t; std::cin >> t;
  while (t--) { solve(); }
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

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

#include <CGAL/Delaunay_triangulation_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;

typedef K::Point_2 Point;
typedef K::Segment_2 Segment;
typedef K::Circle_2 Circle;

typedef int IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

#define ASSERT(c) do { if (!(c)) throw std::runtime_error("Assertion failed: " #c); } while (0)

struct Warehouse {
  Point pos;
  int s;  // Supply of Beer in ml
  int a;  // Alcohol per Liter of Beer in ml
  
  Warehouse(int x, int y, int s, int a) : pos(x, y), s(s * 1000), a(a * 10) {}
};

struct Stadium {
  Point pos;
  int d;  // Demand of Beer in ml
  int u;  // Upper Limit on the pure alcohol in ml
  
  Stadium(int x, int y, int d, int u) : pos(x, y), d(d * 1000), u(u * 1000) {}
};

long floor_to_double(const CGAL::Quotient<ET>& x) {
  double a = std::floor(CGAL::to_double(x));
  while (a > x) a -= 1;
  while (a+1 <= x) a += 1;
  return a;
}

void solve() {
  // ===== READ INPUT =====
  int n, m, c; std::cin >> n >> m >> c;
  
  std::vector<Point> points; points.reserve(n + m);
  std::vector<Warehouse> warehouses; warehouses.reserve(n);
  std::vector<Stadium> stadiums; stadiums.reserve(m);
  std::vector<Circle> contour_lines; contour_lines.reserve(c); 
  std::vector<std::vector<int>> revenues(n, std::vector<int>(m)); 
  
  // Read warehouses
  for(int i = 0; i < n; ++i) {
    int x, y, s, a; std::cin >> x >> y >> s >> a;
    warehouses.emplace_back(x, y, s, a);
    points.emplace_back(x, y);
  }
  
  // Read stadiums
  for(int i = 0; i < m; ++i) {
    int x, y, d, u; std::cin >> x >> y >> d >> u;
    stadiums.emplace_back(x, y, d, u);
    points.emplace_back(x, y);
  }
  
  // Read revenues
  for(int i = 0; i < n; ++i) {
    for(int j = 0; j < m; ++j) {
      std::cin >> revenues[i][j];
    }
  }
  
  // Read contour lines
  for(int i = 0; i < c; ++i) {
    int x, y, r; std::cin >> x >> y >> r;
    contour_lines.emplace_back(Point(x, y), std::pow(r, 2));
  }
  
  // ===== COUNT CONTOUR LINE INTERSECTIONS =====
  // Triangulate warehouses and stadiums
  
  Triangulation t;
  t.insert(points.begin(), points.end());
  
  // Find the contour lines that are actually relevant (have something inside)
  std::vector<Circle*> relevant_contours; relevant_contours.reserve(100);
  for(int i = 0; i < c; ++i) {
    Point center = contour_lines[i].center();
    Point nearest = t.nearest_vertex(center)->point();
    
    if(CGAL::squared_distance(center, nearest) <= contour_lines[i].squared_radius()) {
      relevant_contours.push_back(&contour_lines[i]);
    }
  }
  
  // Count intersections between warehouses and stadiums with contour lines
  std::vector<std::vector<int>> n_intersections(n, std::vector<int>(m, 0));
  for(int w = 0; w < n; ++w) {
    for(int s = 0; s < m; ++s) {
      for(const Circle *contour : relevant_contours) {
        if(contour->has_on_bounded_side(warehouses[w].pos) != contour->has_on_bounded_side(stadiums[s].pos)) {
          n_intersections[w][s]++;
        }
      }
    }
  }
  
  // ===== CONSTRUCT LINEAR PROGRAM =====
  // Variables are numbered by warehouse_idx * n + stadium_idx (w * m + s)
  Program lp(CGAL::SMALLER, true, 0, false, 0); 
  int n_constraints = 0;
  
  // Define Objective Function
  for(int w = 0; w < n; ++w) {
    for(int s = 0; s < m; ++s) {
      lp.set_c(w * m + s, -(100 * revenues[w][s] - n_intersections[w][s]));
    }
  }
  
  // Add constraints to ensure that the amount of Beer sent does not exceed supply
  for(int w = 0; w < n; ++w) {
    for(int s = 0; s < m; ++s) {
      lp.set_a(w * m + s, n_constraints, 1000);
    }
    lp.set_b(n_constraints, warehouses[w].s);
    
    n_constraints += 1;
  }
  
  // Add constraints that the pure alocohol for each stadium does not exceed upper limit and
  // Add constraints to ensure the the Beer delivered is equal to the demand for each stadium
  for(int s = 0; s < m; ++s) {
    for(int w = 0; w < n; ++w) {
      // Constraint to respect Alcohol limit
      lp.set_a(w * m + s, n_constraints, warehouses[w].a);
      
      // Constraints to exactly meet Beer demand
      lp.set_a(w * m + s, n_constraints + 1, 1000);
      lp.set_a(w * m + s, n_constraints + 2, -1000);
    }
    lp.set_b(n_constraints, stadiums[s].u);
    lp.set_b(n_constraints + 1, stadiums[s].d);
    lp.set_b(n_constraints + 2, -stadiums[s].d);
    
    n_constraints += 3;
  }
  
  // ===== SOLVE LP AND OUTPUT =====
  Solution s = CGAL::solve_linear_program(lp, ET());
  
  std::cout << std::fixed << std::setprecision(0);

  if (s.is_infeasible()) { 
    std::cout << "no" << std::endl;
  } else {
    std::cout << floor_to_double(-s.objective_value() / 100) << std::endl;
  }
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
   Test set 1 (25 pts / 4 s) : Correct answer      (0.096s)
   Test set 2 (25 pts / 4 s) : Correct answer      (0.269s)
   Test set 3 (25 pts / 4 s) : Correct answer      (0.282s)
   Test set 4 (25 pts / 4 s) : Time limit exceeded

Total score: 75
```