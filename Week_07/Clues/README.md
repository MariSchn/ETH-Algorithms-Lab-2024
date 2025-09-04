# Clues

## 📝 Problem Description

Given a set of $n$ fixed station locations and a communication range $r$, two clients (which may be stations or personal radio sets) are considered "in range" if the Euclidean distance between them does not exceed $r$.

Each station must be assigned one of two available frequencies for sending signals. To prevent interference, any two stations that are in range of each other must be assigned different frequencies.

The initial task is to determine whether such a frequency assignment for the stations is possible. If it is not, communication cannot be established.

If a valid assignment exists, the problem proceeds to $m$ independent clues. Each clue consists of a pair of locations, $(a_i, b_i)$, representing the positions of two agents. For each clue, the goal is to determine whether a communication path can be established between $a_i$ and $b_i$. A path is possible under the following conditions:
1. A direct path exists if the distance between $a_i$ and $b_i$ is at most $r$.
2. An indirect path exists via the network of stations. An agent at location $p$ connects to the single closest station, provided that station is within range $r$. A path is then formed if agent $a_i$ can connect to a station $s_a$, agent $b_i$ can connect to a station $s_b$, and there is a sequence of stations connecting $s_a$ to $s_b$, where each consecutive pair of stations in the sequence is within range $r$.

## 💡 Hints

<details>
<summary>Hint #1</summary>

Try modeling the stations and their communication range as a graph. How can a Delaunay triangulation help you efficiently determine which stations are close enough to potentially interfere?
</details>

<details>

</summary>Hint #2</summary>

Once you have the triangulation, you might think to simply iterate over each station's Delaunay neighbors. However, this is not enough: some stations within range $r$ may not be directly connected by a Delaunay edge. To ensure you find all relevant pairs, perform a breadth-first search (BFS) from each station, traversing the triangulation and connecting every station within range $r$.
</details>

<details>
<summary>Hint #3</summary>

After building the graph you can simply check if it is bipartite, to check for interferences.
</details>

<details>
<summary>Hint #4</summary>

For each clue, how can you quickly determine if two agents can communicate through the network? Think about using connected components to efficiently answer these queries.
</details>

## ✨ Solutions

<details>

<summary>First Solution (Test Set 1)</summary>

For the first test set, we are given that $m=1$ and the start and end positions of the clue, $a_0$ and $b_0$, are identical. This means connectivity is guaranteed, and the problem reduces to a single question: **can the stations be assigned frequencies without interference?**

### Modeling as a Graph Problem

As described in the hints, this problem can be modeled using a graph. Let each of the $n$ stations be a node. We draw an undirected edge between two nodes if the distance between their corresponding stations is less than or equal to the communication range $r$.

The constraint is that any two stations in range of each other must use different frequencies. Since there are only two frequencies available, this is equivalent to asking if we can color the nodes of our graph with two colors such that no two adjacent nodes have the same color. A graph with this property is a **bipartite graph**.

### Efficient Graph Construction

A naive approach to building this "interference graph" would be to check the distance between every pair of stations, which would take $O(N^2)$ time and be too slow for the given constraints.

To address this efficiently, we can initiate a **BFS** (or DFS) from each station $v$. During the search, we add an edge between $v$ and any station we encounter whose distance to $v$ is less than or equal to $r$. Stations that do not satisfy this distance constraint are not added to the queue, which keeps the search focused and avoids unnecessary steps.

It is important to note that simply constructing the Delaunay triangulation and including all edges shorter than $r$ is insufficient. The triangulation only connects each station to its closest geometric neighbors, but there may be additional stations within range $r$ that are not directly connected in the triangulation. These stations are "close enough" but not necessarily among the closest. Therefore, a more sophisticated approach is required to ensure all relevant pairs are considered.

Therefore, we perform the following to connect the stations:

1.  Construct a Delaunay triangulation of all station locations.
2.  For each station $u$, perform a search (similar to a Breadth-First Search) starting from $u$ and traversing the edges of the triangulation.
3.  This search explores the neighborhood of $u$. Any station $v$ that is encountered during this search and satisfies `distance(u, v) <= r` is a neighbor of $u$ in our interference graph. An edge $\{u, v\}$ is then added.
4.  By repeating this for every station, we can construct the complete interference graph far more efficiently than the brute-force method.

### Checking for Bipartiteness

Once the graph `G` is constructed, we can check if it is bipartite. A standard algorithm for this involves a graph traversal (like BFS or DFS) that attempts to assign one of two colors to each node. If at any point it finds an edge connecting two nodes that must have the same color, the graph is not bipartite. The Boost Graph Library provides a convenient `is_bipartite(G)` function that handles this check for us.

### Code
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
#include <CGAL/Triangulation_node_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef int                                                    Index;
typedef CGAL::Triangulation_node_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;

typedef Tds::node_handle          node_handle;
typedef Delaunay::Edge_iterator     Edge_iterator;
typedef Delaunay::node_iterator   node_iterator;
typedef Delaunay::node_circulator node_circulator;

typedef K::Point_2               Point;
typedef std::pair<Point, Index> IPoint;


void solve() {
  // ===== READ INPUT =====
  int n, m, r; std::cin >> n >> m >> r;
  long r_squared = std::pow(r, 2);
  
  std::vector<IPoint> stations; stations.reserve(n);
  std::vector<IPoint> holmes_positions; holmes_positions.reserve(m);
  std::vector<IPoint> watson_positions; watson_positions.reserve(m);
  
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    stations.emplace_back(Point(x, y), i);
  }
  for(int i = 0; i < m; ++i) {
    int a_x, a_y, b_x, b_y; std::cin >> a_x >> a_y >> b_x >> b_y;
    holmes_positions.emplace_back(Point(a_x, a_y), i);
    watson_positions.emplace_back(Point(b_x, b_y), i);
  }
  
  // ===== SOLVE =====
  // Build graph using Triangulation
  // Note, that we are not able to just loop over all edges and keep those who are shorter than r
  // This is because the Triangulation will just give us the edges for the shortest distances "around" each node v
  // However, there might be other nodes that still lie in the radius of r
  // but the Triangulation does not directly connect them to v, as there is a closer node
  // Therefore, we need to do a BFS from all nodes to determine their connectivity
  Delaunay t;
  t.insert(stations.begin(), stations.end());
  
  graph G(n);
  for (node_iterator v = t.finite_nodes_begin(); v != t.finite_nodes_end(); ++v) {
    // Perform BFS for every node to determine its connectivity
    std::queue<node_handle> q;
    std::set<node_handle> visited;
    
    q.push(v);
    visited.insert(v);
    
    while(!q.empty()) {
      node_handle curr_v = q.front(); q.pop();
      
      // Connect v to curr_v, as only nodes within radius r are pushed to q
      if(v->info() != curr_v->info()) {
        boost::add_edge(v->info(), curr_v->info(), G);
      }
      
      // Iterate over all neighbors and potentially add them to q
      node_circulator neighbor_v = t.incident_nodes(curr_v);
      do {
        if(
            !t.is_infinite(neighbor_v) && // Check if the neighbor is the infinite node (happens at boundary)
            visited.find(neighbor_v) == visited.end() && // Check if neighbor has been visited before
            CGAL::squared_distance(v->point(), neighbor_v->point()) <= r_squared // Check if neighbor is close enough
          ) {
            q.push(neighbor_v);
            visited.insert(neighbor_v);
          }
      } while(++neighbor_v != t.incident_nodes(curr_v));
    }
  }
  
  // Check if G is bipartite
  if(!is_bipartite(G)) {
    std::cout << "n" << std::endl;
  } else {
    std::cout << "y" << std::endl;
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

The main difference compared to the first solution is that, if interference is not present (i.e., the station graph is bipartite), we must check for each pair $(a_i, b_i)$ whether they are reachable.

To do this efficiently, we use **Connected Components**. If both $a_i$ and $b_i$ are associated with stations in the same connected component, they are reachable through the station network. Importantly, we do not add $a_i$ and $b_i$ directly to the graph, as doing so would require rebuilding the graph for each pair, which is inefficient and could alter the connectivity.

There are two distinct cases in which $a_i$ and $b_i$ are considered reachable:

1. **Direct Connection:** If $\|a_i - b_i\| \leq r$, the two agents can communicate directly without involving any stations.
2. **Indirect Connection via Stations:** If the closest stations $s_a$ and $s_b$ to $a_i$ and $b_i$ respectively are each within distance $r$ of their agent, and $s_a$ and $s_b$ belong to the same connected component, then communication is possible via the station network.

The first case is straightforward: compute the (squared) distance between $a_i$ and $b_i$. For the second case, use the Delaunay triangulation to efficiently find the nearest station to each agent and check the corresponding distances. If neither case applies, $a_i$ and $b_i$ are not reachable.

### Code
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
#include <CGAL/Triangulation_node_base_with_info_2.h>
#include <CGAL/Triangulation_face_base_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef int                                                    Index;
typedef CGAL::Triangulation_node_base_with_info_2<Index,K>   Vb;
typedef CGAL::Triangulation_face_base_2<K>                     Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>            Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>                  Delaunay;

typedef Tds::node_handle          node_handle;
typedef Delaunay::Edge_iterator     Edge_iterator;
typedef Delaunay::node_iterator   node_iterator;
typedef Delaunay::node_circulator node_circulator;

typedef K::Point_2               Point;
typedef std::pair<Point, Index> IPoint;


void solve() {
  // ===== READ INPUT =====
  int n, m, r; std::cin >> n >> m >> r;
  long r_squared = std::pow(r, 2);
  
  std::vector<IPoint> stations; stations.reserve(n);
  std::vector<IPoint> holmes_positions; holmes_positions.reserve(m);
  std::vector<IPoint> watson_positions; watson_positions.reserve(m);
  
  for(int i = 0; i < n; ++i) {
    int x, y; std::cin >> x >> y;
    stations.emplace_back(Point(x, y), i);
  }
  for(int i = 0; i < m; ++i) {
    int a_x, a_y, b_x, b_y; std::cin >> a_x >> a_y >> b_x >> b_y;
    holmes_positions.emplace_back(Point(a_x, a_y), i);
    watson_positions.emplace_back(Point(b_x, b_y), i);
  }
  
  // ===== SOLVE =====
  // Build graph using Triangulation
  // Note, that we are not able to just loop over all edges and keep those who are shorter than r
  // This is because the Triangulation will just give us the edges for the shortest distances "around" each node v
  // However, there might be other nodes that still lie in the radius of r
  // but the Triangulation does not directly connect them to v, as there is a closer node
  // Therefore, we need to do a BFS from all nodes to determine their connectivity
  Delaunay t;
  t.insert(stations.begin(), stations.end());
  
  graph G(n);
  for (node_iterator v = t.finite_nodes_begin(); v != t.finite_nodes_end(); ++v) {
    // Perform BFS for every node to determine its connectivity
    std::queue<node_handle> q;
    std::set<node_handle> visited;
    
    q.push(v);
    visited.insert(v);
    
    while(!q.empty()) {
      node_handle curr_v = q.front(); q.pop();
      
      // Connect v to curr_v, as only nodes within radius r are pushed to q
      if(v->info() != curr_v->info()) {
        boost::add_edge(v->info(), curr_v->info(), G);
      }
      
      // Iterate over all neighbors and potentially add them to q
      node_circulator neighbor_v = t.incident_nodes(curr_v);
      do {
        if(
            !t.is_infinite(neighbor_v) && // Check if the neighbor is the infinite node (happens at boundary)
            visited.find(neighbor_v) == visited.end() && // Check if neighbor has been visited before
            CGAL::squared_distance(v->point(), neighbor_v->point()) <= r_squared // Check if neighbor is close enough
          ) {
            q.push(neighbor_v);
            visited.insert(neighbor_v);
          }
      } while(++neighbor_v != t.incident_nodes(curr_v));
    }
  }
  
  // Check if G is bipartite. If not early termination
  if(!is_bipartite(G)) {
    std::cout << std::string(m, 'n') << std::endl;
    return;
  } 
  
  // Determine the connected components, to quickly check if a_i and b_i are connected
  std::vector<int> component_map(n);
  boost::connected_components(G, &component_map[0]);
  
  // Check for every pair a_i b_i if they are connected/reachable
  for(int i = 0; i < m; ++i) {
    Point a = holmes_positions[i].first;
    Point b = watson_positions[i].first;
    
    // Check if they are "directly reachable"
    if(CGAL::squared_distance(a, b) <= r_squared) {
      std::cout << "y";
      continue;
    } 
    
    node_handle neighbor_a = t.nearest_node(a);
    node_handle neighbor_b = t.nearest_node(b);
    
    long dist_a = CGAL::squared_distance(a, neighbor_a->point());
    long dist_b = CGAL::squared_distance(b, neighbor_b->point());

    // Check if a and b are close enough to the stations and if they are in the same component
    if(dist_a <= r_squared && dist_b <= r_squared && 
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