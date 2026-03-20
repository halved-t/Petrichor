#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
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

struct framebuffer {
    uint8_t* address;
    uint64_t width, height, pitch;
    uint16_t bpp;
    size_t ct_x, ct_y;
    uint32_t ct_col;
};

void fb_init(struct framebuffer* fb);
void fb_clear(uint32_t color);
void fb_putchar(char c);
void fb_puts(char* string);

#endif //FRAMEBUFFER_H