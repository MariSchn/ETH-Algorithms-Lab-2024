# Carsharing

## 📝 Problem Description

A carsharing service operates across $S$ rental stations, each with a specified initial number of cars. There are $N$ booking requests, where each request is defined by a starting station $s_i$, a destination station $t_i$, a departure time $d_i$, an arrival time $a_i$, and a profit $p_i$.

The task is to select a subset of requests to fulfill, ensuring that for every accepted request, a car is available at the starting station at the departure time. When a request is fulfilled, a car is removed from the starting station at the departure time and becomes available at the destination station at the arrival time. The objective is to choose a feasible set of requests that maximizes the total profit. For each test case, output a single integer representing the maximum achievable profit.

## 💡 Hints

<details>

<summary>Hint #1</summary>

The problem involves managing a fixed number of resources (cars) that move between different locations (stations) over time. This structure suggests modeling the system's state, which includes the number of cars at each station at any given moment. How can you represent the movement of cars between these states?

</details>

<details>

<summary>Hint #2</summary>

This problem can be modeled as a flow problem. Consider creating a network where nodes represent stations at specific points in time. Cars can be represented as units of flow. What would the edges in such a network represent? How can you incorporate the profits into this model to find the optimal selection of requests?
Notice that for the first three test sets, we have timesteps of 30 minutes, greatly reducing the number of possible nodes.

</details>

<details>

<summary>Hint #3</summary>

Maximizing profit can often be rephrased as a minimum cost problem. By assigning a negative cost (i.e., $-p_i$) to fulfilling a request, you can use a min-cost max-flow algorithm to find the optimal solution. 

</details>

<details>

<summary>Hint #3</summary>

For the general case where time is "continuous", building a node for every single minute would be too slow. Notice that the only time points that matter are the departure and arrival times of the requests. How can you build a more compact network using only these specific time points?

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

Standard min-cost max-flow algorithms are significantly faster if all edge weights are non-negative. We can transform our graph to satisfy this property. The key idea is to add a large constant cost `MAX_PROFIT` to certain edges, making all costs non-negative, and then subtract the corresponding amount from the final result.

Let `MAX_PROFIT` be a value larger than any possible profit. In this case this is `MAX_PROFIT = 100`

*   **"Waiting" Edges**: The edge from $(s, t)$ to $(s, t+1)$ now gets a cost of `MAX_PROFIT`.
*   **Request Edges**: The edge for a request from $(s_i, d_i)$ to $(t_i, a_i)$ has a duration of $\Delta t = a_i - d_i$ time steps. An idle car would have incurred a cost of $M \times \Delta t$ over this period. To represent the profit, we give this edge a cost of $M \times \Delta t - p_i$. Since $M$ is large, this value is guaranteed to be non-negative.

### Calculating the Final Profit

With this new cost structure, a car that remains idle from time 0 to $T_{max}$ incurs a total cost of $M \times T_{max}$. The total baseline cost for all cars if no requests are fulfilled is $N_{cars} \times T_{max} \times M$.

The min-cost flow algorithm will find paths (sequences of requests) that reduce this total cost. The total reduction in cost corresponds to the total profit. Therefore, the maximum profit is:
$$ \text{Profit}_{max} = (\text{Total Cars} \times T_{max} \times M) - \text{MinCost} $$

### Code
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
<summary>Final Solution</summary>

The main problem that prevents us from generalizing the First Solution from the first 3 Test Sets to all, is that there are **no “fixed” timesteps** anymore.
Naiively applying the previous approach to “continuous” time steps will make the graph have $S \cdot t_{\max}$ nodes, which is way too much for the Min Cost Max Flow Algorithm.

However, we can observe, that most of the nodes in the previous graph are **not actually necessary**.

Previously we added a node for each time (step). However, as we can have a maximum of $10^4$  requests but can have a maximum of $10^6$ Nodes. Chances are that most of these nodes are unnecessary

### Compressing the Graph

To remove these nodes, we first need to investigate **which nodes can be removed and which nodes have to be kept**.
Intuitively, every node that is either the **start or the destination of a request has to be kept**, however, all other nodes are not actually necessary, as they simple form a straight line, which can be **reduced to a single edge**. <br />
**Intuition**: All the nodes which are not involved in any request only serve the purpose of representing cars that stay at a station at a given timestep. However, if a car arrives at $t = 3$ but the next request at the station only happens at $t = 6$, it is not necessary to model all the $t$ in between as individual nodes, we can just connect the node for $t = 3$ and $t = 6$

As we are **skipping multiple nodes** with this construction, the **cost of these edges** needs to be adjusted, such that for each station the “straight” path without requests has the same cost.
For this we need to consider the time $t, t'$ of the nodes, which we want to connect with the edge (to skip multiple intermediate results). 
As the edge connecting $t$ and $t'$ skips $(t' - t)$ nodes, we need to multiply its cost by $(t' - t)$

### Implementation Details

To implement this, we can keep track of all the **times that are relevant** (either a request starts or arrives at that time) for each station using a **vector of sets**. This vector will store a set for each station, which contains all the **relevant times**, which can be kept track of during input reading. 

This effectively yields pairs of $(s, t)$, where $s$ is a station and $t$ is the time. However, for our graph we need to describe our nodes using a single integer and not pairs. Therefore we additionally create a **map** that maps these $(s, t)$ pairs to the index of the corresponding node.

### Edge Case

One small **Edge Case** that occurs on the 5th private Test Set but not on the Public is that a Station has **no “relevant times”**. This will cause it to effectively **not have any nodes**, as it is never used by any car. This causes a problem, as there will be no connection from source to sink using this station, which makes flow that starts at that station unable to flow to the sink.

The solution is to simply add the time $0$ to all stations, to **ensure that each station has at least one relevant time/node**.

### Code
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