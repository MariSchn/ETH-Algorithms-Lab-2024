# Idéfix and the Mansions of the Gods

## 📝 Problem Description

This problem asks us to analyze the connectivity of a set of circular regions. We are given the coordinates of $n$ potential trees and $m$ buried bones. Each tree, when planted, creates a circular "shadow" centered at its location.

We need to determine two values:
1.  Given a standard squared radius value $s$, which corresponds to a radius $r$ such that $s = 4r^2$, what is the largest number of distinct bones, $a$, that can be visited on a single continuous walk? A walk is considered continuous if its path remains entirely within the union of all tree shadows.
2.  Given a target number of bones $k$, what is the smallest possible squared radius $q$ that the trees would need to have, such that it becomes possible to visit at least $k$ bones on a single walk? This radius, let's call it $b$, relates to $q$ by $q = 4b^2$.

A set of bones can be visited on a single walk if the shadows of the trees that cover them form a single, connected region. Two tree shadows are connected if they overlap or touch.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The core of the problem lies in understanding when Idéfix can travel from one tree's shadow to another. For a path to exist between the shadows of tree A and tree B, their circular regions must form a single connected area. This happens if the circles touch or overlap. What does this condition imply about the distance between the centers of the two trees in relation to their radius?
</details>
<details>
<summary>Hint #2</summary>
This problem can be effectively modeled by considering trees as vertices in a graph. An edge exists between two vertices if their corresponding tree shadows overlap. The problem of finding which bones can be visited on a single walk is then equivalent to finding connected components in this graph and counting the bones associated with each component. A Union-Find data structure is an excellent tool for efficiently tracking these components as you discover connections.
</details>
<details>
<summary>Hint #3</summary>
To find the smallest radius $b$ (and thus $q=4b^2$), observe that the number of connected bones is a monotonic function of the radius: as the radius increases, more connections are formed, and more bones become reachable. This structure suggests that we don't need to test every possible radius. Instead, we can identify all the discrete "connection events" — either two trees becoming connected or a bone becoming covered by a tree. Each event happens at a specific critical radius. If we process these events in increasing order of their critical radius, the first time we satisfy the condition of reaching $k$ bones, we will have found the minimum required radius.
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1 & 2)</summary>

This approach solves the first part of the problem: finding the maximum number of bones, $a$, that can be reached when trees have a fixed radius $r$ (derived from the input $s$).

### Modeling Connectivity

The problem can be modeled as finding connected components in a graph where trees are vertices. Two trees are considered connected if their circular shadows, each with radius $r$, overlap or touch. This occurs if the distance $d$ between their centers is at most $2r$. Squaring this inequality gives us the condition $d^2 \le (2r)^2 = 4r^2$. Since the problem provides $s = 4r^2$, the connectivity condition simplifies to checking if the **squared distance** between two tree centers is less than or equal to $s$.

A bone is "covered" if it lies within a tree's shadow. This means its distance $d_{\text{bone}}$ to the tree's center is at most $r$. Squaring this gives $d_{\text{bone}}^2 \le r^2$, which is equivalent to $4d_{\text{bone}}^2 \le 4r^2 = s$.

### Algorithm

1.  **Finding Nearby Trees Efficiently:** A naive check of all $O(n^2)$ pairs of trees to see if they are connected would be too slow for the full constraints. A standard optimization for geometric proximity problems is to use a **Delaunay Triangulation**. This structure connects points to their nearest neighbors and provides a much smaller set of candidate edges to check for connectivity. We build a Delaunay triangulation on the $n$ tree locations.

2.  **Building Components with Union-Find:** We use a **Union-Find** data structure to efficiently group trees into connected components. Initially, each of the $n$ trees is in its own component. We then iterate through all edges of the Delaunay triangulation. If an edge's squared length is less than or equal to $s$, it represents a valid connection, and we merge the components of the two trees it connects using the `union` operation.

3.  **Assigning Bones to Components:** After all components for the given radius $r$ are formed, we determine how many bones belong to each. For every bone, we must find which component (if any) covers it.
    - We use the `nearest_vertex` query from the CGAL triangulation data structure to find the closest tree to each bone.
    - We then check if the bone is actually inside that tree's shadow using the condition $4 \cdot d_{\text{bone}}^2 \le s$.
    - If it is, we find the representative of that tree's component (using the `find` operation of Union-Find) and increment a counter for that component. If the condition is not met, the bone is not covered by any shadow and is ignored.

4.  **Finding the Maximum:** Finally, we iterate through the bone counts for all components and find the maximum value. This is our answer, $a$. The second part of the output, $q$, is trivially set to $4s$ for these test sets as per the problem statement's assumptions.

### C++ Implementation
```cpp
#include <iostream>
#include <vector>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <boost/pending/disjoint_sets.hpp>

// CGAL type definitions
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef std::size_t                                         Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>  Vb;
typedef CGAL::Triangulation_face_base_2<K>                    Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>           Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                 Delaunay;

// Custom types for points and edges
typedef K::Point_2 Point;
typedef std::pair<Point,Index> IPoint;
typedef std::tuple<Index,Index,K::FT> Edge;

void solve() {
  // Read input
  int n, m, k; long s;
  std::cin >> n >> m >> s >> k;
  
  std::vector<IPoint> trees;
  trees.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    trees.emplace_back(Point(x, y), i);
  }
  
  std::vector<Point> bones;
  bones.reserve(m);
  for(int i = 0; i < m; ++i) {
    int x, y; std::cin >> x >> y;
    bones.emplace_back(x, y);
  }
  
  // Build Delaunay triangulation of tree locations
  Delaunay t;
  t.insert(trees.begin(), trees.end());
  
  // Extract edges from the triangulation
  std::vector<Edge> edges;
  edges.reserve(3*n); // An approximation for the number of edges
  for (auto e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
    Index i1 = e->first->vertex((e->second+1)%3)->info();
    Index i2 = e->first->vertex((e->second+2)%3)->info();
    if(i1 > i2) std::swap(i1, i2); // Avoid duplicate edges
    edges.emplace_back(i1, i2, t.segment(e).squared_length());
  }
 
  // Build connected components using Union-Find for the given radius s
  boost::disjoint_sets_with_storage<> uf(n);
  for (const auto& e : edges) {
    if (std::get<2>(e) <= s) {
      uf.union_set(std::get<0>(e), std::get<1>(e));
    }
  }
  
  // Count bones per component
  std::vector<int> num_bones(n, 0);
  for(const Point &bone : bones) {
    auto vh = t.nearest_vertex(bone);
    // Check if bone is within the shadow: 4 * dist^2 <= s
    if (4 * CGAL::squared_distance(bone, vh->point()) <= s) {
      Index component_root = uf.find_set(vh->info());
      num_bones[component_root]++;
    }
  }
  
  // Find the component with the maximum number of bones
  int max_num_bones = 0;
  for(int i = 0; i < n; ++i) {
    if (uf.find_set(i) == i) { // Iterate over component representatives
        max_num_bones = std::max(max_num_bones, num_bones[i]);
    }
  }
  
  // Output result (for test sets 1 & 2, q is fixed)
  std::cout << max_num_bones << " " << s * 4 << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
  return 0;
}
```
</details>

<details>
<summary>Final Solution</summary>

This solution solves both parts of the problem: calculating the maximum bones $a$ for a fixed radius and finding the minimum squared radius $q$ to connect at least $k$ bones. The calculation of $a$ is identical to the first solution. The main challenge is finding $q$.

### Finding the Minimum Radius `q`

The core insight is that the number of connected bones only increases at specific, discrete values of the radius. These "events" occur when two components merge or a bone becomes part of a component. We can find the minimum required radius by simulating the process of a gradually increasing radius.

#### Event-Based Simulation

There are two types of events, each occurring at a critical squared radius $q = 4b^2$:

1.  **Tree-Tree Connection:** Two trees $T_i$ and $T_j$ become connected when their shadows first touch. This happens when the radius $b = \text{dist}(T_i, T_j) / 2$. The critical squared radius is therefore $q = (\text{dist}(T_i, T_j))^2$.
2.  **Bone-Tree Inclusion:** A bone $B$ is first covered by a tree $T$ when the radius $b = \text{dist}(B, T)$. The critical squared radius for this event is $q = 4 \cdot (\text{dist}(B, T))^2$.

#### Algorithm

1.  **Gather All Potential Events:** We create a list of all possible connection events, represented as edges with an associated cost (the critical $q$ value).
    -   **Tree-Tree Edges:** We extract all edges from the Delaunay triangulation of the trees. The cost of each edge is its squared length.
    -   **Bone-Tree Edges:** For each bone, we find its nearest tree (using `nearest_vertex`). The connection to this nearest tree will always be the first one to occur as the radius grows. The cost for this event is $4$ times the squared distance from the bone to its nearest tree.

2.  **Sort Events:** We combine both types of edges into a single list and sort it in ascending order based on their cost.

3.  **Simulate with Union-Find:** We process the sorted edges one by one, effectively simulating an increasing radius.
    - We use a Union-Find data structure on $n+m$ elements, where indices `0` to `n-1` represent trees and `n` to `n+m-1` represent bones.
    - We also maintain an array to track the number of bones in each component. Initially, each tree component has 0 bones, and each bone component (indices `n` to `n+m-1`) has 1 bone.
    - We iterate through the sorted edges. For each edge, we merge the components of the two entities it connects (tree-tree, tree-bone). When merging, we also sum their bone counts.
    - After each merge, we check if the newly formed component's bone count has reached or exceeded $k$.

4.  **Determine `q`:** The first time a component's bone count reaches at least $k$, the cost of the edge that caused this merge is our desired minimum squared radius, $q$. We can stop the simulation at this point and report this value.

### C++ Implementation
```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <boost/pending/disjoint_sets.hpp>

// CGAL type definitions
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef std::size_t                                         Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>  Vb;
typedef CGAL::Triangulation_face_base_2<K>                    Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>           Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                 Delaunay;

// Custom types for points and edges
typedef K::Point_2 Point;
typedef std::pair<Point,Index> IPoint;
typedef std::tuple<Index,Index,K::FT> Edge;

void solve() {
  // Read input
  int n, m, k; long s_long;
  std::cin >> n >> m >> s_long >> k;
  K::FT s = s_long;
  
  std::vector<IPoint> trees;
  trees.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    trees.emplace_back(Point(x, y), i);
  }
  
  std::vector<Point> bones;
  bones.reserve(m);
  for(int i = 0; i < m; ++i) {
    int x, y; std::cin >> x >> y;
    bones.emplace_back(x, y);
  }
  
  // Build Delaunay triangulation
  Delaunay t;
  t.insert(trees.begin(), trees.end());
  
  // === Part 1: Calculate a ===
  
  std::vector<Edge> tree_edges;
  tree_edges.reserve(3*n);
  for (auto e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
    Index i1 = e->first->vertex((e->second+1)%3)->info();
    Index i2 = e->first->vertex((e->second+2)%3)->info();
    tree_edges.emplace_back(i1, i2, t.segment(e).squared_length());
  }
  
  boost::disjoint_sets_with_storage<> uf_a(n);
  for (const auto& e : tree_edges) {
    if (std::get<2>(e) <= s) {
      uf_a.union_set(std::get<0>(e), std::get<1>(e));
    }
  }
  
  std::vector<int> bones_in_comp_a(n, 0);
  for(const Point &bone : bones) {
    auto vh = t.nearest_vertex(bone);
    if (4 * CGAL::squared_distance(bone, vh->point()) <= s) {
      bones_in_comp_a[uf_a.find_set(vh->info())]++;
    }
  }
  
  int max_num_bones = 0;
  for (int count : bones_in_comp_a) {
    max_num_bones = std::max(max_num_bones, count);
  }
  
  // === Part 2: Calculate q ===
  
  std::vector<Edge> all_events = tree_edges;
  all_events.reserve(3*n + m);
  for(int i = 0; i < m; ++i) {
    auto vh = t.nearest_vertex(bones[i]);
    // Event: bone i gets covered by tree vh->info()
    // Index for bone i is n+i
    // Cost is 4 * dist^2
    all_events.emplace_back(n + i, vh->info(), 4 * CGAL::squared_distance(bones[i], vh->point()));
  }
  
  std::sort(all_events.begin(), all_events.end(), [](const Edge& e1, const Edge& e2) {
      return std::get<2>(e1) < std::get<2>(e2);
  });
  
  boost::disjoint_sets_with_storage<> uf_q(n + m);
  std::vector<int> bones_in_comp_q(n + m, 0);
  for (int i = 0; i < m; ++i) {
    bones_in_comp_q[n + i] = 1; // Each bone is a component with 1 bone
  }

  K::FT q = -1;
  // Special case: if k=0, q=0. If k=1 and there is at least one bone, find the smallest bone-tree distance.
  if (k <= max_num_bones) { // Check if k is already achievable with radius r
     // This part is tricky. We need to find the minimum radius to achieve k bones.
     // The loop below will find it correctly.
  }
  
  for (const auto& e : all_events) {
    Index c1 = uf_q.find_set(std::get<0>(e));
    Index c2 = uf_q.find_set(std::get<1>(e));
    K::FT cost = std::get<2>(e);

    if (c1 != c2) {
      int merged_bones = bones_in_comp_q[c1] + bones_in_comp_q[c2];
      uf_q.link(c1, c2);
      Index new_root = uf_q.find_set(c1);
      bones_in_comp_q[c1] = bones_in_comp_q[c2] = 0; // Clear old counts
      bones_in_comp_q[new_root] = merged_bones;
      
      if(merged_bones >= k) {
        q = cost;
        break;
      }
    }
  }
  
  // Output result
  std::cout << std::fixed << std::setprecision(0);
  std::cout << max_num_bones << " " << q << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
  return 0;
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1        (20 pts / 1 s) : Correct answer      (0.018s)
   Hidden test set 1 (05 pts / 1 s) : Correct answer      (0.018s)
   Test set 2        (20 pts / 1 s) : Correct answer      (0.085s)
   Hidden test set 2 (05 pts / 1 s) : Correct answer      (0.085s)
   Test set 3        (20 pts / 1 s) : Correct answer      (0.033s)
   Hidden test set 3 (05 pts / 1 s) : Correct answer      (0.033s)
   Test set 4        (20 pts / 1 s) : Correct answer      (0.37s)
   Hidden test set 4 (05 pts / 1 s) : Correct answer      (0.143s)

Total score: 100
```