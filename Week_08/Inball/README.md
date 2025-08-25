# Inball

## 📝 Problem Description

The problem asks for the maximum possible integral radius of a $d$-dimensional ball that can be contained within a convex polyhedron. This polyhedron is defined as the intersection of $n$ half-spaces in $\mathbb{R}^d$. Each half-space is given by a linear inequality of the form $a_i^T x \leq b_i$, where $x$ is a point in $\mathbb{R}^d$, $a_i$ is a $d$-dimensional vector, and $b_i$ is a scalar.

For each given polyhedron, specified by the dimension $d$, the number of inequalities $n$, and the coefficients for each inequality, the goal is to determine this maximum radius. If the polyhedron is an empty set (i.e., no point satisfies all inequalities simultaneously), the output should be "none". If a ball of arbitrarily large radius can fit, the output should be "inf".

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem requires you to maximize a certain value—the radius—while satisfying a given set of linear inequalities. This structure is characteristic of a specific class of optimization problems. Which mathematical framework is designed to solve such problems?
</details>
<details>
<summary>Hint #2</summary>
Let's model the problem more formally. The center of the ball can be represented by a vector of variables $x = (x_1, \dots, x_d)$, and its radius by another variable $r$. For the ball to be contained within the region defined by an inequality $a_i^T x \le b_i$, its center $x$ must be at a sufficient distance from the boundary hyperplane $a_i^T y = b_i$. How can you express the geometric condition "the distance from point $x$ to the hyperplane $a_i^T y = b_i$ is at least $r$" as a mathematical inequality?
</details>
<details>
<summary>Hint #3</summary>
The perpendicular distance from a point $x_0$ to a hyperplane defined by $a^T x = b$ is given by the formula $\frac{|a^T x_0 - b|}{\|a\|_2}$. For our ball to be inside the polyhedron, its center $x$ must satisfy $a_i^T x \le b_i$ for all $i$. The condition that the ball does not cross the hyperplane boundary translates to requiring the distance from its center $x$ to each hyperplane $a_i^T y = b_i$ to be at least $r$.

This gives us the set of constraints:
$$ \frac{b_i - a_i^T x}{\|a_i\|_2} \ge r \quad \text{for all } i = 1, \dots, n $$
This can be rearranged into a linear inequality:
$$ a_i^T x + r \cdot \|a_i\|_2 \le b_i $$
With the objective to maximize $r$, this forms a complete **Linear Program (LP)**.
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1, 2)</summary>

### Approach: Linear Programming with Normalization

This problem can be elegantly modeled as a **Linear Program (LP)**. We are trying to maximize a value, the radius $r$, subject to a set of linear constraints that define the "cave".

Let the center of the $d$-dimensional ball be the vector $x = (x_1, \dots, x_d)$ and its radius be $r$. The cave is defined by $n$ inequalities $a_i^T x \le b_i$.

#### Geometric Intuition
The expression $a_i^T x$ is the scalar projection of vector $x$ onto vector $a_i$, scaled by the length of $a_i$. If we normalize $a_i$ to have a unit norm (i.e., $\|a_i\|_2 = 1$), then $a_i^T x$ gives the signed distance of the point $x$ from a hyperplane passing through the origin with normal vector $a_i$. The term $b_i$ effectively shifts this hyperplane. The distance from a point $x$ to the hyperplane $a_i^T y = b_i$ is $\frac{b_i - a_i^T x}{\|a_i\|_2}$.

For a ball of radius $r$ centered at $x$ to be fully contained within the cave, it must not cross any of the boundary hyperplanes. This means the distance from its center $x$ to every hyperplane must be at least $r$. This gives us the following condition for each constraint $i$:
$$ \frac{b_i - a_i^T x}{\|a_i\|_2} \geq r $$
Rearranging this, we get:
$$ a_i^T x + r \cdot \|a_i\|_2 \le b_i $$

#### LP Formulation
A straightforward way to implement this is to first normalize each constraint. Let $a'_i = \frac{a_i}{\|a_i\|_2}$ and $b'_i = \frac{b_i}{\|a_i\|_2}$. The constraint then simplifies to:
$$ (a'_i)^T x + r \le b'_i $$
This is a linear inequality in terms of the variables $x_1, \dots, x_d$ and $r$.

Our complete LP is:
- **Variables:** $x_1, \dots, x_d, r$
- **Objective:** Maximize $r$
- **Constraints:**
    1. $(a'_i)^T x + r \le b'_i$ for all $i = 1, \dots, n$
    2. $r \ge 0$ (a radius cannot be negative)

#### Implementation
We use the CGAL library to solve this LP. The process is as follows:
1. Read the input values $n$, $d$, and the coefficients for each inequality.
2. For each inequality $i$, calculate the norm $\|a_i\|_2$. The problem statement guarantees this norm is an integer, which simplifies computation and avoids floating-point precision issues.
3. **Normalize** each constraint by dividing all its coefficients, including $b_i$, by its norm $\|a_i\|_2$.
4. Construct the LP using CGAL. We have $d+1$ variables (for $x_1, \dots, x_d$ and $r$). The variable with index $d$ will represent $r$.
5. For each normalized constraint $i$, set the coefficients for the variables $x_1, \dots, x_d$ and set the coefficient for $r$ to 1.
6. Set a lower bound of 0 for $r$.
7. Set the objective function to maximize $r$. Since CGAL's solver minimizes by default, we achieve this by setting the objective to minimize $-r$.
8. Solve the LP and interpret the results. An infeasible solution means the cave is empty ("none"), an unbounded solution means an infinitely large ball fits ("inf"), and an optimal solution gives the maximum radius.

This approach is correct but involves many divisions on `CGAL::Gmpq` rational numbers during the normalization step, which can be computationally expensive and leads to a "Time Limit Exceeded" verdict on larger test sets.

```cpp
#include <iostream>
#include <vector>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>

typedef CGAL::Gmpq IT;
typedef CGAL::Gmpq ET;

typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

int main() {
  std::ios_base::sync_with_stdio(false);
  
  while(true) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    if(n == 0) break;
    int d; std::cin >> d;
    
    
    std::vector<std::vector<IT>> A(n, std::vector<IT>(d));
    std::vector<IT> norms(n);
    std::vector<IT> b(n);
    for(int i = 0; i < n; ++i) {
      double norm = 0;
      for(int j = 0; j < d; ++j) {
        std::cin >> A[i][j];
        norm += std::pow(CGAL::to_double(A[i][j]), 2);
      }
      
      // Using std::sqrt is usually "dangerous" but as the problem specified
      // that all norms are integers, this should be fine
      norms[i] = std::sqrt(norm);
      
      std::cin >> b[i];
    }
    
    // ===== NORMALIZE ALL CONSTRAINTS =====
    for(int i = 0; i < n; ++i) {
      for(int j = 0; j < d; ++j) {
        A[i][j] /= norms[i];
      }
      b[i] /= norms[i];
    }
    
    // ===== CONSTRUCT LINEAR PROGRAM =====
    const int r = d;
    
    Program lp(CGAL::SMALLER, false, 0, false, 0);
    
    for(int i = 0; i < n; ++i) {
      for(int j = 0; j < d; ++j) {
        lp.set_a(j, i, A[i][j]);
      }
      
      lp.set_a(r, i, 1);
      lp.set_b(i, b[i]);
    }
    
    lp.set_l(r, true, 0);
    lp.set_c(r, -1); // Maximize r => Invert objective
    
    // ===== SOLVE AND OUTPUT =====
    Solution s = CGAL::solve_linear_program(lp, ET());
    
    if(s.is_infeasible()) {
      std::cout << "none" << std::endl;
    } else if(s.is_unbounded()) {
      std::cout << "inf" << std::endl;
    } else {
      std::cout << (long) CGAL::to_double(-s.objective_value()) << std::endl;
    }
  }
}
```
</details>

<details>
<summary>Final Solution</summary>

### Approach: Optimized Linear Programming without Explicit Normalization

The previous solution is functionally correct but suffers from performance issues due to the explicit normalization step, which involves a large number of divisions with exact rational numbers. We can significantly optimize the process by reformulating the constraints to avoid these divisions.

#### Algebraic Optimization
Recall the fundamental constraint derived from the distance formula for each hyperplane $i$:
$$ \frac{b_i - a_i^T x}{\|a_i\|_2} \ge r $$
Instead of dividing the left side by the norm $\|a_i\|_2$, we can multiply both sides by it. Since the norm is always non-negative, this does not change the direction of the inequality:
$$ b_i - a_i^T x \ge r \cdot \|a_i\|_2 $$
Rearranging this gives us a new, equivalent linear constraint:
$$ a_i^T x + \|a_i\|_2 \cdot r \le b_i $$

This formulation is algebraically equivalent to the normalized version but computationally superior. It avoids the loop of $n \times d$ divisions, replacing them with a single multiplication by the norm in each constraint setup. Since the norms $\|a_i\|_2$ are integers (as per the problem statement), all coefficients in the LP will be integers, which is much more efficient for the solver.

#### Optimized LP Formulation
- **Variables:** $x_1, \dots, x_d, r$
- **Objective:** Maximize $r$
- **Constraints:**
    1. $a_i^T x + \|a_i\|_2 \cdot r \le b_i$ for all $i = 1, \dots, n$
    2. $r \ge 0$

#### Implementation
The implementation is almost identical to the first solution, with one key change.
1. Read the input and calculate the integer norms $\|a_i\|_2$ as before.
2. **Do not** perform the explicit normalization loop. Use the input coefficients $a_{ij}$ and $b_i$ directly.
3. When setting up the LP constraints in CGAL, for each inequality $i$, set the coefficient of the radius variable $r$ to be $\|a_i\|_2$ instead of 1.
    - `lp.set_a(r_idx, i, norms[i]);`
This single change implements the optimized constraint and leads to a much faster solution that passes all test sets.

```cpp
#include <iostream>
#include <vector>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>

typedef CGAL::Gmpq IT;
typedef CGAL::Gmpq ET;

typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

int main() {
  std::ios_base::sync_with_stdio(false);
  
  while(true) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    if(n == 0) break;
    int d; std::cin >> d;
    
    std::vector<std::vector<IT>> A(n, std::vector<IT>(d));
    std::vector<IT> norms(n);
    std::vector<IT> b(n);
    for(int i = 0; i < n; ++i) {
      double norm = 0;
      for(int j = 0; j < d; ++j) {
        std::cin >> A[i][j];
        norm += std::pow(CGAL::to_double(A[i][j]), 2);
      }
      std::cin >> b[i];
    
      norms[i] = std::sqrt(norm);
    }
    
    // ===== CONSTRUCT LINEAR PROGRAM =====
    const int r = d;
    
    Program lp(CGAL::SMALLER, false, 0, false, 0);
    
    for(int i = 0; i < n; ++i) {
      for(int j = 0; j < d; ++j) {
        lp.set_a(j, i, A[i][j]);
      }
      
      lp.set_a(r, i, norms[i]);
      lp.set_b(i, b[i]);
    }
    
    lp.set_l(r, true, 0);
    lp.set_c(r, -1); // Maximize r => Invert objective
    
    // ===== SOLVE AND OUTPUT =====
    Solution s = CGAL::solve_linear_program(lp, ET());
    
    if(s.is_infeasible()) {
      std::cout << "none" << std::endl;
    } else if(s.is_unbounded()) {
      std::cout << "inf" << std::endl;
    } else {
      std::cout << (long) CGAL::to_double(-s.objective_value()) << std::endl;
    }
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (35 pts / 2 s) : Correct answer      (0.026s)
   Test set 2 (35 pts / 2 s) : Correct answer      (0.233s)
   Test set 3 (30 pts / 2 s) : Correct answer      (1.907s)

Total score: 100
```