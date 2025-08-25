# Hermione Granger

## 📝 Problem Description

The problem asks whether it's possible to gather specific amounts of information about three individuals—Malfoy, Crabbe, and Goyle—within a 24-hour timeframe. We are given a total amount of a potion, $f$, to distribute among $d$ members of a group. There are also $s$ students who possess information.

Each of the $s$ students is located at a specific 2D coordinate and, if interrogated, provides information about Malfoy, Crabbe, and Goyle at constant, given rates per hour. Each of the $d$ group members is also at a 2D coordinate and has an associated cost: the amount of potion required for one hour of activity.

When a group member is given a certain amount of the potion, they gain a proportional amount of active time. During this time, they are guided to interrogate their single closest student from the group of $s$ students.

The goal is to determine if an allocation of the total potion $f$ exists for the $d$ group members such that the total information gathered from all interrogated students meets or exceeds the required amounts for Malfoy ($m$), Crabbe ($c$), and Goyle ($g$). No individual interrogation can last longer than 24 hours. If a valid allocation exists, the output is "L"; otherwise, it is "H".

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem states that each DA member is guided towards their *closest* Slytherin student. Before considering the resource allocation, how can you efficiently determine this mapping for every DA member? This is a classic computational geometry problem.
</details>
<details>
<summary>Hint #2</summary>
The task of finding the nearest neighbor for a set of query points among a static set of sites can be solved efficiently. A Voronoi diagram partitions the plane into regions based on the closest site. The dual of the Voronoi diagram, a **Delaunay triangulation**, is a powerful structure often used in libraries like CGAL to answer nearest neighbor queries. Once this mapping is established, the problem changes. What happens if multiple DA members are closest to the same Slytherin?
</details>
<details>
<summary>Hint #3</summary>
After determining which Slytherin each DA member can interrogate, the problem becomes one of resource allocation. You have a budget of Felix Felicis ($f$) and a time limit (24 hours). You must satisfy three minimum information requirements. This setup, involving allocating resources subject to linear constraints, is a strong indicator that **Linear Programming (LP)** is a suitable framework. Think about what your decision variables should be. A good choice of variables will make the constraints easy to express.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
This problem can be decomposed into two main parts. First, we have a geometric subproblem: for each DA member, we must identify the Slytherin student they will interrogate. Second, we have a resource allocation subproblem: we must decide how to distribute the Felix Felicis to gather the required information.

### Part 1: Finding the Closest Slytherin

The problem states that each DA member is guided to their unique closest Slytherin student. This is a nearest neighbor problem. Given the large number of DA members and Slytherin students, a naive approach of calculating all pairwise distances would be too slow.

A more efficient method is to use a **Delaunay triangulation**. By constructing a Delaunay triangulation on the set of Slytherin student locations, we create a data structure that allows for fast nearest neighbor queries. For each DA member's location, we can query the triangulation to find the closest Slytherin vertex in logarithmic time on average. This gives us a mapping from each DA member to a specific Slytherin student.

### Part 2: Resource Allocation via Linear Programming

Once we have this mapping, the problem becomes one of optimally allocating our resources (Felix Felicis and time) to meet a set of goals (information thresholds). This is a classic scenario for **Linear Programming (LP)**. We need to determine if a *feasible* solution exists that satisfies all constraints.

#### LP Formulation

1.  **Decision Variables:** The most critical step is defining the variables. A natural choice is to let the variable $x_i$ represent the total time (in hours) that Slytherin student $i$ is interrogated. This choice simplifies the formulation of the constraints. Other intuitive choices, like the amount of potion each DA member receives, can lead to non-linear constraints or more complex models.

2.  **Cost Simplification:** Multiple DA members might have the same Slytherin as their closest one. For a given Slytherin $i$, if several DA members can interrogate them, it is always optimal to use the DA member with the lowest Felix Felicis cost per hour. Therefore, for each Slytherin $i$ who is the target of at least one DA member, we can determine a single minimum cost, $\text{cost}_i$, required to interrogate them for one hour. Slytherins who are not the closest to any DA member can be ignored.

3.  **Constraints:** With the variables $x_i$ and costs $\text{cost}_i$ defined, we can formulate the constraints:

    *   **Time Limit:** Each Slytherin can be interrogated for at most 24 hours.
        $0 \le x_i \le 24$ for each active Slytherin $i$.

    *   **Felix Felicis Budget:** The total amount of potion used cannot exceed the available amount, $f$. The potion cost for interrogating Slytherin $i$ for $x_i$ hours is $x_i \cdot \text{cost}_i$.
        $\sum_{i} x_i \cdot \text{cost}_i \le f$

    *   **Information Requirements:** The total information gathered for each of Malfoy, Crabbe, and Goyle must meet the minimum thresholds $m$, $c$, and $g$. Let $(m_i, c_i, g_i)$ be the information rates for Slytherin $i$.
        *   $\sum_{i} x_i \cdot m_i \ge m$
        *   $\sum_{i} x_i \cdot c_i \ge c$
        *   $\sum_{i} x_i \cdot g_i \ge g$

We are not trying to maximize or minimize any particular value; we only need to know if there exists any set of values for $x_i$ that satisfies all these constraints simultaneously. This is a **feasibility problem**. The LP solver will tell us if such a solution exists.

#### Implementation Detail: Variable Indexing

A potential performance issue arises if the input indices for Slytherins are large and sparse (e.g., only Slytherins with indices 0 and $10^9$ are relevant). An LP solver might create variables for all indices in between, leading to a huge, slow model. To avoid this, we only create LP variables for the Slytherins that are actually targeted by at least one DA member. We can use a map or re-indexing scheme to map the original Slytherin indices to a compact set of LP variable indices (e.g., $0, 1, 2, \dots$).

The following C++ code implements this logic using the CGAL library for both the Delaunay triangulation and the LP solver.

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