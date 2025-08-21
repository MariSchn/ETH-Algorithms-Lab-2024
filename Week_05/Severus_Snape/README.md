# Severus Snape

## 📝 Problem Description

The goal is to find the minimum total number of potions required to boost three attributes—magical power, happiness, and wit—to at least some specified threshold values $P$, $H$, and $W$, respectively. All attributes start at zero.

There are two types of potions available:
1.  **Type A Potions:** There are $n$ distinct potions of this type. The $i$-th potion provides $p_i$ magical power and $h_i$ happiness, but decreases wit by a fixed amount $a$.
2.  **Type B Potions:** There are $m$ distinct potions of this type. The $j$-th potion provides $w_j$ wit, but decreases magical power by a fixed amount $b$. It has no effect on happiness.

You must select a subset of the available $n$ type A and $m$ type B potions. The task is to determine the smallest possible size of this subset that meets or exceeds the required thresholds for all three attributes. If no combination of potions can achieve this, the answer should indicate that it's impossible.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem asks for the minimum *total* number of potions. Notice that the two types of potions have distinct primary effects: type A for happiness, and type B for wit. Both affect power. How can you use this separation to simplify the problem?
</details>
<details>
<summary>Hint #2</summary>
Consider the type B potions. If you decide to take a fixed number, say $k_B$, of them, which $k_B$ potions are the best to choose? Since they all have the same power penalty ($b$) but offer different amounts of wit, it is always optimal to pick the ones that provide the most wit. This suggests a greedy choice is possible for type B potions.
</details>
<details>
<summary>Hint #3</summary>
Now consider the type A potions. Choosing them is more complex because they involve a trade-off between power and happiness. A simple greedy choice is not obvious. This subproblem—selecting a specific number of items to satisfy a constraint on one value (happiness) while maximizing another (power)—is a classic pattern. This structure suggests that dynamic programming might be a suitable technique.
</details>
<details>
<summary>Hint #4</summary>
To combine these ideas, you can pre-process one set of potions. For example, you can build a dynamic programming table for type A potions. A state like $dp[j][h]$ could store the maximum power achievable using exactly $j$ type A potions to get at least $h$ happiness. After this pre-computation, you can iterate through the number of type B potions you take ($k_B$), and for each choice, use the DP table to efficiently find the minimum number of type A potions ($k_A$) needed to satisfy the remaining requirements.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
This problem asks for the minimum total number of potions to satisfy three different attribute thresholds. The effects of the two potion types are intertwined, primarily through the magical power attribute. A good strategy is to separate the decision-making process for each type of potion. We can use dynamic programming to analyze the trade-offs for type A potions and a greedy approach for type B potions.

### Overall Strategy

1.  **Pre-computation for Type A Potions:** The choice of type A potions is complex due to the two attributes they provide (power and happiness). We will use dynamic programming to pre-compute the maximum possible magical power for any given number of type A potions and any minimum happiness requirement.
2.  **Greedy Choice for Type B Potions:** For a fixed number of type B potions, it's always best to take those that provide the most wit, as they all have the same power penalty. We can sort them by their wit value.
3.  **Combining Results:** We then iterate through the number of type B potions we could take. For each number, we calculate the resulting wit gain and power penalty. Then, using our pre-computed DP table, we find the minimum number of type A potions required to satisfy the remaining thresholds.

### Dynamic Programming for Type A Potions

Let's define a DP state to capture the necessary information.
$dp[i][j][h]$ = The maximum magical power achievable by choosing exactly $j$ potions from the first $i$ available type A potions, while ensuring their combined happiness is *at least* $h$.

The transitions are as follows: when considering the $i$-th potion (with power $p_{i-1}$ and happiness $h_{i-1}$):
*   **Option 1: Don't take potion $i-1$.** We must still choose $j$ potions from the first $i-1$ available. The maximum power is $dp[i-1][j][h]$.
*   **Option 2: Take potion $i-1$.** We now need to choose $j-1$ potions from the first $i-1$. To meet the total happiness goal of at least $h$, the other $j-1$ potions must provide a combined happiness of at least $h - h_{i-1}$. The total power would be $p_{i-1}$ plus the power from the other potions, which is $dp[i-1][j-1][\max(0, h - h_{i-1})] + p_{i-1}$.

The final value is the maximum of these two options:
$dp[i][j][h] = \max(dp[i-1][j][h], \quad dp[i-1][j-1][\max(0, h - h_{i-1})] + p_{i-1})$

The table is initialized with a very small number (negative infinity) to denote impossible states. Base cases for $j=1$ are handled by finding the best single potion.

### Finding the Minimal Combination

After populating the DP table, we combine the results:
1.  Sort type B potions in descending order of the wit they provide.
2.  Iterate through the number of type B potions to take, $k_B$, from $1$ to $m$. For each $k_B$, we take the top $k_B$ potions.
3.  Calculate the `current_wit` and `power_penalty` for these $k_B$ potions.
4.  If `current_wit` is at least $W$, we proceed. Otherwise, adding A-potions (which reduce wit) won't help, so we need more B-potions.
5.  We then search for the smallest number of type A potions, $k_A$ (from $1$ to $n$), that satisfies the remaining conditions:
    *   **Wit:** `current_wit` - $k_A \times a \ge W$
    *   **Power:** $dp[n][k_A][H] \ge P + \text{power\_penalty}$
6.  The first pair $(k_A, k_B)$ that fulfills all conditions provides a valid solution. The algorithm returns the total count $k_A + k_B$ and terminates. If the loops complete without finding any solution, it is impossible.

```cpp
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

typedef std::vector<long> VL;
typedef std::vector<VL>   VVL;
typedef std::vector<VVL>  VVVL;

const long neg_inf = std::numeric_limits<long>::min();

struct PotionA {
  int power;
  int happiness;
};

struct PotionB {
  int wit;
};


void solve() {
  // ===== READ INPUT =====
  int n, m; long a, b, P, H, W; std::cin >> n >> m >> a >> b >> P >> H >> W;
  
  std::vector<PotionA> a_potions(n);
  for(int i = 0; i < n; ++i) {
    int p, h; std::cin >> p >> h;
    a_potions[i].power = p;
    a_potions[i].happiness = h;
  }
  
  std::vector<PotionB> b_potions(m);
  for(int i = 0; i < m; ++i) {
    int w; std::cin >> w;
    b_potions[i].wit = w;
  }
  
  // ===== SOLVE =====
  // === DP: DETERMINE MAXIMUM POSSIBLE POWER FOR HAPPINESS H (ONLY A POTIONS) ===
  // dp[i][j][h]: max power from taking j of the first i (type A) potions,
  // while reaching at least happiness h.
  VVVL dp(n + 1, VVL(n + 1, VL(H + 1, neg_inf)));
  
  // Base case: Taking 1 potion
  // dp[i][1][h] is the max power from one potion among the first i, with at least h happiness.
  for(int h = 0; h <= H; ++h) {
    for(int i = 1; i <= n; ++i) {
      // Option 1: Don't take potion i-1. Inherit from previous state.
      dp[i][1][h] = dp[i-1][1][h];
      
      // Option 2: Take potion i-1 if it's better.
      if(a_potions[i-1].happiness >= h && dp[i][1][h] < a_potions[i-1].power) {
        dp[i][1][h] = a_potions[i-1].power;
      }
    }
  }
  
  // Fill the rest of the DP table using the recurrence relation.
  for(int j = 2; j <= n; ++j) { // Number of A-potions to take
    for(int h = 0; h <= H; ++h) { // Minimum happiness required
      for(int i = j; i <= n; ++i) { // Number of A-potions to consider from
        int happiness_needed_before = std::max(0, h - a_potions[i-1].happiness);
        long power_if_taken = (dp[i-1][j-1][happiness_needed_before] == neg_inf) 
                                ? neg_inf 
                                : dp[i-1][j-1][happiness_needed_before] + a_potions[i-1].power;
        
        dp[i][j][h] = std::max(
            dp[i-1][j][h],       // Case 1: Don't take potion i-1
            power_if_taken       // Case 2: Take potion i-1
        );
      }
    }
  }
  
  // A-Potions only case: check if thresholds can be met without B-potions
  for (int k_a = 1; k_a <=n; ++k_a) {
      if (0 - k_a * a >= W && dp[n][k_a][H] >= P) {
          std::cout << k_a << std::endl;
          return;
      }
  }

  // === GREEDILY DETERMINE THE NUMBER OF B POTIONS AND COMBINE WITH DP RESULTS ===
  // Sort B Potions by wit in descending order.
  std::sort(b_potions.begin(), b_potions.end(), [](const PotionB &p1, const PotionB &p2){
    return p1.wit > p2.wit;
  });
  
  long min_total_potions = -1;
  long current_wit = 0;
  for(int k_b = 1; k_b <= m; k_b++) {
    current_wit += b_potions[k_b-1].wit;
    long power_penalty = (long)k_b * b;
    
    // Optimization: if B-potions alone don't provide enough wit, adding A-potions won't help.
    if(current_wit < W) { continue; }
      
    // Find the smallest number of A-potions that works with these k_b B-potions.
    for(int k_a = 1; k_a <= n; k_a++) {
      if(current_wit - (long)k_a * a < W) { // Wit penalty from A-potions is too high
        break; 
      }
      
      // Check if A-potions provide enough power (considering B's penalty) and happiness
      if(dp[n][k_a][H] >= P + power_penalty) {
        long current_total = k_a + k_b;
        if (min_total_potions == -1 || current_total < min_total_potions) {
            min_total_potions = current_total;
        }
        break; // Found smallest k_a for this k_b, move to next k_b
      }
    }
  }
  
  std::cout << min_total_potions << std::endl;
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(NULL);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) { solve(); }
}
```
</details>

## ⚡ Result

```plaintext

```