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
#include<iostream>
#include<vector>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>

typedef int IT;
typedef CGAL::Gmpz ET;

typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

void solve() {
  // ===== READ INPUT =====
  int n, m; long s; std::cin >> n >> m >> s;
  
  std::vector<std::vector<int>> noble_houses(n, std::vector<int>(2));
  for(int i = 0; i < n; ++i) {
    std::cin >> noble_houses[i][0] >> noble_houses[i][1];
  }
  
  std::vector<std::vector<int>> common_houses(m, std::vector<int>(2));
  for(int i = 0; i < m; ++i) {
    std::cin >> common_houses[i][0] >> common_houses[i][1];
  }
  
  // ===== CONSTRUCT FIRST LINEAR PROGRAM (CERSEI) =====
  // Check if noble and common houses are linearly sepearble such that the noble houses are on the left of the line
  // Variables: Parameters of the normal form of the sewer canal
  const int a = 0;
  const int b = 1;
  const int c = 2;
  
  Program lp(CGAL::SMALLER, false, 0, false, 0);
  
  // Add noble houses constraints
  for(int i = 0; i < n; ++i) {
    lp.set_a(a, i, -noble_houses[i][0]); 
    lp.set_a(b, i,- noble_houses[i][1]);
    lp.set_a(c, i, -1);
    lp.set_b(i, -1);
  }
  
  // Add common houses constraints
  for(int i = 0; i < m; ++i) {
    lp.set_a(a, n+i, common_houses[i][0]); 
    lp.set_a(b, n+i, common_houses[i][1]);
    lp.set_a(c, n+i, 1);
    lp.set_b(n+i, -1);
  }
  
  // Add constraint, that the normal has to point to the left, to ensure that the noble houses are on the left
  lp.set_u(a, true, 0);
  // No objective function needed, as we only want to check for feasibility/separability
  
  // ===== SOLVE FIRST LINEAR PROGRAM =====
  CGAL::Quadratic_program_options options;
  options.set_pricing_strategy(CGAL::QP_BLAND);
  Solution solution = CGAL::solve_linear_program(lp, ET(), options);
  if(solution.is_infeasible()) {
    std::cout << "Y" << std::endl;
  } else {
    std::cout << "B" << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
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
#include<iostream>
#include<vector>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>

typedef int IT;
typedef CGAL::Gmpz ET;

typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;
typedef Solution::Variable_value_iterator SVI;

// Variables to easier reference the parameters of the line equation (and more)
const int a = 0;
const int b = 1;
const int c = 2;

const int m = 0;
// const int b = 1; // Already defined above
const int d = 2;

void solve() {
  // ===== READ INPUT =====
  int n, m; long s; std::cin >> n >> m >> s;
  
  std::vector<std::vector<int>> noble_houses(n, std::vector<int>(2));
  for(int i = 0; i < n; ++i) {
    std::cin >> noble_houses[i][0] >> noble_houses[i][1];
  }
  
  std::vector<std::vector<int>> common_houses(m, std::vector<int>(2));
  for(int i = 0; i < m; ++i) {
    std::cin >> common_houses[i][0] >> common_houses[i][1];
  }
  
  // ===== THIRD LINEAR PROGRAM (JAIME) =====
  // Minimize the (vertical) distance between the all the houses and the water canal
  Program lp(CGAL::SMALLER, false, 0, false, 0);
  
  // Add noble houses constraints
  for(int i = 0; i < n; ++i) {
    lp.set_a(m, i, -noble_houses[i][0]);
    lp.set_a(b, i, -1);
    lp.set_a(d, i, -1);
    lp.set_b(i, -noble_houses[i][1]);
    
    lp.set_a(m, i + n, noble_houses[i][0]);
    lp.set_a(b, i + n, 1);
    lp.set_a(d, i + n, -1);
    lp.set_b(i + n, noble_houses[i][1]);
  }
  
  // Add common houses constraints
  for(int i = 0; i < m; ++i) {
    lp.set_a(m, 2*n + i, -common_houses[i][0]);
    lp.set_a(b, 2*n + i, -1);
    lp.set_a(d, 2*n + i, -1);
    lp.set_b(2*n + i, -common_houses[i][1]);
    
    lp.set_a(m, 2*n + i + m, common_houses[i][0]);
    lp.set_a(b, 2*n + i + m, 1);
    lp.set_a(d, 2*n + i + m, -1);
    lp.set_b(2*n + i + m, common_houses[i][1]);
  }
  
  lp.set_l(d, true, 0);
  
  // Minimize the maximum distance d
  lp.set_c(d, 1);
  
    // ===== SOLVE FIRST LINEAR PROGRAM =====
    Solution solution = CGAL::solve_linear_program(lp, ET());
    if(solution.is_infeasible() || solution.is_unbounded()) {
      std::cout << "error" << std::endl;
    } else {
      std::cout << (long) std::ceil(CGAL::to_double(solution.objective_value())) << std::endl;
    }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
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
#include<iostream>
#include<vector>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>

typedef long IT;
typedef CGAL::Gmpz ET;

typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;
typedef Solution::Variable_value_iterator SVI;

// https://github.com/haeggee/algolab/blob/main/problems/week06-lannister/src/algorithm.cpp

// Variables to easier reference the parameters of the line equation (and more)
const int a = 0;
const int b = 1;
const int c = 2;

const int a2 = 3;
const int b2 = 4;
const int c2 = 5;
const int d  = 6;

void solve() {
  // ===== READ INPUT =====
  int n, m; long s; std::cin >> n >> m >> s;
  
  long sum_x_nobles, sum_y_nobles, sum_x_commons, sum_y_commons;
  sum_x_nobles = sum_y_nobles = sum_x_commons = sum_y_commons = 0;
  
  std::vector<std::vector<int>> noble_houses(n, std::vector<int>(2));
  for(int i = 0; i < n; ++i) {
    std::cin >> noble_houses[i][0] >> noble_houses[i][1];
    sum_x_nobles += noble_houses[i][0];
    sum_y_nobles += noble_houses[i][1];
  }
  
  std::vector<std::vector<int>> common_houses(m, std::vector<int>(2));
  for(int i = 0; i < m; ++i) {
    std::cin >> common_houses[i][0] >> common_houses[i][1];
    sum_x_commons += common_houses[i][0];
    sum_y_commons += common_houses[i][1];
  }
  
  Program lp(CGAL::SMALLER, false, 0, false, 0);
  
  // ===== FIRST LINEAR PROGRAM (CERSEI) =====
  if(true) {  // Always check Cerseis constraint
    // Check if noble and common houses are linearly sepearble such that the noble houses are on the left of the line
    
    // Add noble houses constraints
    for(int i = 0; i < n; ++i) {
      lp.set_a(a, i, noble_houses[i][0]); 
      lp.set_a(b, i, noble_houses[i][1]);
      lp.set_a(c, i, 1);
    }
    
    // Add common houses constraints
    for(int i = 0; i < m; ++i) {
      lp.set_a(a, n+i, -common_houses[i][0]); 
      lp.set_a(b, n+i, -common_houses[i][1]);
      lp.set_a(c, n+i, -1);
    }
    
    // Add constraint, that the normal has to point to the left, to ensure that the noble houses are on the left
    lp.set_l(a, true, 1);
    lp.set_u(a, true, 1);
    // No objective function needed, as we only want to check for feasibility/separability
    
    // ===== SOLVE =====
    CGAL::Quadratic_program_options options;
    options.set_pricing_strategy(CGAL::QP_BLAND);
    Solution solution = CGAL::solve_linear_program(lp, ET(), options);
    if(solution.is_infeasible()) {
      std::cout << "Y" << std::endl;
      return;
    }
  }
  
  // ===== SECOND LINEAR PROGRAM (TYWIN) =====  
  if(s != -1) {  // Only check for Tywins constraint if it is actually present (s != -1)
    lp.set_a(b, m + n, sum_y_commons - sum_y_nobles);
    lp.set_a(c, m + n, m - n);
    lp.set_b(m + n, s - sum_x_commons + sum_x_nobles);

    // ===== SOLVE =====
    Solution solution = CGAL::solve_linear_program(lp, ET());
    if(solution.is_infeasible()) {
      std::cout << "B" << std::endl;
      return;
    }
  }
  
  int n_rows = n + m + 1;
  
  // ===== THIRD LINEAR PROGRAM (JAIME) =====
  if(true) {  // Always optimize for Jamies requirement
    // Minimize the (vertical) distance between the all the houses and the water canal
    
    // Add noble houses constraints
    for(int i = 0; i < n; ++i) {
      lp.set_a(a2, n_rows + i, noble_houses[i][0]);
      lp.set_a(c2, n_rows + i, 1);
      lp.set_a(d, n_rows + i, -1);
      lp.set_b(n_rows + i, -noble_houses[i][1]);
      
      lp.set_a(a2, n_rows + i + n, -noble_houses[i][0]);
      lp.set_a(c2, n_rows + i + n, -1);
      lp.set_a(d, n_rows + i + n, -1);
      lp.set_b(n_rows + i + n, noble_houses[i][1]);
    }
    
    // Add common houses constraints
    for(int i = 0; i < m; ++i) {
      lp.set_a(a2, n_rows + 2*n + i, common_houses[i][0]);
      lp.set_a(c2, n_rows + 2*n + i, 1);
      lp.set_a(d, n_rows + 2*n + i, -1);
      lp.set_b(n_rows + 2*n + i, -common_houses[i][1]);
      
      lp.set_a(a2, n_rows + 2*n + i + m, -common_houses[i][0]);
      lp.set_a(c2, n_rows + 2*n + i + m, -1);
      lp.set_a(d, n_rows + 2*n + i + m, -1);
      lp.set_b(n_rows + 2*n + i + m, common_houses[i][1]);
    }
    
    n_rows += 2 * n + 2 * m;
    
    // Ensure pipes are orthogonal
    lp.set_a(b, n_rows + 1, 1);
    lp.set_a(a2, n_rows + 1, 1); // b1 + a2 <= 0
    lp.set_a(b, n_rows + 2, -1);
    lp.set_a(a2, n_rows + 2, -1); // -b1 - a2 <= 0
    
    // Fix b2 to 1
    lp.set_l(b2, true, 1);
    lp.set_u(b2, true, 1);
    
    lp.set_l(d, true, 0);
    
    // Minimize the maximum distance d
    lp.set_c(d, 1);
    
    // ===== SOLVE =====
    Solution solution = CGAL::solve_linear_program(lp, ET());
    if(solution.is_infeasible() || solution.is_unbounded()) {
      std::cout << "error" << std::endl;
    } else {
      std::cout << (long) std::ceil(CGAL::to_double(solution.objective_value())) << std::endl;
    }
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (30 pts / 2 s) : Correct answer      (0.07s)
   Test set 2 (30 pts / 2 s) : Correct answer      (0.296s)
   Test set 3 (20 pts / 2 s) : Correct answer      (0.212s)
   Test set 4 (20 pts / 2 s) : Correct answer      (0.692s)

Total score: 80
```