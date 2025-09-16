# Revenge of the Sith

## 📝 Problem Description

A set of $n$ planets is given, each represented by its 2D coordinates. These planets are conquered by the empire in a fixed, predetermined sequence: $p_0, p_1, \ldots, p_{n-1}$. The objective is to determine the maximum possible size $k$ of an alliance that can be formed under the following constraints:

1. The alliance must consist of $k$ distinct planets, all selected from those not yet conquered. After $k$ days, planets $p_0$ through $p_{k-1}$ are no longer available, so the selection must be made from the remaining planets $\{p_k, p_{k+1}, \ldots, p_{n-1}\}$.
2. The chosen set of $k$ planets must be "reachable," meaning they form a single connected group. A connection between any two planets exists if their distance does not exceed a specified scout vessel range $r$. For any partition of the selected planets into two non-empty subsets, there must exist a pair of planets—one from each subset—whose distance is at most $r$.

The task is to find the largest integer $k$ such that an alliance meeting these criteria can be formed.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem asks for the *maximum* possible value of $k$. Notice that if it's possible to form an alliance of size $k$, it is also possible to form an alliance of any size smaller than $k$ (by simply taking a connected subset of the size-$k$ alliance). This monotonic property, if a property holds for $k$, it also holds for all $k' < k$, is a strong indicator that a particular search algorithm might be very effective. Which algorithm excels at finding an optimal value in a monotonic search space?

</details>

<details>

<summary>Hint #2</summary>

You can apply binary search on the answer, $k$. The range for $k$ would be from $1$ to $n$. For a fixed value of $k$ in your binary search, you need a function, let's call it `possible(k)`, that returns `true` if an alliance of size $k$ can be formed and `false` otherwise.

How would you implement `possible(k)`? According to the rules, you must only consider planets $\{p_k, p_{k+1}, \ldots, p_{n-1}\}$. Among these available planets, you need to check if there is any connected group of at least $k$ planets. How can you model this connectivity and find the size of the largest group?

</details>

<details>

<summary>Hint #3</summary>

To implement `possible(k)`, you can think of the available planets $\{p_k, \ldots, p_{n-1}\}$ as vertices in a graph. An edge exists between two vertices if the distance between their corresponding planets is at most $r$. The problem then reduces to finding the size of the largest connected component in this graph. If this size is at least $k$, then `possible(k)` is `true`.

A classic and efficient data structure for managing and counting connected components is the **Union-Find** (or Disjoint Set Union) data structure.

However, constructing the graph by checking all pairs of available planets would result in $O((n-k)^2)$ potential edges, which might be too slow for larger values of $n$. Since connectivity is based on geometric proximity, can you use a **Delaunay triangulation** for finding nearby neighbors.

</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Set 1, 2)</summary>

This problem asks for the maximum integer $k$ that satisfies certain geometric connectivity properties. The monotonic nature of the problem, if an alliance of size $k$ is possible, so is one of size $k-1$—strongly suggests that we can **binary search on the answer $k$**.

For the binary search to work, we need a function `possible(k)` that efficiently checks if an alliance of size $k$ is feasible.

### The `possible(k)` Check

For a **given fixed** $k$ we can **check if it is possible** to find such a set of $k$ connected nodes by:

1. Find **all possible edges/connections** $(u, v)$ between any planets $u$ and $v$ that we are allowed to take for the given $k$. These must fulfill two conditions:
    1. The edge has to be **shorter** (or equal) **to the radius of the scouting vessel** $\|u, v\| \leq r$
        
        **Note**: We can actually “prefilter” all Edges to only contain those that are shorter than $r$ to avoid doing this multiple times.
        
    2. The index of both $u$ and $v$ needs to be at least $k$. So $u,v \geq k$
        
         This is because otherwise taking this connection would bring us to a planet that is already captured by the Empire
        
2. Given these edges, compute its **Connected Components** using the **Union Find Datastructure**
3. Check if there is a component that is **at least $k$ large**

    No → Not possible to find set of size $k$ <br />
    Yes → Possible to find set of size $k$

With this in place, we can now check if for any given $k$ it is possible to find such a set.
We can therefore simply perform a **Binary Search** over the range of $k$ to find the maximum $k$ that allows to find a set of size $k$.

Doing this already yields a solution for the first 2 Test Sets. However, it is too inefficient to solve the others.

### Code
```cpp
#include <iostream>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <boost/pending/disjoint_sets.hpp>
#include <vector>
#include <tuple>
#include <algorithm>
#include <iostream>

// Epic kernel is enough, no constructions needed, provided the squared distance
// fits into a double (!)
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// we want to store an index with each vertex
typedef int                                                    Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;

typedef std::tuple<Index,Index,K::FT> Edge;
typedef std::vector<Edge> EdgeV;

typedef std::pair<K::Point_2,Index> IPoint;

std::ostream& operator<<(std::ostream& o, const Edge& e) {
  return o << std::get<0>(e) << " " << std::get<1>(e) << " " << std::get<2>(e);
}

const bool DEBUG = false;

bool possible(const EdgeV &edges, int k, int n, K::FT r_squared) {
  // Setup and initialize Union-Find Data Structure
  boost::disjoint_sets_with_storage<> uf(n);
  std::vector<int> component_sizes(n, 1);
  for (EdgeV::const_iterator e = edges.begin(); e != edges.end(); ++e) {
    Index v1 = std::get<0>(*e);
    Index v2 = std::get<1>(*e);
    
    // Determine Components of Endpoints
    Index c1 = uf.find_set(v1);
    Index c2 = uf.find_set(v2);
    K::FT dist = std::get<2>(*e);
    
    if (
      c1 != c2 && 
      v1 >= k && v2 >= k &&
      dist <= r_squared
    ) {
      if (DEBUG) std::cout << "Merged " << v1 << " and " << v2 << std::endl;
      int size1 = component_sizes[c1];
      int size2 = component_sizes[c2];
      uf.link(c1, c2);
      
      Index c3 = uf.find_set(v1);
      component_sizes[c3] = size1 + size2;
    }
  }
  
  // Find biggest component
  int max_size = 0;
  if (DEBUG) std::cout << "Component Sizes ";
  for(int i = 0; i < n; ++i) {
    if (DEBUG) std::cout << component_sizes[i] << " ";
    max_size = std::max(max_size, component_sizes[i]);
  }
  if (DEBUG) std::cout << std::endl;
  
  return max_size >= k;
}

void solve() {
  // std::cout << "========================================" << std::endl;
  // ===== READ INPUT =====
  int n; long r; K::FT r_squared;
  std::cin >> n >> r;
  r_squared = r;
  r_squared = r_squared * r_squared;

  std::vector<IPoint> planets; planets.reserve(n);
  for (Index i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    planets.emplace_back(K::Point_2(x, y), i);
  }
  
  // ===== SOLVE =====
  Delaunay t;
  t.insert(planets.begin(), planets.end());
  
  // Extract all edges along with their distance
  EdgeV edges; edges.reserve(n * n);
  for(int i = 0; i < n; ++i) {
    for(int j = i + 1; j < n; ++j) {
      edges.emplace_back(i, j, CGAL::squared_distance(planets[i].first, planets[j].first));
    }
  }
  // EdgeV edges; edges.reserve(3*n);
  // for (auto e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
  //   Index i1 = e->first->vertex((e->second+1)%3)->info();
  //   Index i2 = e->first->vertex((e->second+2)%3)->info();
  //   if (i1 > i2) std::swap(i1, i2);
    
  //   edges.emplace_back(i1, i2, t.segment(e).squared_length());
  // }
  
  // Binary search over possible k
  int min = 0;
  int max = n;
  
  while(min < max) {
    int middle = (min + max) / 2;
    if (DEBUG) std::cout << "=== k=" << middle << " ===" << std::endl; 
    
    if(min >= max - 1) {
      break;
    }
    
    if(possible(edges, middle, n, r_squared)) {
      if (DEBUG) std::cout << "k=" << middle << " is possible" << std::endl;
      min = middle;
    } else {
      if (DEBUG) std::cout << "k=" << middle << " is not possible" << std::endl;
      max = middle;
    }
  }
  
  // ===== OUTPUT =====
  std::cout << min << std::endl;
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

The **main problem** with the previous solution lies in the fact that it **considers all Edges** $(u, v)$ even though many of them **could be ignored**.

**Note**: It might be tempting to use the **Delaunay Triangulation** on all points and then filter out the edges that are shorter than $r$. However, this won’t work, as the Delaunay Triangulation only provides the closes edges. Meaning that an edge a node $v$ is not the closes to $u$, but still has a distance smaller than $r$, the Delaunay Triangulation will not contain this edge. <br />
This itself might not seem problematic, as the actual closes node $v'$ to $u$ will be connected to both nodes and therefore we will get the same component. However, if the index of $v'$ is smaller than $k$ we will not be allowed to take this edge, causing $u$ and $v$ to be in separate components, even though they should be in one  

Instead we will **Triangulate in every Iteration of the Binary Search**.

In every iteration of `possible` we triangulate the points/planets, **ignoring the first** $k$ **planets**. As they are not allowed to be in the set anyway.
This allows us to skip the check if for the edge $(u,v)$, one of $u,v$ is smaller than $k$, as no points that are smaller than $k$ are included in the Triangulation.

Removing the first $k$ points solves the problem from above, as now if two planes $u, v$ have a smaller radius than $r$ they will either be direcly connected or have a common neighbor, as now all planets/points are allowed to be taken, this neighbor will cause them to be in the same component.

Then we can perform the same Component Computation using Union Find as before, to find the largest set and check if  it is larger or equal to $k$

### Code
```cpp
#include <iostream>
#include <vector>
#include <stack>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <boost/pending/disjoint_sets.hpp>
#include <vector>
#include <tuple>
#include <algorithm>
#include <iostream>

// Epic kernel is enough, no constructions needed, provided the squared distance
// fits into a double (!)
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// we want to store an index with each vertex
typedef int                                                    Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;
typedef Delaunay::Vertex_handle Vertex_handle;


typedef std::tuple<Index,Index,K::FT> Edge;
typedef std::vector<Edge> EdgeV;

typedef std::pair<K::Point_2,Index> IPoint;

std::ostream& operator<<(std::ostream& o, const Edge& e) {
  return o << std::get<0>(e) << " " << std::get<1>(e) << " " << std::get<2>(e);
}

const bool DEBUG = false;

bool possible(const std::vector<IPoint> planets, int k, int n, K::FT r_squared) {
  // Triangulate the points starting from k
  Delaunay t;
  t.insert(planets.begin() + k, planets.end());
  
  // Extract edges
  EdgeV edges;
  edges.reserve(3*n); // there can be no more in a planar graph
  for (auto e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
    Index i1 = e->first->vertex((e->second+1)%3)->info();
    Index i2 = e->first->vertex((e->second+2)%3)->info();
    // ensure smaller index comes first
    if (i1 > i2) std::swap(i1, i2);
    edges.emplace_back(i1, i2, t.segment(e).squared_length());
  }
  
  // Sort edges
  std::sort(edges.begin(), edges.end(),
      [](const Edge& e1, const Edge& e2) -> bool {
        return std::get<2>(e1) < std::get<2>(e2);
            });
  
  // Setup and initialize Union-Find Data Structure
  boost::disjoint_sets_with_storage<> uf(n);
  std::vector<int> component_sizes(n, 1);
  for (EdgeV::const_iterator e = edges.begin(); e != edges.end(); ++e) {
    Index v1 = std::get<0>(*e);
    Index v2 = std::get<1>(*e);
    
    // Determine Components of Endpoints
    Index c1 = uf.find_set(v1);
    Index c2 = uf.find_set(v2);
    K::FT dist = std::get<2>(*e);
    
    if (
      c1 != c2 && 
      dist <= r_squared
    ) {
      int size1 = component_sizes[c1];
      int size2 = component_sizes[c2];
      uf.link(c1, c2);
      
      Index c3 = uf.find_set(v1);
      component_sizes[c3] = size1 + size2;
    }
  }
  
  // Find biggest component
  int max_size = 0;
  for(int i = 0; i < n; ++i) {
    max_size = std::max(max_size, component_sizes[i]);
  }

  return max_size >= k;
}

void solve() {
  // std::cout << "========================================" << std::endl;
  // ===== READ INPUT =====
  int n; long r; K::FT r_squared;
  std::cin >> n >> r;
  r_squared = r;
  r_squared = r_squared * r_squared;

  std::vector<IPoint> planets; planets.reserve(n);
  for (Index i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    planets.emplace_back(K::Point_2(x, y), i);
  }
  
  // ===== SOLVE =====
  // Binary search over possible k
  int min = 0;
  int max = n;
  
  while(min < max) {
    int middle = (min + max) / 2;
    if (DEBUG) std::cout << "=== k=" << middle << " ===" << std::endl; 
    
    if(min >= max - 1) {
      break;
    }
    
    if(possible(planets, middle, n, r_squared)) {
      if (DEBUG) std::cout << "k=" << middle << " is possible" << std::endl;
      min = middle;
    } else {
      if (DEBUG) std::cout << "k=" << middle << " is not possible" << std::endl;
      max = middle;
    }
  }
  
  // ===== OUTPUT =====
  std::cout << min << std::endl;
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

- Delaunay Triangulations are super cheap ($O(n \log n)$)

</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (25 pts / 2 s) : Correct answer      (0.059s)
   Test set 2 (25 pts / 2 s) : Correct answer      (0.181s)
   Test set 3 (25 pts / 2 s) : Correct answer      (0.57s)
   Test set 4 (25 pts / 2 s) : Correct answer      (1.356s)

Total score: 100
```