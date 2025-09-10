# Hermione Granger

## 📝 Problem Description

The problem asks whether it's possible to gather specific amounts of information about three individuals, Malfoy, Crabbe, and Goyle, within a 24-hour timeframe. We are given a total amount of a potion, $f$, to distribute among $d$ members of a group. There are also $s$ students who possess information.

Each of the $s$ students is located at a specific 2D coordinate and, if interrogated, provides information about Malfoy, Crabbe, and Goyle at constant, given rates per hour. Each of the $d$ group members is also at a 2D coordinate and has an associated cost: the amount of potion required for one hour of activity.

When a group member is given a certain amount of the potion, they gain a proportional amount of active time. During this time, they are guided to interrogate their single closest student from the group of $s$ students.

The goal is to determine if an allocation of the total potion $f$ exists for the $d$ group members such that the total information gathered from all interrogated students meets or exceeds the required amounts for Malfoy ($m$), Crabbe ($c$), and Goyle ($g$). No individual interrogation can last longer than 24 hours.

## 💡 Hints

<details>

<summary>Hint #1</summary>

This problem naturally splits into two distinct stages. First, you must determine which Slytherin student each DA member will interrogate. Second, you need to decide how to allocate resources to meet the information requirements. Each stage requires a different technique, can you identify which tools are best suited for each?

</details>

<details>

<summary>Hint #2</summary>

To efficiently assign each DA member to their closest Slytherin, consider using Delaunay Triangulation.

</details>

<details>

<summary>Hint #3</summary>

Once the mapping is established, the problem becomes one of resource allocation under linear constraints. This is a classic scenario for Linear Programming (LP): you need to decide how much time to spend interrogating each Slytherin, subject to potion and time limits, and information requirements.

</details>

<details>

<summary>Hint #4</summary>

The key modeling trick is to let $x_i$ be the number of hours Slytherin student $i$ is interrogated. Other choices, like the amount of potion per DA member or the time per DA member, lead to complicated constraints or slow LPs. Using hours per Slytherin as variables makes the constraints simple and efficient.

</details>

<details>

<summary>Hint #5</summary>

To keep the LP efficient, only create variables for Slytherins who are actually interrogated. If you create a variable for every Slytherin in the input, even those who are never assigned to any DA member, your LP will have many unnecessary variables—this can make it much slower or even infeasible for large inputs. Instead, re-index the variables so that each one corresponds only to a Slytherin who is actually interrogated by at least one DA member. This way, the LP solver only needs to consider the relevant variables, which greatly improves performance and avoids wasting memory and computation on unused variables.

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

This problem can be decomposed into two main parts. First, we have a geometric subproblem: for each DA member, we must identify the Slytherin student they will interrogate. Second, we have a resource allocation subproblem: we must decide how to distribute the Felix Felicis to gather the required information.

### Part 1: Finding the Closest Slytherin

To efficiently determine the closest Slytherin student for each DA member, we construct a **Delaunay triangulation** using the Slytherin students' coordinates. For each DA member's position, we can directly query the triangulation to identify their unique closest Slytherin student. This approach ensures that the mapping from DA members to Slytherins is computed efficiently, even for large input sizes.

### Part 2: Resource Allocation via Linear Programming

Once we have this mapping, the problem becomes one of optimally allocating our resources (Felix Felicis and time) to meet a set of goals (information thresholds). This is a classic scenario for **Linear Programming (LP)**. We need to determine if a *feasible* solution exists that satisfies all constraints.

1.  **Variables:** The most critical step is defining the variables. A natural choice is to let the variable $x_i$ represent the total time (in hours) that Slytherin student $i$ is interrogated. This choice simplifies the formulation of the constraints.

2.  **Cost Simplification:** Multiple DA members might have the same Slytherin as their closest one. For a given Slytherin $i$, if several DA members can interrogate them, it is always optimal to use the DA member with the lowest Felix Felicis cost per hour. Therefore, for each Slytherin $i$ who is the target of at least one DA member, we can determine a single minimum cost, $\text{cost}_i$, required to interrogate them for one hour. Slytherins who are not the closest to any DA member can also be ignored.

3.  **Constraints:** With the variables $x_i$ and costs $\text{cost}_i$ defined, we can formulate the constraints:

    *   **Time Limit:** Each Slytherin can be interrogated for at most 24 hours.
        $0 \le x_i \le 24$ for each active Slytherin $i$.

    *   **Felix Felicis Budget:** The total amount of potion used cannot exceed the available amount, $f$. The potion cost for interrogating Slytherin $i$ for $x_i$ hours is $x_i \cdot \text{cost}_i$. Therefore the constraint to not exceed the budget over all DA students is:
        $$\sum_{i} x_i \cdot \text{cost}_i \le f$$

    *   **Information Requirements:** The total information gathered for each of Malfoy, Crabbe, and Goyle must meet the minimum thresholds $m$, $c$, and $g$. Let $(m_i, c_i, g_i)$ be the information rates for Slytherin $i$.
        *   $\sum_{i} x_i \cdot m_i \ge m$
        *   $\sum_{i} x_i \cdot c_i \ge c$
        *   $\sum_{i} x_i \cdot g_i \ge g$

We are not trying to maximize or minimize any particular value, we only need to know if there exists any set of values for $x_i$ that satisfies all these constraints simultaneously. This is a **feasibility problem**.

#### Implementation Detail: Variable Indexing

This solution is semantically already correct, but depending on how it is implemented it might bee too slow.
What is important is that we do not use the “raw” index of each Slytherin $s \leq 10 ^9$, as this can get up to $10^9$. This is because not all Slytherins are interrogated making some variables $x_i$ unnecessary, but they still impact the run time. Therefore we need to remove these uninterrogated Slytherins.


### Code
```cpp
#include<iostream>
#include<vector>
#include<cmath>
#include<map>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>
#include <iomanip>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_with_info_2<int, K> Vb;
typedef CGAL::Triangulation_data_structure_2<Vb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K, Tds>              Triangulation;

typedef int IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

typedef K::Point_2 Point;
typedef std::pair<Point, int> IPoint;

template<typename T>
std::ostream& operator<<(std::ostream &os, const CGAL::Quadratic_program<T> &lp) {
    os << "minimize:\n";
    bool before = false;
    for(int j=0; j<lp.get_n(); j++) {
        T val = *(lp.get_c()+j);
        if(val != 0) {
            if(before)
                os << " + ";
            os << val << " * x" << j;
            before = true;
        }
    }
    if(lp.get_c0() != 0) {
        os << " + " << lp.get_c0();
    }
    os << std::endl << "where\n";
    for(int i=0; i<lp.get_m(); i++) {
        for(int j=0; j<lp.get_n(); j++) {
            os << std::setw(4) << *(*(lp.get_a()+j)+i);
        }
        os << std::setw(4) << "|";
        auto rel = *(lp.get_r()+i);
        os << std::setw(4) << ((rel == CGAL::SMALLER) ? "<=" : ((rel == CGAL::LARGER) ? ">=" : "==")) << " ";
        os << *(lp.get_b()+i);
        os << std::endl;
    }
    return os;
}

void solve() {
  // std::cout << "======================================================" << std::endl;
  // ===== READ INPUT =====
  int f, m, c, g, d, s; std::cin >> f >> m >> c >> g >> d >> s;
  
  std::vector<IPoint> slytherin_positions; slytherin_positions.reserve(s);
  std::vector<std::vector<int>> info_amounts(s, std::vector<int>(3));
  for(int i = 0; i < s; ++i) {
    int x, y; std::cin >> x >> y >> info_amounts[i][0] >> info_amounts[i][1] >> info_amounts[i][2];
    slytherin_positions.emplace_back(Point(x, y), i);
  }
  
  std::vector<Point> da_positions; da_positions.reserve(d);
  std::vector<int> required_ff(d);
  for(int i = 0; i < d; ++i) {
    int x, y;  std::cin >> x >> y >> required_ff[i];
    da_positions.emplace_back(x, y);
  }
  
  // ===== SOLVE =====
  // === Find nearest Slytherin for each DA member ===
  Triangulation t;
  t.insert(slytherin_positions.begin(), slytherin_positions.end());
  
  // Find smallest amount of FF per hour for each slytherin
  std::map<int, int> slytherin_to_min_ff;
  for(int i = 0; i < d; ++i) {
    int slytherin = t.nearest_vertex(da_positions[i])->info();
    
    if(slytherin_to_min_ff.find(slytherin) == slytherin_to_min_ff.end()) {
      slytherin_to_min_ff[slytherin] = required_ff[i];
    } else {
      slytherin_to_min_ff[slytherin] = std::min(slytherin_to_min_ff[slytherin], required_ff[i]);
    }
  }
  
  // === Construct LP ===
  // Variables: x_i time (hours) Slytherin i is interrogated for
  // Minimum: 0 Maximum: 24
  Program lp (CGAL::SMALLER, true, 0, true, 24); 
  
  // Set constraints
  int var_idx = 0;
  for(auto iter = slytherin_to_min_ff.begin(); iter != slytherin_to_min_ff.end(); ++iter) {
    int slytherin = iter->first;
    int ff = iter->second;
    
    // Constraint that amount of distributed FF does not exceed amount of available FF
    lp.set_a(var_idx, 0, ff);
    
    // Constraint that enough information is acquired about all 3 targets
    lp.set_a(var_idx, 1, -info_amounts[slytherin][0]);
    lp.set_a(var_idx, 2, -info_amounts[slytherin][1]);
    lp.set_a(var_idx, 3, -info_amounts[slytherin][2]);
    
    var_idx++;
  }
  lp.set_b(0, f);
  lp.set_b(1, -m);
  lp.set_b(2, -c);
  lp.set_b(3, -g);
  
  // === SOLVE LP ===
  Solution solution = CGAL::solve_linear_program(lp, ET());
  
  // === OUTPUT ===
  if (solution.is_infeasible()) {
    std::cout << "H" << std::endl;
  } else {
    std::cout << "L" << std::endl;
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

- CGAL creates all intermediate variables, e.g. if you create variable 1000 but the previous highest you had was 100, all 900 variables in between are also created, greaetly impacting run time.

</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1        (30 pts / 1 s) : Correct answer      (0.157s)
   Test set 2        (30 pts / 1 s) : Correct answer      (0.388s)
   Test set 3        (20 pts / 3 s) : Correct answer      (1.807s)
   Hidden test set 1 (05 pts / 1 s) : Correct answer      (0.142s)
   Hidden test set 2 (05 pts / 1 s) : Correct answer      (0.373s)
   Hidden test set 3 (10 pts / 3 s) : Correct answer      (1.714s)

Total score: 100
```