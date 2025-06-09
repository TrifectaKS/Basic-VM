#define DEBUG

#include "assemble.h"
#include "debug_utils.h"
#include "instructions.h"
#include "rom_writer.h"
#include <assert.h>

int main() {
  FILE *asmFile = fopen("roms/rom.asm", "r");
  FILE *romFile = fopen("roms/rom.bin", "wb");

  if (asmFile == NULL) {
    perror("Error opening .asm file");
    return 1;
  }

  if (romFile == NULL) {
    perror("Error opening .rom file");
    return 1;
  }

  char asmLineBuffer[256];
  AssembledOperation result;
  // Check if the file was opened successfully.
  while (fgets(asmLineBuffer, sizeof(asmLineBuffer), asmFile)) {
    printf("----------------\n%s\n", asmLineBuffer);
    Instruction *instruction = get_instruction_by_asm(asmLineBuffer);

    switch (instruction->opcode_funct3) {
    case 0x08: {
      result = assemble_arithmetic(instruction, asmLineBuffer);
      break;
    }
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14: {
      result = assemble_immediates(instruction, asmLineBuffer);
      break;
    }
    case 0x18: {
      result = assemble_upper_immediates(instruction, asmLineBuffer);
      break;
    }
    case 0x20:
    case 0x21:
    case 0x22: {
      result = assemble_stores(instruction, asmLineBuffer);
      break;
    }
    };

    assert(result.hasValue == true);

    if (!result.hasValue) {
      perror("Error assembling line: ");
      perror(asmLineBuffer);
      return 1;
    }

    BytePack pack = pack_bytes(instruction, result.value);
    write_to_file(romFile, pack);
  }

  // Close files
  fclose(asmFile);
  fclose(romFile);
}
