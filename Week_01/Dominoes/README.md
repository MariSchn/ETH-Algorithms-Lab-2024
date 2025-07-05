# Dominoes

## 📝 Problem Description

The problem involves a line of dominoes, each with a specific height $h$. The first domino is toppled, and causes subsequent dominoes to fall, based on its height. Specifically, if a domino of height $h$ at position $i$ is toppled, it causes dominoes at positions $j > i$ to fall if $j - i < h$. <br />
The task is to determine the total number of dominoes that will fall after toppling the first domino.

## 💡 Hints

<details><summary>Hint #1</summary>

Consider the height of the first domino. How many dominoes *could* potentially fall as a direct result of toppling it?

</details>

<details><summary>Hint #2</summary>

Think about how the amount of dominoes that *could* fall changes as you move along the line of dominoes. What is the effect of the height of each domino on how many subsequent dominoes can be toppled?

</details>

<details><summary>Hint #3</summary>

The key idea is to maintain a variable that stores how many more dominoes can fall. It starts out based on the height of the first domino, as it is the one that is initially toppled. At each domino, we need to check if the current domino is able to topple more dominoes than we can currently topple. If yes we need to update our variable accordingly.

</details>

## ✨ Solutions

<details><summary>Final Solution</summary>

The problem asks us to determine the number of dominoes that fall when the first domino is toppled. The **core idea** is to simulate the toppling process, iterating over the dominoes and keeping track of the remaining amount of dominoes that can be toppled. As soon as we reach a point where no more dominoes can fall, we stop the iteration and output the count of fallen dominoes.

For this we utilize two variables:
1. `n_can_fall`: This variable keeps track of **how many more dominoes can be toppled**. 
2. `n_fallen`: This variable counts **how many dominoes have fallen so far** and is used to iterate over the dominoes.

The **main part** of the solution is to figure out how `n_can_fall` changes as we iterate through the dominoes. <br />
Initially, it is set to the height of the first domino minus one (`dominoes[0] - 1`), since a height of $h$ means that we can make the next $h-1$ dominoes fall (see [Problem Description](#problem-description)). <br />
At each subsequent domino `i`, we need to check if it allows us to topple more dominoes than we currently can. This is the case if the height of the current domino is greater than the current `n_can_fall`.  <br />
Therefore, we update `n_can_fall` to be the maximum of the current domino's height and the current `n_can_fall`. As in the beginning, we need to subtract one from the height of the current domino to account for the domino itself.

Finally, we output the value of `n_fallen`, which represents the total number of dominoes that fell.

### Code
```c++
#include<iostream>
#include<vector>

int main() {
  std::ios_base::sync_with_stdio(false);
  int n_tests; std::cin >> n_tests;
  
  while(n_tests--) {
    // ===== READ INPUT =====
    int n_dominoes; std::cin >> n_dominoes;
    
    std::vector<int> dominos(n_dominoes);
    for (int i = 0; i < n_dominoes; i++) {
      std::cin >> dominos[i];
    }
    
    // ===== SOLVE =====
    int n_can_fall = dominos[0] - 1;  // Keep track of how many more dominoes can fall
    int n_fallen;                     // Count how many dominoes have fallen
    
    for (n_fallen = 1; n_fallen < n_dominoes; n_fallen++) {
      if (n_can_fall <= 0) break;     // Stop if no more dominoes can fall

      // Check if the new domino allows us to topple more dominoes
      n_can_fall = std::max(dominos[n_fallen] - 1, n_can_fall - 1);
    }

    std::cout << n_fallen << std::endl;
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Testing solution >>>>
   Test set 1 (50 pts / 1 s) : Correct answer      (0.0s)
   Test set 2 (50 pts / 1 s) : Correct answer      (0.238s)

Total score: 100
```