#include "rom_writer.h"

BytePack pack_bytes(Instruction* instruction, uint32_t result) {
    BytePack pack = {0};
    pack.count = instruction->length / 8;

    for (uint8_t i = 0; i < pack.count; i++) {
        pack.bytes[i] = (result >> (i * 8)) & 0xFF;
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
