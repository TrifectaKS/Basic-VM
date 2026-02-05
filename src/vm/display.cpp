#include "display.h"
#include "architecture.h"
#include <string.h>

void display_init(BasicVm *vm) {
    memset(vm->display_buffer, 0, sizeof(vm->display_buffer));
}

void display_clear(BasicVm *vm) {
    memset(vm->display_buffer, 0, sizeof(vm->display_buffer));
}

void display_update(BasicVm *vm) {
    // Placeholder for display rendering
}

void display_shutdown(BasicVm *vm) {
    // No cleanup needed
}
