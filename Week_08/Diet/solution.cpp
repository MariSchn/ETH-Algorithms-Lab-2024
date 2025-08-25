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