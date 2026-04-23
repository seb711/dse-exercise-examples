#include "06-perfevent.hpp"
#include <cstdint>
#include <unistd.h>
#include <vector>
// ── Two implementations
// ───────────────────────────────────────────────────────

static constexpr int N = 1 << 23; // 8M elements

// Bad: random access — every read is an LLC miss
__attribute__((noinline)) static uint64_t
sum_random(const std::vector<uint64_t> &v, const std::vector<int> &idx) {
  uint64_t s = 0;
  for (int i : idx)
    s += v[i];
  return s;
}

// Good: sequential access — prefetcher hides latency, very few misses
__attribute__((noinline)) static uint64_t
sum_sequential(const std::vector<uint64_t> &v) {
  uint64_t s = 0;
  for (uint64_t x : v)
    s += x;
  return s;
}

int main() {
  std::vector<uint64_t> data(N, 1ULL);

  // random index permutation
  std::vector<int> idx(N);
  for (int i = 0; i < N; ++i)
    idx[i] = i;
  for (int i = N - 1; i > 0; --i) {
    int j = rand() % (i + 1);
    std::swap(idx[i], idx[j]);
  }

  uint64_t r1, r2;

  PerfEvent e;

  // ── Measure random access ────────────────────────────────────────────────
  {
    {
      PerfEventBlock blk(e, 1000);
      r1 = sum_random(data, idx);
    }
  }

  // ── Measure sequential access ────────────────────────────────────────────
  {
    PerfEventBlock blk(e, 1000);
    r2 = sum_sequential(data);
  }
}
