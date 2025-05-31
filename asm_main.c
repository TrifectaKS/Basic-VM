#define DEBUG
#include "debug_utils.h"
#include "instructions.h"
#include "rom_writer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

uint8_t regToByte(const char *reg) {
  if (reg[0] == 'r' || reg[0] == 'R') {
    int value = atoi(reg + 1);
    if (value >= 0 && value <= 15) {
      return value;
    }
  }

  return -1;
}

uint32_t assembleArithmetic0(const Instruction *instruction,
                             const char *asmLine) {
  char instructionStr[10];
  char rdStr[5], rs1Str[5], rs2Str[5];

  // TODO: This depends on instruction
  int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instructionStr, rdStr,
                     rs1Str, rs2Str);

  // TODO: Assert count

  // opcode(4)/funct3(3)/funct4(4)/rd(4)/rs1(4)/rs2(4) in uint32_t
  uint32_t rd = regToByte(rdStr);   // 4 bits
  uint32_t rs1 = regToByte(rs1Str); // 4 bits
  uint32_t rs2 = regToByte(rs2Str); // 4 bits
  uint32_t insOp = 0;

  insOp |= (instruction->funct3 & 0b00000111);      // bits 0–2 (funct3)
  insOp |= (instruction->opcode & 0b00011111) << 3; // bits 3–7 (opcode)
  insOp |= (instruction->funct4 & 0b00001111) << 8; // bits 8–11 (funct4)
  insOp |= (rd & 0b00001111) << 12;                 // bits 12–15 (rd)
  insOp |= (rs1 & 0b00001111) << 16;                // bits 16–19 (rs1)
  insOp |= (rs2 & 0b00001111) << 20;                // bits 20–23 (rs2)

#ifdef DEBUG
  printf("Parsed Assembly: ");
  print_binary32(insOp);
#endif

  return insOp;
}

int main(){
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

  // Check if the file was opened successfully.
  while (fgets(asmLineBuffer, sizeof(asmLineBuffer), asmFile)) {
    printf("----------------\n%s\n", asmLineBuffer);
    Instruction *instruction = get_instruction_by_asm(asmLineBuffer);

    uint8_t opfunct3 = (instruction->opcode & 0b00011111) << 3 |
                       (instruction->opcode & 0b00000111);
    print_binary8(instruction->opcode);
    print_binary8(instruction->funct3);
    print_binary8(opfunct3);

    uint32_t result = assembleArithmetic0(instruction, asmLineBuffer);
    BytePack pack = pack_bytes(instruction, result);
    write_to_file(romFile, pack);
  }

  // Close files
  fclose(asmFile);
  fclose(romFile);
}
