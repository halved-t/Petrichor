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

#include <sys/idt.h>
#include <sys/gdt.h>
#include <stddef.h>

static idt_entry_t idt[IDT_ENTRIES];
static idtr_t      idtr;

extern uint64_t idt_stub_table[IDT_ENTRIES];  // from idt.asm

static void idt_set_entry(uint8_t vector, uint64_t handler,
                          uint8_t type_attr, uint8_t ist) {
    idt[vector].offset_low  = handler & 0xFFFF;
    idt[vector].selector    = GDT_KERNEL_CODE;
    idt[vector].ist         = ist & 0x7;
    idt[vector].type_attr   = type_attr;
    idt[vector].offset_mid  = (handler >> 16) & 0xFFFF;
    idt[vector].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[vector].reserved    = 0;
}

void idt_init(void) {
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_entry(i, idt_stub_table[i], IDT_GATE_INTERRUPT, 0);
    }

    // only double fault for now :3
    idt_set_entry(8, idt_stub_table[8], IDT_GATE_INTERRUPT, 1);

    idtr.limit = sizeof(idt) - 1;
    idtr.base  = (uint64_t)idt;

    __asm__ volatile("lidt %0" : : "m"(idtr));
}

void idt_set_handler(uint8_t vector, isr_handler_t handler) {
    idt_set_entry(vector, (uint64_t)handler, IDT_GATE_INTERRUPT, 0);
}