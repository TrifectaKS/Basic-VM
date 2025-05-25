#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include "hashmap.h"

HashMap instructionLengthMap;
    
typedef struct {
    uint8_t* name;
    uint8_t opcode;
    uint8_t funct3;
    uint8_t funct4;
    uint8_t length;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint32_t imm;
} Instruction;

Instruction instructions[] = {
    {"ADD", 0x1, 0x0, 0x1, 24, true, true, true, NULL},
    {"SUB", 0x1, 0x0, 0x1, 24, true, true, true, NULL},
    {"MUL", 0x1, 0x0, 0x2, 24, true, true, true, NULL},
    {"DIV", 0x1, 0x0, 0x3, 24, true, true, true, NULL},
};

void print_binary32(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i % 8 == 0) printf(" | "); // optional spacing every 4 bits
        else if (i % 4 == 0) printf(" "); // optional spacing every 4 bits
    }
    printf("\n");
}

void print_binary8(uint8_t value) {
    for (int i = 7; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i % 4 == 0) printf(" "); // optional spacing every 4 bits
    }
    printf("\n");
}

uint8_t regToByte(const char* reg){
    if(reg[0] == 'r' || reg[0] == 'R'){
        int value = atoi(reg+1);
        if(value >= 0 && value <= 15){
            return value;
        }
    }

    return -1;
}

uint32_t assemble(const char* asmLine){
    char instruction[10];
    char rdStr[5], rs1Str[5], rs2Str[5];

    int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instruction, rdStr, rs1Str, rs2Str);
    
    //TODO: Assert count

    Instruction* op = NULL;

    //TODO: Convert to map
    for(int i = 0; i < sizeof(instructions)/sizeof(instructions[0]); i++){
        if(strcasecmp(instruction, instructions[i].name) == 0){
            op = &instructions[i];
            break;
        }
    }

    //TODO: Assert Op found
    //opcode(4)/funct3(3)/funct4(4)/rd(4)/rs1(4)/rs2(4) in uint32_t
    uint32_t rd = regToByte(rdStr); //4 bits
    uint32_t rs1 = regToByte(rs1Str); //4 bits
    uint32_t rs2 = regToByte(rs2Str); //4 bits
    uint32_t insOp = 0;

    insOp |= (op->funct3 & 0b00000111);       // bits 0–2 (funct3)
    print_binary32(insOp);

    insOp |= (op->opcode & 0b00011111) << 3;            // bits 3–7 (opcode)
    print_binary32(insOp);

    insOp |= (op->funct4 & 0b00001111) << 8;       // bits 8–11 (funct4)
    print_binary32(insOp);

    insOp |= (rd & 0b00001111) << 12;              // bits 12–15 (rd)
    print_binary32(insOp);

    insOp |= (rs1 & 0b00001111) << 16;             // bits 16–19 (rs1)
    print_binary32(insOp);

    insOp |= (rs2 & 0b00001111) << 20;             // bits 20–23 (rs2)
    print_binary32(insOp);


    return insOp;
}

int main(){
    hashmap_init(&instructionLengthMap);
    hashmap_put(&instructionLengthMap, 10, 42);
    hashmap_put(&instructionLengthMap, 255, 99);


    const char* asm_line1 = "ADD r3, r3, r3";
    uint32_t result = assemble(asm_line1);

    FILE* f = fopen("rom.bin", "wb");
    if(!f){
        perror("error opening file");
        return 1;
    }

    //write in little endian: LSB First
    uint8_t bytes[3];
    bytes[0] = (result>>0) & 0xFF;
    print_binary8(bytes[0]);
    bytes[1] = (result>>8) & 0xFF;
    print_binary8(bytes[1]);
    bytes[2] = (result>>16) & 0xFF;
    print_binary8(bytes[2]);

    fwrite(bytes,1,3,f);
    fclose(f);
}