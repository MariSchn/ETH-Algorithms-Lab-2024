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