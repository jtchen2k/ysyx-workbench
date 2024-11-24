#include <stdio.h>
#include <assert.h>
#include <memory>
#include "verilated.h"
#include "Vtop.h"


int main(int argc, char** argv) {
  const std::unique_ptr<VerilatedContext> contextp {new VerilatedContext()};
  contextp->commandArgs(argc, argv);
  const std::unique_ptr<Vtop> top {new Vtop{contextp.get()}};
  contextp->randReset(43);
  int n = 10;
  while (n--) {
    int a = rand() & 1;
    int b = rand() & 1;
    top->a = a;
    top->b = b;
    top->eval();
    printf("a = %d, b = %d, f = %d\n", a, b, top->f);
    assert(top->f == (a ^ b));
  }
  printf("Verification complete.\n");
  return 0;
}
