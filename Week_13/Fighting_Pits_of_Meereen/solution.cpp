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