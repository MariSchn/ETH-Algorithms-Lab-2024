#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <limits>

using vi = std::vector<int>;
using vpii = std::vector<std::pair<int, int>>;

vpii moves;
vi potions;
std::array<std::vector<std::pair<int64_t, int64_t>>, 16> first_half, second_half;
std::array<int64_t, 31> best_without_potion;

void dfs_fill(int move, int moves_used, int64_t distance, int64_t time, int max_move, std::array<std::vector<std::pair<int64_t, int64_t>>, 16> &arr) {
  if (time <= 0) return;
  if (move >= max_move) {
    arr[moves_used].emplace_back(time, distance);
    return;
  }
  dfs_fill(move + 1, moves_used, distance, time, max_move, arr);
  dfs_fill(move + 1, moves_used + 1, distance - moves[move].first, time - moves[move].second, max_move, arr);
}

void sort_and_remove_suboptimal(std::array<std::vector<std::pair<int64_t, int64_t>>, 16> &arr) {
  for (auto &row : arr) {
    if (row.size() <= 1) continue;
    std::sort(row.begin(), row.end(), [](const auto &a, const auto &b) {
      return a.first < b.first || (a.first == b.first && a.second > b.second);
    });
    int64_t min_distance = row.back().second;
    for (int i = row.size() - 2; i >= 0; --i) {
      if (row[i].second >= min_distance) row.erase(row.begin() + i);
      else min_distance = row[i].second;
    }
  }
}

void clear() {
  for (auto &v : first_half) v.clear();
  for (auto &v : second_half) v.clear();
}

void solve() {
  clear();
  int nr_moves, nr_potions;
  int64_t distance, time;
  std::cin >> nr_moves >> nr_potions >> distance >> time;

  moves.resize(nr_moves);
  for (auto &m : moves) std::cin >> m.first >> m.second;

  potions.resize(nr_potions);
  for (auto &p : potions) std::cin >> p;
  std::sort(potions.begin(), potions.end());

  best_without_potion.fill(std::numeric_limits<int64_t>::max());
  dfs_fill(0, 0, distance, time, (nr_moves + 1) / 2, first_half);
  dfs_fill((nr_moves + 1) / 2, 0, distance, time, nr_moves, second_half);

  sort_and_remove_suboptimal(first_half);
  sort_and_remove_suboptimal(second_half);

  for (int i = 0; i <= (nr_moves + 1) / 2; i++) {
    for (const auto &elem : first_half[i]) {
      for (int j = 0; j <= nr_moves / 2; j++) {
        const auto &row = second_half[j];
        auto it = std::upper_bound(row.begin(), row.end(), time - elem.first, [](const auto &value, const auto &elem) {
          return value < elem.first;
        });
        if (it == row.end()) break;
        best_without_potion[i + j] = std::min(best_without_potion[i + j], elem.second + it->second - distance);
      }
    }
  }

  int64_t best = std::numeric_limits<int64_t>::max();
  for (int i = 1; i <= nr_moves; i++) {
    if (best_without_potion[i] <= 0) {
      std::cout << "0\n";
      return;
    }
    if (best_without_potion[i] != std::numeric_limits<int64_t>::max()) {
      int64_t needed_gain = (best_without_potion[i] - 1) / i + 1;
      auto it = std::lower_bound(potions.begin(), potions.end(), needed_gain);
      if (it != potions.end()) best = std::min(best, (int64_t)(it - potions.begin() + 1));
    }
  }

  if (best == std::numeric_limits<int64_t>::max()) {
    std::cout << "Panoramix captured" << std::endl;;
  } else {
    std::cout << best << std::endl;;
  }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n_tests; std::cin >> n_tests;
    while (n_tests--) { solve(); }
}
