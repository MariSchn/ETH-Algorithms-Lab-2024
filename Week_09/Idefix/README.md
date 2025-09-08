# Idéfix and the Mansions of the Gods

## 📝 Problem Description

This problem involves analyzing the connectivity of a collection of circular regions. The coordinates of $n$ potential trees and $m$ buried bones are provided. Each tree, when planted, generates a circular "shadow" centered at its location.

The task is to determine two values:
1.  For a given squared radius value $s$, identify the largest number of distinct bones, $a$, that can be visited during a single continuous walk. A walk is considered continuous if its path remains entirely within the union of all tree shadows.
2.  For a specified target number of bones $k$, find the smallest possible squared radius $q$ required for the trees so that at least $k$ bones can be visited on a single walk.

A set of bones is considered visitable on a single walk if the shadows of the trees covering them form a single, connected region. Two tree shadows are connected if they overlap or touch.

## 💡 Hints

<details>

<summary>Hint #1</summary>

Notice that the problem is fundamentally about proximity: which trees and bones are close enough to interact? This is a classic scenario for using the Delaunay Triangulation, which efficiently encodes nearest-neighbor relationships among points in the plane. Consider how this structure can help you quickly identify which trees are close enough to potentially connect.

</details>

<details>

<summary>Hint #2</summary>

Once you know which trees are close enough to connect, you need to group them into clusters. The Union-Find (Disjoint Set Union) data structure is ideal for efficiently tracking these connected components as you process the connections. How might you use Union-Find to keep track of which trees belong to the same region?

</details>

<details>

<summary>Hint #3</summary>

To answer the first question, focus on a fixed radius. Use the Delaunay Triangulation to find all pairs of trees that are close enough to connect, and Union-Find to group them. Then, for each bone, check if it is covered by any tree and assign it to the corresponding component. The answer is the largest number of bones in any single connected region.

</details>

<details>

<summary>Hint #4</summary>

For the second question, you need to find the minimum radius so that at least $k$ bones are reachable in a single walk. To do this, imagine the trees' shadows gradually expanding. If you process all possible connections in order of increasing distance, you can simulate the growing of the trees. As you go through these edges ascendingly, use Union-Find to merge components and keep track of how many bones are in each. The first time a component reaches $k$ bones, you have found the minimum radius required.

</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Set 1 & 2)</summary>

This solution addresses the first part of the problem: determining the maximum number of bones, $a$, that can be reached when the trees have a fixed radius $r$ (with $s = 4r^2$ given as input).

The key to an efficient approach is to model the connectivity between trees and bones using geometric relationships. Each tree casts a circular shadow of radius $r$, and any bone within this shadow is considered reachable. To efficiently process proximity and connectivity, we use **CGAL** and specifically the **Delaunay Triangulation**.

#### Connectivity Model

Two trees are considered connected if their shadows overlap or touch, which happens when the distance $d$ between their centers is at most $2r$. To avoid computing square roots, we square both sides, yielding $d^2 \le (2r)^2 = 4r^2$. Since $s = 4r^2$, the connectivity condition is simply that the squared distance between two tree centers is less than or equal to $s$. Similarly, a bone is covered if its distance to a tree's center is at most $r$, which translates to $d_{\text{bone}}^2 \le r^2$, or equivalently $4d_{\text{bone}}^2 \le s$.

#### Step-by-Step Algorithm

First, we build a Delaunay triangulation of all tree positions. This efficiently identifies neighboring trees and reduces the number of pairs to consider from $O(n^2)$ to $O(n \log n)$. For each edge in the triangulation, we check if its squared length is at most $s$. If so, the corresponding trees are connected, and we use a **Union-Find data structure** to track connected components among the trees. Each tree starts as its own component, and valid edges merge components.

Next, for each bone, we find its nearest tree using the triangulation's `nearest_vertex` query. If $4d_{\text{bone}}^2 \le s$, the bone is reachable and is assigned to the component of its closest tree. Otherwise, it is ignored. Finally, we count the number of bones assigned to each component, and the answer $a$ is the maximum bone count among all components.

### Code
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

To solve the last 2 Test Sets, we additionally need to calculate $q$ which is the minimum radius such that $k$ bones are reachable. The **general approach will be the same as in the First Solution** (use triangulation, consider edges and use union find to calculate components and determine how many bones are in each component).
**Note**: See First Solution to see how to calculate $a$

We can “simulate” the radius of the trees increasing by** iterating over the edges ascendingly** based on their length.
However, as the shadow of each tree is now **no longer fixed**. Bones **can change** from not being in the shadow to being in the shadow, unlike in the first 2 Test Sets. I.e. a bone that was previously not reachable because it was too far away from the next tree might become reachable after the shadows expand

Therefore we also add the edges between bones and trees to the vector of all edges. Essentially now each **tree and each bone is a component in our Union Find**.
While iterating over the edges ascendingly, we can then check after every `link` operation, if the new component has at least `k` bones.
- If not we continue growing, as the trees are still not big enough
- If yes we have found the exact radius (distance of the “linked” edge) at which `k` bones become reachable, which is exactly what we wanted to calculate.

### Code
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

## 🧠 Learnings

<details> 

<summary> Expand to View </summary>

- Delaunay Triangulation and Union Find are a very common combination that go very well together if you there is some distance constraint on when something is connected and we need to track components.

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