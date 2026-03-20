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

#include <stddef.h>
#include <sys/gdt.h>

typedef union {
    gdt_entry_t entry;
    uint64_t    raw;
} gdt_slot_t;

static gdt_slot_t gdt[7];
static gdtr_t     gdtr;

extern void gdt_flush(gdtr_t *gdtr);
extern void tss_flush(uint16_t selector);

static void gdt_set_entry(int i, uint32_t base, uint32_t limit,
                          uint8_t access, uint8_t gran) {
    gdt[i].entry.base_low    = base & 0xFFFF;
    gdt[i].entry.base_mid    = (base >> 16) & 0xFF;
    gdt[i].entry.base_high   = (base >> 24) & 0xFF;
    gdt[i].entry.limit_low   = limit & 0xFFFF;
    gdt[i].entry.granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[i].entry.access      = access;
}

static void gdt_set_tss(tss_t *tss) {
    uint64_t base  = (uint64_t)tss;
    uint32_t limit = sizeof(tss_t) - 1;

    gdt_tss_entry_t desc = {0};
    desc.limit_low   = limit & 0xFFFF;
    desc.base_low    = base & 0xFFFF;
    desc.base_mid    = (base >> 16) & 0xFF;
    desc.access      = 0x89;
    desc.granularity = (limit >> 16) & 0x0F;
    desc.base_high2  = (base >> 24) & 0xFF;
    desc.base_high3  = (base >> 32) & 0xFFFFFFFF;
    desc.reserved    = 0;

    __builtin_memcpy(&gdt[5], &desc, sizeof(gdt_tss_entry_t));
}

void gdt_init(void) {
    gdt_set_entry(0, 0, 0, 0, 0);
    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xA0);
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xC0);
    gdt_set_entry(3, 0, 0xFFFFF, 0xF2, 0xC0);
    gdt_set_entry(4, 0, 0xFFFFF, 0xFA, 0xA0);
    gdt_set_entry(5, 0, 0, 0, 0);
    gdt_set_entry(6, 0, 0, 0, 0);

    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base  = (uint64_t)gdt;
    gdt_flush(&gdtr);
}

void gdt_load_tss(tss_t *tss) {
    tss->iopb = sizeof(tss_t);
    gdt_set_tss(tss);
    tss_flush(GDT_TSS_LOW);
}