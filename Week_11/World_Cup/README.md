# World Cup

## 📝 Problem Description

The goal is to maximize the total profit from selling beer during a sporting event. We are given a set of $N$ warehouses and $M$ stadiums, each with a specific location.

Each warehouse stores a single brand of beer, characterized by its available supply and alcohol percentage. Each stadium has a required demand for beer (in liters) that must be met exactly. Additionally, for each stadium, there is a maximum permissible limit on the total amount of pure alcohol that can be sold.

Beer can be transported from any warehouse to any stadium. For each warehouse-stadium pair, there is a base revenue per liter. This base revenue is reduced by a transportation cost which depends on crossing geographical contour lines, modeled as circles. The cost is a fixed amount per liter for each contour line crossed. A delivery from a warehouse to a stadium is assumed to cross a circular contour line if and only if one is inside the circle and the other is outside.

The task is to determine the amount of beer to ship from each warehouse to each stadium to maximize the total profit. The total profit is the sum of revenues minus transportation costs over all shipments. If it's impossible to satisfy all constraints (i.e., meet all demands while respecting supply and alcohol limits), that should report.

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

When reading through the problem it seems very similar to a **Max Flow Min Cost** problem. However, this is ultimately misleading, as ultimately this can be modeled (easier) using **Linear Programming**.

This solution focuses in the first 2 test sets. The main reasons why the initial solution is restricted to only these 2 is that the assumption $c=0$ is given. On the first two test sets we do not have any contour lines, which makes the problem easier to solve.

In the problem description, we have the following variables:

- $a_{ws}$ which describes the **liters of Beer transported** from $w$ to $s$
- $r_{ws}$ which describes the **revenue** of one liter of Beer transported from $w$ to $s$
- $s_w$ which describe the **supply of Beer** at Warehouse $w$
- $a_w$ which describes the **alcohol percentage** of the Beer at Warehouse $w$
- $d_s$ which describes the **demand of Beer** of the Stadium $s$
- $u_s$ which describes the **upper bound** of pure alcohol at Stadium $s$

With these variables in place, the problem requires **maximizing the revenue**, which is described by the following equation (assuming $c = 0$):

$$
\sum_w \sum_s a_{ws} \cdot r_{ws}
$$

As $r_{ws}$ is a constant value that is given in the input, naturally $a_{ws}$ **are the variables in the Linear Program**.
With this we are effectively optimizing from which warehouse we sent how much beer to which stadium, which is described by $a_{ws}$

The problem description further lists the following **constraints**:

- $a_{ws} \geq 0$, We can not send “negative” beer
- $\sum_s a_{ws} \leq s_w$, We can not send more beer from $w$ than there is present at $w$
- $\sum_w a_{ws} = d_s$, We need to exactly fulfill the demand of every stadium $s$
- $\sum_w a_{ws} \cdot a_s \leq u_s$, We need to respect the Upper Bound on the Pure Alcohol for Stadium $s$

Solving this linear program in theory already gives the **correct solution** for the first 2 Test Sets.

However, due to **precision errors**, the constraint $\sum_w a_{ws} \cdot a_s \leq u_s$ might be off by a tiny bit, resulting in wrong solutions. Therefore, it is necessary to **switch from Liters to Milliliters**. With this the alcohol percentage can be described by an integer which results in the correct result.

### Code
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

To generalize from the first solution, the **contour lines** need to be considered. As the contour lines only affect the revenues, this only **changes the objective function** of the Linear Program but not any constraints.

As described in the problem description, we simply need to adjust the **Revenues per Liter** by the number of **number of contour lines $t_{ws}$** that are hit when **traveling** from warehouse $w$ to stadium $s$

**Note**: To adjust for the fact that we use millilieter in the LP, we actually subtract $t_{ws} / 100$.

$$
\sum_w \sum_s a_{ws} \cdot ( 100 \cdot r_{ws} - t_{ws})
$$

Now it only remains to determine the number of contour lines $t_{ws}$ that are intersect from warehouse $w$ to stadium $s$.

For this we can simply check if both $s$ and $w$ are **on the same side** (inside or outside) of the Circle using `contour.has_on_bounded_side`.
If **both are inside** the contour does not need to be crossed (same with both outside)
If **one is inside and the other outside**, the contour does need to be crossed ($t_{ws} + 1$)

**Note**: Even though you probably assumed that there is a straight line between the warehouse and the stadium this was never specified in the problem. Be careful when reading the problem

### Brute Force

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

The main idea to generalize to the last test set is to filter the contour lines such that irrelevant contour lines are not considered for the "intersection test".

As we have $c\leq 10^6$ contour lines, but only $100$ of them actually contain any warehouse or stadium (Test Set 4 Assumption), we can filter out the contour lines which do not include any warehouse or stadium as otherwise the intersection test would be meaningless. If a contour line does not contain anything, nothing will intersect it.

To do this efficiently we will use a **Delaunay Triangulation** which triangulates the Warehouses and Stadiums. For every contour line we can then check **if its distance to the nearest point is smaller than its radius**.

- If not, there is no points inside → Irrelevant
- If there is, it is **relevant** and needs to be considered for the intersection count $t_{ws}$

With this filtering, we can afterwards check for every contour line $c$, warehouse $w$ and stadium $s$ if the intersect or not and adjust the objective function as described above.

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

## 🧠 Learnings

<details> 

<summary> Expand to View </summary>

- Switching units/rescaling your input might be necessaty to meet precision requirements
- Similar to how we work with squared distances, we should always avoid fractions in an LP by scaling the input values appropriately.

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (25 pts / 4 s) : Correct answer      (0.096s)
   Test set 2 (25 pts / 4 s) : Correct answer      (0.269s)
   Test set 3 (25 pts / 4 s) : Correct answer      (0.282s)
   Test set 4 (25 pts / 4 s) : Time limit exceeded

Total score: 75
```