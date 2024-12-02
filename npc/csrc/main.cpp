

#include "Vlight.h"
#include <cassert>
#include <cstdio>
#include <nvboard.h>
#include <verilated.h>
#include <verilated_vcd_c.h>

#ifndef TOP_NAME
#define TOP_NAME Vlight
#endif

static TOP_NAME      *dut = new TOP_NAME();
static int            TRACE_DUMP_CYCLE = 1e3;
static int            TRACE_FLUSH_CYCLE = 1e6;
static VerilatedVcdC *g_trace = new VerilatedVcdC();
static int            cur_cycle = 0;

void nvboard_bind_all_pins(TOP_NAME *top);

static void single_cycle_trace() { g_trace->flush(); }

static void single_cycle() {
    cur_cycle++;
    dut->clk = 0;
    dut->eval();
    dut->clk = 1;
    dut->eval();
    if (cur_cycle % TRACE_DUMP_CYCLE == 0)
        g_trace->dump(cur_cycle);
    if (cur_cycle % TRACE_FLUSH_CYCLE == 0) {
        single_cycle_trace();
        printf("dumped trace. cycle = %d\n", cur_cycle);
    }
}

static void reset(int n) {
    dut->rst = 1;
    while (n-- > 0)
        single_cycle();
    dut->rst = 0;
}

static void setup_trace() {
    Verilated::traceEverOn(true);
    dut->trace(g_trace, 99);
    g_trace->open("waveform_light.fsd");
}

int main() {
    setup_trace();
    nvboard_bind_all_pins(dut);
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
