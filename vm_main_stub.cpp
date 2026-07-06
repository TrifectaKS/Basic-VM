#include <stdio.h>
#include "vm/vm_main.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input.asm>\n", argv[0]);
        return 1;
    }
    return vm_main(argv[1]);
}