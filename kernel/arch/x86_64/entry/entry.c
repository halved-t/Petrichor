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
#include <stdint.h>
#include <stdbool.h>

#include <limine.h>
#include <klibc/memory.h>
#include <asm/asm.h>
#include <framebuffer/framebuffer.h>
#include <debug/debug.h>
#include <serial/serial.h>
#include <sys/gdt.h>
#include <sys/idt.h>

// Limine stuff.

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(5);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

// Limine stuff ends here.

static void hcf(void) {
    for (;;) {
        halt();
    }
}


void kmain(void) {
    // Limine checks.
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) hcf();
    if (memmap_request.response == NULL) hcf();
    if (hhdm_request.response == NULL) hcf();
    if(framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) hcf();

    // Get the first framebuffer.
    struct limine_framebuffer* framebuffer = framebuffer_request.response->framebuffers[0];

    // Initialize the framebuffer.
    struct framebuffer fb;
    fb.address = framebuffer->address;
    fb.pitch = framebuffer->pitch;
    fb.bpp = framebuffer->bpp;
    fb.width = framebuffer->width;
    fb.height = framebuffer->height;
    fb.ct_col = 0x000000;
    fb.ct_x = 0;
    fb.ct_y = 0;
    fb_init(&fb);
    fb_clear(0xffffcc);

    // Initialize the GDT and the IDT.
    gdt_init();
    idt_init();

    // Do some printing.
    fb_puts("Hello, framebuffer.\n");
    ser_puts("Hello, serial.\n");
    
    kprintf("%s location: %x\n", "kputs", kputs);

    __asm__ volatile("int3");

    // We're done, hang.
    hcf();
}