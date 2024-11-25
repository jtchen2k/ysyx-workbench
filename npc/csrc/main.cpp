#include "Vtop.h"
#include <assert.h>
#include <memory>
#include <stdio.h>
#include <verilated.h>
#include <verilated_vcd_c.h>

#define MAX_SIM_TIME 30

int main(int argc, char **argv) {
    VerilatedContext *contextp = new VerilatedContext;
    contextp->commandArgs(argc, argv);
    contextp->randReset(43);
    Verilated::traceEverOn(true);

    Vtop *top = new Vtop(contextp);

    // Set up waveform tracing:
    // https://veripool.org/guide/latest/faq.html#how-do-i-generate-waveforms-traces-in-c
    VerilatedVcdC *m_trace = new VerilatedVcdC();
    top->trace(m_trace, 5);
    m_trace->open("waveform.vcd");

    while (contextp->time() < MAX_SIM_TIME && !contextp->gotFinish()) {
        contextp->timeInc(1);
        int a = rand() & 1;
        int b = rand() & 1;
        top->a = a;
        top->b = b;
        top->eval();
        m_trace->dump(contextp->time());
        printf("a = %d, b = %d, f = %d\n", a, b, top->f);
        assert(top->f == (a ^ b));
    }
    printf("Verification complete.\n");

    m_trace->close();
    delete top;
    exit(EXIT_SUCCESS);
}
