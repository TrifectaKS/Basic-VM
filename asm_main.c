#define DEBUG

#include "assemble.h"
#include "debug_utils.h"
#include "instructions.h"
#include "rom_writer.h"
#include <assert.h>

AssembledOperation handle_funct3_artihmetic(Instruction *instruction, char asmLineBuffer[256])
{
  switch (instruction->funct3)
  {
  case 0x00:
  {
    return assemble_arithmetic(instruction, asmLineBuffer);
  }
  }
}

AssembledOperation handle_funct3_immediates(Instruction *instruction, char asmLineBuffer[256])
{
  switch (instruction->funct3)
  {
  case 0x00:
  case 0x01:
  case 0x02:
  case 0x03:
  {
    return assemble_immediates_loads(instruction, asmLineBuffer);
  }
  }
}

AssembledOperation handle_funct3_upper_immediates(Instruction *instruction, char asmLineBuffer[256])
{
  switch (instruction->funct3)
  {
  case 0x00:
  {
    return assemble_upper_immediates_jumps(instruction, asmLineBuffer);
  }
  }
}

AssembledOperation handle_funct3_stores(Instruction *instruction, char asmLineBuffer[256])
{
  switch (instruction->funct3)
  {
  case 0x00:
  case 0x01:
  case 0x02:
  {
    return assemble_stores_branches(instruction, asmLineBuffer);
  }
  }
}

AssembledOperation handle_funct3_branches(Instruction *instruction, char asmLineBuffer[256])
{
  switch (instruction->funct3)
  {
  case 0x00:
  case 0x01:
  case 0x02:
  case 0x03:
  case 0x04:
  case 0x05:
  {
    return assemble_stores_branches(instruction, asmLineBuffer);
  }
  }
}

AssembledOperation handle_funct3_jumps(Instruction *instruction, char asmLineBuffer[256])
{
  switch (instruction->funct3)
  {
  case 0x00:
  {
    return assemble_upper_immediates_jumps(instruction, asmLineBuffer);
  }
  }
}

AssembledOperation handle_funct3_loads(Instruction *instruction, char asmLineBuffer[256])
{
  switch (instruction->funct3)
  {
  case 0x00:
  case 0x01:
  case 0x02:
  {
    return assemble_immediates_loads(instruction, asmLineBuffer);
  }
  }
}

AssembledOperation handle_opcode(Instruction *instruction, char asmLineBuffer[256])
{
  switch (instruction->opcode)
  {
  case 0x01:
  {
    return handle_funct3_artihmetic(instruction, asmLineBuffer);
  }
  case 0x02:
  {
    return handle_funct3_immediates(instruction, asmLineBuffer);
  }
  case 0x03:
  {
    return handle_funct3_upper_immediates(instruction, asmLineBuffer);
  }
  case 0x04:
  {
    return handle_funct3_stores(instruction, asmLineBuffer);
  }
  case 0x05:
  {
    return handle_funct3_branches(instruction, asmLineBuffer);
  }
  case 0x06:
  {
    return handle_funct3_jumps(instruction, asmLineBuffer);
  }
  case 0x07:
  {
    return handle_funct3_loads(instruction, asmLineBuffer);
  }
  default:
    return InvalidOperation;
  }
}

int main()
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
  // Check if the file was opened successfully.
  while (fgets(asmLineBuffer, sizeof(asmLineBuffer), asmFile))
  {
    printf("----------------\n%s\n", asmLineBuffer);
    Instruction *instruction = get_instruction_by_asm(asmLineBuffer);

    switch (instruction->opcode_funct3)
    {
    case 0x00: // NULL - Can't parse line
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

  // Close files
  fclose(asmFile);
  fclose(romFile);
}
