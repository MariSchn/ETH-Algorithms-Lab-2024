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