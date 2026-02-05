#define DEBUG

#include "asm.h"
#include <assert.h>

int asm_main()
{
  FILE *asmFile = fopen("roms/rom.asm", "r");
  FILE *romFile = fopen("roms/rom.bin", "wb");

  if (asmFile == NULL)
  {
    perror("Error opening .asm file");
    return 1;
  }

  if (romFile == NULL)
  {
    perror("Error opening .rom file");
    return 1;
  }

  char asmLineBuffer[256];
  AssembledOperation result;
  uint32_t program_counter = 0;
  uint32_t line = 1;
  while (fgets(asmLineBuffer, sizeof(asmLineBuffer), asmFile))
  {
    printf("----------------\n%s\n", asmLineBuffer);
    Instruction *instruction = get_instruction_by_asm(asmLineBuffer);

    switch (instruction->opcode_funct3)
    {
    case 0x00:
    {
      printf("NULL line detected on line %d\n", line);
      continue;
    }
    default:
    {
      result = handle_opcode(instruction, asmLineBuffer);
    }
    };

    assert(result.hasValue == true);

    if (!result.hasValue)
    {
      perror("Error assembling line: ");
      perror(asmLineBuffer);
      return 1;
    }

    BytePack pack = pack_bytes(instruction, result.value);
    write_to_file(romFile, pack);

    line++;
    program_counter += instruction->length / 8;
  }

  fclose(asmFile);
  fclose(romFile);

  return 0;
}
