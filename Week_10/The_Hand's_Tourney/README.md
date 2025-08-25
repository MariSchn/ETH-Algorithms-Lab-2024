# The Hand's Tourney

## 📝 Problem Description

We are given a set of $N$ tent locations on a 2D plane. The goal is to assign each tent to a family, subject to certain rules. An assignment is considered $(f, s)$-reasonable if it satisfies three conditions:
1. Every tent is assigned to one of $f$ families.
2. Each of the $f$ families receives at least $k$ tents.
3. The squared Euclidean distance between any two tents assigned to *different* families is at least $s$.

Your task is to solve two independent problems based on these rules:
1. Given a fixed number of families $f_0$, find the largest possible value of $s$ for which an $(f_0, s)$-reasonable assignment exists.
2. Given a fixed minimum squared distance $s_0$, find the largest number of families $f$ for which an $(f, s_0)$-reasonable assignment can be made.

For each test case, you will be provided with the number of tents $N$, the minimum tents per family $k$, a target number of families $f_0$, a target squared distance $s_0$, and the coordinates of all $N$ tents. You must output the answers to both questions.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem revolves around distances between tents. A brute-force approach of checking all pairs of tents would be too slow given the potential number of tents. How can we efficiently find the pairs of tents that are closest to each other? Considering the geometric arrangement of the tent locations is key.
</details>

<details>
<summary>Hint #2</summary>
The core constraint states that if two tents are "too close" (i.e., their squared distance is less than $s$), they *must* belong to the same family. This suggests that we can think of tents as being forced into groups. This grouping has a transitive property: if tent A must be with tent B, and tent B must be with tent C, then all three must belong to the same family. This is the definition of a connected component.
</details>

<details>
<summary>Hint #3</summary>
Consider the relationships between tents in increasing order of distance. If we process pairs of tents from closest to farthest, we can dynamically merge groups. A Union-Find data structure is perfectly suited for managing these dynamic sets and tracking which tents must belong to the same family.
</details>

<details>
<summary>Hint #4</summary>
For the general case where a family requires $k > 1$ tents, simply counting the number of groups (components) is not enough. You also need to know the *size* of each group (i.e., how many tents it contains). The problem then becomes: given a collection of components of various sizes, how can we combine them to form the maximum number of valid families, where each family requires at least $k$ tents? This is a combinatorial counting problem on the component sizes.
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1, 2)</summary>

### Approach

For the first two test sets, we have two simplifying assumptions: each family requires at least one tent ($k=1$), and for the first question, the number of families is equal to the number of tents ($f_0 = n$).

The problem's focus on distances between points, especially nearest neighbors, suggests that a geometric structure like a **Delaunay Triangulation** is highly effective. The Delaunay triangulation of a set of points has the property that the closest pair of points will always form an edge in the triangulation. This allows us to avoid checking all $O(n^2)$ pairs and instead focus only on the $O(n)$ edges of the triangulation.

#### Answering Question 1 (Find max $s$ for $f_0=n$ families)

With the assumptions $k=1$ and $f_0=n$, every family must be assigned exactly one tent. An assignment is $(n, s)$-reasonable if the distance between any two tents (which now belong to different families) is at least $s$. We want to find the largest possible $s$. This is determined by the two closest tents. If their squared distance is $d_{min}$, then any $s \le d_{min}$ is valid. The maximum possible value for $s$ is therefore precisely $d_{min}$.

To find this, we can compute the Delaunay triangulation of all tent locations, find the length of every edge, and the smallest of these squared lengths is our answer.

#### Answering Question 2 (Find max $f$ for a given $s_0$)

Here, we are given a minimum squared distance $s_0$ and must find the maximum number of families we can accommodate. The condition states that any two tents with a squared distance less than $s_0$ *must* belong to the same family.

This naturally defines a grouping: all tents that are forced to be together form a single component. A family can then be formed from the tents of such a component. Since $k=1$, each component can house one family. Therefore, the maximum number of families is simply the number of components.

To find this, we can use a **Union-Find** data structure.
1. Initialize a Union-Find structure with $n$ sets, one for each tent. The number of families is initially $n$.
2. Construct the Delaunay triangulation and extract all its edges.
3. Iterate through the edges. If an edge has a squared length less than $s_0$, it connects two tents that must be in the same family. We `union` the sets containing these two tents. If they were not already in the same set, we decrement our count of families (components).
4. After processing all edges shorter than $s_0$, the final count of disjoint sets is the maximum number of families $f$.

For efficiency, it's best to sort the edges by length first. Then, for Question 2, we process edges until their length is $\ge s_0$. For Question 1, the answer is simply the length of the first edge in the sorted list.

### C++ Code

```cpp
#include <iostream> 
#include <vector>
#include <limits>
#include <algorithm>

#include <boost/pending/disjoint_sets.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef std::size_t                                            Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;

typedef std::tuple<Index,Index,K::FT> Edge;
typedef std::vector<Edge> EdgeV;

typedef K::Point_2 Point;
typedef std::pair<Point,Index> IPoint;


void solve() {
  // ===== READ INPUT =====
  long n, k, f_0, s_0; std::cin >> n >> k >> f_0 >> s_0;

  std::vector<IPoint> tents; tents.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    tents.push_back(IPoint(Point(x, y), i));
  }
  
  // ===== TRIANGULATION =====
  Delaunay t;
  t.insert(tents.begin(), tents.end());
  
  // === Get all Edges from the Triangulation ===
  EdgeV edges; edges.reserve(3*n);
  for (auto e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
    Index i1 = e->first->vertex((e->second+1)%3)->info();
    Index i2 = e->first->vertex((e->second+2)%3)->info();

    if (i1 > i2) std::swap(i1, i2);
    edges.emplace_back(i1, i2, t.segment(e).squared_length());
  }
  std::sort(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) -> bool {
    return std::get<2>(e1) < std::get<2>(e2);
  });
  
  // Count number of components for distance s_0
  boost::disjoint_sets_with_storage<> uf(n);
  Index n_components = n;
  for (EdgeV::const_iterator e = edges.begin(); e != edges.end(); ++e) {
    Index c1 = uf.find_set(std::get<0>(*e));
    Index c2 = uf.find_set(std::get<1>(*e));
    K::FT dist = std::get<2>(*e);
    
    if(dist >= s_0) {
      break;
    }
    
    if (c1 != c2) {
      uf.link(c1, c2);
      if (--n_components == 1) break;
    }
  }

  // ===== OUTPUT =====
  std::cout << std::fixed << std::setprecision(0);
  std::cout << (long) std::get<2>(edges[0]) << " " << n_components << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}
```
</details>

<details>
<summary>Second Solution (Test Set 1, 2, 3)</summary>

### Approach

In this version, the assumption $f_0=n$ is dropped, but we still have $k=1$. This primarily affects the first question. The logic for the second question remains unchanged.

#### Answering Question 1 (Find max $s$ for $f_0$ families)

We need to find the largest squared distance $s$ such that we can form at least $f_0$ families. Since $k=1$, one family corresponds to one component of tents. Thus, we need to find the largest $s$ that results in at least $f_0$ components.

There is a clear monotonic relationship: as $s$ increases, more tent pairs are forced into the same component, which *decreases* (or keeps equal) the total number of components. We are looking for the "breaking point".

We can find this by simulating the process of increasing $s$:
1.  Start with $n$ components, one for each tent. This corresponds to $s=0$.
2.  Process the edges of the Delaunay triangulation, sorted by increasing length. Each edge represents a merge event.
3.  For each edge, we `union` the components of its endpoints. Every time we merge two different components, the total number of components decreases by one.
4.  We are looking for the largest $s$ that allows for $\ge f_0$ components. This is equivalent to finding the smallest $s$ that results in *fewer than* $f_0$ components. The length of the edge that causes the number of components to drop from $f_0$ to $f_0-1$ is our answer. Any distance just below this value would have kept the components separate, allowing for $f_0$ families.

#### Answering Question 2 (Find max $f$ for a given $s_0$)

This part is identical to the previous solution. We build components by merging all tents connected by edges shorter than $s_0$ and count the final number of components.

### C++ Code
```cpp
///3
#include <iostream> 
#include <vector>
#include <limits>
#include <algorithm>

#include <boost/pending/disjoint_sets.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef std::size_t                                            Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;

typedef std::tuple<Index,Index,K::FT> Edge;
typedef std::vector<Edge> EdgeV;

typedef K::Point_2 Point;
typedef std::pair<Point,Index> IPoint;


void solve() {
  // ===== READ INPUT =====
  long n, k, f_0, s_0; std::cin >> n >> k >> f_0 >> s_0;

  std::vector<IPoint> tents; tents.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    tents.push_back(IPoint(Point(x, y), i));
  }
  
  // ===== TRIANGULATION =====
  Delaunay t;
  t.insert(tents.begin(), tents.end());
  
  // === Get all Edges from the Triangulation ===
  EdgeV edges; edges.reserve(3*n);
  for (auto e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
    Index i1 = e->first->vertex((e->second+1)%3)->info();
    Index i2 = e->first->vertex((e->second+2)%3)->info();

    if (i1 > i2) std::swap(i1, i2);
    edges.emplace_back(i1, i2, t.segment(e).squared_length());
  }
  std::sort(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) -> bool {
    return std::get<2>(e1) < std::get<2>(e2);
  });
  
  // Iterate and merge components until there are less than f_0 components left
  boost::disjoint_sets_with_storage<> f_uf(n);
  Index f_n_components = n;
  K::FT max_s = 0;
  for (EdgeV::const_iterator e = edges.begin(); e != edges.end(); ++e) {
    Index c1 = f_uf.find_set(std::get<0>(*e));
    Index c2 = f_uf.find_set(std::get<1>(*e));
    K::FT dist = std::get<2>(*e);
    
    if (c1 != c2) {
      f_uf.link(c1, c2);
      if (f_n_components-- == f_0) {
        max_s = dist;
        break;
      }
    }
  }
  
  // Count number of components for distance s_0
  boost::disjoint_sets_with_storage<> s_uf(n);
  Index s_n_components = n;
  for (EdgeV::const_iterator e = edges.begin(); e != edges.end(); ++e) {
    Index c1 = s_uf.find_set(std::get<0>(*e));
    Index c2 = s_uf.find_set(std::get<1>(*e));
    K::FT dist = std::get<2>(*e);
    
    if(dist >= s_0) {
      break;
    }
    
    if (c1 != c2) {
      s_uf.link(c1, c2);
      if (--s_n_components == 1) break;
    }
  }

  // ===== OUTPUT =====
  std::cout << std::fixed << std::setprecision(0);
  std::cout << max_s << " " << s_n_components << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    solve();
  }
}
```
</details>

<details>
<summary>Final Solution</summary>

### Approach

The final version of the problem removes all simplifying assumptions, specifically allowing $k > 1$. This means a family requires multiple tents. The core framework of using a Delaunay triangulation and processing sorted edges remains, but how we count the number of possible families changes significantly.

We must now track not only the number of components but also their **sizes** (the number of tents in each). When we merge two components of size $c_1$ and $c_2$, we create a new component of size $c_1 + c_2$. To avoid dealing with arbitrarily large component sizes, we can cap the tracked size at $k$, since any component with $k$ or more tents can satisfy the requirement for one family on its own, and any additional tents within it are "bonus".

Let's define a function `max_num_families(component_counts, k)` that takes a list of how many components of each size (from 1 to $k$) exist and calculates the maximum number of families we can form.

#### The `max_num_families` subproblem

Given counts of components of sizes 1, 2, ..., $k$, how many families of size $k$ can we form? This is a combinatorial packing problem. A greedy strategy works well here. For example, with $k=4$:
1.  Any component of size 4 or more (capped at 4) directly forms one family.
2.  We can combine smaller components. It's often best to combine the largest available components first. For example, pair a size-3 component with a size-1 component.
3.  Next, pair two size-2 components.
4.  Handle leftovers greedily. For example, a leftover size-2 component could be combined with two size-1 components.
5.  Finally, group any remaining size-1 components in sets of 4.

The provided code implements such a greedy strategy for $k=1, 2, 3, 4$.

#### Answering Question 1 (Find max $s$ for $f_0$ families)

1.  Initialize a Union-Find structure where each tent is in its own component of size 1. Also, maintain a count of components for each size (e.g., an array `comp_of_size` where `comp_of_size[1] = n`).
2.  Calculate the initial number of families possible using `max_num_families`. If it's already less than $f_0$, no solution is possible (though problem constraints prevent this).
3.  Iterate through the sorted edges of the Delaunay triangulation. For each edge:
    a. Find the components and their sizes for the two endpoints.
    b. If they are in different components, merge them. Update the `comp_of_size` array: decrement the counts for the old sizes and increment the count for the new combined size (capped at $k$).
    c. After the merge, calculate the new maximum number of families.
    d. If this number drops below $f_0$, the length of the current edge is our answer $s$. This is the smallest distance that makes it impossible to form $f_0$ families.

#### Answering Question 2 (Find max $f$ for a given $s_0$)

1.  This is a simplified version of the above. First, build the final component configuration for the given distance $s_0$.
2.  Initialize Union-Find and size counts as before.
3.  Iterate through all sorted edges with length strictly less than $s_0$. For each such edge, merge the components and update their sizes, just as in Question 1.
4.  After processing all relevant edges, we have the final distribution of component sizes.
5.  Call `max_num_families` one last time on this final configuration to get the answer $f$.

### C++ Code
```cpp
#include <limits>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <algorithm>
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <boost/pending/disjoint_sets.hpp>

// Epic kernel is enough, no constructions needed, provided the squared distance
// fits into a double (!)
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// we want to store an index with each vertex
typedef std::size_t                                            Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;

typedef std::tuple<Index,Index,K::FT> Edge;
typedef std::vector<Edge> EdgeV;

int max_num_fam(std::vector<int> &comp_of_size, int k) {
    // vector is size == k+1
    int num = comp_of_size[k];
    if(k == 4) {
        // match 3 with ones, then take pairs of 2, match rest
        int match_three_one = std::min(comp_of_size[3], comp_of_size[1]);
        int remaining3 = comp_of_size[3] - match_three_one;
        int remaining1 = comp_of_size[1] - match_three_one;
        // add remaining size 3 to 2
        int remaining2 = comp_of_size[2] + remaining3;
        // if num2 is not divisible by two, we can possibly combine
        // the one leftover with the single
        if(remaining2 % 2 == 1) remaining1 += 2;
        num += match_three_one + remaining2 / 2 + remaining1 / 4;
        
    } else if(k == 3) {
        // match 2 with ones, add rest
        int match_two_one = std::min(comp_of_size[2], comp_of_size[1]);
        int remaining2 = comp_of_size[2] - match_two_one;
        int remaining1 = comp_of_size[1] - match_two_one;
        // the remaining 2size comp have to be div by 2 (two together is one family)
        num += match_two_one + remaining2 / 2 + remaining1 / 3;
    } else if(k == 2) {
        // just take the single comp and divide by 2
        num += comp_of_size[1] / 2;
    }
    return num;
}


void testcase() {
    Index n, k, f0;
    double s0;
    std::cin >> n >> k >> f0 >> s0;

    typedef std::pair<K::Point_2,Index> IPoint;
    std::vector<IPoint> points;
    points.reserve(n);
    for (Index i = 0; i < n; ++i) {
        int x, y;
        std::cin >> x >> y;
        points.emplace_back(K::Point_2(x, y), i);
    }
    Delaunay t;
    t.insert(points.begin(), points.end());
    EdgeV edges;
    edges.reserve(3*n); // there can be no more in a planar graph
    for (auto e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
        Index i1 = e->first->vertex((e->second+1)%3)->info();
        Index i2 = e->first->vertex((e->second+2)%3)->info();
        // ensure smaller index comes first
        if (i1 > i2) std::swap(i1, i2);
        edges.emplace_back(i1, i2, t.segment(e).squared_length());
    }
    std::sort(edges.begin(), edges.end(),
            [](const Edge& e1, const Edge& e2) -> bool {
            return std::get<2>(e1) < std::get<2>(e2);
                });


    // for testcases 1-2: just look at smallest distance, bc otherwise not enough tents
    // std::cout << long(std::get<2>(edges[0])) << " ";


    boost::disjoint_sets_with_storage<> uf(n);
    std::vector<Index> num_tents(n, 1);
    Index n_components = n;
    std::vector<int> comp_of_size(k + 1, 0);
    comp_of_size[1] = n;
    double last_dist = 0;
    for (EdgeV::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        // determine components of endpoints
        Index c1 = uf.find_set(std::get<0>(*e));
        Index c2 = uf.find_set(std::get<1>(*e));
        last_dist = std::get<2>(*e);
        if (c1 != c2) {
            Index n1 = num_tents[c1];
            Index n2 = num_tents[c2];
            uf.link(c1, c2);
            Index c3 = uf.find_set(std::get<1>(*e));
            // set unused indices to zero
            num_tents[c1] = num_tents[c2] = 0;
            // cap it at k
            num_tents[c3] = std::min(n1 + n2, k);
            comp_of_size[n1]--; comp_of_size[n2]--;
            comp_of_size[num_tents[c3]]++;
            if (max_num_fam(comp_of_size, k) < f0) break;
        }
    }
    
    std::cout << long(last_dist) << " ";


    // repeat process with adding edges < s0, then find max num families
    boost::disjoint_sets_with_storage<> uf_s0(n);
    num_tents = std::vector<Index>(n, 1);
    comp_of_size = std::vector<int>(k + 1, 0);
    comp_of_size[1] = n;

    n_components = n;
    for (EdgeV::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        // determine components of endpoints
        Index c1 = uf_s0.find_set(std::get<0>(*e));
        Index c2 = uf_s0.find_set(std::get<1>(*e));
        double dist = std::get<2>(*e);
        if(dist >= s0) {
            break;
        }
        if (c1 != c2) {
            Index n1 = num_tents[c1];
            Index n2 = num_tents[c2];
            uf_s0.link(c1, c2);
            Index c3 = uf_s0.find_set(std::get<1>(*e));
            // set unused indices to zero
            num_tents[c1] = num_tents[c2] = 0;
            // cap component size at k
            num_tents[c3] = std::min(n1 + n2, k);
            comp_of_size[n1]--; comp_of_size[n2]--;
            comp_of_size[num_tents[c3]]++;
            if (--n_components == 1) break;
        }
    }
    std::cout << max_num_fam(comp_of_size, k) << std::endl;
    return;
}

int main() {
    std::ios_base::sync_with_stdio(false);

    int t;
    std::cin >> t;
    for (int i = 0; i < t; ++i)
        testcase();
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1        (25 pts / 2 s) : Correct answer      (0.013s)
   Test set 2        (25 pts / 2 s) : Correct answer      (0.365s)
   Test set 3        (15 pts / 2 s) : Correct answer      (0.317s)
   Test set 4        (15 pts / 2 s) : Correct answer      (0.244s)
   Hidden test set 1 (05 pts / 2 s) : Correct answer      (0.013s)
   Hidden test set 2 (05 pts / 2 s) : Correct answer      (0.365s)
   Hidden test set 3 (05 pts / 2 s) : Correct answer      (0.317s)
   Hidden test set 4 (05 pts / 2 s) : Correct answer      (0.244s)

Total score: 100
```