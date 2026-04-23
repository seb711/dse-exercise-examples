#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h> // getpid
#include <vector>

static constexpr int N = 1 << 26;

// Phase A: simulate slow I/O (just burn some time without interesting work)
__attribute__((noinline)) static void phase_a_io() {
  // read random bytes from /dev/urandom to simulate I/O setup
  FILE *f = fopen("/dev/urandom", "rb");
  char buf[4096];
  for (int i = 0; i < 256; ++i)
    size_t _ = fread(buf, 1, sizeof(buf), f);
  fclose(f);
}

// Phase B: the workload we actually care about — pointer-chasing hash table
__attribute__((noinline)) static uint64_t phase_b_compute() {
  // simple open-addressing table — random probing → cache misses
  const int MASK = (1 << 23) - 1; // 8M slots
  std::vector<uint64_t> table(MASK + 1, 0);

  uint64_t x = 0xdeadbeefcafe;
  for (int i = 0; i < N; ++i) {
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17; // xorshift
    int slot = x & MASK;
    while (table[slot])
      slot = (slot + 1) & MASK; // linear probe
    table[slot] = x;
  }
  uint64_t sum = 0;
  for (uint64_t v : table)
    sum += v;
  return sum;
}

// Phase C: write back (also uninteresting)
__attribute__((noinline)) static void phase_c_write(uint64_t result) {
  FILE *f = fopen("/dev/null", "wb");
  fwrite(&result, sizeof(result), 1, f);
  fclose(f);
}

int main() {
  printf("PID=%d\n", getpid());

  phase_a_io();
  printf("phase A done — press Enter to start phase B (attach perf now)\n");
  getchar();

  uint64_t r = phase_b_compute();

  printf("phase B done — press Enter to finish (stop perf now)\n");
  getchar();

  phase_c_write(r);
  printf("done, result=%lu\n", r);
}
