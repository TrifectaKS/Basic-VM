#include "vm.h"
#include <stdio.h>

int vm_main(int argc, char *argv[])
{
    BasicVm vm;
    vm_init(&vm);

    const char *rom_path = "roms/rom.bin";
    if (argc > 1) {
        rom_path = argv[1];
    }

    if (!vm_load_rom(&vm, rom_path)) {
        return 1;
    }

    vm_run(&vm);

    return 0;
}
