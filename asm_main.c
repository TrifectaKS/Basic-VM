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
  uint32_t program_counter = 0;
  uint32_t line = 1;
  // Check if the file was opened successfully.
  while (fgets(asmLineBuffer, sizeof(asmLineBuffer), asmFile)) {
    printf("----------------\n%s\n", asmLineBuffer);
    Instruction *instruction = get_instruction_by_asm(asmLineBuffer);

    switch (instruction->opcode_funct3) {
    case 0x00: // NULL 
        {
      printf("NULL line detected on line %d\n", line);
      continue;
    }
    case 0x08: // ARITHMETIC
    {
      result = assemble_arithmetic(instruction, asmLineBuffer);
      break;
    }
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14: // IMMEDIATES
    case 0x38:
    case 0x39:
    case 0x3A: // LOADS
    {
      result = assemble_immediates_loads(instruction, asmLineBuffer);
      break;
    }
    case 0x18: // UPPER IMMEDIATES
    case 0x31: // JUMPS
    {
      result = assemble_upper_immediates_jumps(instruction, asmLineBuffer);
      break;
    }
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x28:
    case 0x29:
    case 0x2A:
    case 0x2B:
    case 0x2C:
    case 0x2D: // BRANCHES
    {
      result = assemble_stores_branches(instruction, asmLineBuffer);
      break;
    }
    {}
    };

    assert(result.hasValue == true);

    if (!result.hasValue) {
      perror("Error assembling line: ");
      perror(asmLineBuffer);
      return 1;
    }

    BytePack pack = pack_bytes(instruction, result.value);
    write_to_file(romFile, pack);

    line++;
    program_counter += instruction->length/8;
  }

  // Close files
  fclose(asmFile);
  fclose(romFile);
}
