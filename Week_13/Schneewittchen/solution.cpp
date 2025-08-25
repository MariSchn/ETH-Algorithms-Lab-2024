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