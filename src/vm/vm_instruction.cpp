#include "vm_instruction.h"
#include "config.h"
#include "display.h"
#include <stdio.h>

#ifndef DISABLE_EXECUTION
void vm_execute_instruction(BasicVm *vm, DecodedInstruction dec, uint16_t next_pc) {
    switch (dec.opcode) {
        case 0x01: // Arithmetic R-type (ADD, SUB, MUL, DIV)
            switch (dec.funct4) {
                case 0x00: // ADD
                    vm->registers[dec.rd] = vm->registers[dec.rs1] + vm->registers[dec.rs2];
                    break;
                case 0x01: // SUB
                    vm->registers[dec.rd] = vm->registers[dec.rs1] - vm->registers[dec.rs2];
                    break;
                case 0x02: // MUL
                    vm->registers[dec.rd] = vm->registers[dec.rs1] * vm->registers[dec.rs2];
                    break;
                case 0x03: // DIV
                    if (vm->registers[dec.rs2] != 0) {
                        vm->registers[dec.rd] = vm->registers[dec.rs1] / vm->registers[dec.rs2];
                    } else {
                        printf("Error: Division by zero\n");
                    }
                    break;
            }
            vm->program_counter = next_pc;
            break;

        case 0x02: // Immediates I-type (ADDI, SUBI, MULI, DIVI)
            switch (dec.funct3) {
                case 0x00: // ADDI
                    vm->registers[dec.rd] = vm->registers[dec.rs1] + (dec.imm & 0xFF);
                    break;
                case 0x01: // SUBI
                    vm->registers[dec.rd] = vm->registers[dec.rs1] - (dec.imm & 0xFF);
                    break;
                case 0x02: // MULI
                    vm->registers[dec.rd] = vm->registers[dec.rs1] * (dec.imm & 0xFF);
                    break;
                case 0x03: // DIVI
                    if ((dec.imm & 0xFF) != 0) {
                        vm->registers[dec.rd] = vm->registers[dec.rs1] / (dec.imm & 0xFF);
                    } else {
                        printf("Error: Division by zero\n");
                    }
                    break;
            }
            vm->program_counter = next_pc;
            break;

        case 0x03: // Upper Immediates U-type (LUI, AUIPC)
            switch (dec.funct4) {
                case 0x00: // LUI: rd = imm << 8
                    vm->registers[dec.rd] = (dec.imm >> 8) & 0xFF;
                    break;
                case 0x01: // AUIPC: rd = PC + (imm << 8)
                    vm->registers[dec.rd] = ((next_pc + (dec.imm << 8)) >> 8) & 0xFF;
                    break;
            }
            vm->program_counter = next_pc;
            break;

        case 0x04: // Stores S-type (SB, SH, SW)
            switch (dec.funct3) {
                case 0x00: // SB: store byte
                    vm->memory[(vm->registers[dec.rs1] + dec.imm) & 0xFFF] = vm->registers[dec.rs2];
                    break;
                case 0x01: // SH: store halfword (2 bytes)
                    vm->memory[(vm->registers[dec.rs1] + dec.imm) & 0xFFF] = vm->registers[dec.rs2] & 0xFF;
                    vm->memory[(vm->registers[dec.rs1] + dec.imm + 1) & 0xFFF] = (vm->registers[dec.rs2] >> 8) & 0xFF;
                    break;
                case 0x02: // SW: store word (4 bytes, using two registers)
                    vm->memory[(vm->registers[dec.rs1] + dec.imm) & 0xFFF] = vm->registers[dec.rs2] & 0xFF;
                    vm->memory[(vm->registers[dec.rs1] + dec.imm + 1) & 0xFFF] = (vm->registers[dec.rs2] >> 8) & 0xFF;
                    vm->memory[(vm->registers[dec.rs1] + dec.imm + 2) & 0xFFF] = vm->registers[dec.rs2+1] & 0xFF;
                    vm->memory[(vm->registers[dec.rs1] + dec.imm + 3) & 0xFFF] = (vm->registers[dec.rs2+1] >> 8) & 0xFF;
                    break;
            }
            vm->program_counter = next_pc;
            break;

        case 0x05: // Branches B-type (BEQ, BNE, BLT, BGT, BLE, BGE)
            switch (dec.funct3) {
                case 0x00: // BEQ: if rs1 == rs2, PC = PC + imm
                    if (vm->registers[dec.rs1] == vm->registers[dec.rs2]) {
                        vm->program_counter = next_pc + (int16_t)dec.imm;
                    } else {
                        vm->program_counter = next_pc;
                    }
                    break;
                case 0x01: // BNE: if rs1 != rs2, PC = PC + imm
                    if (vm->registers[dec.rs1] != vm->registers[dec.rs2]) {
                        vm->program_counter = next_pc + (int16_t)dec.imm;
                    } else {
                        vm->program_counter = next_pc;
                    }
                    break;
                case 0x02: // BLT: if rs1 < rs2 (signed), PC = PC + imm
                    if ((int8_t)vm->registers[dec.rs1] < (int8_t)vm->registers[dec.rs2]) {
                        vm->program_counter = next_pc + (int16_t)dec.imm;
                    } else {
                        vm->program_counter = next_pc;
                    }
                    break;
                case 0x03: // BGT: if rs1 > rs2 (signed), PC = PC + imm
                    if ((int8_t)vm->registers[dec.rs1] > (int8_t)vm->registers[dec.rs2]) {
                        vm->program_counter = next_pc + (int16_t)dec.imm;
                    } else {
                        vm->program_counter = next_pc;
                    }
                    break;
                case 0x04: // BLE: if rs1 <= rs2 (signed), PC = PC + imm
                    if ((int8_t)vm->registers[dec.rs1] <= (int8_t)vm->registers[dec.rs2]) {
                        vm->program_counter = next_pc + (int16_t)dec.imm;
                    } else {
                        vm->program_counter = next_pc;
                    }
                    break;
                case 0x05: // BGE: if rs1 >= rs2 (signed), PC = PC + imm
                    if ((int8_t)vm->registers[dec.rs1] >= (int8_t)vm->registers[dec.rs2]) {
                        vm->program_counter = next_pc + (int16_t)dec.imm;
                    } else {
                        vm->program_counter = next_pc;
                    }
                    break;
            }
            break;

        case 0x06: // Jumps (JAL, JALR)
            if (dec.funct4 == 0x01) {
                // JALR: rd = PC + 4, PC = (rs1 + imm) & ~1
                vm->registers[dec.rd] = (next_pc >> 8) & 0xFF;
                vm->program_counter = (vm->registers[dec.rs1] + (int16_t)dec.imm) & ~1;
            } else {
                // JAL: rd = PC + 4, PC = PC + imm
                vm->registers[dec.rd] = (next_pc >> 8) & 0xFF;
                vm->program_counter = next_pc + (int16_t)dec.imm;
            }
            break;

        case 0x07: // Loads I-type (LW, LH, LB)
            switch (dec.funct3) {
                case 0x00: // LW: load word (4 bytes)
                case 0x01: // LH: load halfword (2 bytes)
                case 0x02: { // LB: load byte
                    uint16_t addr = (vm->registers[dec.rs1] + dec.imm) & 0xFFF;
                    if (dec.funct3 == 0x02) { // LB
                        vm->registers[dec.rd] = vm->memory[addr];
                    } else if (dec.funct3 == 0x01) { // LH
                        vm->registers[dec.rd] = vm->memory[addr] | (vm->memory[(addr + 1) & 0xFFF] << 8);
                    } else { // LW
                        vm->registers[dec.rd] = vm->memory[addr] | (vm->memory[(addr + 1) & 0xFFF] << 8);
                        vm->registers[dec.rd + 1] = vm->memory[(addr + 2) & 0xFFF] | (vm->memory[(addr + 3) & 0xFFF] << 8);
                    }
                    break;
                }
            }
            vm->program_counter = next_pc;
            break;

        case 0x08: // Bitwise R-type (AND, OR, XOR)
            switch (dec.funct4) {
                case 0x00: // AND
                    vm->registers[dec.rd] = vm->registers[dec.rs1] & vm->registers[dec.rs2];
                    break;
                case 0x01: // OR
                    vm->registers[dec.rd] = vm->registers[dec.rs1] | vm->registers[dec.rs2];
                    break;
                case 0x02: // XOR
                    vm->registers[dec.rd] = vm->registers[dec.rs1] ^ vm->registers[dec.rs2];
                    break;
            }
            vm->program_counter = next_pc;
            break;

        case 0x09: // Bitwise Immediates I-type (ANDI, ORI, XORI)
            switch (dec.funct3) {
                case 0x00: // ANDI
                    vm->registers[dec.rd] = vm->registers[dec.rs1] & (dec.imm & 0xFF);
                    break;
                case 0x01: // ORI
                    vm->registers[dec.rd] = vm->registers[dec.rs1] | (dec.imm & 0xFF);
                    break;
                case 0x02: // XORI
                    vm->registers[dec.rd] = vm->registers[dec.rs1] ^ (dec.imm & 0xFF);
                    break;
            }
            vm->program_counter = next_pc;
            break;

        case 0x0A: // Shifts (SLL, SRL)
            switch (dec.funct4) {
                case 0x00: // SLL: rd = rs1 << rs2
                    vm->registers[dec.rd] = vm->registers[dec.rs1] << (vm->registers[dec.rs2] & 0x1F);
                    break;
                case 0x01: // SRL: rd = rs1 >> rs2
                    vm->registers[dec.rd] = vm->registers[dec.rs1] >> (vm->registers[dec.rs2] & 0x1F);
                    break;
            }
            vm->program_counter = next_pc;
            break;

        case 0x0C: // Immediate Shifts (SLLI, SRLI)
            switch (dec.funct3) {
                case 0x01: // Shifts
                    if (dec.funct4 == 0) { // SLLI
                        vm->registers[dec.rd] = vm->registers[dec.rs1] << (dec.imm & 0x1F);
                    } else { // SRLI
                        vm->registers[dec.rd] = vm->registers[dec.rs1] >> (dec.imm & 0x1F);
                    }
                    break;
            }
            vm->program_counter = next_pc;
            break;

        case 0x0B: // Display
            switch (dec.funct3) {
                case 0x00: // CLS: clear screen
                    display_clear(vm);
                    break;
                case 0x01: { // CHAR: draw character
                    uint8_t font_idx = dec.rd;       // font character index
                    uint8_t x = dec.rs1;              // x position
                    uint8_t y = dec.rs2;              // y position
                    // Draw 8x8 character from font space at (x*8, y*8)
                    uint16_t font_addr = FONT_ADDR + (font_idx * 8);
                    uint16_t screen_x = x * 8;
                    uint16_t screen_y = y * 8;
                    for (int row = 0; row < 8; row++) {
                        uint8_t font_row = vm->memory[font_addr + row];
                        for (int col = 0; col < 8; col++) {
                            if (font_row & (0x80 >> col)) {
                                uint32_t pixel_idx = (screen_y + row) * DISPLAY_WIDTH + (screen_x + col);
                                if (pixel_idx < DISPLAY_SIZE) {
                                    vm->display_buffer[pixel_idx] = 0xFFFFFFFF; // white
                                }
                            }
                        }
                    }
                    break;
                }
            }
            vm->program_counter = next_pc;
            break;

        default:
            printf("Error: Unknown opcode 0x%02X\n", dec.opcode);
    }
}
#endif // DISABLE_EXECUTION
