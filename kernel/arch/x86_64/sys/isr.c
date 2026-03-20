/*
 * Copyright 2026 Halved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sys/isr.h>
#include <debug/debug.h>

const char *exception_names[32] = {
    "Division by zero",        "Debug",
    "NMI",                     "Breakpoint",
    "Overflow",                "Bound range exceeded",
    "Invalid opcode",          "Device not available",
    "Double fault",            "Coprocessor segment overrun",
    "Invalid TSS",             "Segment not present",
    "Stack-segment fault",     "General protection fault",
    "Page fault",              "(reserved)",
    "x87 FPU error",           "Alignment check",
    "Machine check",           "SIMD FP exception",
    "Virtualization exception","Control protection exception",
    "(reserved)",              "(reserved)",
    "(reserved)",              "(reserved)",
    "(reserved)",              "(reserved)",
    "Hypervisor injection",    "VMM communication",
    "Security exception",      "(reserved)",
};

static isr_handler_t handlers[256];

// Called from isr_common in idt.asm
void isr_dispatch(interrupt_frame_t *frame) {
    if (handlers[frame->vector]) {
        handlers[frame->vector](frame);
        return;
    }

    if (frame->vector < 32) {
        kprintf("EXCEPTION %llu: %s\n",
                     frame->vector, exception_names[frame->vector]);
    } else {
        kprintf("UNHANDLED IRQ %llu\n", frame->vector);
    }

    kprintf("  RIP=%016llx  CS=%04llx  RFLAGS=%016llx\n",
                 frame->rip, frame->cs, frame->rflags);
    kprintf("  RSP=%016llx  SS=%04llx\n",
                 frame->rsp, frame->ss);
    kprintf("  RAX=%016llx  RBX=%016llx  RCX=%016llx\n",
                 frame->rax, frame->rbx, frame->rcx);
    kprintf("  RDX=%016llx  RSI=%016llx  RDI=%016llx\n",
                 frame->rdx, frame->rsi, frame->rdi);
    kprintf("  ERR=%016llx\n", frame->error_code);

    __asm__ volatile("cli; hlt");
    for (;;) __asm__ volatile("hlt");
}

void isr_register_handler(uint8_t vector, isr_handler_t handler) {
    handlers[vector] = handler;
}