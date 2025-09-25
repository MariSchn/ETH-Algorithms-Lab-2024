# The Fighting Pits of Meereen

## 📝 Problem Description

Given a sequence of $N$ fighters, each belonging to one of $K$ possible types, the task is to process these fighters one by one, deciding for each whether to send them to a "North" entrance or a "South" entrance. The objective is to make a sequence of these decisions that maximizes the total excitement score accumulated over all $N$ rounds.

The excitement for each round is calculated based on two factors:

1.  **Variety Bonus:** The audience has a limited memory and can only recall the last $M$ fighters that came through the chosen entrance (including the current one). The excitement from this bonus is $1000$ times the number of distinct fighter types within this memory window. If fewer than $M$ fighters have been sent to an entrance, the memory window includes all fighters sent there so far.
2.  **Imbalance Penalty:** A penalty is subtracted if the number of fighters sent to the two entrances is unbalanced. If $p$ and $q$ are the total counts of fighters sent to the North and South entrances respectively (after the current fighter is assigned), the penalty is $2^{|p-q|}$.

A crucial constraint is that the excitement score for any single round must not be negative. The final output should be this maximum possible total excitement.

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

- The index $i$ of the current fighter.
- The types of the last $M-1$ fighters sent to the North entrance.
- The types of the last $M-1$ fighters sent to the South entrance.
- The difference between the number of fighters sent North and South.

With this state, you can formulate a recurrence relation. The value for a given state would be the maximum excitement obtainable from the current fighter to the end. Be careful about how you handle the base cases and the constraint that no round's excitement can be negative.

</details>

## ✨ Solutions

<details>

<summary>Final Solution</summary>

When reading through the problem, we can realize that $m$ and $k$ are **VERY low**. This allows us to model the problem as a **6D Dynamic Program**. 

### DP State Formulation
The DP has the following Dimensions:

1. `fighter_idx`:The current Fighter (Index, as otherwise we can not iterate one fighter ahead)

    This dimensions is used to store at which position in the fighter vector we are

2. `n_first`: The last Fighter Type sent to the North Gate
3. `n_second`: The second to last Fighter Type sent to the North Gate
4. `s_first`: The last Fighter Type sent to the South Gate
5. `s_second`: The second to last Fighter Type sent ot the South Gate

    These 4 Dimensions are used to calculate the excitement of the audience. As $m \leq 3$ we only need to store the latest 2 fighter types of each gate, as we need the last two + the current one, resulting in a “window” of $m=3$ fighters

6. `diff`: The difference in the amount of fighters, sent to the North and South Gate

    his is used to calculate the penalty, for sending too many fighters to one gate (imbalance)

### DP Transition

For a given input the maximum excitement can then be calculated by:

1. Calculate the “direct” excitement obtained by sending the fighter to the north or south gate
    1. Count the number of unique `num_unique` fighter types among the latest $m$
    2. Calculate the `penalty` $2^{p -q} = 2^{\text{diff}}$
    3. Excitement is then `num_unique * 1000 - penalty`
2. Check if the current Input is valid ($\max\{\text{n\_excitment, s\_excitment}\} >= 0$), as the problem requires every excitement value to be positive. 
**Note**: In the Memo Table this is stored by setting the value to $-\infty$
    
    The problem says that for every fighter that we sent out, the received excitement needs to be greater than  $0$. Therefore if both the north and south excitement is lower than $0$, this is no longer possible
    
3. Calculate the maximum overall excitement, by adding up the direct excitement and the excitement received on the remaining sequence (recursive call) 

**Note**: As always the intermediate results are stored in a Memoization Table, to avoid computing the same value multiple times

This already yields a fast enough approach to solve all 4 Test Sets

### Code
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

## 🧠 Learnings

<details> 

<summary> Expand to View </summary>

- The order of your DP dimensions can play a significant role in the run time of your program.

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