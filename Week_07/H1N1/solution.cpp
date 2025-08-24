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