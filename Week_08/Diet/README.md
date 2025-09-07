# Diet

## 📝 Problem Description

The objective is to determine the minimum cost of a daily diet that satisfies specified nutritional requirements. Given $n$ nutrients and $m$ food products, each nutrient $i$ has a required minimum daily intake $\text{min}_i$ and a maximum daily intake $\text{max}_i$. Each food product $j$ is characterized by its price per unit $p_j$ and the amount of each nutrient $C_{j,i}$ provided per unit.

Any non-negative, fractional amount of any food product may be purchased and consumed. The task is to find a combination of foods such that, for every nutrient, the total intake lies within the specified bounds, while minimizing the total cost.

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

Let $x_j \ge 0$ be the amount of food product $j$ to purchase. The goal is to minimize the objective function $\sum_{j=0}^{m-1} p_j \cdot x_j$. For each nutrient $i$, the total intake is $\sum_{j=0}^{m-1} C_{j,i} \cdot x_j$. This gives us two constraints for each nutrient: $\sum_{j=0}^{m-1} C_{j,i} \cdot x_j \ge \text{min}_i$ and $\sum_{j=0}^{m-1} C_{j,i} \cdot x_j \le \text{max}_i$. These inequalities, along with the non-negativity constraints $x_j \ge 0$, define the full LP.

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

We can solve this problem using linear programming as we want to **minimize the cost of the diet**, while still fulfilling all the **constraints on the amount of nutrients** (minimum and maximum).

We first need to establish **how the cost is calculated**:

$$
\text{cost} = c = \sum_{j = 0}^m n_j \cdot p_j
$$

The overall cost is calculated as the sum of $n_j$ (the amount of food $j$ the person eats) times the price for each unit of that food. E.g. if the person needs 3 apples and 1 banana and one apple costs 1€ and a banana cost 2€ this would be $c = 3 \cdot 1 + 1 \cdot 2 = 5$ 

As the **prices are fixed**, we can not change them, so the **obvious choice for the variables in our linear program is the amount for each food** $n_j$. Now the only thing that is left is that we **define the constraints**, such that the max and min for each nutrient are fulfilled. As an equation this would correspond to this

$$
\begin{align*}
\text{min}_i \leq \sum_{j = 0}^m n_j \cdot C_{ji} \leq \text{max}_i
\end{align*}
$$

The total amount of nutrient $i$ consumed is equal to the sum over all products, where we calculate the amount we eat of product $j$ ($n_j$) times the amount of nutrient $i$, the product $j$ gives us ($C_{ji}$). This amount obviously needs to be between $\min_i$ and $\max_i$

When we bring this into the **format we need for CGAL**, we get the following constraints

$$
\begin{align*}
- \sum_{j = 0}^m n_j \cdot C_{ji} &\leq -\text{min}_i \\
\sum_{j = 0}^m n_j \cdot C_{ji} &\leq \text{max}_i \\
-n_j &\leq 0
\end{align*}
$$

**Note**: The last constraint is necessary as we obviously **can’t give a person a negative amount of some food**. But the last constraint can be implemented in CGAL easier by just **defining a lower bound on all variables during the creation of the linear program.**

Now we can simply turn this into CGAL code and solve the task.

### Code
```cpp
#include<iostream>
#include<vector>
#include<cmath>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

typedef int IT;
typedef CGAL::Gmpz ET;

typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

int main() {
  std::ios_base::sync_with_stdio(false);
  
  while(true) {
    // ===== READ INPUT =====
    int n, m; std::cin >> n >> m;
    if (n == 0 && m == 0) break;
    
    std::vector<IT> min(n);
    std::vector<IT> max(n);
    for(int i = 0; i < n; ++i) {
      std::cin >> min[i] >> max[i];
    }
    
    std::vector<IT> prices(m);
    std::vector<std::vector<IT>> nutrients(m, std::vector<IT>(n));
    for(int i = 0; i < m; ++i) {
      std::cin >> prices[i];
      
      for(int j = 0; j < n; ++j) {
        std::cin >> nutrients[i][j];
      }
    }
    
    // ===== CONSTRUCT LINEAR PROGRAM =====
    Program lp(CGAL::SMALLER, true, 0, false, 0);
    
    // Define min and max constraints
    for(int nutrient_idx = 0; nutrient_idx < n; ++nutrient_idx) {
      for(int foot_idx = 0; foot_idx < m; ++foot_idx) {
        lp.set_a(foot_idx,     nutrient_idx, -nutrients[foot_idx][nutrient_idx]);
        lp.set_a(foot_idx, n + nutrient_idx,  nutrients[foot_idx][nutrient_idx]);
      }
      lp.set_b(    nutrient_idx, -min[nutrient_idx]);
      lp.set_b(n + nutrient_idx,  max[nutrient_idx]);
    }
    
    // Define objective
    for(int foot_idx = 0; foot_idx < m; ++foot_idx) {
      lp.set_c(foot_idx, prices[foot_idx]);
    }
    
    // ===== SOLVE AND OUTPUT =====
    Solution s = CGAL::solve_linear_program(lp, ET());
    if(s.is_infeasible()) {
      std::cout << "No such diet." << std::endl;
    } else {
      std::cout <<(long) std::floor(CGAL::to_double(s.objective_value())) << std::endl;
    }
  }
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