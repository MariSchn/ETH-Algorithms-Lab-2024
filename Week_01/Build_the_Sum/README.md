# Build the Sum

## 📝 Problem Description

The problem requires us to calculate the sum of $n$ integers $a_1, \dots, a_n$.

## 💡 Hints

No hints for this problem.

## ✨ Solutions

<details><summary>Final Solution</summary>

This problem is straightforward. We simply iterate over the integers $a_1, \dots, a_n$ and store their sum in a variable `sum`, that is initialized with $0$.

### Code
```c++
#include<iostream>

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    int n; std::cin >> n;
    
    // Accumulate the sum of the integers while reading them
    int sum = 0;
    for(int i = 0; i < n; ++i) {
      int a; std::cin >> a;
      sum += a;
    }
    
    std::cout << sum << std::endl;
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Testing solution >>>>
   Test set 1 (100 pts / 1 s) : Correct answer      (0.0s)

Total score: 100
```