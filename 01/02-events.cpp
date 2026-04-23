#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <numeric>
#include <vector>

// ── Phase 1: random pointer chasing ─────────────────────────────────────────
// Build a linked list whose nodes are scattered randomly in memory,
// then walk it. Every load is a cache miss.
__attribute__((noinline)) static uint64_t pointer_chase(int n) {
  std::vector<int> idx(n);
  std::iota(idx.begin(), idx.end(), 0);
  // shuffle so consecutive nodes are far apart in memory
  for (int i = n - 1; i > 0; --i)
    std::swap(idx[i], idx[rand() % (i + 1)]);

  std::vector<int> next(n);
  for (int i = 0; i < n - 1; ++i)
    next[idx[i]] = idx[i + 1];
  next[idx[n - 1]] = idx[0];

  int cur = 0;
  for (int i = 0; i < n; ++i)
    cur = next[cur]; // every step = cache miss
  return cur;
}

// ── Phase 2: branch-heavy, cache-friendly ───────────────────────────────────
// Walk a sequential array but branch on each element's value.
// Data is in cache; branches are unpredictable (random values).
__attribute__((noinline)) static uint64_t
branch_heavy(const std::vector<int> &data) {
  uint64_t sum = 0;
  for (int x : data) {
    // branch outcome depends on x — unpredictable for random data
    if (x & 1)
      sum += x;
    else if (x & 2)
      sum -= x;
    else if (x & 4)
      sum ^= x;
    else
      sum += x * 3;
  }
  return sum;
}

int main() {
  constexpr int N = 1 << 22; // 4M elements

  // random array for branch_heavy — fits in L3, so no cache pressure
  std::vector<int> arr(N);
  for (int &v : arr)
    v = rand();

  // run both phases roughly equally long
  uint64_t r = 0;
  for (int rep = 0; rep < 8; ++rep) {
    r += pointer_chase(N / 4); // small N so it doesn't take forever
    r += branch_heavy(arr);
  }
  printf("%lu\n", r);
}
