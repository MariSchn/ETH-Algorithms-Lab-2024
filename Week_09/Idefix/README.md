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

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef std::size_t                                            Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;

typedef std::tuple<Index,Index,K::FT> Edge;

typedef K::Point_2 Point;
typedef std::pair<Point,Index> IPoint;

std::ostream& operator<<(std::ostream& o, const Edge& e) {
  return o << std::get<0>(e) << " " << std::get<1>(e) << " " << std::get<2>(e);
}

void solve() {
  // ===== READ INPUT =====
  int n, m, k; long s; std::cin >> n >> m >> s >> k;
  
  std::vector<IPoint> trees; trees.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    trees.emplace_back(Point(x, y), i);
  }
  
  std::vector<Point> bones; bones.reserve(m);
  for(int i = 0; i < m; ++i) {
    int x, y; std::cin >> x >> y;
    bones.emplace_back(x, y);
  }
  
  // ===== SOLVE =====
  Delaunay t;
  t.insert(trees.begin(), trees.end());
  
  // Calculate edges with their distances
  std::vector<Edge> edges; edges.reserve(3*n);
  for (auto e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
    Index i1 = e->first->vertex((e->second+1)%3)->info();
    Index i2 = e->first->vertex((e->second+2)%3)->info();
    if(i1 > i2) { std::swap(i1, i2); }
    
    edges.emplace_back(i1, i2, t.segment(e).squared_length());
  }
  
  std::sort(edges.begin(), edges.end(),
    [](const Edge& e1, const Edge& e2) -> bool {
      return std::get<2>(e1) < std::get<2>(e2);
          });
  
  // Determine (connected) components using Union Find
  boost::disjoint_sets_with_storage<> uf(n);
  for (auto e = edges.begin(); e != edges.end(); ++e) {
    Index c1 = uf.find_set(std::get<0>(*e));
    Index c2 = uf.find_set(std::get<1>(*e));
    K::FT dist = std::get<2>(*e);

    if (c1 != c2 && dist <= s) {
      uf.link(c1, c2);
    }
  }
  
  // Find maximum number of bones for radius given by s
  std::vector<int> num_bones(n, 0);
  for(const Point &bone : bones) {
    auto vertex_handle = t.nearest_vertex(bone);
    
    if(4 * CGAL::squared_distance(bone, vertex_handle->point()) > s) { continue; }
     
    Index component = uf.find_set(vertex_handle->info());
    num_bones[component]++;
  }
  
  // Find maximum number of bones among components
  int max_num_bones = 0;
  for(const int &n : num_bones) {
    max_num_bones = std::max(max_num_bones, n);
  }
  
  // ===== OUTPUT =====
  std::cout << max_num_bones << " " << s * 4 << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
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

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <boost/pending/disjoint_sets.hpp>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef std::size_t                                            Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;

typedef std::tuple<Index,Index,K::FT> Edge;

typedef K::Point_2 Point;
typedef std::pair<Point,Index> IPoint;

std::ostream& operator<<(std::ostream& o, const Edge& e) {
  return o << std::get<0>(e) << " " << std::get<1>(e) << " " << std::get<2>(e);
}

void solve() {
  // ===== READ INPUT =====
  int n, m, k; long s; std::cin >> n >> m >> s >> k;
  
  std::vector<IPoint> trees; trees.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    trees.emplace_back(Point(x, y), i);
  }
  
  std::vector<Point> bones; bones.reserve(m);
  for(int i = 0; i < m; ++i) {
    int x, y; std::cin >> x >> y;
    bones.emplace_back(x, y);
  }
  
  // ===== SOLVE =====
  Delaunay t;
  t.insert(trees.begin(), trees.end());
  
  // === Calculate a ===
  
  // Calculate edges with their distances
  std::vector<Edge> edges; edges.reserve(3*n);
  for (auto e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
    Index i1 = e->first->vertex((e->second+1)%3)->info();
    Index i2 = e->first->vertex((e->second+2)%3)->info();
    if(i1 > i2) { std::swap(i1, i2); }
    
    edges.emplace_back(i1, i2, t.segment(e).squared_length());
  }
  
  // Determine (connected) components using Union Find
  boost::disjoint_sets_with_storage<> uf(n);
  for (auto e = edges.begin(); e != edges.end(); ++e) {
    Index c1 = uf.find_set(std::get<0>(*e));
    Index c2 = uf.find_set(std::get<1>(*e));
    K::FT dist = std::get<2>(*e);

    if (c1 != c2 && dist <= s) {
      uf.link(c1, c2);
    }
  }
  
  // Find number of bones for radius given by s
  std::vector<int> num_bones(n, 0);
  for(const Point &bone : bones) {
    auto vertex_handle = t.nearest_vertex(bone);
    if(4 * CGAL::squared_distance(bone, vertex_handle->point()) > s) { continue; }
     
    Index component = uf.find_set(vertex_handle->info());
    num_bones[component]++;
  }
  
  // Find maximum number of bones among components
  int max_num_bones = 0;
  for(const int &n : num_bones) {
    max_num_bones = std::max(max_num_bones, n);
  }
  
  // === Calculate q ===
  // Calculate q by adding the edges between bones and trees and stopping the UF as soon as k bones are reached in one component
  std::vector<int> bones_per_component(n + m, 0);
  for(int i = 0; i < m; ++i) {
    // Find Edge for the bone i
    auto vertex_handle = t.nearest_vertex(bones[i]);
    edges.emplace_back(n + i, vertex_handle->info(), 4 * CGAL::squared_distance(bones[i], vertex_handle->point()));
    bones_per_component[n + i] = 1;
  }
  
  // Sort, now with the edges between bones and trees
  std::sort(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) -> bool {
      return std::get<2>(e1) < std::get<2>(e2);
  });
  
  // Determine (connected) components using Union Find
  boost::disjoint_sets_with_storage<> bones_uf(n + m);
  K::FT q;
  for (auto e = edges.begin(); e != edges.end(); ++e) {
    Index c1 = bones_uf.find_set(std::get<0>(*e));
    Index c2 = bones_uf.find_set(std::get<1>(*e));
    K::FT dist = std::get<2>(*e);

    if (c1 != c2) {
      bones_uf.link(c1, c2);
      Index c3 = bones_uf.find_set(std::get<0>(*e));
      
      int total_bones = bones_per_component[c1] + bones_per_component[c2];
      bones_per_component[c1] = 0;
      bones_per_component[c2] = 0;
      bones_per_component[c3] = total_bones;
      
      if(bones_per_component[c3] >= k) {
        // Found radius at which k bones can be obtained
        q = dist;
        break;
      }
    }
  }
  
  // ===== OUTPUT =====
  std::cout << std::fixed << std::setprecision(0);
  std::cout << max_num_bones << " " << q << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1        (30 pts / 1 s) : Correct answer      (0.157s)
   Test set 2        (30 pts / 1 s) : Correct answer      (0.388s)
   Test set 3        (20 pts / 3 s) : Correct answer      (1.807s)
   Hidden test set 1 (05 pts / 1 s) : Correct answer      (0.142s)
   Hidden test set 2 (05 pts / 1 s) : Correct answer      (0.373s)
   Hidden test set 3 (10 pts / 3 s) : Correct answer      (1.714s)

Total score: 100
```