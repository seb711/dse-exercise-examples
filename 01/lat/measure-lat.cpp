#include "../06-perfevent.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <numeric>
#include <stdint.h>
#include <vector>
#include <x86intrin.h>

static inline uint64_t rdtsc() {
  uint64_t lo, hi;
  uint32_t aux;
  __asm__ volatile("rdtscp" : "=a"(lo), "=d"(hi), "=c"(aux));
  return (hi << 32) | lo;
}

__attribute__((noinline)) static uint64_t pointer_chase(int n) {
  std::vector<int> idx(n);
  std::iota(idx.begin(), idx.end(), 0);
  for (int i = n - 1; i > 0; --i)
    std::swap(idx[i], idx[rand() % (i + 1)]);

  std::vector<int> next(n);
  for (int i = 0; i < n - 1; ++i)
    next[idx[i]] = idx[i + 1];
  next[idx[n - 1]] = idx[0];

  int cur = 0;
  {
    uint64_t start = rdtsc();

    for (int i = 0; i < n; ++i) {
      cur = next[cur];
    }
    // __asm__ volatile("" : "+r"(cur)); // sink

    uint64_t end = rdtsc();

    std::cout << (end - start) / n << std::endl;
  }
  return cur;
}

int main(int argc, char *argv[]) {
  pointer_chase(std::stoi(argv[1]) / sizeof(int));
}
