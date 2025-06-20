#include<iostream>
#include<vector>

int main() {
  std::ios_base::sync_with_stdio(false);
  
  int n_tests; std::cin >> n_tests;
  while(n_tests--) {
    // ===== READ INPUT =====
    int n; std::cin >> n;
    
    // Calculate the prefix matrix while reading input
    std::vector<std::vector<int>> prefix_matrix(n + 1, std::vector<int>(n+1, 0));
    for(int i = 1; i < n + 1; i++) {
      for(int j = 1; j < n + 1; j++) {
        int x; std::cin >> x;
        prefix_matrix[i][j] = prefix_matrix[i - 1][j] + 
                              prefix_matrix[i][j - 1] -
                              prefix_matrix[i-1][j-1] + x;
      }
    }
    
    // ===== SOLVE =====
    int total = 0;
    
    // Iterate over all possible "row combinations"
    for(int i_1 = 1; i_1 < n + 1; i_1++) {
      for(int i_2 = i_1; i_2 < n + 1; i_2++) {
        int n_even = 1, n_odd = 0, sum = 0;
        
        // Calculate even pairs for each column from row i_1 to i_2
        for(int j = 1; j < n + 1; j++) {
          sum += prefix_matrix[i_2    ][j    ] +
                 prefix_matrix[i_1 - 1][j - 1] - 
                 prefix_matrix[i_2    ][j - 1] - 
                 prefix_matrix[i_1 - 1][j    ];
                
          if(sum % 2 == 0) {
            n_even++;
          } else {
            n_odd++;
          }
        }
        
        total += (n_even * (n_even - 1)) / 2 + (n_odd * (n_odd - 1)) / 2;
      }
    }
  
    std::cout << total << std::endl;
  }
}