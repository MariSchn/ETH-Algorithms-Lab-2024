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