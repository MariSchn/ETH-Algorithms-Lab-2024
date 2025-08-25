#include <iostream>
#include <vector>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>

typedef CGAL::Gmpq IT;
typedef CGAL::Gmpq ET;

typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

int main() {
  std::ios_base::sync_with_stdio(false);
  
  while(true) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    if(n == 0) break;
    int d; std::cin >> d;
    
    std::vector<std::vector<IT>> A(n, std::vector<IT>(d));
    std::vector<IT> norms(n);
    std::vector<IT> b(n);
    for(int i = 0; i < n; ++i) {
      double norm = 0;
      for(int j = 0; j < d; ++j) {
        std::cin >> A[i][j];
        norm += std::pow(CGAL::to_double(A[i][j]), 2);
      }
      std::cin >> b[i];
    
      norms[i] = std::sqrt(norm);
    }
    
    // ===== CONSTRUCT LINEAR PROGRAM =====
    const int r = d;
    
    Program lp(CGAL::SMALLER, false, 0, false, 0);
    
    for(int i = 0; i < n; ++i) {
      for(int j = 0; j < d; ++j) {
        lp.set_a(j, i, A[i][j]);
      }
      
      lp.set_a(r, i, norms[i]);
      lp.set_b(i, b[i]);
    }
    
    lp.set_l(r, true, 0);
    lp.set_c(r, -1); // Maximize r => Invert objective
    
    // ===== SOLVE AND OUTPUT =====
    Solution s = CGAL::solve_linear_program(lp, ET());
    
    if(s.is_infeasible()) {
      std::cout << "none" << std::endl;
    } else if(s.is_unbounded()) {
      std::cout << "inf" << std::endl;
    } else {
      std::cout << (long) CGAL::to_double(-s.objective_value()) << std::endl;
    }
  }
}