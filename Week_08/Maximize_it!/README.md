# Maximize it!

## 📝 Problem Description

This problem requires solving one of two distinct optimization tasks, determined by an input parameter $p$. For given parameters $a$ and $b$, the task is to find the optimal value.

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

## 💡 Hints

<details>

<summary>Hint #1</summary>

The CGAL LP solver is designed to *minimize* an objective function. The problem for $p=2$ is a minimization problem, so it can be solved directly. However, the problem for $p=1$ is a *maximization* problem. To handle this, you can use the mathematical identity:
$$ \max(f(\mathbf{x})) = - \min(-f(\mathbf{x})) $$
This means you can solve the maximization problem by minimizing the *negation* of its objective function and then negating the resulting optimal value.

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

This is a relatively simple exercise as the statement tells us **EXACTLY** what to do. The main “hard” thing here is to get used to **CGAL's Linear/Quadratic Programming Interface**:

### CGAL Quadratic Programming

- `Program lp(CGAL::SMALLER, true, 0, false, 0);` defines the program we want to solve:
  - `CGAL::SMALLER` sets the **default inequality** for constraints to $\leq$.
  - `true, 0` sets a **lower bound** of $0$ for all variables.
  - `false, 0` means there is **no upper bound** for all variables (the value `0` is ignored).
- `lp.set_a(X, 0, 1); lp.set_a(Y, 0, 1); lp.set_b(0, 4);` defines the individual constraints:
  - `lp.set_a` sets the **coefficients of the matrix $A$** of the linear system. The first parameter is the **variable/column**, the second is the **constraint index/row**, and the last is the **value**.
  - `lp.set_b` sets the **right-hand side $b$** for each constraint (first: constraint index, second: value).
- `lp.set_l`, `lp.set_u` can be used to set lower or upper bounds for individual variables if needed.
- `lp.set_c(X, a); lp.set_c(Y, -b);` sets the coefficients for the objective function:
  - `lp.set_c` assigns a coefficient to a variable in the objective.
  - `lp.set_c0` sets the constant term $c_0$ in the objective.

**Note:** CGAL can **only minimize** a linear program. If you want to maximize instead, you must **multiply the objective function by $-1$** and also multiply the resulting objective value by $-1$ after solving.

### Solution Structure

- For $p=1$ (maximization), set up the program as described, flip the objective, and round the result down.
- For $p=2$ (minimization), set up the program with the appropriate bounds and constraints, and round the result up.

### Interpreting Results

The solution object provides:
- `is_infeasible()`: No solution exists (output "no").
- `is_unbounded()`: The objective can be improved indefinitely (output "unbounded").
- `objective_value()`: The optimal value (rounded as required).

### Code
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

## 🧠 Learnings

<details> 

<summary> Expand to View </summary>

- To maximize an objective function in CGAL, flip all the constraints and the objective function, as CGAL always minimizes.

</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (50 pts / 0.100 s) : Correct answer      (0.0s)
   Test set 2 (50 pts / 0.100 s) : Correct answer      (0.0s)

Total score: 100
```