#include <iostream>
#include <vector>
#include <algorithm>

struct Node {
  int idx;                // Used for Debugging (set during input reading)
  long n_nodes = 1;       // Number of Nodes in the (sub)tree which has this node as root (set in fill_fields)
  long traverse_time = 0; // Time it takes from the root to traverse the (sub)tree which has this node as root (set in fill_fields)
  long raw_value;         // Initial amount of gold in the chamber/node (set during input reading)
  long value;             // Amount of gold obtained from this (sub)tree, respecting the elapsed time (set in calculate_values)
  std::vector<std::pair<Node*, int>> children;
};

void fill_fields(Node *root) {
  // ===== LEAF NODE =====
  if(root->children.empty()) {
    // Leaf nodes already have the fields set either by default or while reading the input
    return;
  }
  
  // ===== INNER NODE =====
  for(std::pair<Node*, int> child_pair : root->children) {
    Node *child = child_pair.first;
    int distance = child_pair.second;
    
    fill_fields(child);
    
    root->n_nodes += child->n_nodes;
    root->traverse_time += child->traverse_time + distance;
  }
}

void calculate_values(Node *root, long elapsed_time) {
  root->value = root->raw_value - elapsed_time;

  // ===== LEAF NODE =====
  if(root->children.empty()) {
    return;
  }
  
  // ===== INNER NODE =====
  // Sort children based on n_nodes / (traverse_time + distance)
  std::sort(root->children.begin(), root->children.end(), [](const std::pair<Node*, int> &a, const std::pair<Node*, int> &b){
    return b.first->n_nodes * (a.first->traverse_time + a.second) < a.first->n_nodes * (b.first->traverse_time + b.second);
  });
  
  // Calculate value for "root"
  for(std::pair<Node*, int> child_pair : root->children) {
    Node *child = child_pair.first;
    int distance = child_pair.second;
 
    calculate_values(child, elapsed_time + distance);
      
    root->value += child->value;
    elapsed_time += 2 * (child->traverse_time + distance); // * 2 as we need to pass all edges twice once on the way in once on the way out
  }
}


void solve() {
  // ===== READ INPUT =====
  int n; std::cin >> n;
  
  std::vector<Node> nodes(n + 1);
  nodes[0].idx = 0;
  for(int i = 1; i < n + 1; ++i) {
    int g; std::cin >> g;
    nodes[i].raw_value = g;
    nodes[i].idx = i;
  }
  
  for(int i = 0; i < n; ++i) {
    int u, v, l; std::cin >> u >> v >> l;
    nodes[u].children.emplace_back(&nodes[v], l);
  }
  
  // ===== CALCULATE VALUES FOR EACH NODE =====
  fill_fields(&nodes[0]);
  calculate_values(&nodes[0], 0);
  
  // ===== OUTPUT =====
  std::cout << nodes[0].value << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}