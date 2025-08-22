# Even Pairs

## 📝 Problem Description

The problem involves a sequence of $n$ bits $x_0, \dots, x_{n-1}$, each being either 0 or 1. <br /> 
The task is to determine the count of pairs $(i, j)$ within the sequence where the sum of the elements between the pair's indices (inclusive) is an even number. <br />
Specifically, given a sequence $x_0, x_1, ..., x_{n-1}$, we need to find the number of pairs $(i, j)$ such that $0 \le i \le j < n$ and the sum $\sum_{k=i}^{j} x_k$ is even.

## 💡 Hints

<details><summary>Hint #1</summary>

Calculating the sum for each pair $(i, j)$ directly is inefficient. Try to find a way to compute subarray sums more efficiently. <br />
One useful technique is the **prefix sum** $s_i$: the sum of all elements up to a given index. With prefix sums, you can compute the sum from $i$ to $j$ in constant time using:

$$
\sum_{k=i}^{j} x_k = s_j - s_{i-1}
$$

</details>

<details><summary>Hint #2</summary>

You don't actually need to compute the sum of each subarray. You only need to know whether it's **even or odd**. Since $s_j - s_{i-1}$ gives the sum from $i$ to $j$, this sum can only be even if both $s_j$ and $s_{i-1}$ are either both even or both odd.

</details>

<details><summary>Hint #3</summary>

If two prefix sums $s_i, s_j$ have the same parity (both even or both odd), their difference, and subsequently the subarray sum, is even:

- Even + Even = Even  
- Odd + Odd = Even

So, if you count how many prefix sums are even ($n_{\text{Even}}$) and how many are odd ($n_{\text{Odd}}$), you can compute the number of valid pairs using combinatorics:

$$
\frac{n_{\text{Even}}(n_{\text{Even}} - 1)}{2} + \frac{n_{\text{Odd}}(n_{\text{Odd}} - 1)}{2}
$$

</details>


## ✨ Solutions

<details><summary>Final Solution</summary>

The **core idea** behind this solution is to leverage the properties of even and odd numbers to efficiently count the pairs with an even sum. We can observe that the pair $(i, j)$ is only even if the sum from $0$ to $j$ minus the sum from $0$ to $i - 1$ is even:

$$
\sum_{k=i}^{j} x_k = \sum_{k=0}^{j} x_k - \sum_{k=0}^{i-1} x_k = s_j - s_{i-1}
$$

This reformulation might seem counterintuitive at first, but it allows us to utilize **prefix sums** $s_i$.

Instead of iterating through all possible pairs and calculating their sums, we compute the prefix sums for the entire sequence and track the number of **even and odd prefix sums**. Afterward, we can use combinatorial counting to determine the number of even pairs.

<details>
<summary>More Details</summary>

For every index $i$, we calculate the prefix sum $s_i$ as:

$$
s_i = \sum_{k=0}^{i} x_k
$$

This simply sums up all elements from the start of the sequence up to index $i$.  
The sum of elements from index $i$ to $j$ can then be derived from these prefix sums:

$$
\sum_{k=i}^{j} x_k = s_j - s_{i-1}
$$

where $s_{-1}$ is defined as 0 (the sum of an empty sequence).  
We can observe that this is only even if:

- $s_j$ and $s_{i-1}$ are **both even**, or  
- $s_j$ and $s_{i-1}$ are **both odd**.

This means that we can count the number of indices with even prefix sums $n_{\text{Even}}$ and odd prefix sums $n_{\text{Odd}}$, and then use combinatorial counting to find the number of valid pairs.

</details>

---

### Combinatorial Counting

Once we have the counts of even and odd prefix sums, we need to count the number of possible pairs where both involved prefix sums have the same parity. Essentially, this is the question: "*Given $n$ elements, how many pairs can we form*?" The answer is given by the Binomial Coefficient, providing us with the number of ways to pair up 2 even prefix sums or 2 odd prefix sums:

$$
\binom{n}{2} = \frac{n(n-1)}{2}
$$

We apply this to both the even and odd prefix sum counts to obtain the total number of subarrays with even sums:

$$
\frac{n_{\text{Even}}(n_{\text{Even}} - 1)}{2} + \frac{n_{\text{Odd}}(n_{\text{Odd}} - 1)}{2}
$$

---

### Final Algorithm

1. Iterate through the sequence, keeping track of the prefix sum and counting how many of them are even.  
2. Calculate the number of odd prefix sums as the total number of prefix sums minus the number of even ones.  
3. Use combinatorial counting to find the number of pairs with even sums based on the counts of even and odd prefix sums.

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

    // Add extra element as i <= j. So i == j is valid.
    std::vector<int> bits(n + 1, 0);
    for(int i = 1; i < n + 1; i++) {
      std::cin >> bits[i];
    }
  
    // ===== SOLVE =====
    int n_even = 0;
    int prefix_sum = 0;  // Keep track of the sum of all previous bits  
    for(int i = 0; i < n + 1; i++) {
      prefix_sum += bits[i];
      
      if (prefix_sum % 2 == 0) {
        n_even++;
      }
    }
    
    int n_odd = (n + 1) - n_even; // Calculate the number of odd pairs as (total - n_even)
    // Use combinatorial counting to find the number of even pairs
    int even_pairs = (n_even * (n_even - 1)) / 2;  // Even + Even -> Even
    int odd_pairs = (n_odd * (n_odd - 1)) / 2;     // Odd + Odd -> Even

    std::cout << even_pairs + odd_pairs << std::endl;
  }
}
```
</details>

## ⚡ Result

```plaintext
Compiling: successful

Judging solution >>>>
   Test set 1 (40 pts / 0.400 s) : Correct answer      (0.0s)
   Test set 2 (40 pts / 0.400 s) : Correct answer      (0.004s)
   Test set 3 (20 pts / 0.400 s) : Correct answer      (0.028s)

Total score: 100
```