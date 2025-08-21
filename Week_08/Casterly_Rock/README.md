# Casterly Rock

## 📝 Problem Description

Given a set of $n$ "noble" houses and $m$ "common" houses, each located at a specific $(x, y)$ coordinate in a 2D plane, the task is to determine the optimal placement of two canals: one for fresh water and one for sewage.

The placement is governed by the following set of rules and objectives:

1.  **Canal Geometry:** Both canals must be straight lines that intersect at a right angle.
2.  **Pipe Connections:** Each house must be connected to the fresh water canal by a **vertical** pipe and to the sewer canal by a **horizontal** pipe. This implies that the fresh water canal cannot be vertical, and the sewer canal cannot be horizontal.
3.  **Separation Constraint (Cersei's):** The sewer canal must act as a separator. All noble houses must lie on one side of the sewer canal (its "left") or on the canal itself. All common houses must lie on the other side (its "right") or on the canal itself.
4.  **Budget Constraint (Tywin's):** The total length of all horizontal sewage pipes must not exceed a given budget $s$. If $s$ is given as -1, the budget is considered infinite.
5.  **Optimization Goal (Jaime's):** Subject to all the above constraints, the primary goal is to **minimize the length of the longest vertical fresh water pipe**.

The required output depends on which constraints can be satisfied:
*   If the separation constraint cannot be met, the output is "Y".
*   If the separation constraint is met but the budget constraint is not, the output is "B".
*   If all constraints can be met, the output is the minimum possible length of the longest fresh water pipe, rounded up to the nearest integer.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem involves finding optimal lines based on a series of conditions. Many of these conditions, such as a point being on one side of a line or the distance from a point to a line, can be expressed as linear inequalities. This structure strongly suggests that the problem can be modeled using a particular mathematical optimization framework.
</details>
<details>
<summary>Hint #2</summary>
This problem is a perfect candidate for **Linear Programming (LP)**. Think about how to represent the unknown canals (lines) using variables. A line in the 2D plane can be described by an equation like $ax + by + c = 0$. The variables of your LP would be the coefficients $a, b, c$. How can you use this form to express the problem's constraints?
</details>
<details>
<summary>Hint #3</summary>
The problem has a clear hierarchy of objectives: first, check Cersei's constraint for feasibility. If that works, check Tywin's. If that also works, finally optimize for Jaime's goal. This suggests an incremental approach. You can build a single LP model and add constraints stage by stage.

A key challenge is the right-angle constraint between the two canals. If the sewer canal is $ax+by+c=0$ and the water canal is $a_2x+b_2y+c_2=0$, their normals must be orthogonal. This gives a non-linear constraint $aa_2 + bb_2 = 0$. Consider using the pipe definitions (vertical/horizontal) and normalization to simplify this into a linear relationship between the coefficients.
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1)</summary>

For the first test set, we only need to determine if Cersei's constraint can be satisfied. The budget $s$ is effectively infinite, and we don't need to optimize for pipe lengths. This simplifies the problem to a question of **linear separability**: can we find a line that separates the noble houses from the common houses, with the noble houses on the "left"?

This is a classic feasibility problem that can be solved with Linear Programming.

### LP Formulation for Cersei's Constraint

Let the sewer canal be represented by the line $ax + by + c = 0$. A point $(x_i, y_i)$ is on one side of the line if $ax_i + by_i + c > 0$ and on the other if $ax_i + by_i + c < 0$. We can enforce a strict separation by requiring:
*   $ax_n + by_n + c \ge 1$ for all noble houses $(x_n, y_n)$.
*   $ax_c + by_c + c \le -1$ for all common houses $(x_c, y_c)$.

The vector $(a, b)$ is normal to the line and points towards the half-plane where $ax+by+c > 0$. According to our formulation, this is the side where the noble houses are. The problem states noble houses must be on the **left** (west). This means the normal vector $(a,b)$ should have a non-positive x-component, i.e., it should point west. We can enforce this with the linear constraint:
*   $a \le 0$

We can now define an LP with variables $a, b, c$ and the constraints above. We are only interested in whether a feasible solution exists. If the LP is feasible, a separating line exists; otherwise, it does not.

*   If the LP is **infeasible**, it's impossible to satisfy Cersei's constraint. We output "Y".
*   If the LP is **feasible**, such a line exists. For this subtask, this means the answer is "B".

**A Note on Implementation:** When using libraries like CGAL to solve LPs, we might encounter cycling issues in the simplex algorithm on certain inputs. To prevent this, it's good practice to enable a cycling-prevention rule, such as **Bland's rule**.

```cpp
/// Solution for Test Set 1
#include <iostream>
#include <vector>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>

// Type definitions for CGAL
typedef int IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

// Variable indices for the sewer canal equation: ax + by + c = 0
const int a = 0;
const int b = 1;
const int c = 2;

void solve() {
  // Read input
  int n, m;
  long s;
  std::cin >> n >> m >> s;

  std::vector<std::pair<int, int>> noble_houses(n);
  for (int i = 0; i < n; ++i) {
    std::cin >> noble_houses[i].first >> noble_houses[i].second;
  }

  std::vector<std::pair<int, int>> common_houses(m);
  for (int i = 0; i < m; ++i) {
    std::cin >> common_houses[i].first >> common_houses[i].second;
  }

  // --- LP for Cersei's Constraint ---
  // We want to find if a line ax + by + c = 0 exists that separates the houses.
  Program lp(CGAL::SMALLER, false, 0, false, 0);

  // Noble houses: require ax_n + by_n + c >= 1
  // This is equivalent to -ax_n - by_n - c <= -1
  for (int i = 0; i < n; ++i) {
    lp.set_a(a, i, -noble_houses[i].first);
    lp.set_a(b, i, -noble_houses[i].second);
    lp.set_a(c, i, -1);
    lp.set_b(i, -1);
  }

  // Common houses: require ax_c + by_c + c <= -1
  for (int i = 0; i < m; ++i) {
    lp.set_a(a, n + i, common_houses[i].first);
    lp.set_a(b, n + i, common_houses[i].second);
    lp.set_a(c, n + i, 1);
    lp.set_b(n + i, -1);
  }

  // Orientation: noble houses on the left (west).
  // The normal vector (a, b) points towards the noble houses' side.
  // We constrain its x-component 'a' to be non-positive.
  lp.set_u(a, true, 0); // a <= 0

  // We don't need an objective function; we only care about feasibility.
  CGAL::Quadratic_program_options options;
  options.set_pricing_strategy(CGAL::QP_BLAND); // Use Bland's rule to prevent cycling
  Solution sol = CGAL::solve_linear_program(lp, ET(), options);

  if (sol.is_infeasible()) {
    std::cout << "Y\n";
  } else {
    std::cout << "B\n";
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
<summary>Second Solution (Test Set 2)</summary>

For the second test set, there are only noble houses ($m=0$) and no budget constraint ($s=-1$). This means Cersei's and Tywin's constraints are trivially satisfied. The problem reduces to fulfilling Jaime's wish: **minimize the length of the longest fresh water pipe**.

The fresh water pipes are vertical. This means we are looking for a non-vertical water canal (a line) that minimizes its maximum vertical distance to any of the houses.

### LP Formulation for Jaime's Constraint

Let the water canal be represented by the line $y = m_w x + c_w$. The vertical distance from a house $(x_i, y_i)$ to this line is $|y_i - (m_w x_i + c_w)|$. Our goal is to find $m_w$ and $c_w$ that solve:
$$ \min_{m_w, c_w} \left( \max_i |y_i - (m_w x_i + c_w)| \right) $$

This is a **minimax** problem, which can be converted into a standard LP. We introduce an auxiliary variable $d$ that represents the maximum vertical distance. The objective is to minimize $d$. The constraints are that for every house $i$, its distance to the line must be at most $d$:
$$ |y_i - (m_w x_i + c_w)| \le d \quad \text{for all } i $$

This single inequality with an absolute value can be rewritten as two linear inequalities:
1.  $y_i - (m_w x_i + c_w) \le d \quad \implies \quad -m_w x_i - c_w - d \le -y_i$
2.  $-(y_i - (m_w x_i + c_w)) \le d \quad \implies \quad m_w x_i + c_w - d \le y_i$

The final LP has variables $m_w, c_w, d$. The objective is to **minimize $d$**, subject to the two inequalities above for each house and the non-negativity constraint $d \ge 0$. The optimal objective value will be the minimum possible longest pipe length. We round this value up to the next integer as required.

```cpp
/// Solution for Test Set 2
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>

// Type definitions
typedef int IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

// Variable indices for the water canal line y = mx + c and max distance d
const int m_w = 0;
const int c_w = 1;
const int d = 2;

void solve() {
  // Read input
  int n, m;
  long s;
  std::cin >> n >> m >> s;
  
  std::vector<std::pair<int, int>> houses(n + m);
  for (int i = 0; i < n + m; ++i) {
    std::cin >> houses[i].first >> houses[i].second;
  }
  
  // --- LP for Jaime's Constraint ---
  // Minimize the maximum vertical distance 'd'
  Program lp(CGAL::SMALLER, false, 0, false, 0);
  
  // For each house (x_i, y_i), add two constraints:
  // m*x_i + c - d <= y_i
  // -m*x_i - c - d <= -y_i
  for (int i = 0; i < n + m; ++i) {
    const int x_i = houses[i].first;
    const int y_i = houses[i].second;

    // Constraint 1: m_w*x_i + c_w - d <= y_i
    lp.set_a(m_w, 2 * i, x_i);
    lp.set_a(c_w, 2 * i, 1);
    lp.set_a(d,   2 * i, -1);
    lp.set_b(2 * i, y_i);
    
    // Constraint 2: -m_w*x_i - c_w - d <= -y_i
    lp.set_a(m_w, 2 * i + 1, -x_i);
    lp.set_a(c_w, 2 * i + 1, -1);
    lp.set_a(d,   2 * i + 1, -1);
    lp.set_b(2 * i + 1, -y_i);
  }
  
  // We must have a non-negative distance
  lp.set_l(d, true, 0);
  
  // Objective function: minimize d
  lp.set_c(d, 1);
  
  Solution sol = CGAL::solve_linear_program(lp, ET());
  
  double result = CGAL::to_double(sol.objective_value());
  std::cout << (long)std::ceil(result) << "\n";
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
<summary>Third Solution (Test Set 1, 2, 3)</summary>

This solution addresses the first three test sets. The key assumption for test set 3 is that the right-angle constraint can be ignored, meaning we can find the optimal sewer and water canals independently. This allows us to combine the logic from the previous two solutions in a sequential manner.

The overall strategy is:
1.  **Check Cersei:** Solve an LP to see if a separating sewer canal exists. If not, output "Y".
2.  **Check Tywin:** Add the budget constraint to the LP from step 1. If it becomes infeasible, output "B".
3.  **Optimize Jaime:** If the first two checks pass, solve a separate LP (as in Solution 2) to find the minimum longest water pipe.

### LP Formulation

**Step 1 & 2: Sewer Canal (Cersei & Tywin)**

Let the sewer canal be $ax + by + c = 0$. Since sewage pipes are horizontal, the canal must be non-horizontal, so $a \neq 0$. We can **normalize** the equation by setting $a=1$. The line is now $x + by + c = 0$.

*   **Cersei's Constraint:** The normal vector is $(1, b)$, which always points into the right half-plane. To place noble houses on the left and common houses on the right, we need:
    *   $x_n + by_n + c \le 0$ for all noble houses $(x_n, y_n)$.
    *   $x_c + by_c + c \ge 0$ for all common houses $(x_c, y_c)$.
*   **Tywin's Constraint:** The sum of horizontal pipe lengths must be at most $s$. The horizontal distance from a house $(x_i, y_i)$ to the line $x=-by-c$ is $|x_i - (-by_i-c)| = |x_i + by_i + c|$. Since we know the sign of this expression for each house type from Cersei's constraints:
    *   For nobles: $|x_n + by_n + c| = -(x_n + by_n + c)$
    *   For commons: $|x_c + by_c + c| = x_c + by_c + c$
    
    The total length is $\sum_{\text{commons}}(x_c+by_c+c) - \sum_{\text{nobles}}(x_n+by_n+c)$. The constraint is:
    $$ \left(\sum x_c - \sum x_n\right) + b\left(\sum y_c - \sum y_n\right) + c(m-n) \le s $$
    This is a single linear inequality involving variables $b$ and $c$.

We first solve an LP with just Cersei's constraints. If it's infeasible, we print "Y". Otherwise, we add Tywin's constraint (if $s \neq -1$) and solve again. If this is now infeasible, we print "B".

**Step 3: Water Canal (Jaime)**

If both checks pass, we know a valid configuration is possible. We then solve a completely separate LP for the water canal, exactly as described in the "Second Solution", to minimize the maximum vertical pipe length.

```cpp
/// Solution for Test Sets 1, 2, 3
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>

// Type definitions
typedef long IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

// Var indices for sewer canal: x + by + c = 0
const int b_s = 0;
const int c_s = 1;

// Var indices for water canal: y = m_w*x + c_w
const int m_w = 0;
const int c_w = 1;
const int d = 2;

struct Point { long x, y; };

void solve() {
  int n, m;
  long s;
  std::cin >> n >> m >> s;

  std::vector<Point> nobles(n), commons(m);
  long noble_x_sum = 0, noble_y_sum = 0;
  for (int i = 0; i < n; ++i) {
    std::cin >> nobles[i].x >> nobles[i].y;
    noble_x_sum += nobles[i].x;
    noble_y_sum += nobles[i].y;
  }
  long common_x_sum = 0, common_y_sum = 0;
  for (int i = 0; i < m; ++i) {
    std::cin >> commons[i].x >> commons[i].y;
    common_x_sum += commons[i].x;
    common_y_sum += commons[i].y;
  }

  // --- LP for Sewer Canal (Cersei & Tywin) ---
  Program lp_sewer(CGAL::SMALLER, false, 0, false, 0);

  // Cersei's constraints
  // Nobles: x_n + b_s*y_n + c_s <= 0
  for (int i = 0; i < n; ++i) {
    lp_sewer.set_a(b_s, i, nobles[i].y);
    lp_sewer.set_a(c_s, i, 1);
    lp_sewer.set_b(i, -nobles[i].x);
  }
  // Commons: x_c + b_s*y_c + c_s >= 0  => -x_c - b_s*y_c - c_s <= 0
  for (int i = 0; i < m; ++i) {
    lp_sewer.set_a(b_s, n + i, -commons[i].y);
    lp_sewer.set_a(c_s, n + i, -1);
    lp_sewer.set_b(n + i, commons[i].x);
  }

  Solution sol_sewer = CGAL::solve_linear_program(lp_sewer, ET());
  if (sol_sewer.is_infeasible()) {
    std::cout << "Y\n";
    return;
  }

  // Tywin's constraint
  if (s != -1) {
    long y_diff = common_y_sum - noble_y_sum;
    long x_diff = common_x_sum - noble_x_sum;
    lp_sewer.set_a(b_s, n + m, y_diff);
    lp_sewer.set_a(c_s, n + m, m - n);
    lp_sewer.set_b(n + m, s - x_diff);

    sol_sewer = CGAL::solve_linear_program(lp_sewer, ET());
    if (sol_sewer.is_infeasible()) {
      std::cout << "B\n";
      return;
    }
  }

  // --- LP for Water Canal (Jaime) ---
  Program lp_water(CGAL::SMALLER, true, 0, false, 0);

  // Constraints for nobles
  for (int i = 0; i < n; ++i) {
    lp_water.set_a(m_w, 2 * i, nobles[i].x);
    lp_water.set_a(c_w, 2 * i, 1);
    lp_water.set_a(d, 2 * i, -1);
    lp_water.set_b(2 * i, nobles[i].y);

    lp_water.set_a(m_w, 2 * i + 1, -nobles[i].x);
    lp_water.set_a(c_w, 2 * i + 1, -1);
    lp_water.set_a(d, 2 * i + 1, -1);
    lp_water.set_b(2 * i + 1, -nobles[i].y);
  }
  // Constraints for commons
  for (int i = 0; i < m; ++i) {
    lp_water.set_a(m_w, 2 * n + 2 * i, commons[i].x);
    lp_water.set_a(c_w, 2 * n + 2 * i, 1);
    lp_water.set_a(d, 2 * n + 2 * i, -1);
    lp_water.set_b(2 * n + 2 * i, commons[i].y);

    lp_water.set_a(m_w, 2 * n + 2 * i + 1, -commons[i].x);
    lp_water.set_a(c_w, 2 * n + 2 * i + 1, -1);
    lp_water.set_a(d, 2 * n + 2 * i + 1, -1);
    lp_water.set_b(2 * n + 2 * i + 1, -commons[i].y);
  }

  lp_water.set_c(d, 1); // Minimize d

  Solution sol_water = CGAL::solve_linear_program(lp_water, ET());
  double result = CGAL::to_double(sol_water.objective_value());
  std::cout << (long)std::ceil(result) << "\n";
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

The final solution must handle all constraints simultaneously, including the crucial **right-angle constraint**. Instead of solving separate LPs, we can build a single, comprehensive LP model that incorporates all requirements. The logic follows the same incremental checking process as the previous solution.

### Unified LP Formulation

Let the sewer canal be $L_S: ax+by+c=0$ and the water canal be $L_W: a_2x+b_2y+c_2=0$.

1.  **Normalization:** To make the problem linear and manageable, we apply normalization.
    *   Sewer pipes are horizontal, so $L_S$ is non-horizontal ($a \neq 0$). We can set **$a=1$**.
    *   Water pipes are vertical, so $L_W$ is non-vertical ($b_2 \neq 0$). We can set **$b_2=1$**.

2.  **Right-Angle Constraint:** The normal vectors $(a, b)$ and $(a_2, b_2)$ must be perpendicular. Their dot product is zero: $a a_2 + b b_2 = 0$. With our normalization ($a=1, b_2=1$), this simplifies to a linear constraint: $1 \cdot a_2 + b \cdot 1 = 0 \implies \mathbf{a_2 = -b}$.

This is a powerful simplification. The slope of the water canal is determined by the slope of the sewer canal. Our LP variables are now $b, c, c_2,$ and an auxiliary variable $d$ for the optimization.

The unified LP constraints are:

*   **Cersei (Sewer Canal $x+by+c=0$):**
    *   $x_n + by_n + c \le 0$ for all noble houses.
    *   $x_c + by_c + c \ge 0$ for all common houses.

*   **Tywin (Sewer Canal $x+by+c=0$):**
    *   $\sum_{\text{all houses}} |x_i + by_i + c| \le s$. This is linearized as before:
        $\left(\sum x_c - \sum x_n\right) + b\left(\sum y_c - \sum y_n\right) + c(m-n) \le s$.

*   **Jaime (Water Canal $-bx+y+c_2=0$):**
    *   We want to minimize $d = \max_i(\text{vertical distance to } L_W)$.
    *   The vertical distance from $(x_i, y_i)$ to $y = bx-c_2$ is $|y_i - (bx_i - c_2)| = |y_i - bx_i + c_2|$.
    *   This gives two constraints per house:
        1.  $y_i - bx_i + c_2 \le d \implies -bx_i + c_2 - d \le -y_i$
        2.  $-(y_i - bx_i + c_2) \le d \implies bx_i - c_2 - d \le y_i$

### Incremental Solving Strategy

We construct a single LP object and add constraints in stages:
1.  Add Cersei's constraints (with $a=1$). Check feasibility. If infeasible, output "Y".
2.  Add Tywin's constraint. Check feasibility. If infeasible, output "B".
3.  Add Jaime's constraints and the right-angle relation ($a_2 = -b$ with $b_2=1$). Set the objective to **minimize $d$**. Solve the full LP and output the result.

This approach correctly solves the full problem by finding parameters for both canals that satisfy all conditions simultaneously.

```cpp
/// Final Solution
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>

// Type definitions
typedef long IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

// LP Variables:
// Sewer: ax + by + c = 0. We fix a=1.
const int b = 0; // LP var for b
const int c = 1; // LP var for c

// Water: a2x + b2y + c2 = 0. We fix b2=1. Right-angle => a2 = -b.
const int c2 = 2; // LP var for c2
const int d = 3;  // LP var for max distance

struct Point { long x, y; };

void solve() {
  int n, m;
  long s;
  std::cin >> n >> m >> s;

  std::vector<Point> nobles(n), commons(m);
  long noble_x_sum = 0, noble_y_sum = 0;
  for (int i = 0; i < n; ++i) {
    std::cin >> nobles[i].x >> nobles[i].y;
    noble_x_sum += nobles[i].x;
    noble_y_sum += nobles[i].y;
  }
  long common_x_sum = 0, common_y_sum = 0;
  for (int i = 0; i < m; ++i) {
    std::cin >> commons[i].x >> commons[i].y;
    common_x_sum += commons[i].x;
    common_y_sum += commons[i].y;
  }

  // --- Unified LP Model ---
  Program lp(CGAL::SMALLER, false, 0, false, 0);
  int constraint_idx = 0;

  // == Stage 1: Check Cersei ==
  // Nobles: x_n + b*y_n + c <= 0
  for (int i = 0; i < n; ++i) {
    lp.set_a(b, constraint_idx, nobles[i].y);
    lp.set_a(c, constraint_idx, 1);
    lp.set_b(constraint_idx, -nobles[i].x);
    constraint_idx++;
  }
  // Commons: x_c + b*y_c + c >= 0  => -x_c - b*y_c - c <= 0
  for (int i = 0; i < m; ++i) {
    lp.set_a(b, constraint_idx, -commons[i].y);
    lp.set_a(c, constraint_idx, -1);
    lp.set_b(constraint_idx, commons[i].x);
    constraint_idx++;
  }

  Solution sol = CGAL::solve_linear_program(lp, ET());
  if (sol.is_infeasible()) {
    std::cout << "Y\n";
    return;
  }

  // == Stage 2: Check Tywin ==
  if (s != -1) {
    long y_diff = common_y_sum - noble_y_sum;
    long x_diff = common_x_sum - noble_x_sum;
    lp.set_a(b, constraint_idx, y_diff);
    lp.set_a(c, constraint_idx, m - n);
    lp.set_b(constraint_idx, s - x_diff);
    constraint_idx++;

    sol = CGAL::solve_linear_program(lp, ET());
    if (sol.is_infeasible()) {
      std::cout << "B\n";
      return;
    }
  }

  // == Stage 3: Optimize Jaime ==
  // Water canal: -bx + y + c2 = 0. Distance is |y_i - bx_i + c2|.
  // We minimize d subject to |y_i - bx_i + c2| <= d for all i.
  
  // Constraints for nobles
  for (int i = 0; i < n; ++i) {
    // y_i - b*x_i + c2 <= d  =>  -b*x_i + c2 - d <= -y_i
    lp.set_a(b, constraint_idx, -nobles[i].x);
    lp.set_a(c2, constraint_idx, 1);
    lp.set_a(d, constraint_idx, -1);
    lp.set_b(constraint_idx, -nobles[i].y);
    constraint_idx++;

    // -(y_i - b*x_i + c2) <= d => b*x_i - c2 - d <= y_i
    lp.set_a(b, constraint_idx, nobles[i].x);
    lp.set_a(c2, constraint_idx, -1);
    lp.set_a(d, constraint_idx, -1);
    lp.set_b(constraint_idx, nobles[i].y);
    constraint_idx++;
  }
  // Constraints for commons
  for (int i = 0; i < m; ++i) {
    lp.set_a(b, constraint_idx, -commons[i].x);
    lp.set_a(c2, constraint_idx, 1);
    lp.set_a(d, constraint_idx, -1);
    lp.set_b(constraint_idx, -commons[i].y);
    constraint_idx++;

    lp.set_a(b, constraint_idx, commons[i].x);
    lp.set_a(c2, constraint_idx, -1);
    lp.set_a(d, constraint_idx, -1);
    lp.set_b(constraint_idx, commons[i].y);
    constraint_idx++;
  }

  // Objective: minimize d
  lp.set_c(d, 1);
  lp.set_l(d, true, 0); // d >= 0

  sol = CGAL::solve_linear_program(lp, ET());
  double result = CGAL::to_double(sol.objective_value());
  std::cout << (long)std::ceil(result) << "\n";
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