#include "asm.h"

AssembledOperation handle_opcode(Instruction *instruction, char asmLineBuffer[256], uint32_t current_pc)
{
 switch (instruction->opcode)
    {
    case 0x00:
        return assemble_nop(instruction, asmLineBuffer);
    case 0x01:
        if (instruction->funct3 == 0) {
            return assemble_rtype(instruction, asmLineBuffer);
        } else {
            return assemble_itype(instruction, asmLineBuffer);
        }
    case 0x02:
        return assemble_lui(instruction, asmLineBuffer);
    case 0x03:
        return assemble_store(instruction, asmLineBuffer);
    case 0x04:
        return assemble_branch(instruction, asmLineBuffer, current_pc);
    case 0x05:
        if (instruction->funct4 == 0x1) {
            return assemble_jal(instruction, asmLineBuffer);
        } else {
            return assemble_jalr(instruction, asmLineBuffer);
        }
    case 0x06:
        return assemble_itype(instruction, asmLineBuffer);
    case 0x07:
        if (instruction->funct3 == 0) {
            return assemble_rtype(instruction, asmLineBuffer);
        } else {
            return assemble_itype(instruction, asmLineBuffer);
        }
    case 0x08:
        if (instruction->funct3 == 0) {
            return assemble_rtype(instruction, asmLineBuffer);
        } else {
            return assemble_itype(instruction, asmLineBuffer);
        }
    case 0x09:
        return assemble_sys(instruction, asmLineBuffer);
    default:
        return InvalidOperation;
    }
}