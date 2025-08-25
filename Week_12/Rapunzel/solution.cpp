#include <iostream>
#include <vector>
#include <set>
#include <deque>

struct Node {
  int idx;
  int brightness;
  Node* parent = nullptr;
  std::vector<Node*> children;
};

int calculateContrast(const std::multiset<int> &brightnesses) {
  // Note: multisets are sorted, so first *rbegin() is max and *begin() is min
  return *brightnesses.rbegin() - *brightnesses.begin();
}

void dfs(Node *curr, std::deque<Node*> &path, std::multiset<int> &brightnesses, std::vector<bool> &result, const size_t m, const int k) {
  // Add current node to path and to the brightnesses
  path.push_back(curr);
  brightnesses.insert(curr->brightness);
  
  // Check if the path is too long and remove the first element if it is too long
  // As we pop as soon as the path reaches m+1, it is not necessary to pop multiple times
  if(path.size() > m) {
    Node *first_node = path[0];
    path.pop_front();
    brightnesses.erase(brightnesses.find(first_node->brightness));
  }

  // Check if the current path is valid
  if(path.size() == m) {
    int contrast = calculateContrast(brightnesses);
    
    // Check if contrast is low enough
    if (contrast <= k) {
      // Mark the current start node as valid
      result[path[0]->idx] = true;
    }
  }

  // Recurse into children
  for(Node *child : curr->children) {
    dfs(child, path, brightnesses, result, m, k);
  }

  // Finished with this node -> Remove it from path and its brightness
  path.pop_back();
  brightnesses.erase(brightnesses.find(curr->brightness));


  // Restore the first element of the path, we removed because the path was too long
  // If the first node in the path is the root node, there is no need to do this as the path has not gotten to long
  if(path[0]->parent != nullptr) {
    Node *first_node = path[0]->parent;
    path.push_front(first_node);
    brightnesses.insert(first_node->brightness);
  }
}

void solve() {
  // ===== READ INPUT =====
  int n, m, k; std::cin >> n >> m >> k;
  
  std::vector<Node> nodes(n);
  for(int i = 0; i < n; ++i) { 
    std::cin >> nodes[i].brightness; 
    nodes[i].idx = i;
  }
  for(int i = 0; i < n - 1; ++i) {
    int u, v; std::cin >> u >> v;
    nodes[u].children.push_back(&nodes[v]);
    nodes[v].parent = &nodes[u];
  }
  
  // ===== SOLVE =====
  std::deque<Node*> path;
  std::multiset<int> brightnesses;
  std::vector<bool> result(n, false);

  dfs(&nodes[0], path, brightnesses, result, m, k);
  
  // ===== OUTPUT =====
  int n_outputs = 0;
  
  for(int i = 0; i < n; ++i) {
    if(result[i]) { 
      std::cout << i << " "; 
      n_outputs++;
    }
  }
  
  if(n_outputs == 0) { std::cout << "Abort mission"; }
  std::cout << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}