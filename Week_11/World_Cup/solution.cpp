#include <iostream>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/QP_models.h>
#include <CGAL/QP_functions.h>
#include <CGAL/Gmpz.h>

#include <CGAL/Delaunay_triangulation_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;

typedef K::Point_2 Point;
typedef K::Segment_2 Segment;
typedef K::Circle_2 Circle;

typedef int IT;
typedef CGAL::Gmpz ET;
typedef CGAL::Quadratic_program<IT> Program;
typedef CGAL::Quadratic_program_solution<ET> Solution;

#define ASSERT(c) do { if (!(c)) throw std::runtime_error("Assertion failed: " #c); } while (0)

struct Warehouse {
  Point pos;
  int s;  // Supply of Beer in ml
  int a;  // Alcohol per Liter of Beer in ml
  
  Warehouse(int x, int y, int s, int a) : pos(x, y), s(s * 1000), a(a * 10) {}
};

struct Stadium {
  Point pos;
  int d;  // Demand of Beer in ml
  int u;  // Upper Limit on the pure alcohol in ml
  
  Stadium(int x, int y, int d, int u) : pos(x, y), d(d * 1000), u(u * 1000) {}
};

long floor_to_double(const CGAL::Quotient<ET>& x) {
  double a = std::floor(CGAL::to_double(x));
  while (a > x) a -= 1;
  while (a+1 <= x) a += 1;
  return a;
}

void solve() {
  // ===== READ INPUT =====
  int n, m, c; std::cin >> n >> m >> c;
  
  std::vector<Point> points; points.reserve(n + m);
  std::vector<Warehouse> warehouses; warehouses.reserve(n);
  std::vector<Stadium> stadiums; stadiums.reserve(m);
  std::vector<Circle> contour_lines; contour_lines.reserve(c); 
  std::vector<std::vector<int>> revenues(n, std::vector<int>(m)); 
  
  // Read warehouses
  for(int i = 0; i < n; ++i) {
    int x, y, s, a; std::cin >> x >> y >> s >> a;
    warehouses.emplace_back(x, y, s, a);
    points.emplace_back(x, y);
  }
  
  // Read stadiums
  for(int i = 0; i < m; ++i) {
    int x, y, d, u; std::cin >> x >> y >> d >> u;
    stadiums.emplace_back(x, y, d, u);
    points.emplace_back(x, y);
  }
  
  // Read revenues
  for(int i = 0; i < n; ++i) {
    for(int j = 0; j < m; ++j) {
      std::cin >> revenues[i][j];
    }
  }
  
  // Read contour lines
  for(int i = 0; i < c; ++i) {
    int x, y, r; std::cin >> x >> y >> r;
    contour_lines.emplace_back(Point(x, y), std::pow(r, 2));
  }
  
  // ===== COUNT CONTOUR LINE INTERSECTIONS =====
  // Triangulate warehouses and stadiums
  
  Triangulation t;
  t.insert(points.begin(), points.end());
  
  // Find the contour lines that are actually relevant (have something inside)
  std::vector<Circle*> relevant_contours; relevant_contours.reserve(100);
  for(int i = 0; i < c; ++i) {
    Point center = contour_lines[i].center();
    Point nearest = t.nearest_vertex(center)->point();
    
    if(CGAL::squared_distance(center, nearest) <= contour_lines[i].squared_radius()) {
      relevant_contours.push_back(&contour_lines[i]);
    }
  }
  
  // Count intersections between warehouses and stadiums with contour lines
  std::vector<std::vector<int>> n_intersections(n, std::vector<int>(m, 0));
  for(int w = 0; w < n; ++w) {
    for(int s = 0; s < m; ++s) {
      for(const Circle *contour : relevant_contours) {
        if(contour->has_on_bounded_side(warehouses[w].pos) != contour->has_on_bounded_side(stadiums[s].pos)) {
          n_intersections[w][s]++;
        }
      }
    }
  }
  
  // ===== CONSTRUCT LINEAR PROGRAM =====
  // Variables are numbered by warehouse_idx * n + stadium_idx (w * m + s)
  Program lp(CGAL::SMALLER, true, 0, false, 0); 
  int n_constraints = 0;
  
  // Define Objective Function
  for(int w = 0; w < n; ++w) {
    for(int s = 0; s < m; ++s) {
      lp.set_c(w * m + s, -(100 * revenues[w][s] - n_intersections[w][s]));
    }
  }
  
  // Add constraints to ensure that the amount of Beer sent does not exceed supply
  for(int w = 0; w < n; ++w) {
    for(int s = 0; s < m; ++s) {
      lp.set_a(w * m + s, n_constraints, 1000);
    }
    lp.set_b(n_constraints, warehouses[w].s);
    
    n_constraints += 1;
  }
  
  // Add constraints that the pure alocohol for each stadium does not exceed upper limit and
  // Add constraints to ensure the the Beer delivered is equal to the demand for each stadium
  for(int s = 0; s < m; ++s) {
    for(int w = 0; w < n; ++w) {
      // Constraint to respect Alcohol limit
      lp.set_a(w * m + s, n_constraints, warehouses[w].a);
      
      // Constraints to exactly meet Beer demand
      lp.set_a(w * m + s, n_constraints + 1, 1000);
      lp.set_a(w * m + s, n_constraints + 2, -1000);
    }
    lp.set_b(n_constraints, stadiums[s].u);
    lp.set_b(n_constraints + 1, stadiums[s].d);
    lp.set_b(n_constraints + 2, -stadiums[s].d);
    
    n_constraints += 3;
  }
  
  // ===== SOLVE LP AND OUTPUT =====
  Solution s = CGAL::solve_linear_program(lp, ET());
  
  std::cout << std::fixed << std::setprecision(0);

  if (s.is_infeasible()) { 
    std::cout << "no" << std::endl;
  } else {
    std::cout << floor_to_double(-s.objective_value() / 100) << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}