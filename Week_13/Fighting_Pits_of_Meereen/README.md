# The Fighting Pits of Meereen

## 📝 Problem Description

You are given a sequence of $N$ fighters, each belonging to one of $K$ possible types. Your task is to process these fighters one by one, deciding for each whether to send it to a "North" entrance or a "South" entrance. The objective is to make a sequence of these decisions that maximizes the total excitement score accumulated over all $N$ rounds.

The excitement for each round is calculated based on two factors:

1.  **Variety Bonus:** The audience has a limited memory and can only recall the last $M$ fighters that came through the chosen entrance (including the current one). The excitement from this bonus is $1000$ times the number of distinct fighter types within this memory window. If fewer than $M$ fighters have been sent to an entrance, the memory window includes all fighters sent there so far.
2.  **Imbalance Penalty:** A penalty is subtracted if the number of fighters sent to the two entrances is unbalanced. If $p$ and $q$ are the total counts of fighters sent to the North and South entrances respectively (after the current fighter is assigned), the penalty is $2^{|p-q|}$.

A crucial constraint is that the excitement score for any single round must not be negative. You must find an assignment schedule that adheres to this rule and results in the highest possible sum of round excitements. The final output should be this maximum possible total excitement.

## 💡 Hints

<details>
<summary>Hint #1</summary>
The problem requires making a sequence of decisions, where each choice influences the potential outcomes of future choices. This structure suggests that we need to keep track of some information, or "state," as we process each fighter. What is the minimum information required at each step to calculate the excitement for the current round and to make decisions for all subsequent fighters?
</details>
<details>
<summary>Hint #2</summary>
Notice that the constraints on the number of fighter types, $K$, and the audience's memory, $M$, are very small. In contrast, the total number of fighters, $N$, can be quite large. This is a strong indicator that the complexity of an optimal solution should depend polynomially on $N$, but possibly exponentially on $K$ and $M$. This structure often points towards a Dynamic Programming approach.
</details>
<details>
<summary>Hint #3</summary>
To build a dynamic programming solution, we need to define a state that captures all necessary information. To calculate the excitement for the $i$-th fighter, we need to know:
<ol>
  <li>The index $i$ of the current fighter.</li>
  <li>The types of the last $M-1$ fighters sent to the North entrance.</li>
  <li>The types of the last $M-1$ fighters sent to the South entrance.</li>
  <li>The difference between the number of fighters sent North and South.</li>
</ol>
With this state, you can formulate a recurrence relation. The value for a given state would be the maximum excitement obtainable from the current fighter to the end. Be careful about how you handle the base cases and the constraint that no round's excitement can be negative.
</details>

## ✨ Solutions

<details>
<summary>Final Solution</summary>
The problem asks for an optimal sequence of decisions to maximize a total score, where each decision depends on previous ones. This structure is a classic fit for **Dynamic Programming (DP)**. The key is to identify a state that captures all necessary information to make future decisions.

### DP State Formulation
The constraints provide a major clue: the number of fighter types $K$ and the audience memory $M$ are very small ($K \le 4, M \le 3$). This allows us to include information about recent fighter types directly into our DP state.

To calculate the excitement for the current fighter `i`, we need:
1.  **`fighter_idx`**: The index of the current fighter we are assigning (from $0$ to $N-1$).
2.  **Recent North Fighters**: To calculate the variety bonus, we need the types of the last $M-1$ fighters sent to the North gate. Since $M \le 3$, we need to store at most the last two types. Let's call them `n_first` and `n_second`.
3.  **Recent South Fighters**: Similarly, we need the last two types for the South gate: `s_first` and `s_second`.
4.  **Gate Imbalance**: To calculate the penalty, we need the difference between the number of fighters sent North and South. Let `diff = num_north - num_south`.

Our DP function will be `solve(fighter_idx, n_first, n_second, s_first, s_second, diff)`, which computes the maximum possible excitement from `fighter_idx` to the end of the queue, given the history encapsulated by the other state parameters.

### Recurrence Relation
For any given state, we have two choices for `fighters[fighter_idx]`: send them North or South.

1.  **Send to North**:
    *   Calculate the immediate excitement, `n_excitement`.
        *   The variety bonus is $1000 \times (\text{number of unique types in } \{\text{current_type, n_first, n_second}\})$.
        *   The new difference becomes `diff + 1`, leading to a penalty of $2^{|diff+1|}$.
    *   If `n_excitement >= 0`, this is a valid move. The total score for this path is `n_excitement` plus the result of the recursive call with an updated state: `solve(fighter_idx + 1, current_type, n_first, s_first, s_second, diff + 1)`.

2.  **Send to South**:
    *   Calculate the immediate excitement, `s_excitement`, similarly.
    *   If `s_excitement >= 0`, this is a valid move. The total score is `s_excitement` plus the result from the recursive call: `solve(fighter_idx + 1, n_first, n_second, current_type, s_first, diff - 1)`.

The value for the current state is the maximum of the scores from the valid choices. If both choices lead to negative immediate excitement, this state leads to no valid schedules, so we should signify this with a value like negative infinity.

**Base Case**: When `fighter_idx == n`, we have processed all fighters, so the remaining excitement is 0.

To avoid recomputing results for the same state, we use memoization (a top-down DP approach), storing the results in a 6D array.

### Implementation Details
*   **State Representation**: We can use a special integer `NONE_TYPE` (e.g., 4, since types are 0-3) to represent an empty slot in the history of a gate.
*   **Difference Range**: The penalty $2^{|diff|}$ grows very quickly. A large imbalance $|diff|$ will almost certainly result in negative excitement. The maximum possible bonus is $1000 \times M = 3000$. Since $2^{12} = 4096 > 3000$, the absolute difference $|diff|$ will not exceed approximately 11 in any valid schedule. We can safely bound the difference, for example, from -12 to +12, and use an offset to map it to non-negative array indices.
*   **Constraint Handling**: It's critical to check if `n_excitement >= 0` and `s_excitement >= 0` *before* making a recursive call. A path is only considered if its immediate excitement is non-negative, as per the problem statement.

Below is the C++ implementation of this logic.

**Code**
```cpp
#include <iostream>
#include <vector>
#include <limits>

// FightherIdx x LastNorth x SecondLastNorth x LastSouth x SecondLastSouth x n_num-s_num
typedef std::vector<int> VI;
typedef std::vector<VI> VVI;
typedef std::vector<VVI> VVVI;
typedef std::vector<VVVI> VVVVI;
typedef std::vector<VVVVI> VVVVVI;
typedef std::vector<VVVVVI> VVVVVVI;

const int NONE_TYPE = 4;

int maxExcitment(
  VVVVVVI &memo,
  const VI &fighters,
  const int m,
  const int fighter_idx,
  const int n_first,
  const int n_second,
  const int s_first,
  const int s_second,
  const int diff
) {
  // std::cout << fighter_idx << ": " << fighters[fighter_idx] << " (" << n_first << " " << n_second << ") (" << s_first << " " << s_second << ") " << diff << " ";
  if(memo[fighter_idx][n_first][n_second][s_first][s_second][diff+12] != -1) {
    // std::cout << "USED MEMO" << std::endl;
    return memo[fighter_idx][n_first][n_second][s_first][s_second][diff+12];
  }
  int num_unique, penalty;
  int curr_type = fighters[fighter_idx];
  
  // Calculate immediate excitment when sending fighter north
  num_unique = 1;
  if(curr_type != n_first && n_first != NONE_TYPE) { num_unique++; }
  if(m == 3 && curr_type != n_second && n_first != n_second && n_second != NONE_TYPE) { num_unique++; }
  
  if((diff + 1) == 0) { penalty = 1; }
  else{ penalty = 2 << (std::abs(diff + 1) - 1); }
  
  int n_excitment = num_unique * 1000 - penalty;
  
  // std::cout << "n_penalty " << penalty << " "; 
  
  // Calculate immediate excitment when sending fighter north
  num_unique = 1;
  if(curr_type != s_first && s_first != NONE_TYPE) { num_unique++; }
  if(m == 3 && curr_type != s_second && s_first != s_second && s_second != NONE_TYPE) { num_unique++; }
  
  if((diff - 1) == 0) { penalty = 1; }
  else{ penalty = 2 << (std::abs(diff - 1) - 1); }
  
  int s_excitment = num_unique * 1000 - penalty;
  
  // std::cout << "s_penalty " << penalty << " ";
  // std::cout << "n_excitment " << n_excitment << " s_excitment " << s_excitment << std::endl;
  
  // Check if the excitments are valid
  if(std::max(n_excitment, s_excitment) < 0) {
    memo[fighter_idx][n_first][n_second][s_first][s_second][diff+12] = std::numeric_limits<int>::min();
    return memo[fighter_idx][n_first][n_second][s_first][s_second][diff+12];
  }
  
  // Calculate maximum for current state
  if(fighter_idx == fighters.size() - 1) {
    // Base case
    memo[fighter_idx][n_first][n_second][s_first][s_second][diff+12] = std::max(n_excitment, s_excitment);
  } else {
    // Recursive Case
    memo[fighter_idx][n_first][n_second][s_first][s_second][diff+12] = std::max(
      n_excitment + maxExcitment(memo, fighters, m, fighter_idx + 1, curr_type, n_first, s_first, s_second, diff + 1),
      s_excitment + maxExcitment(memo, fighters, m, fighter_idx + 1, n_first, n_second, curr_type, s_first, diff - 1)
    );
  }
  
  return memo[fighter_idx][n_first][n_second][s_first][s_second][diff+12];
}


void solve() {
  // std::cout << "=============================================" << std::endl;
  // ===== READ INPUT =====
  int n, k, m; std::cin >> n >> k >> m;
  
  VI fighters(n);
  for(int i = 0; i < n; ++i) { std::cin >> fighters[i]; }
  
  // ===== SOLVE =====
  VVVVVVI memo(n, VVVVVI(5, VVVVI(5, VVVI(5, VVI(5, VI(25, -1))))));
  std::cout << maxExcitment(memo, fighters, m, 0, NONE_TYPE, NONE_TYPE, NONE_TYPE, NONE_TYPE, 0) << std::endl;
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
   Test set 1 (25 pts / 2 s) : Correct answer      (0.171s)
   Test set 2 (25 pts / 2 s) : Correct answer      (0.234s)
   Test set 3 (25 pts / 2 s) : Correct answer      (0.216s)
   Test set 4 (25 pts / 2 s) : Correct answer      (0.64s)

Total score: 100
```