#define DEBUG

#include "assemble.h"
#include "instructions.h"
#include "rom_writer.h"
#include <assert.h>

AssembledOperation handle_funct3_artihmetic(Instruction *instruction, char asmLineBuffer[256])
{
  switch (instruction->funct3)
  {
  case 0x00:
  {
    return assemble_arithmetic_bitwise(instruction, asmLineBuffer);
  }
  }

  return InvalidOperation;
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
  
  return InvalidOperation;
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

  return InvalidOperation;
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

  return InvalidOperation;
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

  return InvalidOperation;
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
  
  return InvalidOperation;
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
  
  return InvalidOperation;
}


AssembledOperation assemble_shift_immediates(const Instruction *instruction, const char *asmLine)
// Handles SLLI/SRLI with 8-bit immediate where only lower 5 bits are used
{
  char instructionStr[20];
  char rdStr[5], rs1Str[5], immStr[10];

  int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instructionStr, rdStr,
                     rs1Str, immStr);
  assert(count == 4);
  if (count != 4) {
    return (AssembledOperation){.hasValue = false};
  }

  uint32_t rd = register_to_byte(rdStr);   // 4 bits
  uint32_t rs1 = register_to_byte(rs1Str); // 4 bits
  uint32_t imm = imm_to_word_unsigned(immStr); // 16 bits, but only lower 5 bits used
  uint32_t insOp = 0;

  // NOTE: Shift amount uses only 5 bits (0-31), upper 3 bits of 8-bit imm are zeroed
  imm = imm & 0x1F;

  insOp |= (instruction->funct3 & 0b00000111); // bits 0-2 (funct3)
  insOp |= (instruction->opcode & 0b00011111) << 3; // bits 3-7 (opcode)
  insOp |= (rd & 0b00001111) << 8; // bits 8-11 (rd)
  insOp |= (rs1 & 0b00001111) << 12; // bits 12-15 (rs1)
  insOp |= imm << 16; // bits 16-23 (imm - only lower 5 bits used)

  return (AssembledOperation){.value = insOp, .hasValue = true};
}

AssembledOperation handle_funct3_bitwise(Instruction *instruction, char asmLineBuffer[256])
{
  switch(instruction->funct3){
    case 0x0:
      {
        return assemble_arithmetic_bitwise(instruction, asmLineBuffer);
      }
    case 0x1:
      {
        // SLLI, SRLI - use shift immediate handler (masks to 5 bits)
        return assemble_shift_immediates(instruction, asmLineBuffer);
      }
  }
  return InvalidOperation;
}

AssembledOperation handle_funct3_bitwise_immediates(Instruction *instruction, char asmLineBuffer[256])
{
  switch(instruction->funct3){
    case 0x0:
    case 0x1:
    case 0x2:
      {
        return assemble_immediates_loads(instruction, asmLineBuffer);
      }
  }
  return InvalidOperation;
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
  case 0x08:
  {
    return handle_funct3_bitwise(instruction, asmLineBuffer);
  }
  case 0x09:
  {
    return handle_funct3_bitwise_immediates(instruction, asmLineBuffer);
  }
  case 0x0A:
  {
    return handle_funct3_bitwise(instruction, asmLineBuffer);
  }
  case 0x0F:
  {
    // HALT instruction - no operands
    // Encoding: funct3(3) | opcode(5) | rd(4) | rs1(4) | imm(16) = 0x78xxxxxx
    // HALT: funct3=0, opcode=0x0F, rd=0, rs1=0, imm=0
    uint32_t insOp = 0;
    insOp |= 0x0;                        // funct3 = 0
    insOp |= (0x0F & 0x1F) << 3;        // opcode = 0x0F
    insOp |= 0 << 8;                     // rd = 0
    insOp |= 0 << 12;                    // rs1 = 0
    insOp |= 0 << 16;                    // imm = 0
    return (AssembledOperation){.value = insOp, .hasValue = true};
  }
  default:
    return InvalidOperation;
  }

  return InvalidOperation;
}

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

  return 0;
}
