


#include "Vlight.h"
#include <assert.h>
#include <cstdio>
#include <cassert>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <nvboard.h>

#ifndef TOP_NAME
#define TOP_NAME Vlight
#endif


static TOP_NAME dut;

void nvboard_bind_all_pins(TOP_NAME *top);

static void single_cycle() {
    dut.clk = 0; dut.eval();
    dut.clk = 1; dut.eval();
}

static void reset(int n) {
    dut.rst = 1;
    while (n -- > 0) single_cycle();
    dut.rst = 0;
}

int main() {
    nvboard_bind_all_pins(&dut);
    nvboard_init();

    reset(10);

    while (1) {
        nvboard_update();
        single_cycle();
    }
    return 0;
}

// int main(int argc, char **argv) {
//     VerilatedContext *contextp = new VerilatedContext;
//     contextp->commandArgs(argc, argv);
//     contextp->randReset(43);
//     Verilated::traceEverOn(false);

//     Vtop *top = new Vlight()

//     // Set up waveform tracing:
//     // https://veripool.org/guide/latest/faq.html#how-do-i-generate-waveforms-traces-in-c
//     VerilatedVcdC *m_trace = new VerilatedVcdC();
//     top->trace(m_trace, 5);
//     m_trace->open("waveform.fsd");

//     while (contextp->time() < MAX_SIM_TIME && !contextp->gotFinish()) {
//         contextp->timeInc(1);
//         int a = rand() & 1;
//         int b = rand() & 1;
//         top->a = a;
//         top->b = b;
//         top->eval();
//         m_trace->dump(contextp->time());
//         printf("a = %d, b = %d, f = %d\n", a, b, top->f);
//         assert(top->f == (a ^ b));
//     }
//     printf("Verification complete.\n");

//     m_trace->close();
//     delete top;
//     exit(EXIT_SUCCESS);
// }
