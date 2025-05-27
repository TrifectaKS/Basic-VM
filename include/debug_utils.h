#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include <stdint.h>
#include <stdio.h>

void print_binary32(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i % 8 == 0) printf(" | "); // optional spacing every 4 bits
        else if (i % 4 == 0) printf(" "); // optional spacing every 4 bits
    }
    printf("\n");
}

void print_binary8(uint8_t value) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i % 4 == 0) printf(" "); // optional spacing every 4 bits
    }
    printf("\n");
}

#endif