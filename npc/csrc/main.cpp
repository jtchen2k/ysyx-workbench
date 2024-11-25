#include <stdio.h>
#include <assert.h>
#include <memory>
#include "verilated.h"
#include "Vtop.h"

#define MAX_SIM_TIME 20
vluint64_t sim_time = 0;

int main(int argc, char** argv) {
  VerilatedContext *contextp = new VerilatedContext();
  contextp->commandArgs(argc, argv);
  Vtop *top = new Vtop(contextp);
  contextp->randReset(43);
  int n = 10;
  while (sim_time < MAX_SIM_TIME) {
    int a = rand() & 1;
    int b = rand() & 1;
    top->a = a;
    top->b = b;
    top->eval();
    printf("a = %d, b = %d, f = %d\n", a, b, top->f);
    assert(top->f == (a ^ b));
    sim_time++;
  }
  printf("Verification complete.\n");
  return 0;
}
