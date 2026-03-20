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

#include <debug/debug.h>
#include <framebuffer/framebuffer.h>
#include <serial/serial.h>

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

void kputc(char c) {
    if (c == '\n')
        kputc('\r');
    ser_putc(c);
    fb_putchar(c);
}

void kputs(char *string) {
    ser_puts(string);
    fb_puts(string);
}

static void kprintf_(char *fmt, va_list args) {
    while (*fmt) {
        if (*fmt != '%') {
            kputc(*fmt++);
            continue;
        }

        fmt++; // skip '%'

        int width = 0;
        int zero_pad = 0;
        if (*fmt == '0') {
            zero_pad = 1;
            fmt++;
        }
        while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10 + (*fmt - '0');
            fmt++;
        }

        // skip %l, %ll length prefixes
        if (*fmt == 'l') {
            fmt++;
            if (*fmt == 'l')
                fmt++;
        }

        switch (*fmt) {
            case '%':
                kputc('%');
                break;

            case 's': {
                char *str = va_arg(args, char *);
                kputs(str ? str : "(null)");
                break;
            }

            case 'S': {
                const char *str = va_arg(args, const char *);
                size_t len = va_arg(args, size_t);
                for (size_t i = 0; i < len; i++)
                    kputc(str[i]);
                break;
            }

            case 'x':
            case 'p': {
                char buf[17];
                uint64_t n = va_arg(args, uint64_t);
                buf[16] = '\0';
                for (int i = 15; i >= 0; i--) {
                    buf[i] = "0123456789abcdef"[n & 0xf];
                    n >>= 4;
                }
                // find first non-zero digit
                int start = 0;
                while (start < 15 && buf[start] == '0')
                    start++;
                // apply zero padding
                if (zero_pad && width > 0) {
                    int len = 16 - start;
                    while (len < width) {
                        start--;
                        buf[start] = '0';
                        len++;
                    }
                }
                kputs(&buf[start]);
                break;
            }

            case 'u': {
                char buf[21];
                uint64_t n = va_arg(args, uint64_t);
                buf[20] = '\0';
                int i = 19;
                if (n == 0) {
                    buf[i--] = '0';
                } else {
                    while (n > 0) {
                        buf[i--] = '0' + (n % 10);
                        n /= 10;
                    }
                }
                kputs(&buf[i + 1]);
                break;
            }

            case 'd': {
                char buf[21];
                int64_t n = va_arg(args, int64_t);
                buf[20] = '\0';
                int i = 19;
                int neg = n < 0;
                uint64_t u = neg ? -(uint64_t)n : (uint64_t)n;
                if (u == 0) {
                    buf[i--] = '0';
                } else {
                    while (u > 0) {
                        buf[i--] = '0' + (u % 10);
                        u /= 10;
                    }
                }
                if (neg)
                    buf[i--] = '-';
                kputs(&buf[i + 1]);
                break;
            }

            case 'c':
                kputc((char)va_arg(args, int));
                break;

            default:
                kputc('%');
                kputc(*fmt);
                break;
        }

        fmt++;
    }
}

void kprintf(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    kprintf_(fmt, args);
    va_end(args);
}