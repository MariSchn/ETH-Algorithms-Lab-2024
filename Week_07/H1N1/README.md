# H1N1

## 📝 Problem Description

Given a set of $N$ points in the plane, the task is to answer $M$ independent queries. Each query specifies a starting location $(x, y)$ and a minimum required separation distance $d$. For each query, determine whether there exists a continuous path beginning at $(x, y)$ that can extend to infinity, such that every point along the path maintains a distance of at least $\sqrt{d}$ from all of the $N$ given points.

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


This problem is a classic instance of motion planning: we must determine whether a disk of radius $\sqrt{d}$, centered at a query point, can escape to infinity while always maintaining at least distance $d$ from a given set of points.

To solve this, we use the **Delaunay Triangulation** of the point set. Conceptually, the dual structure, the **Voronoi Diagram**, is relevant, as the optimal escape paths for the disk follow the Voronoi edges, which maximize the distance to the nearest points. However, for efficiency, we only construct and use the Delaunay triangulation, since explicit Voronoi construction is computationally more expensive.

The key idea is to **precompute the largest disk** that can escape from each face (triangle) of the Delaunay triangulation. Once this is done, each query can be answered in constant time.

The precomputation proceeds as follows:

1. **Initialization:**
  - All infinite (boundary) faces are considered to be outside the point set. Any disk in such a face has already escaped, so the largest disk that can escape from these faces is set to `std::numeric_limits<double>::max()`.

2. **Propagation:**
  - Perform a breadth-first search (BFS) over all faces, starting from the infinite faces.
  - For each face, propagate the maximum disk size to its neighbors. The maximum disk size that can escape from a neighboring face is the minimum of:
    - The maximum disk size that can escape from the current face.
    - The length of the edge connecting the current face and the neighbor.
  - This ensures that the escape path is always feasible: the disk must fit through every edge along its path to the outside.

By repeating this process for all faces, we obtain for each triangle the largest disk size that can escape from it.

To answer a query, we proceed as follows:

1. Check that the query point is not closer than $d$ to any point in the set (otherwise, escape is impossible).
2. Locate the triangle containing the query point and compare the required disk size to the precomputed maximum for that triangle. If the required disk size is less than or equal to the maximum, escape is possible; otherwise, it is not.

### Implementation Details

- We store the maximum escape radius for each face in the face's info field.
- To efficiently manage the propagation of escape clearances, we utilize a priority queue (max-heap) to always expand the most promising face first.

### Code
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

## 🧠 Learnings

<details> 

<summary> Expand to View </summary>

- Even if its not feasible to use, thinking in terms of the Voronoi Diagram in a Delaunay Triangulation task can be very helpful
- In a Delaunay Triangulation we can directly store information in the faces using the `info` field.

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