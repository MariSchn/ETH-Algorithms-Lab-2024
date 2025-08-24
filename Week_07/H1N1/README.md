# H1N1

## 📝 Problem Description

Given a set of $N$ points in a 2D plane, you are tasked with answering $M$ independent queries. Each query provides a starting location $(x,y)$ and a required minimum separation distance, which we will denote by a value $d$. For each query, you must determine if it is possible to find a continuous path starting from $(x,y)$ that can extend infinitely far away, such that at every point along this path, the distance to any of the $N$ initial points is at least $\sqrt{d}$.

The output for each test case must be a single string containing $M$ characters. The $i$-th character should be 'y' if an escape path exists for the $i$-th query, and 'n' otherwise.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem requires finding a "safe" path. A path is safe if it maintains a certain minimum distance from a set of fixed points. Where would such a path lie? Intuitively, to maximize safety, one should travel as far as possible from all points. This suggests considering the regions of the plane that are equidistant from the nearest two or three fixed points.
</details>
<details>
<summary>Hint #2</summary>
Let's rephrase the problem. A person at location $P$ needing to maintain a distance of at least $\sqrt{d}$ is equivalent to a disk of radius $\sqrt{d}$ centered at $P$ not being allowed to contain any of the fixed points. The question is then: can this disk move from its initial position to infinity without ever "colliding" with any of the fixed points? This is a classic motion planning problem. The "highways" for such movement are defined by the Voronoi diagram of the fixed points, as its edges are maximally distant from the nearest sites.
</details>
<details>
<summary>Hint #3</summary>
Solving each query from scratch would be too slow given the constraints. This points towards a precomputation strategy. We can partition the plane into regions and, for each region, precompute the maximum size of a disk that can escape from it. The Delaunay triangulation, which is the dual of the Voronoi diagram, provides a convenient partitioning of the plane into triangles. The "bottlenecks" for movement between adjacent triangles are the shared Delaunay edges. This suggests an algorithm that propagates "escape clearance" values from the outside inwards through the faces of the triangulation.
</details>
<details>
<summary>Hint #4</summary>
The process of propagating escape clearances can be modeled as a shortest path algorithm on the dual graph of the triangulation, where faces are nodes. Since we want to find the path that allows the *largest* disk to pass, we need to maximize the minimum clearance along the path. This is a "widest path" or "bottleneck shortest path" problem. A Dijkstra-like algorithm using a max-priority queue is perfectly suited for this. Start the search from the infinite faces, which have infinite clearance, and explore inwards.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
This problem is a classic example of motion planning. We need to determine if a person, who can be modeled as a point, can find a path from a starting location to "infinity" while always maintaining a minimum distance from a set of fixed "infected" points.

### Core Idea: Disk Escape and Precomputation

The condition of maintaining a distance of at least $\sqrt{d}$ from all infected points is equivalent to saying that a disk of radius $\sqrt{d}$ centered at the person's location must never contain any of the infected points. The problem then becomes: can this disk move from its initial position to an unbounded region of the plane?

Since we have many queries for the same set of infected points, a precomputation approach is ideal. We can analyze the structure of the plane defined by the infected points and, for every region, determine the maximum size of a disk that can escape from it.

### Geometric Framework: Delaunay Triangulation

The paths that stay maximally far from a set of points are the edges of the **Voronoi diagram**. The center of our escaping disk would ideally travel along these edges. However, working with the Voronoi diagram's dual, the **Delaunay triangulation**, is often computationally more convenient. The vertices of the Delaunay triangulation are the infected points themselves.

The key insight is to consider the faces (triangles) of the Delaunay triangulation. Movement between adjacent triangles is only possible by crossing their shared edge. This shared edge acts as a bottleneck. A disk can only pass through this "gap" if its diameter is less than or equal to the length of the edge.

To avoid floating-point inaccuracies and expensive `sqrt` operations, we will work with **squared distances**. Let the input value `d` be the required *squared* distance. Our disk has a squared radius of $d$.

### Algorithm: Widest Path on the Dual Graph

We can determine the escape capability of each face using an algorithm similar to Dijkstra's on the dual graph of the triangulation (where faces are nodes). Our goal is to find the "widest" path from each face to the outside, i.e., the path that maximizes the minimum clearance.

1.  **Model:**
    *   **Nodes:** The faces of the Delaunay triangulation.
    *   **"Weight":** For each face `f`, we want to compute `f->info()`, which will store the squared *diameter* of the largest disk that can escape from `f`. The squared diameter is used because it corresponds directly to the squared length of the bottleneck edge.
    *   **Edges:** Adjacency between faces. The capacity of the "passage" between two faces is the squared length of their shared Delaunay edge.

2.  **Initialization:**
    *   We use a max-priority queue to manage faces to visit. It will store pairs of `(escape_clearance, face_handle)`.
    *   The infinite faces of the triangulation are already "outside". Any disk within them has escaped. Therefore, they have an infinite escape clearance. We add all infinite faces to the priority queue with a symbolic infinite clearance.
    *   We initialize the `info` for all finite faces to 0.

3.  **Execution (Dijkstra-like process):**
    *   While the priority queue is not empty, extract the entry `(D_sq, F)` with the highest clearance `D_sq`.
    *   If we have already found a better or equal path for face `F`, we can skip it. Otherwise, we set `F->info() = D_sq`.
    *   For each neighbor `N` of `F` across a shared edge `e`:
        *   The squared length of `e` is the maximum squared diameter that can pass between `F` and `N`.
        *   The maximum clearance for a path from `N` to the outside via `F` is `min(D_sq, squared_length(e))`.
        *   If this new clearance is greater than the current `N->info()`, we update `N`'s potential and add it to the priority queue.

After this process completes, `face->info()` for every face will hold the maximum squared diameter of a disk that can escape from that region.

### Answering Queries

With the precomputed values, each query can be answered quickly:

1.  For a query at point `p` with required squared distance `d`:
2.  **Initial Position Check:** First, find the infected point `v` nearest to `p`. If the squared distance between `p` and `v` is less than `d`, the person is already in a forbidden zone. The answer is 'n'.
3.  **Escape Path Check:** If the initial position is valid, locate the Delaunay face `f` that contains `p`.
    *   The precomputed value `f->info()` tells us the maximum squared diameter of a disk that can escape from this face.
    *   Our disk has a squared radius of `d`, which means its squared diameter is $(2 \times \text{radius})^2 = 4 \times (\text{radius})^2 = 4d$.
    *   Escape is possible if and only if the required squared diameter is no more than the available clearance: `4*d <= f->info()`.
    *   If the condition holds, the answer is 'y'; otherwise, it's 'n'.

```cpp
#include <iostream>
#include <vector>
#include <queue>
#include <limits>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Triangulation_face_base_with_info_2<double, K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb> Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds> Triangulation;

typedef Triangulation::Face_handle FaceHandle;
typedef Triangulation::Vertex_handle VertexHandle;
typedef K::Point_2 Point;

int main() {
  std::ios_base::sync_with_stdio(false);
  
  while(true) {
    // ===== READ INPUT =====
    int n, m; 
    
    // Read infected people
    std::cin >> n;
    if(n == 0) { break; } // Terminate when "0" is read
    
    std::vector<Point> infected_points; infected_points.reserve(n);
    for(int i = 0; i < n; ++i) {
      double x, y; std::cin >> x >> y;
      infected_points.push_back(Point(x, y));
    }
    
    // Read query people
    std::cin >> m;
    
    std::vector<Point> query_points; query_points.reserve(m);
    std::vector<double> query_distances; query_distances.reserve(m);
    for(int i = 0; i < m; ++i) {
      int x, y; std::cin >> x >> y;
      double d; std::cin >> d;
      
      query_points.push_back(Point(x, y));
      query_distances.push_back(d);
    }
    
    // ===== PREPROCESSING =====
    Triangulation t;
    t.insert(infected_points.begin(), infected_points.end());
    
    // Precompute the maximum distance for which a disk can escape for each face
    std::priority_queue<std::pair<double, FaceHandle>> q;
    
    // Add all the infinite faces to the priority queue, as from them any disk can escape
    for (Triangulation::All_faces_iterator f = t.all_faces_begin(); f != t.all_faces_end(); ++f) {
      if(t.is_infinite(f)) {
        q.emplace(std::numeric_limits<double>::max(), f);
      }
      
      // Initialize all max distances to 0
      f->info() = 0;
    }
    
    // Calculate the max distance for each face using a BFS over faces
    while(!q.empty()) {
      auto top = q.top(); q.pop();
      auto distance = top.first;
      auto face_handle = top.second;
      
      // We are visiting a face, that we have previously already visited with a "cheaper" distance. Therefore we can simply skip it
      if(face_handle->info() >= distance) { continue; }
  
      // Set the maximum distance, that can escape from the current face
      face_handle->info() = distance;
      
      // Go over to neighbors
      for(int i = 0; i < 3; ++i) {
        auto neighbor = face_handle->neighbor(i);
        
        // Skip neighbors that have already been visited
        if(t.is_infinite(neighbor) || neighbor->info() != 0) { continue; }
      
        // Calculate the maximum distance that can pass to this neighbor
        auto v1 = face_handle->vertex((i+1) % 3) -> point();
        auto v2 = face_handle->vertex((i+2) % 3) -> point();
          
        double min_distance = std::min(CGAL::squared_distance(v1, v2), distance);
        q.emplace(min_distance, neighbor);
      }
    }
    
    // ===== ANSWER QUERIES =====
    for(int i = 0; i < m; ++i) {
      Point p = query_points[i];
      double d = query_distances[i];
      
      FaceHandle face = t.locate(p);
      VertexHandle vertex = t.nearest_vertex(p);
      
      if(CGAL::squared_distance(vertex->point(), p) < d) { std::cout << "n"; }
      else {
        if(face->info() >= 4 * d) { std::cout << "y"; }
        else { std::cout << "n"; }
      }
    }
    
    std::cout << std::endl;
  }
}
```

</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (20 pts / 1 s) : Correct answer      (0.005s)
   Test set 2 (20 pts / 1 s) : Correct answer      (0.05s)
   Test set 3 (20 pts / 1 s) : Correct answer      (0.367s)
   Test set 4 (20 pts / 1 s) : Correct answer      (0.576s)
   Test set 5 (20 pts / 1 s) : Correct answer      (0.494s)

Total score: 100
```