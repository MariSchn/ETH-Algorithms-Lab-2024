# Bistro

## 📝 Problem Description

This problem asks us to work with two sets of points in a 2D plane. We are given an initial set of $N$ points, representing existing restaurant locations. Subsequently, we are given a second set of $M$ query points, representing potential locations for a new restaurant.

For each of the $M$ potential new locations, the task is to find the closest existing restaurant. The final output for each query point should be the squared Euclidean distance to its nearest neighbor in the set of existing restaurants.

## 💡 Hints

<details>
<summary>Hint #1</summary>
Consider the most straightforward approach. For a single potential new location, how would you find the distance to the closest existing restaurant? You would likely need to compare its distance to every single one of the $N$ existing locations. Now, consider that you have to repeat this process for all $M$ potential locations. What would be the total time complexity of this naive method? Is it efficient enough given the potential size of the input?
</details>
<details>
<summary>Hint #2</summary>
The problem of repeatedly finding the closest point in a fixed set to a series of query points is a classic computational geometry problem known as the **Nearest Neighbor Search**. A brute-force approach is often too slow. To speed this up, we can preprocess the initial set of $N$ points into a specialized data structure that organizes them spatially, allowing for much faster queries.
</details>
<details>
<summary>Hint #3</summary>
A powerful data structure for solving proximity problems on a set of points is the **Delaunay Triangulation**. After constructing a Delaunay triangulation from the set of existing restaurant locations, finding the nearest neighbor for any new query point becomes a very efficient operation. Libraries like the Computational Geometry Algorithms Library (CGAL) provide robust implementations of these geometric structures.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
The core of this problem is to efficiently answer multiple nearest neighbor queries. We are given a set of $N$ points (existing restaurants) and a set of $M$ query points (potential new locations). For each query point, we need to find the point in the initial set that is closest to it.

### Brute-Force Approach (and why it's too slow)

A naive solution would be to iterate through all $N$ existing restaurants for each of the $M$ query points. This involves calculating $N \cdot M$ distances. With $N$ and $M$ up to $110,000$, this approach would have a time complexity of $O(N \cdot M)$, which is too slow and would not pass within the time limit.

### An Efficient Approach using Delaunay Triangulation

To solve this problem efficiently, we can preprocess the $N$ existing restaurant locations by building a spatial data structure. A **Delaunay Triangulation** is an excellent choice for this task.

1.  **Construction:** We first insert all $N$ points representing the existing restaurants into a Delaunay triangulation. This partitions the plane into triangles whose vertices are the input points. The construction itself is efficient.

2.  **Querying:** The key advantage of a Delaunay triangulation is that it enables very fast nearest neighbor searches. For any given query point, the structure can quickly identify the vertex (an existing restaurant) that is closest to it. This query operation is, on average, very fast, making the overall approach highly performant.

### Implementation with CGAL

The Computational Geometry Algorithms Library (CGAL) provides a robust and easy-to-use implementation of Delaunay triangulations.

*   **Data Types:** We use `CGAL::Exact_predicates_inexact_constructions_kernel` as our geometric kernel. This kernel is crucial because the input coordinates can be large (up to $2^{24}$). It uses exact arithmetic for geometric predicates (like orientation tests) to avoid errors and ensure the triangulation is constructed correctly, while using faster floating-point arithmetic for constructions and distance calculations. The squared distance calculation will be exact for integer coordinates up to $2^{24}$.
*   **Triangulation:** We use `CGAL::Delaunay_triangulation_2` to store the points of the existing restaurants.
*   **Algorithm:**
    1.  Read the $N$ existing restaurant locations and insert them into the `Triangulation` object.
    2.  For each of the $M$ potential new locations:
        a. Use the `t.nearest_vertex(query_point)` method to find a handle to the closest vertex in the triangulation.
        b. Retrieve the point associated with that vertex.
        c. Calculate the squared Euclidean distance between the query point and the found closest point using `CGAL::squared_distance`.
        d. Print the result.

This method reduces the complexity from the inefficient $O(N \cdot M)$ to roughly $O(N \log N + M \log N)$, which is well within the time limits.

```cpp
#include <iostream>
#include <vector>
#include <iomanip>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

// Define CGAL kernel and data types
using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Triangulation = CGAL::Delaunay_triangulation_2<K>;
using Point = K::Point_2;

void solve() {
  int n;
  std::cin >> n;
  if (n == 0) exit(0); // Terminate program if n is 0

  // Read existing restaurant locations
  std::vector<Point> restaurants(n);
  for (int i = 0; i < n; ++i) {
    long x, y; // Use long to handle large coordinates
    std::cin >> x >> y;
    restaurants[i] = Point(x, y);
  }

  // Construct the Delaunay triangulation from existing restaurant locations
  Triangulation t;
  t.insert(restaurants.begin(), restaurants.end());

  int m;
  std::cin >> m;
  
  // Set output to fixed-point notation for large numbers
  std::cout << std::setprecision(0) << std::fixed;

  // Process each query for a new restaurant location
  for (int i = 0; i < m; ++i) {
    long x, y;
    std::cin >> x >> y;
    Point query_point(x, y);

    // Find the nearest existing restaurant (vertex in the triangulation)
    Point closest_point = t.nearest_vertex(query_point)->point();
    
    // Compute and print the squared distance
    K::FT distance = CGAL::squared_distance(query_point, closest_point);
    std::cout << distance << std::endl;
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  while (true) {
    solve();
  }
  return 0;
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (20 pts / 0.600 s) : Correct answer      (0.026s)
   Test set 2 (20 pts / 0.600 s) : Correct answer      (0.161s)
   Test set 3 (20 pts / 0.600 s) : Correct answer      (0.14s)
   Test set 4 (20 pts / 0.600 s) : Correct answer      (0.212s)
   Test set 5 (20 pts / 0.600 s) : Correct answer      (0.459s)

Total score: 100
```