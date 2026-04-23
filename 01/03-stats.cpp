// Step 5: perf stat — IPC as a first diagnosis
//
// Compile:
//   g++ -O2 -g -o stat 05_stat.cpp
//
// Run:
//   perf stat ./stat compute    # compute-bound: high IPC
//   perf stat ./stat memory     # memory-bound:  low  IPC
//
// Or with explicit events:
//   perf stat -e cycles,instructions,cache-misses,branch-misses ./stat compute
//   perf stat -e cycles,instructions,cache-misses,branch-misses ./stat memory
//
// perf stat shows totals — no sampling, zero overhead on what you care about.
// IPC (instructions per cycle) is the single most useful first number:
//   IPC >> 1.0  → compute-bound (ALU is the bottleneck)
//   IPC  < 0.5  → almost certainly memory-bound (CPU stalls waiting for data)

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

static constexpr int N = 1 << 27; // 128M

// ── compute-bound: tight math loop, everything fits in registers ─────────────
__attribute__((noinline)) static uint64_t compute_bound() {
  uint64_t a = 1, b = 1;
  for (int i = 0; i < N; ++i) {
    a = a * 6364136223846793005ULL + 1442695040888963407ULL;
    b = b * 2862933555777941757ULL + 3037000493ULL;
  }
  return a ^ b;
}

// ── memory-bound: stride through a large array — constant LLC misses ─────────
__attribute__((noinline)) static uint64_t
memory_bound(const std::vector<uint64_t> &v) {
  // stride = 64 bytes = one cache line, array >> LLC so every access is a miss
  uint64_t sum = 0;
  for (size_t i = 0; i < v.size(); i += 8)
    sum += v[i];
  return sum;
}

int main(int argc, char **argv) {
  bool do_memory = (argc > 1 && argv[1][0] == 'm');

  uint64_t r;
  if (do_memory) {
    // 1 GB array — doesn't fit in any cache level
    std::vector<uint64_t> big(128 * 1024 * 1024, 1ULL);
    // walk it many times to accumulate enough samples
    r = 0;
    for (int rep = 0; rep < 4; ++rep)
      r += memory_bound(big);
  } else {
    r = compute_bound();
  }
  printf("%lu\n", r);
}

// Typical perf stat numbers:
//
//   ./stat compute
//     instructions / cycles  ≈  3.5–4.0   (pipeline stays busy)
//     cache-misses           ≈  very low
//
//   ./stat memory
//     instructions / cycles  ≈  0.1–0.3   (CPU mostly stalling)
//     cache-misses           ≈  very high
//
// Lesson: run perf stat *first*. IPC + cache-miss rate tell you the class of
//         problem before you spend time on flame graphs or deeper analysis.
