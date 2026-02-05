#ifndef ROM_WRITER_H
#define ROM_WRITER_H

#include "instructions.h"

#ifdef DEBUG
#include "debug_utils.h"
#endif

typedef struct {
    uint8_t bytes[4];
    uint8_t count;
} BytePack;

BytePack pack_bytes(Instruction* instruction, uint32_t result);
void write_to_file(FILE* romFile, BytePack pack);

#endif
