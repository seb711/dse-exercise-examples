#include <cstdint>

__attribute__((noinline)) uint64_t inner(uint64_t x) {
  uint64_t acc = 0;
  for (uint64_t i = 0; i < x; i++)
    acc += i * i; // simple work so the function isn't optimized away
  return acc;
}

__attribute__((noinline)) uint64_t middle(uint64_t x) {
  return inner(x / 2) + inner(x / 3);
}

__attribute__((noinline)) uint64_t outer(uint64_t x) {
  return middle(x) + middle(x / 4);
}

int main() {
  // volatile prevents dead-code elimination of the whole chain
  volatile uint64_t result = outer(1000);
  return result == 0 ? 1 : 0;
}
