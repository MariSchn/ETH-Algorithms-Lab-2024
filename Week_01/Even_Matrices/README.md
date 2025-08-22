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

<details>

<summary> First Solution (Test Set 1, 2) </summary>

### Description
This initial approach uses a brute-force method to count all submatrices with an even sum. The core of this solution is to iterate through every possible submatrix, calculate the sum of its elements, and check if the sum is even.

To optimize the calculation of submatrix sums, we first compute an integral image (also known as a prefix sum matrix). The integral image allows us to find the sum of any rectangular submatrix in constant time, i.e., $O(1)$. The integral image $I$ is constructed such that each element $I[i][j]$ stores the sum of all elements in the rectangle from the origin $(0,0)$ to $(i,j)$.

The sum of a submatrix with corners at $(i_1, j_1)$ and $(i_2, j_2)$ can then be calculated using the formula:
`sum = I[i_2][j_2] - I[i_2][j_1-1] - I[i_1-1][j_2] + I[i_1-1][j_1-1]`

The time complexity of this solution is dominated by the four nested loops, resulting in an overall complexity of $O(n^4)$. The space complexity is $O(n^2)$ to store the integral image. This approach is feasible for small $n$ and passes the first two test sets.

### Code
```cpp
#include<iostream>
#include<vector>

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    int n; std::cin >> n;
    
    // Read in the matrix
    std::vector<std::vector<int>> M;
    for(int i = 0; i < n; i++) {
      std::vector<int> row;
      for(int j = 0; j < n; j++) {
        int x; std::cin >> x;
        row.push_back(x);
      }
      M.push_back(row);
    }

    // Calculate integral image
    std::vector<std::vector<int>> M_integral;
    for(int i = 0; i < n; i++) {
      std::vector<int> row;
      for(int j = 0; j < n; j++) {
        int x = M[i][j];
        
        if(i == 0 && j == 0) {
          row.push_back(x);
        } else if (i == 0) {
          row.push_back(row[j - 1] + x);
        } else if (j == 0) {
          row.push_back(M_integral[i - 1][j] + x);
        } else {
          row.push_back(M_integral[i - 1][j] + row[j-1] - M_integral[i - 1][j - 1] + x);
        }
      }
      M_integral.push_back(row);
    }
    
    // Add padding
    for(auto &row : M_integral) {
      *row.insert(row.begin(), 0);
    }
    M_integral.insert(M_integral.begin(), std::vector<int>(n+1, 0));
    
    // Calculate number of even pairs
    int n_even_pairs = 0;
    int sum;
    
    for(int i_1 = 1; i_1 < n + 1; i_1++) {
      for(int i_2 = i_1; i_2 < n + 1; i_2++) {
        for(int j_1 = 1; j_1 < n + 1; j_1++) {
          for(int j_2 = j_1; j_2 < n + 1; j_2++) {
            sum = M_integral[i_2][j_2] -
                  M_integral[i_2][j_1 - 1] -
                  M_integral[i_1 - 1][j_2] +
                  M_integral[i_1 - 1][j_1 - 1];
                  
            if(sum % 2 == 0) {
              n_even_pairs++;
            }
          }
        }
      }
    }
    
    std::cout << n_even_pairs << std::endl;
    
  }
}
```

</details>

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

Judging solution >>>>
   Test set 1 (30 pts / 0.600 s) : Correct answer      (0.0s)
   Test set 2 (50 pts / 0.600 s) : Correct answer      (0.001s)
   Test set 3 (20 pts / 0.600 s) : Correct answer      (0.066s)

Total score: 100
```
