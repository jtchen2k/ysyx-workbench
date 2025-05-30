#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>
#include <stdint.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 0x0000000b:
        ev.event = EVENT_YIELD;
        c->mepc = c->mepc + 4;
        break;
      default:
        ev.event = EVENT_ERROR; break;
    }
    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : /* no output */ : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

// |               |
// +---------------+ <---- kstack.end
// |               |
// |    context    |
// |               |
// +---------------+ <--+
// |               |    |
// |               |    |
// |               |    |
// |               |    |
// +---------------+    |
// |       cp      | ---+
// +---------------+ <---- kstack.start
// |               |

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *ctx = (Context *)kstack.end - sizeof(Context);
  ctx->mepc = (uintptr_t)entry;
#ifdef CONFIG_ISA64
  ctx->mstatus = 0xa00001800;
#else
  ctx->mstatus = 0x1800;
#endif
  ctx->gpr[10] = (uintptr_t)arg;
  return ctx;
}

void yield() {
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
