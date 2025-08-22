# Schneewittchen

## 📝 Problem Description

The goal is to determine the minimum cost to acquire specific quantities of various minerals for building a magic mirror. There are $N$ mines and $M$ different types of minerals.

The mines are connected by a system of railway tracks such that there is a unique path from any mine to a designated "entry" mine. For each mine, we know the available quantity of each of the $M$ minerals.

Some mines are classified as "dangerous". When a railcar full of minerals passes through a dangerous mine, the amount of every mineral in it is halved. Furthermore, each dangerous mine has a danger threshold; the total amount of all minerals entering it throughout the entire process must not exceed this threshold. Dangerous mines themselves contain no minerals, and the entry mine is never dangerous.

To supplement the mined resources, minerals can also be purchased from a shop. For each mineral type, the shop has a limited supply and a specific price per unit.

You are asked to find a plan to collect minerals from the mines and purchase them from the shop to meet all requirements. Your output should be the minimum possible cost to be paid at the shop. If it's impossible to meet the requirements, you should report that.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem asks to minimize a cost, which is a linear function of the amounts bought from the shop. This optimization is subject to several conditions: meeting the required amounts, not exceeding shop supply, and adhering to the danger thresholds in the mines. This structure—minimizing a linear objective under linear constraints—is characteristic of a specific class of optimization problems.
</details>
<details>
<summary>Hint #2</summary>
This problem can be effectively modeled using **Linear Programming (LP)**. The core task is to define variables that represent the quantities of minerals being moved or purchased and then translate the problem's rules into a set of linear equations and inequalities (constraints).
</details>
<details>
<summary>Hint #3</summary>
A crucial simplification is to realize that you don't need a variable for every single mine. The state of the system is entirely determined by the flow of minerals through a small set of "relevant" mines: the **entry mine** and all **dangerous mines**. The flow of minerals from any safe mine is only constrained by the next dangerous mine it encounters on its path to the entry. This significantly reduces the number of variables and constraints in your LP model.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
The problem of minimizing cost under a set of linear constraints is a classic application of **Linear Programming (LP)**. Our main task is to correctly model the flow of minerals, the associated costs, and all the given restrictions as an LP problem.

### Core Idea: Modeling with a Linear Program

The fundamental challenge is to determine the maximum amount of each mineral we can extract from the mines and deliver to the entry, subject to the constraints imposed by dangerous mines. Once we know this, we can calculate the remaining amount needed for each mineral and determine the minimum cost to purchase it from the shop.

A key insight is that we only need to model the flow of minerals through the "relevant" mines: the entry (mine 0) and the dangerous mines. The resources from any non-dangerous mine contribute to the flow of the first dangerous mine on its path to the entry (or to the entry itself if no such mine exists). This allows us to drastically reduce the complexity of our model.

### LP Variables
We define two types of variables:

1.  $g_{ij}$: The total amount of mineral $j$ that originates from the sub-structure "governed" by relevant mine $i$ and successfully arrives at mine $i$. The set of relevant mines includes the entry and all dangerous mines.
2.  $b_j$: The amount of mineral $j$ that we buy from the shop.

Our goal is to find the values of these variables that satisfy all constraints while minimizing the total shopping cost.

### LP Constraints
We can now express the problem's rules as a series of linear constraints.

1.  **Resource and Flow Conservation:** For each relevant mine $i$ and each mineral $j$, the amount $g_{ij}$ is constrained by two factors: the total available resources and the flow from its descendant dangerous mines.
    To formalize this, we first need to identify, for each relevant mine $i$, which other mines are in its "domain". The domain of $i$ consists of all mines `u` such that the path from `u` to the entry passes through `i`, but not through any other dangerous mine between `u` and `i`. We can find these mines using a traversal (like a DFS or BFS) starting from `i` and stopping the traversal down any path as soon as another dangerous mine is encountered.

    Let's denote the set of first-level dangerous children of `i` as $C_i$. The flow constraints for $g_{ij}$ are:

    *   **Lower Bound:** The flow $g_{ij}$ must account for the (halved) flows arriving from its dangerous children.
        $$ g_{ij} \geq \sum_{c \in C_i} \frac{1}{2} g_{cj} $$
        This can be rewritten as: $g_{ij} - \sum_{c \in C_i} \frac{1}{2} g_{cj} \geq 0$.

    *   **Upper Bound:** The flow $g_{ij}$ cannot exceed the total amount of mineral $j$ available in its domain, plus the contributions from its dangerous children. Let $R_{ij}$ be the total amount of mineral $j$ available in all non-dangerous mines within the domain of $i$ (including $i$ itself if it's the entry).
        $$ g_{ij} \leq R_{ij} + \sum_{c \in C_i} \frac{1}{2} g_{cj} $$
        This can be rewritten as: $g_{ij} - \sum_{c \in C_i} \frac{1}{2} g_{cj} \leq R_{ij}$.

2.  **Danger Threshold:** For each dangerous mine $i$ (where $i \neq 0$), the total amount of all minerals flowing through it must not exceed its danger threshold $d_i$.
    $$ \sum_{j=0}^{m-1} g_{ij} \leq d_i $$

3.  **Final Requirement:** For each mineral $j$, the amount extracted from the entry mine ($g_{0j}$) plus the amount bought from the shop ($b_j$) must be at least the required amount $c_j$.
    $$ g_{0j} + b_j \geq c_j $$

4.  **Shop Supply:** The amount of each mineral $j$ bought from the shop cannot exceed the available supply $s_j$.
    $$ 0 \leq b_j \leq s_j $$

5.  **Non-negativity:** All flow variables must be non-negative.
    $$ g_{ij} \geq 0 $$

### Objective Function
We want to minimize the total cost paid at the shop. This is a linear function of our "buy" variables $b_j$ and their corresponding prices $p_j$.
$$ \text{minimize} \quad \sum_{j=0}^{m-1} b_j \cdot p_j $$

### Implementation Note: Variable Indexing
Since we only create variables for a small subset of mines (at most 21: 1 entry + 20 dangerous), we should map their original mine indices to a compact range (e.g., $0, 1, \dots, k$) for the LP solver. This prevents the creation of a large, sparse variable matrix, which would be inefficient. For instance, if mine 999 is dangerous, we wouldn't want the LP solver to implicitly create variables for mines 0 through 998. The code below uses a `std::unordered_map` for this purpose.

By setting up and solving this LP, we can find the optimal solution. If the LP solver reports that the problem is infeasible, it means there is no way to satisfy all constraints, and the output should be "Impossible!". Otherwise, the objective value gives the minimum cost.

```cpp
#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <iomanip>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpq.h>

// CGAL types
typedef double IT;
typedef CGAL::Gmpq ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

// Struct to hold mineral properties
struct Mineral {
  long required;
  long supply;
  int price;
};

// Function to safely convert CGAL's exact rational type to a double and floor it
double floor_to_double(const CGAL::Quotient<ET>& x) {
 double a = std::floor(CGAL::to_double(x));
 while (a > x) a -= 1;
 while (a+1 <= x) a += 1;
 return a;
}

void solve() {
  // ===== READ INPUT =====
  int n, m;
  std::cin >> n >> m;
  
  std::vector<long> danger_levels(n);
  std::vector<bool> is_dangerous(n, false);
  std::vector<std::vector<int>> available_minerals(n, std::vector<int>(m));
  
  std::vector<int> relevant_mines;
  relevant_mines.push_back(0); // The entry is always relevant

  for (int i = 0; i < n; ++i) {
    std::cin >> danger_levels[i];
    if (danger_levels[i] >= 0) {
      is_dangerous[i] = true;
      if (i != 0) relevant_mines.push_back(i);
    }
    for (int j = 0; j < m; ++j) {
      std::cin >> available_minerals[i][j];
    }
  }
  
  std::vector<std::vector<int>> children_adj(n);
  for (int i = 0; i < n - 1; ++i) {
    int u, v;
    std::cin >> u >> v;
    children_adj[v].push_back(u);
  }
  
  std::vector<Mineral> minerals(m);
  for (int i = 0; i < m; ++i) {
    std::cin >> minerals[i].required >> minerals[i].supply >> minerals[i].price;
  }

  // ===== SETUP LP =====
  
  // Map relevant mine indices to compact LP variable indices to avoid a sparse matrix
  // b_j variables are at indices 0 to m-1
  // g_ij variables start from index m
  int var_idx = m;
  std::unordered_map<int, int> mine_to_var_map;
  for (int mine_id : relevant_mines) {
    mine_to_var_map[mine_id] = var_idx;
    var_idx += m;
  }

  Program lp(CGAL::SMALLER, true, 0, false, 0);
  int constraint_idx = 0;

  // Build constraints for each relevant mine
  for (int i : relevant_mines) {
    // For mine i, find its domain: all descendant mines up to the next dangerous one
    std::vector<int> domain_children;
    std::vector<int> dangerous_descendants;
    std::stack<int> s;
    for (int child : children_adj[i]) {
      s.push(child);
    }
    
    while (!s.empty()) {
      int curr = s.top();
      s.pop();
      if (is_dangerous[curr]) {
        dangerous_descendants.push_back(curr);
      } else {
        domain_children.push_back(curr);
        for (int child : children_adj[curr]) {
          s.push(child);
        }
      }
    }
    
    // Calculate total minerals available in the domain of i
    std::vector<long> total_minerals_in_domain(m, 0);
    if (!is_dangerous[i]) { // Entry mine can have minerals
        for(int j=0; j<m; ++j) total_minerals_in_domain[j] += available_minerals[i][j];
    }
    for (int child : domain_children) {
      for (int j = 0; j < m; ++j) {
        total_minerals_in_domain[j] += available_minerals[child][j];
      }
    }
    
    // Create flow conservation and resource constraints
    for (int j = 0; j < m; ++j) {
      int current_g_var = mine_to_var_map[i] + j;
      
      // Constraint: g_ij - 0.5 * sum(g_cj) <= R_ij
      lp.set_a(current_g_var, constraint_idx, 1);
      for (int child_mine : dangerous_descendants) {
        lp.set_a(mine_to_var_map[child_mine] + j, constraint_idx, -0.5);
      }
      lp.set_b(constraint_idx, total_minerals_in_domain[j]);
      constraint_idx++;
      
      // Constraint: g_ij - 0.5 * sum(g_cj) >= 0  (or -g_ij + 0.5 * sum(g_cj) <= 0)
      lp.set_a(current_g_var, constraint_idx, -1);
      for (int child_mine : dangerous_descendants) {
        lp.set_a(mine_to_var_map[child_mine] + j, constraint_idx, 0.5);
      }
      lp.set_b(constraint_idx, 0);
      constraint_idx++;
    }
    
    // Danger threshold constraint for dangerous mines
    if (is_dangerous[i]) {
      for (int j = 0; j < m; ++j) {
        lp.set_a(mine_to_var_map[i] + j, constraint_idx, 1);
      }
      lp.set_b(constraint_idx, danger_levels[i]);
      constraint_idx++;
    }
  }
  
  // Requirement constraints: g_0j + b_j >= c_j (or -g_0j - b_j <= -c_j)
  for (int j = 0; j < m; ++j) {
    lp.set_a(j, constraint_idx, -1); // b_j variable
    lp.set_a(mine_to_var_map[0] + j, constraint_idx, -1); // g_0j variable
    lp.set_b(constraint_idx, -minerals[j].required);
    constraint_idx++;
  }
  
  // Shop supply constraints and objective function
  for (int j = 0; j < m; ++j) {
    lp.set_u(j, true, minerals[j].supply); // b_j <= supply
    lp.set_c(j, minerals[j].price);        // objective coefficient for b_j
  }
  
  // ===== SOLVE AND OUTPUT =====
  Solution s = CGAL::solve_linear_program(lp, ET());
  
  if (s.is_infeasible()) {
    std::cout << "Impossible!" << std::endl;
  } else {
    std::cout << static_cast<long>(floor_to_double(s.objective_value())) << std::endl;
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

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1        (20 pts / 1 s) : Correct answer      (0.07s)
   Test set 2        (15 pts / 1 s) : Correct answer      (0.451s)
   Test set 3        (20 pts / 1 s) : Correct answer      (0.295s)
   Test set 4        (15 pts / 1 s) : Correct answer      (0.321s)
   Test set 5        (10 pts / 1 s) : Correct answer      (0.495s)
   Hidden test set 2 (05 pts / 1 s) : Correct answer      (0.531s)
   Hidden test set 3 (05 pts / 1 s) : Correct answer      (0.557s)
   Hidden test set 4 (05 pts / 1 s) : Correct answer      (0.583s)
   Hidden test set 5 (05 pts / 1 s) : Correct answer      (0.643s)

Total score: 100
```