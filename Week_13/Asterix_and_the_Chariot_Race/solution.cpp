#include <iostream>
#include <vector>
#include <limits>

const int MAX_COST = std::numeric_limits<int>::max();

struct Node {
  int idx; // DEBUG
  int raw_cost;
  int take_child_cost;
  int take_parent_cost;
  int take_self_cost;
  Node* parent;
  std::vector<Node*> children;
};

void dfs(Node *root) {
  // ===== BASE CASE =====
  if(root->children.empty()) {
    root->take_self_cost = root->raw_cost;
    root->take_child_cost = MAX_COST;
    root->take_parent_cost = 0;
    
    return;
  }
  
  // ===== RECURSIVE CASE =====
  // Calculate necessary values at children
  for(Node *child : root->children) {
    dfs(child);
  }
  
  // Calculate `take_self_cost`
  root->take_self_cost = root->raw_cost;
  for(Node *child : root->children) {
    root->take_self_cost += child->take_parent_cost;
  }
  
  // Calculate `take_child_cost`
  root->take_child_cost = 0;
  bool found_volunteer = false;;
  for(Node *child : root->children) {
    if(child->take_self_cost <= child->take_child_cost) { found_volunteer = true; }
    root->take_child_cost += std::min(child->take_self_cost, child->take_child_cost);
  }
  
  if(!found_volunteer) {
    root->take_child_cost = MAX_COST;
      
    for(Node *take_child : root->children) {
      int cost = take_child->take_self_cost;
      
      for(Node *other_child : root->children) {
        if(other_child == take_child) { continue; }
        cost += std::min(other_child->take_self_cost, other_child->take_child_cost);
      }
      
      root->take_child_cost = std::min(root->take_child_cost, cost);
    }
  }
  
  // Calculate `take_parent_cost`
  root->take_parent_cost = 0;
  for(Node *child : root->children) {
    root->take_parent_cost += std::min(child->take_self_cost, child->take_child_cost);
  }
  root->take_parent_cost = std::min(root->take_parent_cost, root->take_self_cost);
  
}

void solve() {
  // ===== READ INPUT =====
  int n; std::cin >> n;
  std::vector<Node> nodes(n);
  
  for(int i = 0; i < n - 1; ++i) {
    int u, v; std::cin >> u >> v;
    nodes[u].children.push_back(&nodes[v]);
    nodes[v].parent = &nodes[u];
  }
  for(int i = 0; i < n; ++i) {
    int c; std::cin >> c;
    nodes[i].raw_cost = c;
    nodes[i].idx = i; // DEBUG
  }
  
  // ===== SOLVE =====
  dfs(&nodes[0]);
  
  // ===== OUTPUT =====
  std::cout << std::min(nodes[0].take_self_cost, nodes[0].take_child_cost) << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}