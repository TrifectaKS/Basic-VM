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

BytePack pack_bytes(Instruction* instruction, uint32_t result) {
    BytePack pack = {0};
    pack.count = instruction->length / 8;

    for (uint8_t i = 0; i < pack.count; i++) {
        pack.bytes[i] = (result >> (i * 8)) & 0xFF;  // LSB first
    }

    return pack;
}

void write_to_file(FILE* romFile, BytePack pack){
#ifdef DEBUG
    for(int i = 0; i < pack.count; i++){
        printf("Packing byte %d: ", i);
        print_binary8(pack.bytes[i]);
    }
#endif

    fwrite(pack.bytes,1,pack.count,romFile);
}

#endif
