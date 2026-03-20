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
#include <framebuffer/framebuffer.h>
#include <framebuffer/font.h>

uint8_t initialized = 0;

#define ISO_CHAR_MIN 0x00
#define ISO_CHAR_MAX 0xFF
#define ISO_CHAR_WIDTH 8
#define ISO_CHAR_HEIGHT 16

struct framebuffer fbf;

void fb_init(struct framebuffer* fb) {
    fbf.address = fb->address;
    fbf.pitch = fb->pitch;
    fbf.bpp = fb->bpp;
    fbf.width = fb->width;
    fbf.height = fb->height;
    fbf.ct_x = fb->ct_x;
    fbf.ct_y = fb->ct_y;
    fbf.ct_col = fb->ct_col;
    
    initialized = 1;
}

void fb_putpx(uint64_t x, uint64_t y, uint32_t color) {
    if (!initialized) return;

    size_t offset = (size_t)(y * fbf.pitch + x * (fbf.bpp / 8));
    uint8_t* base = fbf.address;
    uint32_t* pixel = (uint32_t *)(base+offset);

    *pixel = color;
}

void fb_clear(uint32_t color) {
    for (uint64_t w = 0; w < fbf.width; w++) {
        for(uint64_t h = 0; h < fbf.height; h++) {
            fb_putpx(w, h, color);
        }
    }
    fbf.ct_x = 0;
    fbf.ct_y = 0;
}

static void fb_putc(char c, uint64_t x, uint64_t y) {
    unsigned char uc = (unsigned char)c;

    x *= ISO_CHAR_WIDTH;
    y *= ISO_CHAR_HEIGHT;

    for(uint32_t row = 0; row < ISO_CHAR_HEIGHT; row++) {
        uint8_t line = font[uc * ISO_CHAR_HEIGHT + row];
        for(uint32_t col = 0; col < ISO_CHAR_WIDTH; col++) {
            if (line & (1 << (ISO_CHAR_WIDTH - col - 1))) {
                uint64_t px = (uint64_t)x + col;
                uint64_t py = (uint64_t)y + row;
                if (py < fbf.height && px < fbf.width) {
                    fb_putpx(px, py, fbf.ct_col);
                }
            }
        }
    }
}

void fb_putchar(char c) {
    switch (c) {
        case '\n':
            fbf.ct_y++;
            fbf.ct_x = 0;
            break;
        case '\r':
            fbf.ct_x = 0;
            break;
        default:
            fb_putc(c, (int)fbf.ct_x, (int)fbf.ct_y);
            fbf.ct_x++;
            break;
    }
}

void fb_puts(char *string) {
    while(*string != '\0') {
        fb_putchar(*string++);
    }
}