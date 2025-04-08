#include "klibtest.h"

// define a function pointer list called tests
static void (*tests[])() = {
  test_memw,
  test_memr,
  test_strw,
  test_strr,
  test_io,

};

int main(const char *args) {
  for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    tests[i]();
  }
  PASS("All tests passed!");
  return 0;
}
