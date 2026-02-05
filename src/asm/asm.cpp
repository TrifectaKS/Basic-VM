#include "asm.h"

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
    return assemble_upper_immediates(instruction, asmLineBuffer);
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
  case 0x01:
  {
    return assemble_jumps(instruction, asmLineBuffer);
  }
  case 0x02:
  {
    return assemble_jumps_register(instruction, asmLineBuffer);
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
{
  char instructionStr[20];
  char rdStr[5], rs1Str[5], immStr[10];

  int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instructionStr, rdStr, rs1Str, immStr);
  assert(count == 4);
  if (count != 4) {
    return (AssembledOperation){.hasValue = false};
  }

  uint32_t rd = register_to_byte(rdStr);
  uint32_t rs1 = register_to_byte(rs1Str);
  uint32_t imm = imm_to_word_unsigned(immStr);
  uint32_t insOp = 0;

  insOp |= (instruction->funct3 & 0b00000111);
  insOp |= (instruction->opcode & 0b00011111) << 3;
  insOp |= (instruction->funct4 & 0b00001111) << 8;
  insOp |= (rd & 0b00001111) << 12;
  insOp |= (imm & 0xFF) << 16;
  insOp |= (rs1 & 0xF) << 24;

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
  case 0x01: return handle_funct3_artihmetic(instruction, asmLineBuffer);
  case 0x02: return handle_funct3_immediates(instruction, asmLineBuffer);
  case 0x03: return handle_funct3_upper_immediates(instruction, asmLineBuffer);
  case 0x04: return handle_funct3_stores(instruction, asmLineBuffer);
  case 0x05: return handle_funct3_branches(instruction, asmLineBuffer);
  case 0x06: return handle_funct3_jumps(instruction, asmLineBuffer);
  case 0x07: return handle_funct3_loads(instruction, asmLineBuffer);
  case 0x08: return handle_funct3_bitwise(instruction, asmLineBuffer);
  case 0x09: return handle_funct3_bitwise_immediates(instruction, asmLineBuffer);
  case 0x0A: return handle_funct3_bitwise(instruction, asmLineBuffer);
  case 0x0F:
  {
    uint32_t insOp = 0;
    insOp |= 0x0;
    insOp |= (0x0F & 0x1F) << 3;
    return (AssembledOperation){.value = insOp, .hasValue = true};
  }
  default: return InvalidOperation;
  }
}
