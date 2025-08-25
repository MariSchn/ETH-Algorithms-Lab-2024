#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

typedef std::vector<int> VI;
typedef std::vector<VI> VVI;
typedef std::vector<VVI> VVVI;

typedef std::vector<long> VL;
typedef std::vector<VL> VVL;

const long INF = std::numeric_limits<long>::max();

// Function to calculate overlap between two patterns
int calculate_overlap(const VI &a, const VI &b, int k) {
    for (int o = k; o >= 0; --o) { // Try overlaps of size o from k down to 0
        bool valid = true;
        for (int i = 0; i < o; ++i) {
            if (a[k - o + i] != b[i]) {
                valid = false;
                break;
            }
        }
        if (valid) return o;
    }
    return 0;
}

void solve() {
    // ===== READ INPUT =====
    int n, m, k, d;
    std::cin >> n >> m >> k >> d;

    VVVI head_to_patterns(n);
    VI head_to_num_patterns(n, 0);
    for (int i = 0; i < m; ++i) {
        // Read Pattern i
        std::vector<int> pattern(k);
        for (int j = 0; j < k; ++j) {
            std::cin >> pattern[j];
        }

        // Store Pattern i for the appropriate head
        head_to_patterns[pattern[k - 1]].push_back(pattern);
        head_to_num_patterns[pattern[k - 1]]++;
    }

    // ===== SOLVE =====
    // Current Head i x Current Pattern p to kill Head i
    VVL dp(n, VL(d, INF));

    // Initialize DP table for the first head
    for (int p = 0; p < head_to_num_patterns[0]; ++p) {
        dp[0][p] = k; // All patterns take k cuts initially
    }

    // Fill remaining entries
    for (int i = 1; i < n; ++i) {
        for (int p = 0; p < head_to_num_patterns[i]; ++p) {
            const VI &current_pattern = head_to_patterns[i][p];

            // Check if the pattern can independently eradicate the current head
            bool possible = true;
            for (int h = 0; h < k; ++h) {
                if (current_pattern[h] < i) {
                    possible = false;
                    break;
                }
            }

            // Update entry for the current head when no overlap is considered
            if (possible) {
                for (int prev_p = 0; prev_p < head_to_num_patterns[i - 1]; ++prev_p) {
                    if (dp[i - 1][prev_p] == INF) continue; // Skip unreachable states
                    
                    dp[i][p] = std::min(dp[i][p], dp[i - 1][prev_p] + k);
                }
            }

            // Check overlaps between the current pattern and all patterns of the previous head
            for (int prev_p = 0; prev_p < head_to_num_patterns[i - 1]; ++prev_p) {
                if (dp[i - 1][prev_p] == INF) continue; // Skip unreachable states

                const VI &prev_pattern = head_to_patterns[i - 1][prev_p];
                int o = calculate_overlap(prev_pattern, current_pattern, k);

                // Check if the overlap forms a valid transition
                bool valid = true;
                for (int h = o; h < k; ++h) {
                    if (current_pattern[h] < i) {
                        valid = false;
                        break;
                    }
                }

                // Update DP table for valid transitions
                if (valid) {
                    dp[i][p] = std::min(dp[i][p], dp[i - 1][prev_p] + k - o);
                }
            }
        }
    }

    // Find the minimum cuts to kill the last head
    long min_cuts = INF;
    for (int p = 0; p < head_to_num_patterns[n - 1]; ++p) {
        min_cuts = std::min(min_cuts, dp[n - 1][p]);
    }

    // ===== OUTPUT =====
    if (min_cuts == INF) {
        std::cout << "Impossible!\n";
    } else {
        std::cout << min_cuts << "\n";
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);

    int t;
    std::cin >> t;
    while (t--) {
        solve();
    }
    return 0;
}
