# Carsharing

## 📝 Problem Description

You are tasked with managing a carsharing service across $S$ rental stations. You are given the initial number of cars available at each station. You also receive $N$ booking requests. Each request is specified by a starting station $s_i$, a destination station $t_i$, a departure time $d_i$, an arrival time $a_i$, and an associated profit $p_i$.

Your goal is to select a subset of these requests to fulfill. A set of requests is considered feasible if, for every accepted request, a car is available at its starting station at its departure time. When a request is fulfilled, a car is consumed from the starting station at the departure time and becomes available at the destination station at the arrival time. The objective is to choose a feasible set of requests that maximizes the total profit. For each test case, you should output a single integer representing this maximum possible profit.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem involves managing a fixed number of resources (cars) that move between different locations (stations) over time. This structure suggests modeling the system's state, which includes the number of cars at each station at any given moment. How can you represent the movement of cars between these states?
</details>
<details>
<summary>Hint #2</summary>
This problem can be modeled as a flow problem. Consider creating a network where nodes represent stations at specific points in time. Cars can be represented as units of flow. What would the edges in such a network represent? How can you incorporate the profits into this model to find the optimal selection of requests?
</details>
<details>
<summary>Hint #3</summary>
Maximizing profit can often be rephrased as a minimum cost problem. By assigning a negative cost (i.e., $-p_i$) to fulfilling a request, you can use a min-cost max-flow algorithm to find the optimal solution. For the general case where time is continuous, building a node for every single minute would be too slow. Notice that the only time points that matter are the departure and arrival times of the requests. How can you build a more compact network using only these specific time points?
</details>

## ✨ Solutions

<details>
<summary>First Solution (Test Set 1, 2, 3)</summary>

### Core Idea: Min-Cost Max-Flow on a Time-Expanded Graph

The problem of maximizing profit by selecting a set of feasible actions is a classic optimization problem. Common approaches include Dynamic Programming, Greedy algorithms, or network flow models. The movement of a fixed number of resources (cars) between locations (stations) over time is a strong indicator that a **min-cost max-flow** formulation is a good fit.

The main challenge is how to represent time. For the first three test sets, all departure and arrival times are multiples of 30 minutes. This allows us to discretize time into fixed steps without loss of generality. We can construct a **time-expanded graph** where each node represents a specific station at a specific time step.

### Graph Construction

Let's define the components of our flow network:

1.  **Nodes**: We create a node for each pair $(s, t)$, where $s$ is a station index ($0, \dots, S-1$) and $t$ is a time step ($0, \dots, T_{max}$). We also add a global source `v_source` and a global sink `v_target`.

2.  **Edges**: The edges will represent the movement or idling of cars.
    *   **Source Edges**: For each station $s$, we add an edge from `v_source` to the node $(s, 0)$. The capacity of this edge is the initial number of cars at station $s$, and its cost is 0. This injects the initial cars into the network.
    *   **"Waiting" Edges**: For each station $s$ and each time step $t$, we add an edge from node $(s, t)$ to $(s, t+1)$. This represents a car staying idle at the station. Its capacity should be large enough to accommodate all cars (e.g., the total number of cars), and its cost is 0.
    *   **Request Edges**: For each booking request from station $s_i$ at departure time $d_i$ to station $t_i$ at arrival time $a_i$ with profit $p_i$, we add an edge from node $(s_i, d_i)$ to $(t_i, a_i)$. This edge represents fulfilling the request. Its capacity is **1** (since each request can be fulfilled at most once), and its cost is **$-p_i$**. By sending one unit of flow through this edge, we "pay" a negative cost, which is equivalent to gaining a profit of $p_i$.
    *   **Sink Edges**: For each station $s$, we add an edge from the final time step node $(s, T_{max})$ to `v_target`. This allows all cars to exit the network. The capacity is large, and the cost is 0.

### Cost Transformation for Non-Negative Weights

Standard min-cost max-flow algorithms are significantly faster if all edge weights are non-negative. We can transform our graph to satisfy this property. The key idea is to add a large constant cost `M` to certain edges, making all costs non-negative, and then subtract the corresponding amount from the final result.

Let `M` be a value larger than any possible profit (e.g., `MAX_PROFIT = 100`).

*   **"Waiting" Edges**: The edge from $(s, t)$ to $(s, t+1)$ now gets a cost of `M`.
*   **Request Edges**: The edge for a request from $(s_i, d_i)$ to $(t_i, a_i)$ has a duration of $\Delta t = a_i - d_i$ time steps. An idle car would have incurred a cost of $M \times \Delta t$ over this period. To represent the profit, we give this edge a cost of $M \times \Delta t - p_i$. Since $M$ is large, this value is guaranteed to be non-negative.

### Calculating the Final Profit

With this new cost structure, a car that remains idle from time 0 to $T_{max}$ incurs a total cost of $M \times T_{max}$. The total baseline cost for all cars if no requests are fulfilled is $N_{cars} \times T_{max} \times M$.

The min-cost flow algorithm will find paths (sequences of requests) that reduce this total cost. The total reduction in cost corresponds to the total profit. Therefore, the maximum profit is:
$$ \text{Profit}_{max} = (\text{Total Cars} \times T_{max} \times M) - \text{MinCost} $$

This approach correctly models the problem for the constrained test sets.

```cpp
#include <iostream>
#include <vector>

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
  from(from-1), to(to-1), t_start(t_start / 30), t_end(t_end / 30), profit(profit) {}
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
  // std::cout << "====================================================" << std::endl;
  // ===== READ INPUT =====
  int N, S; std::cin >> N >> S;
  int n_steps = 0;
  int n_cars = 0;
  
  std::vector<int> cars_per_station(N);
  for(int i = 0; i < S; ++i) { 
    std::cin >> cars_per_station[i]; 
    n_cars += cars_per_station[i];
  }
  
  std::vector<Request> requests; requests.reserve(N);
  for(int i = 0; i < N; ++i) {
    int s, t, d, a, p; std::cin >> s >> t >> d >> a >> p;
    requests.emplace_back(s, t, d, a, p);
    
    n_steps = std::max(n_steps, a);
  }
  n_steps /= 30;
  
  // ===== SOLVE =====
  graph G(S * (n_steps + 1));
  edge_adder adder(G);
  
  const vertex_desc v_source = boost::add_vertex(G);
  const vertex_desc v_target = boost::add_vertex(G);
  
  // Add sink and source connections at the beginnning and end
  for(int s = 0; s < S; ++s) {
    adder.add_edge(v_source, s, cars_per_station[s], 0);
    adder.add_edge(n_steps * S + s, v_target, MAX_NUM_CARS, 0);
  }
  
  // Add edges between time steps
  for(int t = 0; t < n_steps; ++t) {
    for(int s = 0; s < S; ++s) {
      adder.add_edge(t * S + s, (t + 1) * S + s, MAX_NUM_CARS, MAX_PROFIT);
    }
  }
  
  for(const Request &r : requests) {
    adder.add_edge(r.t_start * S + r.from, 
                   r.t_end   * S + r.to,
                   1, MAX_PROFIT * (r.t_end - r.t_start) - r.profit);
                   
    // std::cout << "Connected " << r.from + 1 << " with " << r.to + 1 << " with start time " << r.t_start << " (" << (r.t_start * S + r.from) << ") end time " << r.t_end << " (" << (r.t_end * S + r.to) << ") cost: " << MAX_PROFIT * (r.t_end - r.t_start) - r.profit << std::endl;
  }
  
  boost::successive_shortest_path_nonnegative_weights(G, v_source, v_target);
  int cost = boost::find_flow_cost(G);

  // ===== OUTPUT =====
  std::cout << n_cars * n_steps * MAX_PROFIT - cost << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}
```
</details>

<details>

<summary>Second Solution (Test Set 1, 2 , 3, 4)</summary>

### Code
```cpp
///5
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
  // std::cout << "====================================================" << std::endl;
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
    // std::cout << "===== " << s << " =====" << std::endl;
    std::set<int> time_set = station_time_sets[s];
    
    // Add source connection to the first node
    int first_time = *time_set.begin();
    adder.add_edge(v_source, 
                   station_time_to_node[{s, first_time}], 
                   l_i[s], 
                   MAX_PROFIT * first_time);
                   
    // std::cout << "Connected: source with " << station_time_to_node[{s, first_time}] << " with capacity " << l_i[s] << " and cost " << MAX_PROFIT * first_time << std::endl;
    
    
    // Add sink connection to the last node
    int last_time = *(--time_set.end());
    adder.add_edge(station_time_to_node[{s, last_time}], 
                   v_target, 
                   MAX_NUM_CARS, 
                   MAX_PROFIT * (max_t - last_time));
                   
    // std::cout << "Connected: " << station_time_to_node[{s, last_time}] << " with target with capacity " << MAX_NUM_CARS << " and cost " << MAX_PROFIT * (max_t - last_time) << std::endl;
    
    // Add edges between subsequent nodes
    auto it_start = time_set.begin();
    auto it_end = time_set.end()--;
    for(auto it_start = time_set.begin(); it_start != (--time_set.end()); ++it_start) {
      adder.add_edge(station_time_to_node[{s, *it_start}],
                     station_time_to_node[{s, *std::next(it_start)}],
                     MAX_NUM_CARS,
                     MAX_PROFIT * (*std::next(it_start) - *it_start));
                     
      // std::cout << "Connected: " << station_time_to_node[{s, *it_start}] << " with " << station_time_to_node[{s, *std::next(it_start)}] <<  " with capacity " << MAX_NUM_CARS << " and cost " << MAX_PROFIT * (*std::next(it_start) - *it_start) << std::endl;
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
```

</details>

<details>
<summary>Final Solution</summary>

### The Challenge of Continuous Time

The previous solution relies on a discrete grid of time steps. This approach fails when departure and arrival times can be any integer value up to $10^5$, as the number of nodes ($S \times T_{max}$) would become prohibitively large.

The key observation is that most of these time-step nodes are redundant. The state of the system only changes when a request begins or ends. Therefore, we only need to create nodes for the specific time points that are mentioned in the booking requests. This leads to a much smaller, "compressed" graph.

### Optimized Graph Construction

We can build a more efficient graph by only considering relevant `(station, time)` events.

1.  **Identify Relevant Nodes**: For each station $s$, we collect all unique departure and arrival times associated with it. This gives us a sorted list of relevant time points $t_1, t_2, \dots, t_k$ for that station. Each pair $(s, t_j)$ will become a node in our graph. We use a map to assign a unique integer ID to each such node.

2.  **Edge Construction**:
    *   **"Waiting" Edges (Compressed)**: For each station $s$, and for every pair of consecutive relevant times $t_j$ and $t_{j+1}$, we add an edge from node $(s, t_j)$ to $(s, t_{j+1})$. This edge represents cars staying idle between these two events.
        *   Capacity: Sufficiently large (e.g., total number of cars).
        *   Cost: To maintain the cost transformation for non-negative weights, the cost must be proportional to the duration. The cost is $M \times (t_{j+1} - t_j)$, where $M$ is our large constant (`MAX_PROFIT`).
    *   **Request Edges**: For each request from $(s_i, d_i)$ to $(t_i, a_i)$ with profit $p_i$, we add an edge from node $(s_i, d_i)$ to $(t_i, a_i)$.
        *   Capacity: 1.
        *   Cost: $M \times (a_i - d_i) - p_i$. This is identical to the previous approach, but now $d_i$ and $a_i$ are the actual times, not discretized steps.
    *   **Source and Sink Edges**:
        *   An edge from `v_source` to the node for each station $s$ at its *first* relevant time point, $t_{first}$. The capacity is the initial number of cars $l_s$. The cost is $M \times t_{first}$ to account for the "idle" time from 0 to $t_{first}$.
        *   An edge from the node for each station $s$ at its *last* relevant time point, $t_{last}$, to `v_target`. The cost is $M \times (T_{max} - t_{last})$, where $T_{max}$ is the latest arrival time across all requests. This accounts for the final idle period.

### Handling an Edge Case

A subtle issue arises if a station has cars initially but is never part of any request. In our construction, such a station would have no "relevant" time points and thus no nodes, stranding its initial cars from the flow network. To fix this, we ensure that time 0 is considered a relevant time point for *every* station. This guarantees that each station has at least one node and a connection path from the source, allowing its initial cars to be properly accounted for.

### Calculating the Final Profit

The final profit calculation is analogous to the first solution, but uses the global maximum time $T_{max}$:
$$ \text{Profit}_{max} = (\text{Total Cars} \times T_{max} \times M) - \text{MinCost} $$
This optimized model is compact enough to solve the problem for all test cases within the time limits.

```cpp
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
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (20 pts / 5 s) : Correct answer      (0.067s)
   Test set 2 (20 pts / 5 s) : Correct answer      (0.001s)
   Test set 3 (20 pts / 5 s) : Correct answer      (0.16s)
   Test set 4 (20 pts / 5 s) : Correct answer      (0.692s)
   Test set 5 (20 pts / 5 s) : Correct answer      (1.372s)

Total score: 100
```