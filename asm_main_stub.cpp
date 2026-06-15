#include "asm/asm_main.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.asm> <output.rom>\n", argv[0]);
        return 1;
    }
    return asm_main(argv[1], argv[2]);
}