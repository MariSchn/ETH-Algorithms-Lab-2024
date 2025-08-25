///4
#include <iostream>
#include <vector>
#include <limits>

#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef int IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;
typedef Solution::Variable_value_iterator SVI;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;

typedef K::Point_2 Point;

double ceil_to_double(const CGAL::Quotient<ET> &x) {
  double a = std::ceil(CGAL::to_double(x));
  while (a < x) a += 1;
  while (a-1 >= x) a -= 1;
  return a;
}

void solve() {
  // ===== READ INPUT =====
  int n, m, h, w; std::cin >> n >> m >> h >> w;
  
  // Read new nails
  std::vector<Point> new_nails; new_nails.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    new_nails.emplace_back(x, y);
  }
  
  // Read old nails
  std::vector<Point> old_nails; old_nails.reserve(m);
  for(int i = 0; i < m; ++i) {
    int x, y; std::cin >> x >> y;
    old_nails.emplace_back(x, y);
  }
  
  // ===== SOLVE =====
  // Triangulate old nails
  Triangulation t;
  t.insert(old_nails.begin(), old_nails.end());
  
  // Setup Linear Program
  Program lp (CGAL::SMALLER, true, 1, false, 0); 
  int n_constraints = 0;
  
  // Set constraints 
  for(int i = 0; i < n; ++i) {
    const Point p_1 = new_nails[i];
    
    // Set constraints for new nails
    for(int j = i + 1; j < n; ++j) {
      const Point p_2 = new_nails[j];
      
      // Calculate distance in x and y direction
      double x_dist = std::abs(p_1.x() - p_2.x()) / ((double) w);
      double y_dist = std::abs(p_1.y() - p_2.y()) / ((double) h);
      
      // Determine in which dimensions to add constraints
      bool add_x_constraint = false;
      bool add_y_constraint = false;
      if(x_dist < y_dist) { add_y_constraint = true; }
      else if(x_dist > y_dist) { add_x_constraint = true; }
      else { add_x_constraint = add_y_constraint = true; }

      // Add constraints
      if(add_x_constraint) {
        const Point *p_left, *p_right;
        if(p_1.x() < p_2.x()) {
          p_left = &p_1;
          p_right = &p_2;
        } else {
          p_left = &p_2;
          p_right = &p_1;
        }
        
        // Add constraint
        lp.set_a(i, n_constraints, w);
        lp.set_a(j, n_constraints, w);
        lp.set_b(n_constraints, 2 * p_right->x() - 2 * p_left->x());
        
        n_constraints++;
      }
      if(add_y_constraint) {
        const Point *p_bot, *p_top;
        if(p_1.y() < p_2.y()) {
          p_bot = &p_1;
          p_top = &p_2;
        } else {
          p_bot = &p_2;
          p_top = &p_1;
        }
        
        // Add constraint
        lp.set_a(i, n_constraints, h);
        lp.set_a(j, n_constraints, h);
        lp.set_b(n_constraints, 2 * p_top->y() - 2 * p_bot->y());
        
        n_constraints++;
      }
    }
    
    // Set constraint for old nails
    if(m > 0) {
      // Find closes old nail in terms of L1 Norm
      int min_dist = std::numeric_limits<int>::max();
      int min_idx = -1;
      for(int j = 0; j < m; j++) {
        const Point p_2 = old_nails[j];
        
        // Calculate distance in x and y direction
        double x_dist = std::abs(p_1.x() - p_2.x()) / ((double) w);
        double y_dist = std::abs(p_1.y() - p_2.y()) / ((double) h);
        
        if(std::max(x_dist, y_dist) < min_dist) {
          min_dist = std::max(x_dist, y_dist);
          min_idx = j;
        }
      }
      
      // Calculate distance in x and y direction
      const Point p_2 = old_nails[min_idx];
      
      double x_dist = std::abs(p_1.x() - p_2.x()) / ((double) w);
      double y_dist = std::abs(p_1.y() - p_2.y()) / ((double) h);
      
      // Determine in which dimensions to add constraints
      bool add_x_constraint = false;
      bool add_y_constraint = false;
      if(x_dist < y_dist) { add_y_constraint = true; }
      else if(x_dist > y_dist) { add_x_constraint = true; }
      else { add_x_constraint = add_y_constraint = true; }
  
      // Add constraints
      if(add_x_constraint) {
        const Point *p_left, *p_right;
        if(p_1.x() < p_2.x()) {
          p_left = &p_1;
          p_right = &p_2;
        } else {
          p_left = &p_2;
          p_right = &p_1;
        }
        
        // Add constraint
        lp.set_a(i, n_constraints, w);
        lp.set_b(n_constraints, 2 * p_right->x() - 2 * p_left->x() - w);
        
        n_constraints++;
      }
      if(add_y_constraint) {
        const Point *p_bot, *p_top;
        if(p_1.y() < p_2.y()) {
          p_bot = &p_1;
          p_top = &p_2;
        } else {
          p_bot = &p_2;
          p_top = &p_1;
        }
        
        // Add constraint
        lp.set_a(i, n_constraints, h);
        lp.set_b(n_constraints, 2 * p_top->y() - 2 * p_bot->y() - h);
        
        n_constraints++;
      }
    }
  }
  
  // Set objective
  for(int i = 0; i < n; ++i) {
    lp.set_c(i, -2*w - 2*h);
  }
  
  // Solve LP and calculate sum of perimeters
  Solution s = CGAL::solve_linear_program(lp, ET());
  if(!s.is_optimal()) { std::cout << "ERROR: SOLUTION NOT OPTIMAL" << std::endl; return; }
  
  // ===== OUTPUT =====
  std::cout << std::fixed << std::setprecision(0);
  std::cout << ceil_to_double(-s.objective_value()) << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}