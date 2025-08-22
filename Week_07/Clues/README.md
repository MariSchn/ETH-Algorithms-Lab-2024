# Clues

## 📝 Problem Description

You are given a set of $n$ fixed station locations and a communication range $r$. Two clients (which can be stations or personal radio sets) are considered "in range" if the Euclidean distance between them is at most $r$.

The stations must be assigned one of two available frequencies for sending signals. To prevent interference, any two stations that are in range of each other must be assigned different frequencies.

Your first task is to determine if such a frequency assignment for the stations is possible. If it is not, no communication is possible.

If a valid assignment exists, you must then process $m$ independent clues. Each clue is defined by a pair of locations, $(a_i, b_i)$, representing the positions of two agents. For each clue, you need to determine if a communication path can be established between $a_i$ and $b_i$. A path is possible under the following conditions:
1.  A direct path exists if the distance between $a_i$ and $b_i$ is at most $r$.
2.  An indirect path exists through the network of stations. An agent at location $p$ connects to the single closest station, provided that station is within range $r$. A path is then formed if agent $a_i$ can connect to a station $s_a$, agent $b_i$ can connect to a station $s_b$, and there is a sequence of stations connecting $s_a$ to $s_b$, where each consecutive pair of stations in the sequence is within range $r$.

For each of the $m$ clues, you must output 'y' if a routable path exists (which requires both a valid frequency assignment and a communication path), and 'n' otherwise. The final output for each test case should be a single string of $m$ characters.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem involves two main checks: one for interference-free frequency assignment and another for connectivity. Let's focus on the first part. The stations have two available frequencies, and any two stations within range $r$ must use different ones. How can you model the relationship between stations based on their distance? What does the two-frequency constraint imply about this model?
</details>
<details>
<summary>Hint #2</summary>
The relationships between stations can be modeled as a graph. Let each station be a vertex. An edge exists between two vertices if the distance between the corresponding stations is at most $r$. The problem of assigning one of two frequencies so that no two adjacent stations share a frequency is a classic graph problem. This is equivalent to checking if the graph can be colored with two colors, which means you need to check if the graph is **bipartite**.
</details>
<details>
<summary>Hint #3</summary>
Constructing the station graph by checking all $O(N^2)$ pairs of stations is too slow. To do this more efficiently, you can use a geometric data structure like a **Delaunay triangulation**. It connects points that are geometrically close. However, be careful: simply including all Delaunay edges with length at most $r$ is **not sufficient**. Two stations might be within range $r$ but not be directly connected by a Delaunay edge. To find all pairs within distance $r$, you must use the triangulation to guide a more thorough search, for instance, by exploring the local neighborhood of each station.
</details>

## ✨ Solutions

<details>
<summary>Approach for Test Set 1: Interference Checking</summary>

For the first test set, we are given that $m=1$ and the start and end positions of the clue, $a_0$ and $b_0$, are identical. This means connectivity is guaranteed, and the problem reduces to a single question: **can the stations be assigned frequencies without interference?**

### Modeling as a Graph Problem

As described in the hints, this problem can be modeled using a graph. Let each of the $n$ stations be a vertex. We draw an undirected edge between two vertices if the distance between their corresponding stations is less than or equal to the communication range $r$.

The constraint is that any two stations in range of each other must use different frequencies. Since there are only two frequencies available, this is equivalent to asking if we can color the vertices of our graph with two colors such that no two adjacent vertices have the same color. A graph with this property is called a **bipartite graph**.

### Efficient Graph Construction

A naive approach to building this "interference graph" would be to check the distance between every pair of stations, which would take $O(N^2)$ time and be too slow for the given constraints.

We can significantly speed this up using a **Delaunay Triangulation**. This data structure efficiently partitions the space and connects points that are geometric neighbors. While it's true that not all pairs of stations within range $r$ will be connected by an edge in the triangulation, the triangulation provides a highly effective "road map" to find these pairs.

The solution implements a robust search strategy:
1.  Construct a Delaunay triangulation of all station locations.
2.  For each station $u$, perform a search (similar to a Breadth-First Search) starting from $u$ and traversing the edges of the triangulation.
3.  This search explores the neighborhood of $u$. Any station $v$ that is encountered during this search and satisfies `distance(u, v) <= r` is a neighbor of $u$ in our interference graph. An edge $\{u, v\}$ is then added.
4.  By repeating this for every station, we can construct the complete interference graph far more efficiently than the brute-force method.

### Checking for Bipartiteness

Once the graph `G` is constructed, we can check if it is bipartite. A standard algorithm for this involves a graph traversal (like BFS or DFS) that attempts to assign one of two colors to each vertex. If at any point it finds an edge connecting two vertices that must have the same color, the graph is not bipartite. The Boost Graph Library provides a convenient `is_bipartite(G)` function that handles this check for us.

For Test Set 1, if the graph is bipartite, the answer is 'y'; otherwise, it's 'n'.

```cpp
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <set>
#include <queue>

// ===== BOOST INCLUDES AND TYPEDEFS =====
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bipartite.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> graph;

// ===== CGAL INCLUDES AND TYPEDEFS =====
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef int                                                    Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;

typedef Tds::Vertex_handle          Vertex_handle;
typedef Delaunay::Edge_iterator     Edge_iterator;
typedef Delaunay::Vertex_iterator   Vertex_iterator;
typedef Delaunay::Vertex_circulator Vertex_circulator;

typedef K::Point_2               Point;
typedef std::pair<Point, Index> IPoint;


void solve() {
  // ===== READ INPUT =====
  int n, m;
  long r;
  std::cin >> n >> m >> r;
  long r_squared = r * r;
  
  std::vector<IPoint> stations; stations.reserve(n);
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    stations.emplace_back(Point(x, y), i);
  }
  
  // For Test Set 1, we can ignore the clue positions as a_i = b_i
  for(int i = 0; i < m; ++i) {
    int a_x, a_y, b_x, b_y; std::cin >> a_x >> a_y >> b_x >> b_y;
  }
  
  // ===== SOLVE =====
  // Build the interference graph using a Delaunay triangulation to guide the search.
  Delaunay t;
  t.insert(stations.begin(), stations.end());
  
  graph G(n);
  for (Vertex_iterator v = t.finite_vertices_begin(); v != t.finite_vertices_end(); ++v) {
    // Perform a localized search for each vertex to find all neighbors within radius r.
    std::queue<Vertex_handle> q;
    std::set<Vertex_handle> visited;
    
    q.push(v);
    visited.insert(v);
    
    while(!q.empty()) {
      Vertex_handle curr_v = q.front(); q.pop();
      
      // An edge exists if curr_v is reachable and within range.
      if(v->info() != curr_v->info()) {
        boost::add_edge(v->info(), curr_v->info(), G);
      }
      
      // Explore neighbors in the triangulation.
      Vertex_circulator neighbor_v = t.incident_vertices(curr_v);
      do {
        // Continue search if neighbor is valid, unvisited, AND within range of the original vertex v.
        if(
            !t.is_infinite(neighbor_v) &&
            visited.find(neighbor_v) == visited.end() &&
            CGAL::squared_distance(v->point(), neighbor_v->point()) <= r_squared
          ) {
            q.push(neighbor_v);
            visited.insert(neighbor_v);
          }
      } while(++neighbor_v != t.incident_vertices(curr_v));
    }
  }
  
  // A valid frequency assignment is possible if and only if the graph is bipartite.
  if(boost::is_bipartite(G)) {
    // For TS1, m=1, so we just print one character.
    std::cout << "y" << std::endl;
  } else {
    std::cout << "n" << std::endl;
  }
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
<summary>Full Solution</summary>

The full solution combines the interference check with a connectivity check for each clue. The overall algorithm is structured to be highly efficient by pre-computing properties of the station network before processing the individual clues.

### Algorithm Outline

1.  **Build Interference Graph:** Construct the graph of stations where an edge connects stations within distance $r$, using the efficient Delaunay triangulation-based search from the first solution.
2.  **Check for Interference:** Check if this graph is bipartite. If it is **not**, no valid frequency assignment exists. Therefore, no clue can be routed. We can immediately print a string of $m$ 'n's and finish.
3.  **Pre-compute Connectivity:** If the graph is bipartite, a valid frequency assignment exists. The station network is usable. We now need to determine which stations can communicate with each other. Two stations can communicate if there is a path of stations between them. This is equivalent to them being in the same **connected component** of the interference graph. We can pre-compute these components for all stations in one pass using `boost::connected_components`. This gives us a `component_map` array, where `component_map[i]` stores the component ID of station $i$.
4.  **Process Each Clue:** For each clue $(a_i, b_i)$, we check for a routable path:
    *   **Case A: Direct Connection.** First, check if Holmes and Watson are in range of each other directly. If `distance(a_i, b_i) <= r`, a path exists. The answer for this clue is 'y'.
    *   **Case B: Station-Mediated Connection.** If not directly connected, they must use the station network. This requires three conditions to be met simultaneously:
        1.  Holmes must be able to reach the network. Find the station $s_a$ closest to $a_i$. This must satisfy `distance(a_i, s_a) <= r`.
        2.  Watson must be able to reach the network. Find the station $s_b$ closest to $b_i$. This must satisfy `distance(b_i, s_b) <= r`.
        3.  The entry points must be connected. Stations $s_a$ and $s_b$ must be in the same connected component. We can check this in $O(1)$ with our pre-computed map: `component_map[s_a_index] == component_map[s_b_index]`.

        The closest stations can be found efficiently using the `nearest_vertex` method of the Delaunay triangulation. If all three conditions hold, the answer is 'y'.
    *   **Case C: No Connection.** If neither of the above cases apply, no path exists. The answer is 'n'.

This approach is efficient because the expensive graph construction and component analysis are done only once. Each of the $m$ clues can then be processed very quickly.

_Note:_ Using squared distances throughout the implementation avoids costly `sqrt` operations and floating-point precision errors.

```cpp
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <set>
#include <queue>

// ===== BOOST INCLUDES AND TYPEDEFS =====
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bipartite.hpp>
#include <boost/graph/connected_components.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> graph;

// ===== CGAL INCLUDES AND TYPEDEFS =====
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef int                                                    Index;
typedef CGAL::Triangulation_vertex_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;

typedef Tds::Vertex_handle          Vertex_handle;
typedef Delaunay::Edge_iterator     Edge_iterator;
typedef Delaunay::Vertex_iterator   Vertex_iterator;
typedef Delaunay::Vertex_circulator Vertex_circulator;

typedef K::Point_2               Point;
typedef std::pair<Point, Index> IPoint;


void solve() {
  // ===== READ INPUT =====
  int n, m;
  long r;
  std::cin >> n >> m >> r;
  long r_squared = r * r;
  
  std::vector<IPoint> stations; stations.reserve(n);
  std::vector<Point> holmes_positions; holmes_positions.reserve(m);
  std::vector<Point> watson_positions; watson_positions.reserve(m);
  
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    stations.emplace_back(Point(x, y), i);
  }
  for(int i = 0; i < m; ++i) {
    int a_x, a_y, b_x, b_y; std::cin >> a_x >> a_y >> b_x >> b_y;
    holmes_positions.emplace_back(a_x, a_y);
    watson_positions.emplace_back(b_x, b_y);
  }
  
  // ===== SOLVE =====
  Delaunay t;
  t.insert(stations.begin(), stations.end());
  
  graph G(n);
  for (Vertex_iterator v = t.finite_vertices_begin(); v != t.finite_vertices_end(); ++v) {
    std::queue<Vertex_handle> q;
    std::set<Vertex_handle> visited;
    
    q.push(v);
    visited.insert(v);
    
    while(!q.empty()) {
      Vertex_handle curr_v = q.front(); q.pop();
      
      if(v->info() != curr_v->info()) {
        boost::add_edge(v->info(), curr_v->info(), G);
      }
      
      Vertex_circulator neighbor_v = t.incident_vertices(curr_v);
      do {
        if(
            !t.is_infinite(neighbor_v) &&
            visited.find(neighbor_v) == visited.end() &&
            CGAL::squared_distance(v->point(), neighbor_v->point()) <= r_squared
          ) {
            q.push(neighbor_v);
            visited.insert(neighbor_v);
          }
      } while(++neighbor_v != t.incident_vertices(curr_v));
    }
  }
  
  // If not bipartite, no solution is possible for any clue.
  if(!boost::is_bipartite(G)) {
    std::cout << std::string(m, 'n') << std::endl;
    return;
  }
  
  // Pre-compute connected components for fast lookups.
  std::vector<int> component_map(n);
  boost::connected_components(G, &component_map[0]);
  
  // Check each clue for connectivity.
  for(int i = 0; i < m; ++i) {
    Point a = holmes_positions[i];
    Point b = watson_positions[i];
    
    // Case A: Direct connection.
    if(CGAL::squared_distance(a, b) <= r_squared) {
      std::cout << "y";
      continue;
    }
    
    // Case B: Station-mediated connection.
    Vertex_handle neighbor_a = t.nearest_vertex(a);
    Vertex_handle neighbor_b = t.nearest_vertex(b);
    
    // Check if both agents are close enough to their nearest stations,
    // and if those stations are in the same component.
    if(neighbor_a != Vertex_handle() && // check if any station exists
       CGAL::squared_distance(a, neighbor_a->point()) <= r_squared && 
       CGAL::squared_distance(b, neighbor_b->point()) <= r_squared && 
       component_map[neighbor_a->info()] == component_map[neighbor_b->info()]) {
        std::cout << "y";   
     } else {
        std::cout << "n";
     }
  }
  std::cout << std::endl;
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

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (20 pts / 2 s) : Correct answer      (0.106s)
   Test set 2 (30 pts / 2 s) : Correct answer      (0.824s)
   Test set 3 (30 pts / 2 s) : Correct answer      (1.11s)
   Test set 4 (20 pts / 2 s) : Correct answer      (1.426s)

Total score: 100
```