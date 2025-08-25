# Maximize it!

## 📝 Problem Description

This problem requires solving one of two distinct optimization tasks, determined by an input parameter $p$. For given parameters $a$ and $b$, you must find the optimal value.

If $p=1$, the task is to **maximize** the objective function $b \cdot y - a \cdot x$, subject to the following constraints on variables $x$ and $y$:
- $x, y \ge 0$
- $x + y \le 4$
- $4x + 2y \le ab$
- $-x + y \le 1$

If $p=2$, the task is to **minimize** the objective function $a \cdot x + b \cdot y + z$, subject to the following constraints on variables $x$, $y$, and $z$:
- $x, y, z \le 0$
- $x + y \ge -4$
- $4x + 2y + z \ge -ab$
- $-x + y \ge -1$

For each set of inputs, you must provide the optimal value. For the maximization problem, the result should be rounded down to the nearest integer. For the minimization problem, it should be rounded up. If no solution satisfies the constraints, the output should be "no". If the objective function can be made arbitrarily large (for maximization) or arbitrarily small (for minimization), the output should be "unbounded".

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem is presented in a very structured, mathematical form. You are asked to optimize a linear function of several variables, subject to a set of linear inequality constraints. What is the general name for this class of optimization problems?
</details>
<details>
<summary>Hint #2</summary>
This problem is a classic example of **Linear Programming (LP)**. Rather than implementing a solver from scratch (like the Simplex algorithm), you can leverage existing, highly-optimized libraries. The CGAL (Computational Geometry Algorithms Library) is an excellent choice for this, as it provides a robust LP solver.
</details>
<details>
<summary>Hint #3</summary>
The CGAL LP solver is designed to *minimize* an objective function. The problem for $p=2$ is a minimization problem, so it can be solved directly. However, the problem for $p=1$ is a *maximization* problem. To handle this, you can use the mathematical identity:
$$ \max(f(\mathbf{x})) = - \min(-f(\mathbf{x})) $$
This means you can solve the maximization problem by minimizing the *negation* of its objective function and then negating the resulting optimal value.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
This problem is a direct application of **Linear Programming (LP)**. An LP problem involves optimizing (minimizing or maximizing) a linear objective function, subject to a set of linear equality and inequality constraints. Given the explicit mathematical formulation, the most straightforward approach is to use a dedicated LP solver, such as the one provided by the CGAL library.

### Core Idea: Translating the Problem for CGAL

The main task is to translate the two given optimization problems into a format that the CGAL LP solver understands. This involves:
1.  Defining the variables.
2.  Setting the bounds for each variable (e.g., $x \ge 0$ or $z \le 0$).
3.  Specifying the linear constraints.
4.  Defining the objective function to be optimized.

### Handling Problem Type 1 (Maximization)

-   **Objective:** Maximize $b \cdot y - a \cdot x$.
-   **CGAL Adaptation:** As mentioned in the hints, CGAL's solver minimizes. We can find $\max(b \cdot y - a \cdot x)$ by computing $-\min(-(b \cdot y - a \cdot x))$. This means we ask CGAL to minimize the new objective function $a \cdot x - b \cdot y$. The final answer will be the negative of the value returned by the solver.
-   **Variables and Bounds:** We have two variables, $x$ and $y$. The constraint $x, y \ge 0$ sets a lower bound of 0 for both. This is configured in the `Program` constructor: `Program lp(CGAL::SMALLER, true, 0, false, 0);`. The `true, 0` part sets a lower bound of 0 for all variables.
-   **Constraints:** The inequalities are of the form $A \cdot x \le b$. We translate each one:
    -   $x + y \le 4 \implies 1 \cdot x + 1 \cdot y \le 4$
    -   $4x + 2y \le ab \implies 4 \cdot x + 2 \cdot y \le ab$
    -   $-x + y \le 1 \implies -1 \cdot x + 1 \cdot y \le 1$
-   **Output:** If a solution exists, we take `-s.objective_value()` and round it down using `std::floor`.

### Handling Problem Type 2 (Minimization)

-   **Objective:** Minimize $a \cdot x + b \cdot y + z$. This is a standard minimization problem, so we can use the solver directly.
-   **Variables and Bounds:** We have three variables: $x, y, z$. The constraint $x, y, z \le 0$ sets an upper bound of 0 for all of them. This is configured as `Program lp(CGAL::LARGER, false, 0, true, 0);`. The `true, 0` part here sets an *upper* bound of 0.
-   **Constraints:** The inequalities are of the form $A \cdot x \ge b$. The `CGAL::LARGER` parameter in the constructor tells the solver to interpret constraints as $\ge$.
    -   $x + y \ge -4 \implies 1 \cdot x + 1 \cdot y + 0 \cdot z \ge -4$
    -   $4x + 2y + z \ge -ab \implies 4 \cdot x + 2 \cdot y + 1 \cdot z \ge -ab$
    -   $-x + y \ge -1 \implies -1 \cdot x + 1 \cdot y + 0 \cdot z \ge -1$
-   **Output:** If a solution exists, we take `s.objective_value()` and round it up using `std::ceil`.

### Interpreting Solver Results

The `CGAL::Quadratic_program_solution` object `s` provides methods to check the status of the solution:
-   `s.is_infeasible()`: Returns `true` if no solution satisfies the constraints. We output "no".
-   `s.is_unbounded()`: Returns `true` if the objective function can be improved indefinitely. We output "unbounded".
-   `s.objective_value()`: Returns the optimal value if one exists.

The following C++ code implements this logic.

```cpp
#include<iostream>
#include<cmath>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

typedef int IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;
typedef Solution::Variable_value_iterator SVI;

const int X = 0;
const int Y = 1;
const int Z = 2;

int main() {
  std::ios_base::sync_with_stdio(false);
  while(true) {
    // ===== READ INPUT =====
    int p; std::cin >> p;
    if(p == 0) break;
    
    IT a, b; std::cin >> a >> b;
    
    // ===== SOLVE DEPENDING ON p =====
    if(p == 1) {
      Program lp(CGAL::SMALLER, true, 0, false, 0);
      
      // Set constraints
      lp.set_a(X, 0, 1); lp.set_a(Y, 0, 1); lp.set_b(0, 4);
      lp.set_a(X, 1, 4); lp.set_a(Y, 1, 2); lp.set_b(1, a * b);
      lp.set_a(X, 2, -1); lp.set_a(Y, 2, 1); lp.set_b(2, 1);
      
      // Set objective
      lp.set_c(X, a); lp.set_c(Y, -b);
      
      // Solve
      Solution s = CGAL::solve_linear_program(lp, ET());
      
      if(s.is_unbounded()) {
        std::cout << "unbounded" << std::endl;
      } else if(s.is_infeasible()) {
        std::cout << "no" << std::endl;
      } else {
        std::cout << (long)std::floor(-CGAL::to_double(s.objective_value())) << std::endl;
      }
    } else {
      Program lp(CGAL::LARGER, false, 0, true, 0);
      
      // Set constraints
      lp.set_a(X, 0, 1); lp.set_a(Y, 0, 1); lp.set_b(0, -4);
      lp.set_a(X, 1, 4); lp.set_a(Y, 1, 2); lp.set_a(Z, 1, 1); lp.set_b(1, -a * b);
      lp.set_a(X, 2, -1); lp.set_a(Y, 2, 1); lp.set_b(2, -1);
      
      // Set objective
      lp.set_c(X, a); lp.set_c(Y, b); lp.set_c(Z, 1);
      
      // Solve
      Solution s = CGAL::solve_linear_program(lp, ET());
      
      if(s.is_infeasible()) {
        std::cout << "no" << std::endl;
      } else if(s.is_unbounded()) {
        std::cout << "unbounded" << std::endl;
      } else {
        std::cout << (long)std::ceil(CGAL::to_double(s.objective_value())) << std::endl;
      }
    }
  }
}
```

</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (50 pts / 0.100 s) : Correct answer      (0.0s)
   Test set 2 (50 pts / 0.100 s) : Correct answer      (0.0s)

Total score: 100
```