# Even Matrices

## 📝 Problem Description

The problem involves an $n \times n$ matrix $M$ of bits. <br />
The task is to count the number of quadruples $(i_1, i_2, j_1, j_2)$ such that $1 \leq i_1 \leq i_2 \leq n$ and $1 \leq j_1 \leq j_2 \leq n$, and the sum of the elements within the submatrix defined by these quadruples is even. Specifically, the sum is calculated as $\sum_{i' = i_1}^{i_2} \sum_{j' = j_1}^{j_2} x_{i', j'}$.

## 💡 Hints

<details><summary>Hint #1</summary>

I would strongly recommend to solve the **Even Pairs** problem first, as it will help you understand how to efficiently count pairs with even sums.  The key idea is very similar.

</details>

<details><summary>Hint #2</summary>

Consider how to efficiently compute the sum of elements within a submatrix defined by $(i_1, i_2, j_1, j_2)$. Can you precompute some values to avoid redundant calculations?
</details>

<details><summary>Hint #3</summary>

An integral matrix (also known as a prefix sum matrix) can be used to calculate the sum of any submatrix in $O(1)$ time. Think about how to construct such a matrix for this problem.
</details>

<details><summary>Hint #4</summary>

The sum of a submatrix is even if and only if an even number of its elements are $1$. Consider how the parity (evenness or oddness) of the sum affects the counting process. You can calculate the number of even and odd sums and use combinations to find the total count.

</details>

## ✨ Solutions

<details><summary>Final Solution</summary>

The **core idea** behind this solution is very similar to the **Even Pairs** problem. Instead of a 1D array, we now work with a 2D grid (a matrix), and instead of subarrays, we count submatrices whose total sum is even. To generalize the approach, we simply iterate over all row pairs $(i_1, i_2)$ and reduce the matrix in-between these rows to a 1D array by summing up the columns between these rows. This allows us to apply the same combinatorial counting technique used in the 1D case.

---

We start by constructing a **prefix sum matrix** where each entry $S_{i,j}$ represents the sum of elements in the submatrix from the top-left corner $(1,1)$ to the position $(i,j)$. This matrix allows us to compute the sum of any submatrix in constant time.

To compute the sum of the submatrix defined by rows $i_1$ to $i_2$ and columns $j_1$ to $j_2$, we use:

$$
\text{Sum} = S_{i_2,j_2} - S_{i_1-1,j_2} - S_{i_2,j_1-1} + S_{i_1-1,j_1-1}
$$

But as in the 1D case, we don’t care about the exact sum, we only care whether it is **even or odd** and use combinatorial counting to find the number of even submatrices.

---

### Combinatorial Counting in 2D

We fix a pair of rows $(i_1, i_2)$ and collapse the matrix between these two rows into a temporary 1D array where each element represents the sum of the column slice between rows $i_1$ and $i_2$. <br />
**Note:** We do not actually have to do this, as the prefix sum matrix already allows us to compute the sum of any submatrix in constant time, but conceptually, we can think of it as a 1D array where each element is the sum of the column from row $i_1$ to row $i_2$.

$$
\begin{pmatrix}
x_{i_1,1} & x_{i_1,2} & \ldots & x_{i_1,n} \\
\vdots & \vdots & \ddots & \vdots \\
x_{i_2,1} & x_{i_2,2} & \ldots & x_{i_2,n}
\end{pmatrix} \rightarrow 
\begin{pmatrix}
\sum_{i=i_1}^{i_2} x_{i,1} & \sum_{i=i_1}^{i_2} x_{i,2} & \ldots & \sum_{i=i_1}^{i_2} x_{i,n}
\end{pmatrix}
$$

This reduces the 2D problem to the 1D **Even Pairs** problem. We then compute a running prefix sum of this array (only its parity matters), and count how many times the prefix sum is even or odd.

For each such prefix parity, we apply combinatorics:

- Even + Even = Even  
- Odd + Odd = Even

So we count how many prefix sums are even ($n_{\text{Even}}$) and odd ($n_{\text{Odd}}$), and compute the number of valid submatrices using:

$$
\frac{n_{\text{Even}}(n_{\text{Even}} - 1)}{2} + \frac{n_{\text{Odd}}(n_{\text{Odd}} - 1)}{2}
$$

---

### Final Algorithm

1. Construct the prefix sum matrix for the original input.
2. For each pair of rows $(i_1, i_2)$, apply the same approach as in the **Even Pairs** problem
3. Accumulate the result across all row pairs.

### Code

```c++
#include<iostream>
#include<vector>

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    
    // Construct prefix matrix
    std::vector<std::vector<int>> prefix_matrix(n + 1, std::vector<int>(n + 1, 0));
    for(int i = 1; i <= n; i++) {
      for(int j = 1; j <= n; j++) {
        int x; std::cin >> x;
        prefix_matrix[i][j] = prefix_matrix[i - 1][j] + 
                              prefix_matrix[i][j - 1] - 
                              prefix_matrix[i - 1][j - 1] + x;
      }
    }
    
    // ===== SOLVE =====
    int total = 0;
    
    // Iterate over all possible row pairs
    for(int i_1 = 1; i_1 <= n; i_1++) {
      for(int i_2 = i_1; i_2 <= n; i_2++) {
        std::vector<int> col_prefix_parity(n + 1, 0);
        int prefix_sum = 0;
        int n_even = 1, n_odd = 0;  // prefix sum = 0 is considered even
        
        for(int j = 1; j <= n; j++) {
          // Compute the column sum for column j between rows i_1 and i_2
          int col_sum = prefix_matrix[i_2][j] - prefix_matrix[i_1 - 1][j] -
                        prefix_matrix[i_2][j - 1] + prefix_matrix[i_1 - 1][j - 1];
          
          prefix_sum += col_sum;
          
          if (prefix_sum % 2 == 0) {
            total += n_even;
            n_even++;
          } else {
            total += n_odd;
            n_odd++;
          }
        }
      }
    }
  
    std::cout << total << std::endl;
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Testing solution >>>>
   Test set 1 (30 pts / 0.600 s) : Correct answer      (0.0s)
   Test set 2 (50 pts / 0.600 s) : Correct answer      (0.001s)
   Test set 3 (20 pts / 0.600 s) : Correct answer      (0.066s)

Total score: 100
```
