# Casterly Rock

## 📝 Problem Description

Given a set of $n$ "noble" houses and $m$ "common" houses, each located at a specific $(x, y)$ coordinate in a 2D plane, the task is to determine the optimal placement of two canals: one for fresh water and one for sewage.

The placement is governed by the following set of rules and objectives:

1.  **Canal Geometry:** Both canals must be straight lines that intersect at a right angle.
2.  **Pipe Connections:** Each house must be connected to the fresh water canal by a **vertical** pipe and to the sewer canal by a **horizontal** pipe. This implies that the fresh water canal cannot be vertical, and the sewer canal cannot be horizontal.
3.  **Separation Constraint (Cersei's):** The sewer canal must act as a separator. All noble houses must lie on one side of the sewer canal (its "left") or on the canal itself. All common houses must lie on the other side (its "right") or on the canal itself.
4.  **Budget Constraint (Tywin's):** The total length of all horizontal sewage pipes must not exceed a given budget $s$. If $s$ is given as -1, the budget is considered infinite.
5.  **Optimization Goal (Jaime's):** Subject to all the above constraints, the primary goal is to **minimize the length of the longest vertical fresh water pipe**.

## 💡 Hints

<details>

<summary>Hint #1</summary>

Since both canals are lines, and we are aiming to optimize objectives and check for feasibility, this is a perfect fit for **Linear Programming (LP)**. The key is to express all constraints and the objective in linear form.

</details>

<details>

<summary>Hint #2</summary>

Model any line as $ax + by + c = 0$. The sign of $ax_i + by_i + c$ tells you a house’s side; $(a,b)$ is the normal. This turns “left/right of the sewer” into linear inequalities in $(a,b,c)$ and lets you forbid degenerate orientations (e.g., sewer not horizontal, water not vertical) by constraining coefficients.

</details>

<details>

<summary>Hint #3</summary>

Treat each requirement as a small model on its own before combining. This keeps the algebra manageable and makes debugging easier. For each of them first try to express the constraint using simple inequalities. If it turns out they are non-linear, look for standard linearization tricks: fix a scale, use sign information, or add auxiliary variables.

<details>

<summary>Cersei</summary> 

Write simple inequalities that put nobles on one side of the sewer line and commons on the other. To avoid the trivial all-zero solution, fix a scale.

</details>

<details>

<summary>Tywin</summary>

Express the total horizontal pipe length as a sum of distances from houses to the sewer line. If absolute values appear, use the separation to argue about signs or introduce an auxiliary variable so the constraint becomes linear.

</details>

<details>

<summary>Jaime</summary>

Model the longest vertical pipe by introducing a single variable that upper-bounds every house’s vertical deviation from the water line, then minimize that variable.

</details>

Once each piece is clear in isolation, combine them in one LP.

</details>

<details>

<summary>Hint #4</summary>

Add constraints in stages: check Cersei first (feasibility only). If that passes, add Tywin’s budget. Only when both hold, switch (or start a new LP) to minimize Jaime’s $d$. This fails fast and keeps the model compact.

</details>

<details>

<summary>Hint #5</summary>

Orthogonality in itself is a non-linear constraint. 
$$a_1a_2 + b_1b_2 = 0$$
However we can linearize it by fixing some coefficients.
For this we can use the fact that the water canal must be non-vertical (to allow vertical pipes) and the sewer canal must be non-horizontal (to allow horizontal pipes). This means we can set $b_2 = 1$ (to avoid vertical) and $a_1 = 1$ (to avoid horizontal) without loss of generality. The right-angle condition then simplifies to:
$$a_1a_2 + b_1b_2 = 0 \implies a_2 + b_1 = 0 \implies a_2 = -b_1$$

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
 $$a \le 0$$

We can now define an LP with variables $a, b, c$ and the constraints above. We are only interested in whether a feasible solution exists. If the LP is feasible, a separating line exists; otherwise, it does not.

*   If the LP is **infeasible**, it's impossible to satisfy Cersei's constraint. We output "Y".
*   If the LP is **feasible**, such a line exists. For this subtask, this means the answer is "B".

**Note**: With the code right now, we run into some problems on the hidden test set, as we run into a cycle on the simplex, while solving the linear program using CGAL. Therefore we need to use Bland’s Rule. To avoid the cycling.

### Code
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

### Code
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

<summary>Third Solution (Test Set 1, 2, 3)</summary>
  
The third solution handles all three subtasks in sequence, each time forming and solving a small Linear Program (LP). The goal is to exit early on failure or proceed to the next stage, finally optimizing the water pipe length.  
  
1. **Cersei’s Constraint (Separation):**  
    - Variables: $a,b,c$ define the sewer canal $ax+by+c=0$.  
    - Noble houses: enforce $ax_n + by_n + c \ge 1$.  
    - Common houses: enforce $ax_c + by_c + c \le -1$.  
    - Direction: $a \le 0$ ensures the normal points west (nobles on left).  
    - Solve for feasibility. If **infeasible**, output `Y` and terminate.  
  
2. **Tywin’s Constraint (Budget):**  
    - We need to cap the **sum of horizontal pipe lengths**. For a house at $(x_i,y_i)$, its pipe reaches the sewer canal at the unique point with the same $y_i$, so the length is  
      $$|x_i - x_{\text{canal}}(y_i)| = \frac{\bigl|x_i + b\,y_i + c\bigr|}{\sqrt{a^2+b^2}}$$
      but since $\sqrt{a^2+b^2}$ is constant in this LP we absorb it into the budget.  
    - Thanks to Cersei’s separation, all nobles lie on one side and commons on the other, so the signs of $(x_i + b y_i + c)$ are uniform within each group.  
      Thus, the sum of absolute values collapses into a **single linear combination** of prefix sums:  
      $$
      (S_{x,\text{commons}} - S_{x,\text{nobles}})
        + b\,(S_{y,\text{commons}} - S_{y,\text{nobles}})
        + c\,(m - n) \le s.
      $$
    - We simply add this one constraint to the LP. If the program becomes **infeasible**, we output `B` and stop.
  
3. **Jaime’s Objective (Minimax):**  
    - Start a new LP with variables $m,c,d$, where the water canal is $y = m x + c$ and $d$ represents the **maximum vertical pipe length** across all houses.  
    - Each house at $(x_i,y_i)$ connects via a vertical segment of length  
      $$|y_i - (m x_i + c)|$$
      By introducing $d$ and enforcing  
      $$y_i - (m x_i + c) \le d, \quad (m x_i + c) - y_i \le d$$
      for every house, we guarantee $d$ is at least as large as **every** individual pipe.  
    - This is the classic **minimax** trick: the maximum of absolute deviations becomes a single LP objective.  
    - We add the non-negativity constraint $d \ge 0$, set **minimize** $d$ as the objective, solve the LP, and then round up with `ceil(d)` to satisfy integer length units.
  
By solving these LPs incrementally, the code quickly handles feasibility checks before engaging in the final minimization step.

### Code

**Note**: The code is adapted from [this solution](https://github.com/haeggee/algolab/blob/main/problems/week06-lannister/src/algorithm.cpp)

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

const int m = 0;
// const int b = 1; // Already defined above
const int d = 2;

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
  
  // ===== THIRD LINEAR PROGRAM (JAIME) =====
  if(true) {  // Always optimize for Jamies requirement
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

<details>

<summary>Final Solution</summary>

The final solution approaches the problem similarly to the third solution. The key difference is in the handling of the right-angle constraint between the two canals.

The right-angle constraint states that the two canals must be perpendicular. If the sewer canal is represented by $a_1x + b_1y + c_1 = 0$ and the water canal by $a_2x + b_2y + c_2 = 0$, their normals $(a_1,b_1)$ and $(a_2,b_2)$ must satisfy:
$$a_1a_2 + b_1b_2 = 0$$
This is a non-linear constraint, which complicates the LP formulation. However, we can leverage the fact that the water canal must be non-vertical (to allow vertical pipes) and the sewer canal must be non-horizontal (to allow horizontal pipes). This means we can set $b_2 = 1$ (to avoid vertical) and $a_1 = 1$ (to avoid horizontal) without loss of generality. The right-angle condition then simplifies to:
$$a_2 + b_1 = 0 \implies a_2 = -b_1$$
This allows us to express the water canal in terms of $b_1$ and $c_2$, reducing the number of independent variables and keeping the LP linear, while ensuring that the right-angle constraint is satisfied.

Additionally, we use the same linear program throughout the three stages, adding constraints incrementally and finally optimizing Jaime's objective. This avoids the overhead of constructing and solving multiple LPs, making the solution more efficient.

### Code

**Note**: The code is adapted from [this solution](https://github.com/haeggee/algolab/blob/main/problems/week06-lannister/src/algorithm.cpp)

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

## 🧠 Learnings

<details> 

<summary> Expand to View </summary>

- You can solve the same LP multiple times, adding constraints incrementally. 

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