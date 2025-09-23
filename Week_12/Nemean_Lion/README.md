# The Nemean Lion

## 📝 Problem Description

The problem involves a set of points in a two-dimensional plane, representing Platanos trees. The goal is to find subsets of these trees that satisfy a specific geometric condition: 

There exists a point in the plane that is equidistant from all trees in the subset, and closer to these trees than to any other tree in the set.  

The problem asks for counting these subsets of size 2 and 3 for a given distance, and for determining the maximum number of such subsets of size 2 for any distance. Input consists of the number of trees, a target squared distance, and a hint value. 

The output consists of:
- $a_k$ the number of admissible sets of size $k \in \{2, 3\}$ for the smallest distance $d_k$
- $a_s$ the number of admissible sets of size 2 for the given distance $\sqrt s$
- $a_\text{max}$ the maximum number of admissible sets of size 2 for any distance.

## 💡 Hints

<details>

<summary>Hint #1</summary>

This problem involves finding points equidistant from subsets of trees in 2D space. For pairs of points, the equidistant point lies on their perpendicular bisector. For triples, it's the circumcenter of their triangle. Use a **Delaunay Triangulation** to efficiently identify the relevant pairs and triples, the edges and faces of the triangulation correspond to the closest point relationships you need to analyze.

</details>

<details>

<summary>Hint #2</summary>

To find $a_2$ (admissible sets of size 2 for the smallest distance), examine all **edges** in the Delaunay triangulation. For each edge, the optimal equidistant point is at the midpoint, and the distance is half the edge length. The smallest such distance gives you $d_2$, and $a_2$ is the number of edges with this minimum length. By the properties of Delaunay triangulation, these edges represent the closest pairs that can form valid admissible sets.

</details>

<details>

<summary>Hint #3</summary>

To find $a_3$ (admissible sets of size 3 for the smallest distance), examine all **faces** (triangles) in the Delaunay triangulation. For each face, calculate the circumcenter and its distance to any vertex (the circumradius). This gives you the distance $d_3$ for that triangle. Find the minimum circumradius among all faces, this is your smallest $d_3$, and $a_3$ is the count of faces with this minimum circumradius. The empty circumcircle property of Delaunay triangulation ensures these are valid admissible sets.

</details>

<details>

<summary>Hint #4</summary>

To find $a_s$ (admissible sets of size 2 for distance $\sqrt{s}$), you can't just use edge midpoints. For each edge, calculate the **interval** $[d_{\min}, d_{\max}]$ of valid distances where a point can be placed while maintaining the admissible property. $d_{\min}$ is either half the edge length (if the midpoint is valid) or the circumradius of adjacent faces. $d_{\max}$ is the circumradius of adjacent faces, or infinity for boundary edges. Count how many intervals contain the value $s$.

</details>

<details>

<summary>Hint #5</summary>

To find $a_{\max}$ (maximum number of admissible sets of size 2 for any distance), reuse the intervals $[d_{\min}, d_{\max}]$ from Hint #4. This becomes an interval overlap problem: find the distance value that lies within the maximum number of intervals. Collect all interval endpoints, then sweep through them while maintaining a count of active intervals. The maximum count encountered during this sweep is your $a_{\max}$.

</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Set 1)</summary>

While reading this problem it becomes very apparent that this will involve some **Geometry/CGAL** as we are working with points in **Euclidean 2D Space** $\mathbb R^2$ and need to consider **distances** between them.

Additionally we are only interested in **Admissible Sets** of Trees $T$. These admissible sets have the property that there exists a point $p \in \mathbb R^2$ such that:

- Every **point** in $T$ has the **same distance $d$** to $p$
- Every **other point** $\mathcal F \setminus T$ has a **larger distance** $d$ to $p$

I.e. there needs to be a point $p$ to which all points in $T$ have the same distance $d$ and every other point/tree has a larger distance towards this $p$

### Why Delaunay Triangulation

More specifically we are interested in the number of Admissable Sets of Size $2$ ($a_2$) and $3$ ($a_3$). For these we can make the following observation:

- For $a_2$ we only have **two** points in $T$. The point $p$ has to lie exactly in between the 2 points
- For $a_3$ we only have **three** points $T$. The point $p$ that has the same distance to all 3 points in $T$ is the **Center of the Circumcircle** of these 3 points.

Given that all other points $\mathcal F$ need to be further away from $p$, this sparks the Idea of using a **Delaunay Triangulation**. <br />
That is because the Delaunay Triangulation provides us with a Triangulation of the points in which the closest two or three points form either an Edge or a Face. If we then consider only these edges, we can be sure that no points is closer to it, by the definition of the triangle

### Using Test Set 1 Constraints

Lets first try to solve Test Set 1. In the Test Set 1 we only need to calculate $a_2$. That is the amount of **admissible sets of size** 2, for the **smallest distance $d_2$** that yields more than $0$ admissible sets.
I.e. How small can we make the distance $d_2$ that we can still find at least one set of two points/trees $T$ that both have distance $d_2$ to a point $p$ such that every other point has a larger distance than $d_2$ to $p$

Observe, that the “optimal” points $p$ will always lie at the **Midpoint of the Delaunay Triangulation Edges**.
With “optimal” I mean here the point $p$ that has the same distance $d_2$ to the two points in $T$ and minimizes that distance $d_2$. This is obviously minimized when $p$ lies exactly between the two points, which happens at the Midpoint of the Delaunay Triangulation Edges

Therefore, we can simply **iterate over all Edges of the Delaunay Triangulation Edges**, and **count for every length of these edges, how many Edges/Admissible Sets there were**. 

In the end we are simply take the smallest distance $d_2$ which yields at least one such edge, and choose that as our $a_2$

### Code
```c++
#include <iostream>
#include <vector>
#include <unordered_map>
#include <limits>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;
typedef Triangulation::Edge_iterator  Edge_iterator;
typedef Triangulation::Finite_faces_iterator Face_iterator;

typedef K::Point_2 Point;
typedef K::Circle_2 Circle;

void solve() {
  // ===== READ INPUT =====
  int n, h; long s; std::cin >> n >> s >> h;
  
  std::vector<Point> trees; trees.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    trees.emplace_back(x, y);
  }
  
  // ===== SOLVE =====
  int a_2, a_3, a_s, a_max; a_2 = a_3 = a_s = a_max = 0;

  a_3 = 1;         // ! TEST SET 1 ASSUMPTION !
  a_s = a_max = h; // ! TEST SET 1 ASSUMPTION !
  
  // Triangulate trees
  Triangulation t;
  t.insert(trees.begin(), trees.end());
  
  // Count how many edges there are, for each distance d and take the max count as a_2
  K::FT min_dist = std::numeric_limits<K::FT>::max();
  std::unordered_map<K::FT, int> edge_dist_to_count; // Edge Length -> Num Edges
  for (Edge_iterator e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
    K::FT dist = t.segment(e).squared_length();
    
    // Increment counter for the distance of the edge `dist`
    if(edge_dist_to_count.find(dist) == edge_dist_to_count.end()) { 
      edge_dist_to_count[dist] = 1; 
    } 
    else { 
      edge_dist_to_count[dist]++; 
    }
    
    min_dist = std::min(dist, min_dist);
  }
  a_2 = edge_dist_to_count[min_dist];
  
  // ===== OUTPUT =====
  std::cout << a_2 << " " << a_3 << " " << a_s << " " << a_max << std::endl;
  
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}
```
</details>

<details>

<summary>Second Solution (Test Set 1, 2)</summary>

To generalize from Test Set 1 to Test Set 2, we need to additionally calculate $a_3$. However, we can observe that the concept of $a_3$ is the exact same as for $a_2$, just that now the **Faces of the Delaunay Triangulation** need to be considered instead of the **Edges**.

Here it is vital to know that each face of the Delaunay Triangulation provides an **Empty Circumcircle** for the vertices of that face.
For this circumcircle, by the definition of a circle, the 3 vertices of the face have an equal distance to the center of the circle $c$.
Therefore the distance $d$ for this face can be calculated as the distance from the **circumcenter** $c$ to any of the $3$ vertices of the face.

**Note**: For every face we can take the 3 points of it and define a circle using that. By the definition of a circle, all 3 points will then have the same distance to the center point of that circle, which is the radius, or in our case the distance $d$. Additionally, by the definition of the Delaunay Triangulation it is known that for every face, this Circumcircle is empty. From this we can follow, that there are no other points that are closer to the circumcenter.
Therefore all conditions for a Admissable Set are fulfilled 

**Note**: This requires using Constructions to calculate the Circumcenter for the Face. Therefore we now need to switch from EPIC to EPEC which requires some additional Code adjustments

### Code
```c++
#include <iostream>
#include <vector>
#include <limits>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;
typedef Triangulation::Edge_iterator  Edge_iterator;
typedef Triangulation::Finite_faces_iterator Face_iterator;

typedef K::Point_2 Point;
typedef K::Circle_2 Circle;

// TODO: Maybe use EPEC only for Circumcenter calculation


void solve() {
  // ===== READ INPUT =====
  int n, h; long s; std::cin >> n >> s >> h;
  
  std::vector<Point> trees; trees.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    trees.emplace_back(x, y);
  }
  
  // ===== SOLVE =====
  int a_2, a_3, a_s, a_max; a_2 = a_3 = a_s = a_max = 0;

  a_s = a_max = h; // ! TEST SET 2 ASSUMPTION !
  
  // Triangulate trees
  Triangulation t;
  t.insert(trees.begin(), trees.end());
  
  // Find the minimum distance among the edges and count how many of these edges exist (this yields a_2)
  K::FT min_dist = std::numeric_limits<double>::max();
  for (Edge_iterator e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
    const K::FT dist = CGAL::to_double(t.segment(e).squared_length());
    
    if(dist == min_dist) {
      a_2++;
    } else if(dist < min_dist) {
      min_dist = dist;
      a_2 = 1;
    }
  }
  
  // Find the minimum radius among the faces and count how many of these faces exist (this yields a_3)
  K::FT min_radius = std::numeric_limits<double>::max();
  for (Face_iterator f = t.finite_faces_begin(); f != t.finite_faces_end(); ++f) {
    const Point center = t.circumcenter(f);  // ! REQUIRES CONSTRUCTION !
    const K::FT radius = CGAL::squared_distance(center, t.triangle(f)[0]);
    
    if(radius == min_radius) {
      a_3++;
    } else if(radius < min_radius) {
      min_radius = radius;
      a_3 = 1;
    }
  }
  
  // ===== OUTPUT =====
  std::cout << a_2 << " " << a_3 << " " << a_s << " " << a_max << std::endl;
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

### Calculate $a_s$

The main problem we will face when trying to calculate $a_s$ is that we can no longer assume that, the point lies on the edge between two points $u,  v$.
This is because while the point can lie there, the question is whether it can be moved somewhere, such that it has distance $\sqrt s$ to both points $u, v$ while still fulfilling the constraints

Therefore we now need to consider the question, whether there exists a point $p$ that still fulfills the constraints from before and in addition has a distance $\sqrt d = \sqrt s$.

**Intuition**: Imagine it like this. Before we just set every point to be the midpoint of the edge, since this minimized the distance. However, now minimizing the distance is not the goal. The goal is to check if the point $p$ can be moved to a position where it has distance $\sqrt s$ while still fulfilling the constraints

The main idea to solve this will be to **calculate an interval** $[d_{\min}^{(i)}, d_{\max}^{(i)}]$ for every two points/edge $(u, v)$, which gives **all the possible distances we can place the point $p$ at** while still being **admissible/valid**. <br />
**Intuition**: Instead of asking us the question whether it is possible to place the point $p$ at distance $\sqrt s$ while still being valid, we just find the interval of valid distances $d$ and can then just check if $\sqrt s$ is in that interval or not.

To calculate $d_{\min}$ for an edge $e = (u, v)$, we have 2 options

1. The **Distance** from $u$ or $v$ **to the Midpoint** of the Edge
**Note**: For this we need to make sure that $u$ or $v$ is actually the closes point to the Midpoint, otherwise this is not valid <br />
    **Intuition**: This is the inuitive minimum distance, as we just put the point $p$ perfectly in the middle between $u$ and $v$. However this might not always be valid, as there can be another point in the Triangulation that is closer to that Midpoint (imagine a very “flat” triangle). Therefore we need the 2nd option
    
2. The **Radius** $r$ of the ****Circumcirles of the faces that $e$ belongs to <br />
    **Intuition**: Note that by the Definition of the Delaunay Triangulation/Voronoi Diagram, the circumcenter is the point where all 3 points of a face are equally as close. Therefore going beyond this (going further away than $r$) would be too far.
    

To calculate $d_{\max }$ for an edge $e=(u, v)$ we again have 2 options:

1. The **Radius** $r$ of the ****Circumcircles of the faces that $e$ belongs to <br />
    **Intuition**: Again, this is the maximum distance we can move the point $p$ before it will become closer to another point
    
2. $\infty$ if the Edge is on the **Boundary** of the Triangulation <br />
    **Intuition**: If the Edge is on the Boundary, we can move the point infinitely far away without ever being closer to another point
    

With these intervals in place, we can simply iterate over them and count the number of times $s$ (as we look at squared distances) is in these intervals, which yields $a_s$

### Calculate $a_{\max}$

We can then **reuse these intervals** to calculate $a_{\max }$. Note that finding the distance $d$ for which the amount of admissible/valid edges is maximized is equivalent to finding the $d$ that is as many intervals $[d_{\min}^{(i)}, d_{\max}^{(i)}]$ as possible.

For this we will gather the unique bounds $d_i$  (all $d_{\min}^{(i)}$ and  $d_{\max}^{(i)}$) of all Intervals and count how many intervals are entered and left at each $d_i$. These $d_i$ can then simply be iterated, counting how many are entered and left each time and then we simply take the maximum of all of these

### Code
```c++
#include <iostream>
#include <vector>
#include <limits>
#include <map>
#include <set>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel      K;
typedef std::size_t                                            Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Triangulation;
typedef Triangulation::Edge_iterator                           Edge_iterator;
typedef Triangulation::Finite_faces_iterator                   Face_iterator ;

typedef K::Point_2 Point;
typedef std::pair<Point, Index> IPoint;

int to_edge_idx(int u, int v, int n) {
  if(u < v)
    return u * n + v;
  else
    return v * n + u;
}

void solve() {
  // ===== READ INPUT =====
  int n, h; long s; std::cin >> n >> s >> h;
  
  std::vector<IPoint> trees; trees.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    trees.emplace_back(Point(x, y), i);
  }
  
  // ===== SOLVE =====
  int a_2, a_3, a_s, a_max; a_2 = a_3 = a_s = a_max = 0;

  // Triangulate trees
  Triangulation t;
  t.insert(trees.begin(), trees.end());
  
  // Find the minimum distance among the edges and count how many of these edges exist (this yields a_2)
  K::FT min_dist = std::numeric_limits<double>::max();
  for (Edge_iterator e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e) {
    const K::FT dist = CGAL::to_double(t.segment(e).squared_length());
    
    if(dist == min_dist) {
      a_2++;
    } else if(dist < min_dist) {
      min_dist = dist;
      a_2 = 1;
    }
  }
  
  // Find the minimum radius among the faces and count how many of these faces exist (this yields a_3)
  K::FT min_radius = std::numeric_limits<double>::max();
  for (Face_iterator f = t.finite_faces_begin(); f != t.finite_faces_end(); ++f) {
    const Point center = t.circumcenter(f);  // ! REQUIRES CONSTRUCTION !
    const K::FT radius = CGAL::squared_distance(center, t.triangle(f)[0]);
    
    if(radius == min_radius) {
      a_3++;
    } else if(radius < min_radius) {
      min_radius = radius;
      a_3 = 1;
    }
  }
  
  // For each pair of tree (edge) determine minimum and maximum distance from the trees (this yields a_s)
  std::map<int, K::FT> d_min; // Edge Index -> Minimum Distance that a point p can be distant from u, v without being closer to another point
  std::map<int, K::FT> d_max; // Edge Index -> Maximum Distance that a point p can be distant from u, v without being closer to another point
  for(auto f = t.all_faces_begin(); f != t.all_faces_end(); f++) {
    if(t.is_infinite(f)) {
      // Set d_max to infinite for infinite edges
      for(int i = 0; i < 3; i++) {
        if(t.is_infinite(f->vertex(i))) {
          int idx_1 = f->vertex((i+1)%3)->info();
          int idx_2 = f->vertex((i+2)%3)->info();
          d_max[to_edge_idx(idx_1, idx_2, n)] = std::numeric_limits<double>::max();
        }
      }
    } else {
      // Calculate d_min and d_max based on the radius of the circumcircle and distance of the edge
      for(int i = 0; i < 3; i++) {
        auto v_1 = f->vertex((i+1)%3);
        auto v_2 = f->vertex((i+2)%3);
        int edge_idx = to_edge_idx(v_1->info(), v_2->info(), n);
        
        Point circumcenter = t.dual(f);
        K::FT radius = CGAL::squared_distance(v_1->point(), circumcenter);

        // Update based on radius
        if(d_min.find(edge_idx) == d_min.end() || radius < d_min[edge_idx]) d_min[edge_idx] = radius;
        if(d_max.find(edge_idx) == d_max.end() || radius > d_max[edge_idx]) d_max[edge_idx] = radius;

        // Update based on edge length
        // Note that we can choose d_min as half of the edge length, only if there is not another point closer to that midpoint
        Point midpoint = CGAL::midpoint(v_1->point(), v_2->point());
        Point closest = t.nearest_vertex(midpoint)->point();
        if(closest == v_1->point() || closest == v_2->point())
          d_min[edge_idx] = CGAL::squared_distance(midpoint, v_1->point());
        
      }
    }
  }
  
  // Check for how many edges d_min < s < d_max
  K::FT s_exact(s); 
  for(auto it = d_min.begin(); it != d_min.end(); it++) {
    auto edge = it->first;
    if(d_min[edge] <= s_exact && s_exact <= d_max[edge])
      a_s++;
  }
  
  // Calculate a_max by iterating over the d_min and d_max and determining the d that is contained in the most intervals
  std::map<K::FT, int> enter_count;
  std::map<K::FT, int> leave_count;
  std::set<K::FT> unique_bounds;
  
  // Find all unique d_min and d_max and record how many interavls are entered and how many are left at d_min and d_max
  for (const auto& entry : d_min) {
      auto edge = entry.first;
      K::FT min_val = d_min[edge];
      K::FT max_val = d_max[edge];
  
      enter_count[min_val]++;
      leave_count[max_val]++;
  
      unique_bounds.insert(min_val);
      unique_bounds.insert(max_val);
  }
  
  // Iterate over all possible bounds d_i and find the one that overlaps the most intervals
  int current_active = 0;
  for (const auto& bound : unique_bounds) {
      current_active += enter_count[bound];
      a_max = std::max(a_max, current_active);
      current_active -= leave_count[bound];
  }
  
  // ===== OUTPUT =====
  std::cout << a_2 << " " << a_3 << " " << a_s << " " << a_max << std::endl;
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
   Test set 1 (30 pts / 2 s) : Correct answer      (0.901s)
   Test set 2 (30 pts / 2 s) : Correct answer      (0.868s)
   Test set 3 (20 pts / 2 s) : Correct answer      (0.776s)
   Test set 4 (20 pts / 2 s) : Correct answer      (0.777s)

Total score: 100
```