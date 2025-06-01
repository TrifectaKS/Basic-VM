#define DEBUG
#include "debug_utils.h"
#include "instructions.h"
#include "rom_writer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdbool.h>
#include <assert.h>

uint8_t register_to_byte(const char *reg) {
  if (reg[0] == 'r' || reg[0] == 'R') {
    int value = atoi(reg + 1);
    if (value >= 0 && value <= 15) {
      return value;
    }
  }

  return -1;
}

typedef struct {
  uint32_t value;
  bool hasValue;
} AssembledOperation;

AssembledOperation assemble_arithmetic_0(const Instruction *instruction,
                             const char *asmLine) {
  char instructionStr[10];
  char rdStr[5], rs1Str[5], rs2Str[5];

  // TODO: This depends on instruction
  int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instructionStr, rdStr,
                     rs1Str, rs2Str);
  assert(count == 4);
  if(count != 4){
    return (AssembledOperation){.hasValue = false};
  }

  // opcode(4)/funct3(3)/funct4(4)/rd(4)/rs1(4)/rs2(4) in uint32_t
  uint32_t rd = register_to_byte(rdStr);   // 4 bits
  uint32_t rs1 =register_to_byte(rs1Str); // 4 bits
  uint32_t rs2 = register_to_byte(rs2Str); // 4 bits
  uint32_t insOp = 0;

  insOp |= (instruction->funct3 & 0b00000111);      // bits 0–2 (funct3)
  insOp |= (instruction->opcode & 0b00011111) << 3; // bits 3–7 (opcode)
  insOp |= (instruction->funct4 & 0b00001111) << 8; // bits 8–11 (funct4)
  insOp |= (rd & 0b00001111) << 12;                 // bits 12–15 (rd)
  insOp |= (rs1 & 0b00001111) << 16;                // bits 16–19 (rs1)
  insOp |= (rs2 & 0b00001111) << 20;                // bits 20–23 (rs2)

#ifdef DEBUG
  printf("Parsed Assembly \n");
  print_binary32(insOp);
#endif

  return (AssembledOperation){.value = insOp, .hasValue = true};
}

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

    uint8_t opfunct3 = (instruction->opcode & 0b00011111) << 3 |
                       (instruction->funct3 & 0b00000111);

    switch (opfunct3) {
    case 0x08: {
        result = assemble_arithmetic_0(instruction, asmLineBuffer);
      break; 
    }
    };

    assert(result.hasValue == true);

    if(!result.hasValue){
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
