#include "config.h"
#include "architecture.h"
#include "vm.h"
#include "vm_instruction.h"
#include "decode.h"
#include "instructions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void vm_init(BasicVm *vm) {
    memset(vm, 0, sizeof(BasicVm));
    vm->stack_pointer = 0;
    vm->program_counter = PROGRAM_ROM;
}

bool vm_load_rom(BasicVm *vm, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("Error: Could not open ROM file: %s\n", filename);
        fflush(stdout);
        return false;
    }

    // Get file size
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_size > PROGRAM_SIZE) {
        printf("Error: ROM file too large: %s (%ld bytes, max %d)\n", filename, file_size, PROGRAM_SIZE);
        fclose(fp);
        return false;
    }

    // Load into program ROM area (0x1000)
    size_t bytes_read = fread(vm->memory + PROGRAM_ROM, 1, file_size, fp);
    fclose(fp);

#ifdef VERBOSE
    printf("Loaded ROM: %s (%zu bytes)\n", filename, bytes_read);
    fflush(stdout);
#endif
    return true;
}

void vm_print_state(BasicVm *vm) {
    printf("PC: 0x%04X | IR: 0x%04X | SP: %d | I: 0x%04X\n",
           vm->program_counter, vm->opcode, vm->stack_pointer, vm->index_register);
    printf("Registers: ");
    for (int i = 0; i < 16; i++) {
        printf("r%d:0x%02X ", i, vm->registers[i]);
    }
    printf("\n");
}

void vm_print_instruction(BasicVm *vm, DecodedInstruction decodedInstruction) {
    if (decodedInstruction.instruction && decodedInstruction.instruction->name[0] != 'N') {
        printf("[%s] ", decodedInstruction.instruction->name);
        switch (decodedInstruction.opcode) {
            case 0x01: // Arithmetic R-type
            case 0x08: // Bitwise R-type
                printf("[funct4=0x%X] r%d, r%d, r%d\n", decodedInstruction.funct4, decodedInstruction.rd, decodedInstruction.rs1, decodedInstruction.rs2);
                break;
            case 0x02: // Immediates I-type
            case 0x07: // Loads I-type
            case 0x09: // Bitwise Immediates I-type
                printf("r%d, r%d, 0x%04X\n", decodedInstruction.rd, decodedInstruction.rs1, decodedInstruction.imm);
                break;
            case 0x03: // Upper Immediates U-type (LUI/AUIPC)
                // Format: opcode(5)|funct3(3) | funct4(4) | rd(4) | imm(16)
                printf("[funct4=0x%X] r%d, 0x%04X\n", decodedInstruction.funct4, decodedInstruction.rd, decodedInstruction.imm);
                break;
            case 0x04: // Stores S-type
                printf("r%d, 0x%04X(r%d)\n", decodedInstruction.rs2, decodedInstruction.imm, decodedInstruction.rs1);
                break;
            case 0x05: // Branches B-type
                printf("r%d, r%d, 0x%04X\n", decodedInstruction.rs1, decodedInstruction.rs2, decodedInstruction.imm);
                break;
            case 0x06: // Jumps
                if (decodedInstruction.funct3 == 0x02) { // JALR (funct3=0x02)
                    printf("r%d, r%d, 0x%04X\n", decodedInstruction.rd, decodedInstruction.rs1, decodedInstruction.imm);
                } else { // JAL (funct3=0x01)
                    printf("r%d, 0x%05X\n", decodedInstruction.rd, decodedInstruction.imm);
                }
                break;
            case 0x0A: // Shifts
                // opcode_funct3 0x50 = SLL/SRL (24-bit, register-based)
                // opcode_funct3 0x51 = SLLI/SRLI (32-bit, immediate-based)
                if (decodedInstruction.opcode_funct3 == 0x50) {
                    // 24-bit: SLL/SRL - register-based
                    printf("[funct4=0x%X] r%d, r%d, r%d\n",
                           decodedInstruction.funct4, decodedInstruction.rd, decodedInstruction.rs1, decodedInstruction.rs2);
                } else {
                    // 32-bit: SLLI/SRLI - immediate-based (8-bit immediate)
                    printf("[funct4=0x%X] r%d, r%d, 0x%01X\n",
                           decodedInstruction.funct4, decodedInstruction.rd, decodedInstruction.rs1, decodedInstruction.imm & 0xFF);
                }
                break;
            case 0x0F: // HALT
                printf("\n");
                break;
            default:
                printf("opcode=0x%02X funct3=0x%X\n", decodedInstruction.opcode, decodedInstruction.funct3);
                break;
        }
    }
}

static void push_stack(BasicVm *vm, uint16_t value) {
    if (vm->stack_pointer >= 16) {
        printf("Error: Stack overflow!\n");
        return;
    }
    vm->stack[vm->stack_pointer++] = value;
}

static uint16_t pop_stack(BasicVm *vm) {
    if (vm->stack_pointer == 0) {
        printf("Error: Stack underflow!\n");
        return 0;
    }
    return vm->stack[--vm->stack_pointer];
}

static bool check_halt(BasicVm *vm, DecodedInstruction *dec) {
    if (dec->opcode == 0x0F) {
        printf("HALT instruction encountered.\n");
        return true;
    }
    return false;
}

DecodedInstruction vm_step(BasicVm *vm) {
    // Fetch instruction from program ROM
    if (vm->program_counter >= PROGRAM_ROM + PROGRAM_SIZE) {
        printf("Error: PC out of bounds (0x%04X)\n", vm->program_counter);
        return (DecodedInstruction){0};
    }

    // Read first byte to determine opcode and instruction length
    uint8_t byte0 = vm->memory[vm->program_counter];
    uint8_t funct3 = byte0 & 0x7;
    uint8_t opcode = (byte0 >> 3) & 0x1F;
    uint8_t opcode_funct3 = byte0;

    Instruction *ins = NULL;

    // Check if this is a 24-bit R-type instruction that needs funct4 lookup
    if (opcode_funct3 == 0x08 || opcode_funct3 == 0x40 || opcode_funct3 == 0x50|| opcode_funct3 == 0x51) {
        // For 24-bit instructions, we need funct4 to distinguish ADD/SUB/MUL/DIV etc.
        // Read byte1 to get funct4
        uint8_t byte1 = vm->memory[vm->program_counter + 1];
        uint8_t funct4 = byte1 & 0xF;
        ins = get_instruction_by_all(opcode, funct3, funct4);
    } else if (opcode == 0x03) {
        // For 32-bit instructions that use funct4 (LUI/AUIPC)
        // Format: opcode(5)|funct3(3) | rd(4) | funct4(4) | imm(16)
        // funct4 is in bits 0-3 of byte1, rd is in bits 4-7
        uint8_t byte1 = vm->memory[vm->program_counter + 1];
        uint8_t funct4 = byte1 & 0xF;  // Read bits 0-3 for funct4
        ins = get_instruction_by_all(opcode, funct3, funct4);
    } else {
        // For other 32-bit instructions, use opcode and funct3 only
        ins = get_instruction_by_opcode_funct3(opcode, funct3);
    }

    if (!ins) {
        printf("Error: Unknown instruction at PC=0x%04X (opcode=0x%02X, funct3=0x%X)\n", vm->program_counter, opcode, funct3);
        return (DecodedInstruction){0};
    }
#ifdef VERBOSE
    else{
        printf("Fetched instruction: %s at PC=0x%04X\n", ins->name, vm->program_counter);
    }
#endif

    // Determine instruction length (24-bit = 3 bytes, 32-bit = 4 bytes)
    // Check if this is a 32-bit instruction format
    int instr_length = ins->length / 8;  // Convert bits to bytes
    if (instr_length == 0) {
        instr_length = 3;  // Default to 24-bit instructions if length is not set properly
    }

    // Read instruction bytes (little-endian)
    uint32_t instruction = byte0;
    for (int i = 1; i < instr_length; i++) {
        instruction |= (vm->memory[vm->program_counter + i] << (i * 8));
    }

    vm->opcode = instruction;  // Store full instruction

    DecodedInstruction dec = vm_decode(instruction, *ins);

    // Print debug info
    vm_print_instruction(vm, dec);

    // Check for HALT
    if (check_halt(vm, &dec)) {
        return dec;
    }

    // Increment PC for next instruction (will be adjusted by branches/jumps)
    uint16_t next_pc = vm->program_counter + instr_length;

#ifdef DISABLE_EXECUTION
    // Just decode and print, don't execute
    vm->program_counter = next_pc;
#else
    // Execute instruction
    vm_execute_instruction(vm, dec, next_pc);
#endif  // DISABLE_EXECUTION

#ifdef VERBOSE
    vm_print_state(vm);
#endif
    return dec;
}

bool vm_run(BasicVm *vm) {
#ifdef VERBOSE
    printf("=== VM Starting ===\n");
    printf("ROM loaded, memory[0]=0x%02X memory[1]=0x%02X\n", vm->memory[0], vm->memory[1]);
    fflush(stdout);

    vm_print_state(vm);
    fflush(stdout);
#endif

    int instruction_count = 0;
    const int MAX_INSTRUCTIONS = 1000000;  // Safety limit

#ifdef VERBOSE
    printf("Entering execution loop...\n");
    fflush(stdout);
#endif

    while (instruction_count < MAX_INSTRUCTIONS) {
#ifdef VERBOSE
        printf("Step %d: PC=0x%04X\n", instruction_count, vm->program_counter);
        fflush(stdout);
#endif
        DecodedInstruction dec = vm_step(vm);
        if (dec.opcode == 0) {
            printf("VM error at instruction %d\n", instruction_count);
            fflush(stdout);
            return false;
        }
        instruction_count++;

        // Check if PC is at a halt or out of bounds
        if (vm->opcode == 0) {
            // Null instruction, continue
            continue;
        }

        if (dec.opcode == 0x0F) {
            break;
        }
    }

    if (instruction_count >= MAX_INSTRUCTIONS) {
        printf("Warning: Reached maximum instruction limit\n");
    }

    printf("Executed %d instructions\n", instruction_count);
    vm_print_state(vm);
    return true;
}
