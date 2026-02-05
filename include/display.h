#ifndef DISPLAY_H
#define DISPLAY_H

#include "architecture.h"

void display_init(BasicVm *vm);
void display_clear(BasicVm *vm);
void display_update(BasicVm *vm);
void display_shutdown(BasicVm *vm);

#endif // DISPLAY_H
