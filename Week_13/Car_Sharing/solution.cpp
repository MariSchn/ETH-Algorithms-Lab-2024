#include <iostream>
#include <vector>
#include <set>
#include <map>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/cycle_canceling.hpp>
#include <boost/graph/push_relabel_max_flow.hpp>
#include <boost/graph/successive_shortest_path_nonnegative_weights.hpp>
#include <boost/graph/find_flow_cost.hpp>

// Graph Type with nested interior edge properties for Cost Flow Algorithms
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> traits;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_capacity_t, long,
        boost::property<boost::edge_residual_capacity_t, long,
            boost::property<boost::edge_reverse_t, traits::edge_descriptor,
                boost::property <boost::edge_weight_t, long> > > > > graph; // new! weightmap corresponds to costs

typedef boost::graph_traits<graph>::edge_descriptor             edge_desc;
typedef boost::graph_traits<graph>::vertex_descriptor           vertex_desc;
typedef boost::graph_traits<graph>::out_edge_iterator           out_edge_it; // Iterator

// Custom edge adder class
class edge_adder {
 graph &G;

 public:
  explicit edge_adder(graph &G) : G(G) {}
  void add_edge(int from, int to, long capacity, long cost) {
    auto c_map = boost::get(boost::edge_capacity, G);
    auto r_map = boost::get(boost::edge_reverse, G);
    auto w_map = boost::get(boost::edge_weight, G); // new!
    const edge_desc e = boost::add_edge(from, to, G).first;
    const edge_desc rev_e = boost::add_edge(to, from, G).first;
    c_map[e] = capacity;
    c_map[rev_e] = 0; // reverse edge has no capacity!
    r_map[e] = rev_e;
    r_map[rev_e] = e;
    w_map[e] = cost;   // new assign cost
    w_map[rev_e] = -cost;   // new negative cost
  }
};

struct Request {
  int from;
  int to;
  int t_start;
  int t_end;
  int profit;
  
  Request(int from, int to, int t_start, int t_end, int profit) :
  from(from), to(to), t_start(t_start), t_end(t_end), profit(profit) {}
};

std::ostream& operator<<(std::ostream& os, const Request& req) {
    os << "Request(from: " << req.from
       << ", to: " << req.to
       << ", t_start: " << req.t_start
       << ", t_end: " << req.t_end
       << ", profit: " << req.profit << ")";
    return os;
}

const int MAX_PROFIT = 100;
const int MAX_NUM_CARS = 1000;

void solve() {
  // ===== READ INPUT =====
  int N, S; std::cin >> N >> S;
  int max_t = 0;
  int n_cars = 0;
  int n_nodes = 0;
  
  std::vector<int> l_i(N);
  for(int i = 0; i < S; ++i) { 
    std::cin >> l_i[i]; 
    n_cars += l_i[i];
  }
  
  std::vector<std::set<int>> station_time_sets(S, std::set<int>{});
  std::map<std::pair<int, int>, int> station_time_to_node;
  
  // Ensure that each station has at least the start node
  for(int s = 0; s < S; ++s) {
    station_time_sets[s].insert(0);
    station_time_to_node[{s, 0}] = n_nodes++;
  }
  
  std::vector<Request> requests; requests.reserve(N);
  for(int i = 0; i < N; ++i) {
    int s, t, d, a, p; std::cin >> s >> t >> d >> a >> p;
    s--; t--;  // Adjust station index to be 0-indexed
    requests.emplace_back(s, t, d, a, p);

    station_time_sets[s].insert(d);
    station_time_sets[t].insert(a);

    if(station_time_to_node.find({s, d}) == station_time_to_node.end()) {
      station_time_to_node[{s, d}] = n_nodes++;
    }
    if(station_time_to_node.find({t, a}) == station_time_to_node.end()) {
      station_time_to_node[{t, a}] = n_nodes++;
    }
    
    max_t  = std::max(max_t, a);
  }

  // ===== SOLVE =====
  graph G(n_nodes);
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_target = boost::add_vertex(G);
  
  // Add edges for each station
  for(int s = 0; s < S; ++s) {
    std::set<int> time_set = station_time_sets[s];
    
    // Add source connection to the first node
    int first_time = *time_set.begin();
    adder.add_edge(v_source, 
                   station_time_to_node[{s, first_time}], 
                   l_i[s], 
                   MAX_PROFIT * first_time);
                   
    // Add sink connection to the last node
    int last_time = *(--time_set.end());
    adder.add_edge(station_time_to_node[{s, last_time}], 
                   v_target, 
                   MAX_NUM_CARS, 
                   MAX_PROFIT * (max_t - last_time));
                   
    // Add edges between subsequent nodes
    for(auto it_start = time_set.begin(); it_start != (--time_set.end()); ++it_start) {
      adder.add_edge(station_time_to_node[{s, *it_start}],
                     station_time_to_node[{s, *std::next(it_start)}],
                     MAX_NUM_CARS,
                     MAX_PROFIT * (*std::next(it_start) - *it_start));
    }
  }
  
  // Add edge for each request
  for(const Request &r : requests) {
    adder.add_edge(station_time_to_node[{r.from, r.t_start}],
                   station_time_to_node[{r.to, r.t_end}],
                   1,
                   MAX_PROFIT * (r.t_end - r.t_start) - r.profit);
  }

  boost::successive_shortest_path_nonnegative_weights(G, v_source, v_target);
  int cost = boost::find_flow_cost(G);

  // ===== OUTPUT =====
  std::cout << n_cars * max_t * MAX_PROFIT - cost << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}