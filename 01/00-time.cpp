#include <cstdio>
#include <cstdlib>
#include <cstring>

static constexpr size_t N = 128 * 1024 * 1024;

int main() {
  int *data = static_cast<int *>(malloc(N * sizeof(int)));

  // touch every page so the OS actually backs it with physical memory
  memset(data, 0xff, N * sizeof(int));

  // tiny workload so wall time looks innocent
  long long sum = 0;
  for (size_t i = 0; i < N; i += 64)
    sum += data[i]; // stride 64 ints = 1 cache line

  printf("sum = %lld\n", sum); // prevent dead-code elimination
  free(data);
}
