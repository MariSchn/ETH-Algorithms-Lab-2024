# Diet

## 📝 Problem Description

The problem asks us to find the minimum cost for a daily diet that satisfies a given set of nutritional requirements. We are given $n$ nutrients and $m$ different food products. For each nutrient $i$, we have a required minimum daily intake, $\text{min}_i$, and a maximum daily intake, $\text{max}_i$. For each food product $j$, we are given its price per unit, $p_j$, and the amount of each nutrient $C_{j,i}$ that one unit of this food provides.

We can purchase and consume any non-negative, fractional amount of any food product. The task is to determine the combination of foods that meets all nutrient constraints (i.e., for each nutrient, the total amount consumed is between its minimum and maximum required values) at the lowest possible total cost. We need to output this minimum cost, rounded down to the nearest integer. If no combination of foods can satisfy all the nutritional constraints, we should indicate that no such diet exists.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The core of the problem is to minimize a total cost, which is a sum of quantities multiplied by prices. This minimization is subject to several conditions, each of which limits the total amount of a nutrient to a specific range. What mathematical framework is designed for optimizing a linear objective function under linear inequality constraints?
</details>
<details>
<summary>Hint #2</summary>
The problem can be modeled as a **Linear Program (LP)**. The variables of the LP would represent the quantities of each food to be purchased. The objective function would be the total cost, and the constraints would ensure that the nutrient intake levels are within the specified bounds.
</details>
<details>
<summary>Hint #3</summary>
Let $x_j \ge 0$ be the amount of food product $j$ to purchase. The goal is to minimize the objective function $\sum_{j=0}^{m-1} p_j \cdot x_j$. For each nutrient $i$, the total intake is $\sum_{j=0}^{m-1} C_{j,i} \cdot x_j$. This gives us two constraints for each nutrient: $\sum_{j=0}^{m-1} C_{j,i} \cdot x_j \ge \text{min}_i$ and $\sum_{j=0}^{m-1} C_{j,i} \cdot x_j \le \text{max}_i$. These inequalities, along with the non-negativity constraints $x_j \ge 0$, define the full LP. You can use a library like CGAL to solve this.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
This problem is a classic example of an optimization problem that can be solved using **Linear Programming (LP)**. Our goal is to minimize a linear function (the total cost of the diet) subject to a set of linear inequality constraints (the nutritional requirements).

### LP Formulation

Let's define the components of our linear program.

#### 1. Variables
The quantities we need to determine are the amounts of each food product to purchase. Let's define a variable $x_j$ for each food product $j=0, \dots, m-1$, representing the amount of that food in the diet. Since we cannot consume a negative amount of food, we have the implicit constraint $x_j \ge 0$.

#### 2. Objective Function
We want to minimize the total cost of the diet. The cost is calculated by summing the price of each food multiplied by the amount consumed.
$$ \text{Minimize: } \quad \text{Cost} = \sum_{j=0}^{m-1} p_j \cdot x_j $$
Here, $p_j$ is the price of one unit of food product $j$.

#### 3. Constraints
The diet must satisfy the requirements for each of the $n$ nutrients. For any given nutrient $i$ (where $i=0, \dots, n-1$), its total amount in the diet must be between $\text{min}_i$ and $\text{max}_i$.

The total amount of nutrient $i$ consumed is the sum of contributions from all food products. One unit of food $j$ provides $C_{j,i}$ of nutrient $i$, so $x_j$ units provide $x_j \cdot C_{j,i}$. Summing over all foods gives the total:
$$ \text{Total Nutrient}_i = \sum_{j=0}^{m-1} x_j \cdot C_{j,i} $$
This leads to a pair of inequalities for each nutrient $i$:
$$ \text{min}_i \le \sum_{j=0}^{m-1} x_j \cdot C_{j,i} \le \text{max}_i $$

### Implementation with CGAL

Most LP solvers, including the one in the CGAL library, require constraints to be in the standard form $A \cdot x \le b$. We need to convert our constraints to match this format.

The double-sided inequality for each nutrient can be split into two separate inequalities:
1.  $\sum_{j=0}^{m-1} x_j \cdot C_{j,i} \le \text{max}_i$
2.  $\sum_{j=0}^{m-1} x_j \cdot C_{j,i} \ge \text{min}_i$

The first inequality is already in the required form. The second one can be converted by multiplying both sides by -1, which reverses the inequality sign:
$$ -\sum_{j=0}^{m-1} x_j \cdot C_{j,i} \le -\text{min}_i $$
This gives us a total of $2n$ linear constraints. The non-negativity constraint ($x_j \ge 0$) is handled directly by CGAL by specifying that all variables have a lower bound of 0.

In the code, we set up a `Program` object.
- The variables $x_0, \dots, x_{m-1}$ correspond to the indices `0, ..., m-1`.
- For each nutrient $i=0, \dots, n-1$, we add two constraint rows:
    - Constraint row `i`: Represents $-\sum x_j \cdot C_{j,i} \le -\text{min}_i$.
    - Constraint row `n+i`: Represents $\sum x_j \cdot C_{j,i} \le \text{max}_i$.
- The coefficients of the objective function, $p_j$, are set using `lp.set_c()`.

After setting up the program, we call `CGAL::solve_linear_program`. If the solution is `infeasible`, no diet satisfies the constraints. Otherwise, we retrieve the optimal objective value, which is the minimum cost, and print it rounded down.

```cpp
#include <iostream>
#include <vector>
#include <cmath>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

// Type definitions for the linear program
typedef int IT; // Input type
typedef CGAL::Gmpz ET; // Exact type for solver

// Program and solution types
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

void testcase() {
    int n, m;
    std::cin >> n >> m;
    if (n == 0 && m == 0) exit(0);

    // Read nutrient constraints
    std::vector<IT> min_nutrient(n), max_nutrient(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> min_nutrient[i] >> max_nutrient[i];
    }

    // Read food data
    std::vector<IT> prices(m);
    std::vector<std::vector<IT>> food_nutrients(m, std::vector<IT>(n));
    for (int i = 0; i < m; ++i) {
        std::cin >> prices[i];
        for (int j = 0; j < n; ++j) {
            std::cin >> food_nutrients[i][j];
        }
    }

    // Create a linear program:
    // Variables x_j >= 0 (true, 0)
    // Constraints are of the form A*x <= b (CGAL::SMALLER)
    Program lp(CGAL::SMALLER, true, 0, false, 0);

    // Set constraints
    for (int i = 0; i < n; ++i) { // For each nutrient i
        // Constraint for min_nutrient: sum(x_j * C_ji) >= min_i
        // -> -sum(x_j * C_ji) <= -min_i
        for (int j = 0; j < m; ++j) { // For each food j
            lp.set_a(j, i, -food_nutrients[j][i]);
        }
        lp.set_b(i, -min_nutrient[i]);

        // Constraint for max_nutrient: sum(x_j * C_ji) <= max_i
        for (int j = 0; j < m; ++j) {
            lp.set_a(j, n + i, food_nutrients[j][i]);
        }
        lp.set_b(n + i, max_nutrient[i]);
    }

    // Set objective function: minimize sum(price_j * x_j)
    for (int j = 0; j < m; ++j) {
        lp.set_c(j, prices[j]);
    }

    // Solve the linear program
    Solution s = CGAL::solve_linear_program(lp, ET());
    
    if (s.is_infeasible()) {
        std::cout << "No such diet." << std::endl;
    } else {
        std::cout << (long)std::floor(CGAL::to_double(s.objective_value())) << std::endl;
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    while (true) {
        testcase();
    }
    return 0;
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (35 pts / 0.200 s) : Correct answer      (0.001s)
   Test set 2 (35 pts / 0.200 s) : Correct answer      (0.006s)
   Test set 3 (30 pts / 0.200 s) : Correct answer      (0.07s)

Total score: 100
```