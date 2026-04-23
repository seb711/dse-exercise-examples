#include <cstdint>
#include <cstdio>

static constexpr int N = 1 << 26; // 64M iterations

// buried three calls deep — hard to spot without a flame graph
__attribute__((noinline)) static uint64_t the_real_culprit(uint64_t x) {
  // fake "expensive" work: many dependent multiplications
  for (int i = 0; i < 100; ++i)
    x = x * 6364136223846793005ULL + 1;
  return x;
}

__attribute__((noinline)) static uint64_t helper_b(uint64_t x) {
  return the_real_culprit(x);
}

__attribute__((noinline)) static uint64_t helper_a(uint64_t x) {
  return helper_b(x + 1);
}

__attribute__((noinline)) static uint64_t process(uint64_t x) {
  return helper_a(x * 3);
}

int main() {
  uint64_t result = 42;
  for (int i = 0; i < N; ++i)
    result = process(result);
  printf("result = %lu\n", result);
}
