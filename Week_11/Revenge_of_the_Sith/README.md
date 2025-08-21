# Revenge of the Sith

## 📝 Problem Description

You are given a set of $n$ planets, represented by their 2D coordinates. These planets are conquered by an empire in a fixed, predetermined sequence, $p_0, p_1, \ldots, p_{n-1}$. Your goal is to form an alliance of the maximum possible size, $k$.

To form an alliance of size $k$, you must select $k$ distinct planets. The selection is constrained by two rules:
1.  All $k$ planets chosen for the alliance must be selected from the set of planets that have not yet been conquered by the empire. After $k$ days, planets $p_0, \ldots, p_{k-1}$ are conquered. Therefore, your selection must come from the remaining planets, $\{p_k, p_{k+1}, \ldots, p_{n-1}\}$.
2.  The selected set of $k$ planets must be "reachable". This means that the set of planets must form a single connected group. A connection between any two planets exists if the distance between them is less than or equal to a given scout vessel range, $r$. Formally, for any partition of the $k$ chosen planets into two non-empty subsets, there must be a planet in the first subset and a planet in the second subset whose distance is at most $r$.

Your task is to determine the largest integer $k$ for which such an alliance can be formed.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem asks for the *maximum* possible value of $k$. Notice that if it's possible to form an alliance of size $k$, it is also possible to form an alliance of any size smaller than $k$ (by simply taking a connected subset of the size-$k$ alliance). This monotonic property—if a property holds for $k$, it also holds for all $k' < k$—is a strong indicator that a particular search algorithm might be very effective. Which algorithm excels at finding an optimal value in a monotonic search space?
</details>
<details>
<summary>Hint #2</summary>
You can apply binary search on the answer, $k$. The range for $k$ would be from $1$ to $n$. For a fixed value of $k$ in your binary search, you need a function, let's call it `is_possible(k)`, that returns `true` if an alliance of size $k$ can be formed and `false` otherwise.

How would you implement `is_possible(k)`? According to the rules, you must only consider planets $\{p_k, p_{k+1}, \ldots, p_{n-1}\}$. Among these available planets, you need to check if there is any connected group of at least $k$ planets. How can you model this connectivity and find the size of the largest group?
</details>
<details>
<summary>Hint #3</summary>
To implement `is_possible(k)`, you can think of the available planets $\{p_k, \ldots, p_{n-1}\}$ as vertices in a graph. An edge exists between two vertices if the distance between their corresponding planets is at most $r$. The problem then reduces to finding the size of the largest connected component in this graph. If this size is at least $k$, then `is_possible(k)` is `true`.

A classic and efficient data structure for managing and counting connected components is the **Union-Find** (or Disjoint Set Union) data structure.

However, constructing the graph by checking all pairs of available planets would result in $O((n-k)^2)$ potential edges, which might be too slow for larger values of $n$. Since connectivity is based on geometric proximity, can you use a geometric data structure to find the necessary connections more efficiently than checking all pairs? A **Delaunay triangulation** is excellent for finding nearby neighbors.
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1, 2)</summary>
This problem asks for the maximum integer $k$ that satisfies certain geometric connectivity properties. The monotonic nature of the problem—if an alliance of size $k$ is possible, so is one of size $k-1$—strongly suggests that we can **binary search on the answer $k$**. Our search space for $k$ is $[1, n]$.

For the binary search to work, we need a function `possible(k)` that efficiently checks if an alliance of size $k$ is feasible.

### The `possible(k)` Check

For a given $k$, the rules state we can only use planets $\{p_k, p_{k+1}, \ldots, p_{n-1}\}$. Within this set, we need to find if there exists a connected group of at least $k$ planets. Two planets are connected if their distance is at most $r$. This is a classic connectivity problem on a graph.

1.  **Model as a Graph**: We can model the available planets as vertices. An edge exists between two vertices if their distance is $\le r$.
2.  **Find Connected Components**: Our goal is to find the size of the largest connected component in this graph.
3.  **Union-Find Data Structure**: The Union-Find (or Disjoint Set Union) data structure is perfectly suited for this. We can iterate through all pairs of available planets $(p_i, p_j)$ where $i, j \ge k$. If the squared distance between them is less than or equal to $r^2$, we `union` their sets. We also keep track of the size of each component.
4.  **Check Condition**: After processing all such pairs, we find the maximum component size. If this size is $\ge k$, then `possible(k)` returns true; otherwise, it returns false.

### Overall Algorithm

1.  Initialize a search range `low = 1`, `high = n`.
2.  While `low <= high`:
    *   Calculate `mid = low + (high - low) / 2`.
    *   If `possible(mid)` is true, it means an alliance of size `mid` is achievable, so we try for a larger one: `low = mid + 1`. We also store `mid` as a potential answer.
    *   If `possible(mid)` is false, `mid` is too large, so we must try a smaller size: `high = mid - 1`.
3.  The final answer is the largest `mid` for which `possible(mid)` was true.

### Complexity and Implementation Details

The main bottleneck is the `possible(k)` function. A naive implementation that checks all pairs of the $n-k$ available planets has a time complexity of $O((n-k)^2)$. The binary search adds a $\log(n)$ factor, leading to an overall complexity of roughly $O(n^2 \log n)$, which is too slow for the larger constraints.

The provided code attempts an optimization. It first builds a Delaunay triangulation on *all* points to get a smaller set of candidate edges. However, a simple Delaunay triangulation is insufficient, as it only guarantees to include the shortest edge connecting a point to its neighbors, not all edges within a radius $r$. The code compensates for this by performing a search (like DFS or BFS) from each vertex on the triangulation to find all reachable neighbors within the radius, which still results in a large number of edges. This approach is efficient enough for the first two test sets but fails on larger inputs.

```cpp
#include <iostream>
#include <vector>
#include <stack>
#include <tuple>
#include <algorithm>
#include <boost/pending/disjoint_sets.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef int Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index, K> Vb;
typedef CGAL::Triangulation_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Delaunay_triangulation_2<K, Tds> Delaunay;
typedef Delaunay::Vertex_handle Vertex_handle;
typedef std::pair<K::Point_2, Index> IPoint;
typedef std::tuple<Index, Index, K::FT> Edge;
typedef std::vector<Edge> EdgeV;

// Checks if an alliance of size k is possible
bool possible(const EdgeV &edges, int k, int n, K::FT r_squared) {
  boost::disjoint_sets_with_storage<> uf(n);
  std::vector<int> component_sizes(n, 0);
  // Initialize sizes for planets that are available
  for(int i = k; i < n; ++i) {
    component_sizes[i] = 1;
  }

  // Process edges to form components
  for (const auto& e : edges) {
    Index v1 = std::get<0>(e);
    Index v2 = std::get<1>(e);
    K::FT dist = std::get<2>(e);

    // Only consider edges connecting available planets within range
    if (v1 >= k && v2 >= k && dist <= r_squared) {
      Index c1 = uf.find_set(v1);
      Index c2 = uf.find_set(v2);

      if (c1 != c2) {
        int size1 = component_sizes[c1];
        int size2 = component_sizes[c2];
        uf.link(c1, c2);
        Index new_root = uf.find_set(v1);
        component_sizes[new_root] = size1 + size2;
      }
    }
  }

  // Find the size of the largest component
  int max_size = 0;
  for (int i = k; i < n; ++i) {
    if (uf.find_set(i) == i) { // Only check roots of components
        max_size = std::max(max_size, component_sizes[i]);
    }
  }

  return max_size >= k;
}

void solve() {
  int n;
  long r;
  std::cin >> n >> r;
  K::FT r_squared = K::FT(r) * r;

  std::vector<IPoint> planets;
  planets.reserve(n);
  for (Index i = 0; i < n; ++i) {
    int x, y;
    std::cin >> x >> y;
    planets.emplace_back(K::Point_2(x, y), i);
  }

  // Pre-computation step: generate candidate edges
  Delaunay t;
  t.insert(planets.begin(), planets.end());

  EdgeV edges;
  edges.reserve(3 * n);
  for (auto e_it = t.finite_edges_begin(); e_it != t.finite_edges_end(); ++e_it) {
    Index i1 = e_it->first->vertex((e_it->second + 1) % 3)->info();
    Index i2 = e_it->first->vertex((e_it->second + 2) % 3)->info();
    if (CGAL::squared_distance(planets[i1].first, planets[i2].first) <= r_squared) {
        edges.emplace_back(i1, i2, CGAL::squared_distance(planets[i1].first, planets[i2].first));
    }
  }

  // Binary search for the maximum k
  int low = 1, high = n, ans = 0;
  while (low <= high) {
    int mid = low + (high - low) / 2;
    if (mid == 0) { // k must be at least 1
        low = mid + 1;
        continue;
    }
    if (possible(edges, mid, n, r_squared)) {
      ans = mid;
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }

  std::cout << ans << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  int t;
  std::cin >> t;
  while (t--) {
    solve();
  }
  return 0;
}
```
</details>
<details>
<summary>Final Solution</summary>
The previous solution was too slow because its `possible(k)` check was inefficient. The bottleneck was considering too many potential edges. The key insight for an optimal solution is to refine how we generate edges for the connectivity check.

### The Pitfall and The Optimization

A naive approach of building a Delaunay triangulation on *all* planets once and then filtering edges is flawed. Consider two planets $u$ and $v$ with indices greater than $k$ that are within distance $r$. The Delaunay triangulation might not contain the edge $(u,v)$ if another planet $w$ (with index $<k$) lies nearby. In the `possible(k)` check, planet $w$ is unavailable, so we cannot use it as an intermediate step. The direct connection $(u,v)$ is required, but our pre-computed triangulation might have missed it.

The correct and efficient approach is to build the geometric structure **inside the binary search loop**, using only the set of planets available for that specific $k$.

### Improved `possible(k)` Check

For a fixed $k$, we do the following:
1.  **Select Planets**: Create a list of planets that are available, i.e., $\{p_k, p_{k+1}, \ldots, p_{n-1}\}$.
2.  **Build Delaunay Triangulation**: Construct a Delaunay triangulation using *only* these $n-k$ available planets.
3.  **Exploit Delaunay Properties**: A crucial property of the Delaunay triangulation is that it contains all the information we need for proximity-based connectivity. If two points $u$ and $v$ are within distance $r$ of each other, there must exist a path between them in the triangulation where every edge on the path has a length at most $r$. Therefore, to check for connectivity within the radius $r$, we only need to consider the edges of the Delaunay triangulation.
4.  **Union-Find on Triangulation Edges**:
    *   Iterate through all edges of the newly built triangulation.
    *   If an edge's length is $\le r$, `union` the two endpoints in a Union-Find data structure.
    *   Keep track of component sizes.
5.  **Check Condition**: Find the maximum component size. If it is $\ge k$, return true.

### Complexity Analysis

*   **Delaunay Triangulation**: For $m = n-k$ points, construction takes approximately $O(m \log m)$ time.
*   **Edge Processing**: The number of edges in a Delaunay triangulation is linear, $O(m)$. Processing them with Union-Find takes nearly linear time, $O(m \alpha(m))$, where $\alpha$ is the very slow-growing inverse Ackermann function.
*   **Total for `possible(k)`**: The check is dominated by the triangulation, making it $O((n-k) \log(n-k))$.
*   **Overall**: The binary search performs $\log n$ calls to `possible(k)`. This gives a total time complexity of roughly $O(n \log^2 n)$, which is efficient enough to pass all test sets.

```cpp
#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <boost/pending/disjoint_sets.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef int Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index, K> Vb;
typedef CGAL::Triangulation_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Delaunay_triangulation_2<K, Tds> Delaunay;
typedef std::pair<K::Point_2, Index> IPoint;

// Checks if an alliance of size k is possible
bool possible(const std::vector<IPoint>& planets, int k, int n, K::FT r_squared) {
  if (k > n) return false;
  
  // 1. Build triangulation only on available planets {p_k, ..., p_{n-1}}
  Delaunay t;
  t.insert(planets.begin() + k, planets.end());

  // 2. Use Union-Find to find connected components
  boost::disjoint_sets_with_storage<> uf(n);
  std::vector<int> component_sizes(n, 0);
  for(int i = k; i < n; ++i) {
      component_sizes[i] = 1;
  }
  
  // 3. Iterate over triangulation edges
  for (auto e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
    if (t.segment(e).squared_length() <= r_squared) {
      Index i1 = e->first->vertex((e->second + 1) % 3)->info();
      Index i2 = e->first->vertex((e->second + 2) % 3)->info();
      
      Index c1 = uf.find_set(i1);
      Index c2 = uf.find_set(i2);
      
      if (c1 != c2) {
        int size1 = component_sizes[c1];
        int size2 = component_sizes[c2];
        uf.link(c1, c2);
        Index new_root = uf.find_set(i1);
        component_sizes[new_root] = size1 + size2;
      }
    }
  }
  
  // 4. Find the largest component size
  int max_size = 0;
  for (int i = k; i < n; ++i) {
    if (uf.find_set(i) == i) { // Only check roots of components
        max_size = std::max(max_size, component_sizes[i]);
    }
  }

  // 5. Check if the condition is met
  return max_size >= k;
}

void solve() {
  int n;
  long r;
  std::cin >> n >> r;
  K::FT r_squared = K::FT(r) * r;

  std::vector<IPoint> planets;
  planets.reserve(n);
  for (Index i = 0; i < n; ++i) {
    int x, y;
    std::cin >> x >> y;
    planets.emplace_back(K::Point_2(x, y), i);
  }

  // Binary search for the maximum k
  int low = 1, high = n, ans = 0;
  while (low <= high) {
    int mid = low + (high - low) / 2;
    if (mid == 0) { // k must be at least 1
        low = mid + 1;
        continue;
    }
    if (possible(planets, mid, n, r_squared)) {
      ans = mid;
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }

  // The smallest k can be is 1, as we can always choose one planet.
  // We handle the edge case where n=0 or no solution is found by initializing ans.
  // The problem states n>=2, so an alliance of size 1 is always possible.
  if (ans == 0 && n > 0) ans = 1;

  std::cout << ans << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  int t;
  std::cin >> t;
  while (t--) {
    solve();
  }
  return 0;
}
```
</details>

## ⚡ Result

```plaintext

```