# Schneewittchen

## 📝 Problem Description

The goal is to determine the minimum cost to acquire specific quantities of various minerals for building a magic mirror. There are $N$ mines and $M$ different types of minerals.

The mines are connected by a system of railway tracks such that there is a unique path from any mine to a designated "entry" mine. For each mine, we know the available quantity of each of the $M$ minerals.

Some mines are classified as "dangerous". When a railcar full of minerals passes through a dangerous mine, the amount of every mineral in it is halved. Furthermore, each dangerous mine has a danger threshold; the total amount of all minerals entering it throughout the entire process must not exceed this threshold. Dangerous mines themselves contain no minerals, and the entry mine is never dangerous.

To supplement the mined resources, minerals can also be purchased from a shop. For each mineral type, the shop has a limited supply $s_j$ and a specific price per unit $p_j$.

The task is to find a plan to collect minerals from the mines and purchase them from the shop to meet all requirements. The output should be the minimum possible cost to be paid at the shop. If it's impossible to meet the requirements, you should report that.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem asks to minimize a cost, which is a linear function of the amounts bought from the shop. This optimization is subject to several conditions: meeting the required amounts, not exceeding shop supply, and adhering to the danger thresholds in the mines. This structure, minimizing a linear objective under linear constraints, is characteristic of a specific class of optimization problems.

</details>

<details>

<summary>Hint #2</summary>

This problem can be effectively modeled using **Linear Programming (LP)**. The core task is to define variables that represent the quantities of minerals being moved or purchased and then translate the problem's rules into a set of linear equations and inequalities (constraints).

</details>

<details>

<summary>Hint #3</summary>

A crucial simplification is to realize that you don't need a variable for every single mine. The state of the system is entirely determined by the flow of minerals through a small set of "relevant" mines: the **entry mine** and all **dangerous mines**. The flow of minerals from any safe mine is only constrained by the next dangerous mine it encounters on its path to the entry. This significantly reduces the number of variables and constraints in your LP model.

</details>

<details>

<summary>Hint #4</summary>

When modeling the flow through each relevant mine, you need to establish **lower bounds** on the throughput. The key insight is that if minerals flow from dangerous children to a parent mine, the parent must transport at least the sum of what arrives from its children (accounting for the halving effect at dangerous mines). This creates a constraint that ensures flow conservation in your LP model.

</details>

<details>

<summary>Hint #5</summary>

You also need **upper bounds** on the throughput of each relevant mine. Consider two limiting factors: First, a dangerous mine cannot exceed its danger threshold for the total amount of all minerals. Second, you cannot transport more of any mineral than what's actually available in the mine's domain (including resources from non-dangerous mines in its subtree). Both constraints are necessary to ensure a feasible solution.

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

1.  $g_{ij}$: How much of **mineral** $j$ is **transported through the dangerous mine (or root)** $i$
    
    **Note**: Intuitively you want to have a $g_{ij}$ for every Mine $i$. However this leads to too many variables and as we will later see, that, as the dangerous mines pose thresholds, they are the actual important mines to consider.
    
2.  $b_j$: How much of **mineral** $j$ do we need to **buy from the shop**

Our goal is to find the values of these variables that satisfy all constraints while minimizing the total shopping cost.

### LP Constraints
We can now express the problem's rules as a series of linear constraints.

The key observation to define the constraints is that the "**throughput**" of any **relevant mine** (dangerous or root) **can be expressed in terms of its first dangerous children**.
**Note**: With first dangerous children we here mean that along each path, the first dangerous child that is encountered

To determine the throughput $g_{ij}$ of each mine, we will define a **Lower and an Upper Bound on it**.

1.  **Lower Bound:** We observe that the throughput $g_{ij}$ of a node $i$ is **Lower Bounded** by the throughput of its (first) **dangerous children** $c$:

    $$
    \sum_{c} \frac 1 2  g_{cj} \leq g_{ij}
    $$

    If we e.g. transport $3, 2, 1$ from 3 Children, the current node has to at least transport these $6$ minerals. This is why they are a lower bound. As we essentially aggregate everything to only the Dangerous Mines, and at each dangerous mine we lose half of the materials we need to multiple the dangerous children's throughput $g_{cj}$ by $\frac 1 2$

    **Note**: Note that we can find all the first dangerous children by performing a DFS from the mine $i$ and stopping along each path as soon as we found the first dangerous mine (or hit a leaf)

2.  **Upper Bounds:** Additionally we have 2 different **Upper Bounds** on $g_{ij}$:

    *   The **Sum of all transported materials** from relevant mine $i$ **can not exceed its danger threshold $d_i$**:
        
        $$
        \sum_{j = 1} ^m g_{ij} \leq d_i
        $$
        
        We can not transport more than $d_i$ minerals through the mine $i$ as otherwise the dangerous mine would collapse which has to be avoided as stated in the problem
        
    *   The **transported amount for material $j$** can **not exceed the total amount of $j$ available in ALL children (until the first dangerous mine) $c$ of $i$**
        
        $$
        g_{ij} \leq \sum_c g_{cj} 
        $$
        
        Imagine our danger threshold is $d_i = 99999$, but we only have $1$ of mineral $j$ available in all of the children, then we still can only transport $1$, which is why this additional constraint is necessary
        
        **Note**: Here $c$ means ALL children of the node not only the first dangerous ones. This can be obtained during the DFS from the node $i$

3.  **Final Requirement:** For each mineral $j$, the amount extracted from the entry mine ($g_{0j}$) plus the amount bought from the shop ($b_j$) must be at least the required amount $c_j$.
    $$ g_{0j} + b_j \geq c_j $$

4.  **Shop Supply:** The amount of each mineral $j$ bought from the shop cannot exceed the available supply $s_j$.
    $$ 0 \leq b_j \leq s_j $$

5.  **Non-negativity:** All flow variables must be non-negative.
    $$ g_{ij} \geq 0 $$

### Objective Function
We want to minimize the total cost paid at the shop. This is a linear function of our "buy" variables $b_j$ and their corresponding prices $p_j$.
$$ \text{minimize} \quad \sum_{j = 1}^m b_j \cdot s_j $$

### Implementation Note: Variable Indexing
Since we only create variables for a small subset of mines (at most 21: 1 entry + 20 dangerous), we should map their original mine indices to a compact range (e.g., $0, 1, \dots, k$) for the LP solver. This prevents the creation of a large, sparse variable matrix, which would be inefficient. For instance, if mine 999 is dangerous, we wouldn't want the LP solver to implicitly create variables for mines 0 through 998. The code below uses a `std::unordered_map` for this purpose.

### Code
```cpp
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <stack>
#include <vector>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpq.h>

typedef double IT;
typedef CGAL::Gmpq ET;

// program and solution types
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

struct Mineral {
  int required;
  int supply;
  int price;
};

double floor_to_double(const CGAL::Quotient<ET>& x) {
 double a = std::floor(CGAL::to_double(x));
 while (a > x) a -= 1;
 while (a+1 <= x) a += 1;
 return a;
}

void solve() {
  // ===== READ INPUT =====
  int n, m; std::cin >> n >> m;
  
  std::vector<int> danger_levels(n);
  std::vector<bool> is_dangerous(n);
  std::vector<std::vector<int>> available_minerals(n, std::vector<int>(m));
  
  // Read Mines
  for(int i = 0; i < n; i++) {
    std::cin >> danger_levels[i];
    
    if(danger_levels[i] >= 0) {
      is_dangerous[i] = true;
    }
    
    for(int j = 0; j < m; j++) {
      std::cin >> available_minerals[i][j];
    }
  }
  
  // Read Edges
  std::vector<std::vector<int>> edges(n);
  for(int i = 0; i < n-1; i++) {
    int u, v;
    std::cin >> u >> v;
    edges[v].push_back(u);
  }
  
  // Read Minerals
  std::vector<Mineral> minerals(m);
  for(int i = 0; i < m; i++) {
    std::cin >> minerals[i].required >> minerals[i].supply >> minerals[i].price;
  }

  // ===== SOLVE =====
  
  // Create a map from each relevant mine (root and dangerous mines) to the corresponding LP variable indices
  int index = 1;
  std::unordered_map<int, int> index_map;
  for(int i = 0; i < n; i++) {
    if(i == 0 || is_dangerous[i]) {
      index_map[i] = m * index;
      index++;
    }
  }
  
  // Create the LP
  int lp_row = 0;
  Program lp (CGAL::SMALLER, true, 0, false, 0);
  
  for(int i = 0; i < n; i++) {
    // Skip irrelevant nodes
    if(i != 0 && !is_dangerous[i]) continue;
    
    // Find all the children of the current node up using a DFS until either a leaf or a dangerous mine is hit
    std::vector<int> children;
    std::stack<int> stack;
    stack.push(i);
    while(!stack.empty()) {
      int current = stack.top();
      stack.pop();
      for(int child : edges[current]) {
        children.push_back(child);
        if(!is_dangerous[child]) {
          stack.push(child);
        }
      }
    }
    
    // Calculate maximum amount of materials available from the current node
    std::vector<int> total_minerals = available_minerals[i];
    for(int child : children) {
      for(int j = 0; j < m; j++) {
        total_minerals[j] += available_minerals[child][j];
      }
    }
    
    // Define throughput of current mine
    // Lower Bound (lp_row): Sum of the Childrens output (at least as much has to go through the current node as through its children)
    // Upper Bound (lp_row+1): Cant transport more than there is available (total_materials)
    for(int j = 0; j < m; j++) {
      lp.set_a(index_map[i]+j, lp_row, -1);
      lp.set_a(index_map[i]+j, lp_row+1, 1);
      
      // From each dangerous child, only half of what comes in will arrive at the current node.
      // Therefore multiple their throughput by 0.5
      for(int child : children) {
        if(!is_dangerous[child]) continue;
        lp.set_a(index_map[child]+j, lp_row, 0.5);
        lp.set_a(index_map[child]+j, lp_row+1, -0.5);
      }
      lp.set_b(lp_row, 0);
      lp.set_b(lp_row+1, total_minerals[j]);
      
      lp_row += 2;
    }
    
    // Upper Bound the througput of the dangerous mines based on their danger level
    if(i != 0) {
      for(int j = 0; j < m; j++) {
        lp.set_a(index_map[i]+j, lp_row, 1);
      }
      lp.set_b(lp_row, danger_levels[i]);
      lp_row++;
    }
  }
  
  // Put Lower bound on gathered materials (from root and shop) to the required amout
  for(int j = 0; j < m; j++) {
    lp.set_a(j, lp_row, -1); // Minerals from shop
    lp.set_a(index_map[0]+j, lp_row, -1);
    lp.set_b(lp_row, -minerals[j].required);
    lp_row++;
  }
  
  // Define shop constraints and objective function
  for(int j = 0; j < m; j++) {
    lp.set_u(j, true, minerals[j].supply);  // Cant buy more than supply
    lp.set_c(j, minerals[j].price);
  }
  
  Solution s = CGAL::solve_linear_program(lp, ET());
  
  // ===== OUTPUT =====
  if(s.is_infeasible()) {
    std::cout << "Impossible!" << std::endl;
  } else {
    std::cout << (long) floor_to_double(s.objective_value()) << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
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