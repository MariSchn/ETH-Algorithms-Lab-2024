# Severus Snape

## 📝 Problem Description

The task is to determine the minimum total number of potions required to increase three attributes—magical power, happiness, and wit—to at least specified threshold values $P$, $H$, and $W$, respectively. All attributes initially have a value of zero.

Two types of potions are available:
1.  **Type A Potions:** There are $n$ distinct potions of this type. The $i$-th potion increases magical power by $p_i$ and happiness by $h_i$, but decreases wit by a fixed amount $a$.
2.  **Type B Potions:** There are $m$ distinct potions of this type. The $j$-th potion increases wit by $w_j$, but decreases magical power by a fixed amount $b$. This type has no effect on happiness.

A subset of the available $n$ type A and $m$ type B potions must be selected. The task is to determine the smallest possible size of such a subset that meets or exceeds the required thresholds for all three attributes.

## 💡 Hints

<details>

<summary>Hint #1</summary>

Notice that happiness is only ever increased by type A potions and never decreased by any potion. This means that once you reach the required happiness threshold, you do not need to worry about losing it. Therefore, your main challenge is to optimize the selection of potions to maximize magical power and wit while ensuring happiness is at least the required value.

</details>

<details>

<summary>Hint #2</summary>

When working with type A potions, focus on how to reach the happiness threshold as efficiently as possible, but also pay attention to the resulting magical power. Since happiness cannot decrease, you can use a dynamic programming approach to precompute, for each possible number of type A potions and happiness value, the maximum magical power achievable. How might you structure your DP state and transitions to efficiently answer queries about possible happiness and power combinations?

</details>

<details>

<summary>Hint #3</summary>

Once you have precomputed the best possible outcomes for type A potions, think about how to select type B potions. For a fixed number of type B potions, how would you choose which ones to take? Is there a way to select the most effective subset for a given number? What impact does this choice have on the other attributes?

</details>

<details>

<summary>Hint #4</summary>

Try to combine your strategies for both potion types. Can you iterate over possible numbers of type B potions, and for each, use your precomputed results for type A potions to check if all thresholds can be met? How can you efficiently check if a combination of type A and type B potions meets all three requirements?

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

A good strategy is to separate the decision-making process for each type of potion. We can use **dynamic programming** to analyze the trade-offs for type A potions and a **greedy** approach for type B potions.

### Approach

1.  **Pre-computation for Type A Potions:** The choice of type A potions is complex due to the two attributes they provide (power and happiness). We will use dynamic programming to pre-compute the maximum possible **magical power** for any given number of type A potions and any minimum happiness requirement.
2.  **Greedy Choice for Type B Potions:** For a fixed number of type B potions, it's always best to take those that provide the most wit, as they all have the same power penalty. Therefore, we can sort them by their wit value.
3.  **Combining Results:** We then iterate through the number of type B potions we could take. For each number, we calculate the resulting wit gain and power penalty. Then, using our pre-computed DP table, we find the minimum number of type A potions required to satisfy the remaining thresholds.
4.  **Finalizing the Solution:** The first valid combination of type A and type B potions that meets all requirements gives us the answer. 

### Dynamic Programming for Type A Potions

To efficiently select type A potions, we use a three-dimensional dynamic programming table that tracks the best possible outcome for different choices:

**DP State Definition:**
$\text{DP}[i][j][h]$ represents the maximum total magical power achievable by choosing exactly $j$ potions from the first $i$ type A potions, such that their combined happiness is at least $h$.


**Base Case:** For $j=1$ (only take one potion), the base case is handled by selecting the single potion among the first $i$ that provides at least $h$ happiness and the highest possible power.

**Recursion:**
When considering the $i$-th potion (with power $p_{i-1}$ and happiness $h_{i-1}$), we have two choices:

1. **Don't take the $i$-th potion:**
  We still need to select $j$ potions from the first $i-1$ potions, maintaining the same happiness requirement $h$. We have previously computed the optimal value for this and can read it from the DP table. The value is $\text{DP}[i-1][j][h]$.

2. **Take the $i$-th potion:** We must select $j-1$ potions from the first $i-1$ potions. To ensure the total happiness is at least $h$, the remaining $j-1$ potions must provide at least $h - h_{i-1}$ happiness (if this is negative, we use $0$ instead). This is because since we are taking potion $i$ we get $h_{i-1}$ from the potion itself, so we only need $\max(0, h - h_{i-1})$ from the other potions. <br />
We can read how much magical power we will get from this from our DP table as $\text{DP}[i-1][j-1][\max(0, h - h_{i-1})]$. Since we also take the $i$-th potion, we add its power to the total. The value is $\text{DP}[i-1][j-1][\max(0, h - h_{i-1})] + p_{i-1}$.

We take the maximum of these two options:

$$
	\text{DP}[i][j][h] = \max\left(\text{DP}[i-1][j][h],\; \text{DP}[i-1][j-1][\max(0, h - h_{i-1})] + p_{i-1}\right)
$$

Using this recursion, we can simply fill the entire DP table $\text{DP}[i][j][h]$

### Greedy Approach for Type B Potions

After populating the DP table, we use a greedy approach to optimize the type B potions. 
1.  Sort type B potions in descending order of the wit they provide. <br /> This was not possible for the type A potions, as they provide both magical power and happiness.
2.  Iterate through the number of type B potions to take, $k_B$, from $1$ to $m$. For each $k_B$, we take the top $k_B$ type B potions.
3.  Calculate the `current_wit` and `power_penalty` the best $k_B$ type B potions provide.
4.  If `current_wit` is at least $W$, we proceed. Otherwise, adding A-potions (which reduce wit) won't help, so we need more B-potions.
5.  We then search for the smallest number of type A potions, $k_A$ (from $1$ to $n$) in our precomputed DP table, that satisfies the remaining conditions:
    *   **Wit:** `current_wit` - $k_A \cdot a \ge W$, the wit needs to be enough even after applying the penalty $k_A \cdot a$ we get from the type A potions
    *   **Power:** $\text{DP}[n][k_A][H] \ge P + \text{power\_penalty}$, the magiacal power when taking $k_A$ type A potions among all $n$ potions and having at least $H$ happiness needs to fulfill the magical power constraint, even when applying the power penalty from the type B potions.
6.  Since the type B potions are sorted descendingly, the first pair $(k_A, k_B)$ that fulfills all conditions provides a valid solution. The algorithm returns the total count $k_A + k_B$ and terminates.

### Code
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
  // === DETERMINE MAXIMUM POSSIBLE POWER FOR HAPPINESS H (ONLY A POTIONS) ===
  // dp[i][j][h] = p: Only taking j of the first i (type A) potions, 
  // how much magical power p can be reached while reaching at least happiness h?
  VVVL dp(n + 1, VVL(n + 1, VL(H + 1, neg_inf)));
  
  // Fill all values dp[i][1][h], where only one potion is taken
  // In these cases, simply take the potion among the first i potions,
  // that yields the highest magical power p while having at least happiness h
  for(int h = 0; h <= H; ++h) {    // Iterate over minimum required happiness h
    for(int i = 1; i <= n; ++i) {  // Iterate over available potions (first i potions)
      // Not take the new potion [i-1]
      dp[i][1][h] = dp[i-1][1][h];
      
      // Check if the new potion [i-1] would provide more magical power
      if(a_potions[i-1].happiness >= h &&   // Check if the new potion [i-1] provides enough happiness
        dp[i][1][h] < a_potions[i-1].power  // Check if taking the new potion would provide more power than any previous potion
      ) {
        // Take the new potion [i-1]
        dp[i][1][h] = a_potions[i-1].power;
      }
    }
  }
  
  // Recursively fill the remaining dp entries
  // For each new potion [i-1] we can either 
  // Not take it -> Power stays the same dp[i][j][h] = dp[i-1][j][h]
  // Take it     -> Power becomes        dp[i-1][j-1][h - a_potions[i-1].happiness] + a_potions[i-1].power
  //                This is because we take the maximum power we were able to get with j - 1 potions + the power of the potion itself
  for(int j = 2; j <= n; ++j) {
    for(int h = 0; h <= H; ++h) {
      for(int i = j; i <= n; ++i) { // Start at i = j, as taking j potions among the first i < j would not be possible (invalid)
        int happiness_before = std::max(0, h - a_potions[i-1].happiness);
        dp[i][j][h] = std::max(
            dp[i-1][j][h],                                        // Power when not taking the new potion [i-1]
            dp[i-1][j-1][happiness_before] + a_potions[i-1].power // Power when     taking the new potion [i-1]
          );
      }
    }
    
  }
  
  // === GREEDILY DETERMINE THE NUMBER OF B POTIONS ===
  // Sort B Potions according to their wit (descendingly)
  std::sort(b_potions.begin(), b_potions.end(), [](const PotionB &a, const PotionB &b){
    return a.wit > b.wit;
  });
  
  long curr_wit = 0;
  for(int num_potions_b = 0; num_potions_b < m; num_potions_b++) {
    // Take the next B potion
    long power_penalty = (num_potions_b + 1) * b;
    curr_wit += b_potions[num_potions_b].wit;
    
    // Check if enough wit is reached
    if(curr_wit >= W) {
      // Look for the smallest amount of A potions that provides enough power
      for(int num_potions_a = 1; num_potions_a <= n; num_potions_a++) {
        // Check if taking num_potions_a A potions, has a too large wit penalty, causing the wit to no longer be above the threshold
        if(curr_wit - num_potions_a * a < W) { break; }
        
        // Check if taking num_potions_a potions is enough to reach the power threshold P while also meeting the happiness threshold H
        if(dp[n][num_potions_a][H] >= P + power_penalty) {
          std::cout << num_potions_a + num_potions_b + 1 << std::endl;
          return;
        }
      }
    }
  }
  
  // No way of reaching all 3 threshold was found -> Impossible
  std::cout << -1 << std::endl;
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
   Test set 1 (30 pts / 5.500 s) : Correct answer      (0.729s)
   Test set 2 (30 pts / 5.500 s) : Correct answer      (1.035s)
   Test set 3 (20 pts / 5.500 s) : Correct answer      (1.058s)
   Test set 4 (20 pts / 5.500 s) : Correct answer      (1.007s)

Total score: 100
```