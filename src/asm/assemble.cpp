#include "assemble.h"

static Label *labels = NULL;

void clear_labels(void) {
    Label *current, *tmp;
    HASH_ITER(hh, labels, current, tmp) {
        HASH_DEL(labels, current);
        free(current);
    }
    labels = NULL;
}

int add_label(const char *name, uint32_t address) {
    Label *existing = NULL;
    char name_lower[MAX_LABEL_LENGTH];
    strncpy(name_lower, name, MAX_LABEL_LENGTH - 1);
    name_lower[MAX_LABEL_LENGTH - 1] = '\0';
    for (int i = 0; name_lower[i]; i++) {
        name_lower[i] = tolower(name_lower[i]);
    }
    
    HASH_FIND(hh, labels, name_lower, strlen(name_lower), existing);
    if (existing) {
        return -1;
    }
    
    Label *new_label = (Label *)malloc(sizeof(Label));
    if (!new_label) return -1;
    
    strncpy(new_label->name, name_lower, MAX_LABEL_LENGTH - 1);
    new_label->name[MAX_LABEL_LENGTH - 1] = '\0';
    new_label->address = address;
    
    HASH_ADD(hh, labels, name, strlen(name_lower), new_label);
    return 0;
}

Label *find_label(const char *name) {
    char name_lower[MAX_LABEL_LENGTH];
    strncpy(name_lower, name, MAX_LABEL_LENGTH - 1);
    name_lower[MAX_LABEL_LENGTH - 1] = '\0';
    for (int i = 0; name_lower[i]; i++) {
        name_lower[i] = tolower(name_lower[i]);
    }
    
    Label *result = NULL;
    HASH_FIND(hh, labels, name_lower, strlen(name_lower), result);
    return result;
}

int is_label(const char *str) {
    if (!str || *str == '\0') return 0;
    
    if (!isalpha(str[0]) && str[0] != '_') return 0;
    
    for (int i = 1; str[i]; i++) {
        if (!isalnum(str[i]) && str[i] != '_') return 0;
    }
    
    return 1;
}

uint32_t resolve_label(const char *name, uint32_t current_pc, bool is_branch) {
    Label *label = find_label(name);
    if (!label) {
        return 0xFFFFFFFF;
    }
    
    if (is_branch) {
        int32_t offset = (int32_t)label->address - (int32_t)current_pc;
        if (offset < -2048 || offset > 2047) {
            return 0xFFFFFFFF;
        }
        uint32_t result = (uint32_t)((int16_t)offset);
        return result & 0xFFF;
    } else {
        return label->address;
    }
}

int8_t register_to_byte(const char *reg) {
  if (reg[0] == 'r' || reg[0] == 'R') {
    int value = atoi(reg + 1);
    if (value >= 0 && value <= 15) {
      return (int8_t)value;
    }
  }
  return -1;
}

int parse_immediate_unsigned(const char *immStr, uint16_t max_value, uint16_t *out_value) {
    if (immStr == NULL || *immStr == '\0') return 0;
    char *endptr = NULL;
    unsigned long value = strtoul(immStr, &endptr, 0);
    if (*endptr != '\0') return 0;
    if (value > max_value) return 0;
    *out_value = (uint16_t)value;
    return 1;
}

int parse_immediate_signed(const char *immStr, int16_t *out_value) {
    if (immStr == NULL || *immStr == '\0') return 0;
    char *endptr = NULL;
    int64_t value = strtol(immStr, &endptr, 0);
    if (*endptr != '\0') return 0;
    if (value < -2048 || value > 2047) return 0;
    *out_value = (int16_t)value;
    return 1;
}

uint32_t imm_to_word_unsigned(const char *immStr) {
    if (immStr == NULL || *immStr == '\0') return 0x0000;
    char *endptr = NULL;
    unsigned long value = strtoul(immStr, &endptr, 0);
    return (uint32_t)value;
}

uint16_t imm_to_word_signed(const char *immStr) {
    size_t len = strlen(immStr);
    if (len == 0) return 0x0000;
    char *endptr = NULL;
    int64_t value = strtol(immStr, &endptr, 0);
    if (*endptr == '\0') {
        if (value > 2047) {
            value = value - 4096;
        }
        if (value >= -2048 && value <= 2047) {
            return (uint16_t)(int16_t)value;
        }
    }
    return 0x0000;
}

AssembledOperation assemble_rtype(const Instruction *instruction, const char *asmLine) {
    char instructionStr[10];
    char rdStr[5], rs1Str[5], rs2Str[5];

    int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instructionStr, rdStr, rs1Str, rs2Str);
    if (count != 4) {
        return (AssembledOperation){.hasValue = false};
    }

    int8_t rd = register_to_byte(rdStr);
    int8_t rs1 = register_to_byte(rs1Str);
    int8_t rs2 = register_to_byte(rs2Str);
    if (rd < 0 || rs1 < 0 || rs2 < 0) {
        return (AssembledOperation){.hasValue = false};
    }
    uint32_t insOp = 0;

    insOp |= (instruction->funct3 & 0x7);
    insOp |= (instruction->opcode& 0x1F) << 3;
    insOp |= (instruction->funct4 & 0xF) << 8;
    insOp |= ((uint8_t)rd & 0xF) << 12;
    insOp |= ((uint8_t)rs1 & 0xF) << 16;
    insOp |= ((uint8_t)rs2 & 0xF) << 20;

    return (AssembledOperation){.value = insOp, .hasValue = true};
}

AssembledOperation assemble_itype(const Instruction *instruction, const char *asmLine) {
    char instructionStr[20];
    char rdStr[5], rs1Str[5], immStr[10];

    int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instructionStr, rdStr, rs1Str, immStr);
    if (count != 4) {
        return (AssembledOperation){.hasValue = false};
    }

    int8_t rd = register_to_byte(rdStr);
    int8_t rs1 = register_to_byte(rs1Str);
    if (rd < 0 || rs1 < 0) {
        return (AssembledOperation){.hasValue = false};
    }

    uint16_t imm;
    if (!parse_immediate_unsigned(immStr, 0xFFF, &imm)) {
        return (AssembledOperation){.hasValue = false};
    }

    uint32_t insOp = 0;

    insOp |= (instruction->funct3 & 0x7);
    insOp |= (instruction->opcode& 0x1F) << 3;
    insOp |= (instruction->funct4 & 0xF) << 8;
    insOp |= ((uint8_t)rd & 0xF) << 12;
    insOp |= ((uint8_t)rs1 & 0xF) << 16;
    insOp |= (imm & 0xF) << 20;
    insOp |= ((imm >> 4) & 0xFF) << 24;

    return (AssembledOperation){.value = insOp, .hasValue = true};
}

AssembledOperation assemble_store(const Instruction *instruction, const char *asmLine) {
    char instructionStr[20];
    char rs1Str[5], rs2Str[5], immStr[10];

    int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instructionStr, rs1Str, rs2Str, immStr);
    if (count != 4) {
        return (AssembledOperation){.hasValue = false};
    }

    int8_t rs1 = register_to_byte(rs1Str);
    int8_t rs2 = register_to_byte(rs2Str);
    if (rs1 < 0 || rs2 < 0) {
        return (AssembledOperation){.hasValue = false};
    }

    uint16_t imm;
    if (!parse_immediate_unsigned(immStr, 0xFFF, &imm)) {
        return (AssembledOperation){.hasValue = false};
    }

    uint32_t insOp = 0;

    insOp |= (instruction->funct3 & 0x7);
    insOp |= (instruction->opcode& 0x1F) << 3;
    insOp |= (instruction->funct4 & 0xF) << 8;
    insOp |= ((uint8_t)rs1 & 0xF) << 12;
    insOp |= ((uint8_t)rs2 & 0xF) << 16;
    insOp |= (imm& 0xF) << 20;
    insOp |= ((imm >> 4) & 0xFF) << 24;

    return (AssembledOperation){.value = insOp, .hasValue = true};
}


AssembledOperation assemble_branch(const Instruction *instruction, const char *asmLine, uint32_t current_pc) {
    char instructionStr[20];
    char rs1Str[5], rs2Str[5], immStr[10];

    int count = sscanf(asmLine, "%s %[^,], %[^,], %s", instructionStr, rs1Str, rs2Str, immStr);
    if (count != 4) {
        return (AssembledOperation){.hasValue = false};
    }

    int8_t rs1 = register_to_byte(rs1Str);
    int8_t rs2 = register_to_byte(rs2Str);
    if (rs1 < 0 || rs2 < 0) {
        return (AssembledOperation){.hasValue = false};
    }

    uint32_t imm;
    if (is_label(immStr)) {
        imm = resolve_label(immStr, current_pc, true);
        if (imm == 0xFFFFFFFF) {
            return (AssembledOperation){.hasValue = false};
        }
    } else {
        int16_t signed_imm;
        if (!parse_immediate_signed(immStr, &signed_imm)) {
            return (AssembledOperation){.hasValue = false};
        }
        imm = (uint32_t)(int16_t)signed_imm;
    }
    
    uint32_t insOp = 0;

    insOp |= (instruction->funct3 & 0x7);
    insOp |= (instruction->opcode& 0x1F) << 3;
    insOp |= (instruction->funct4 & 0xF) << 8;
    insOp |= ((uint8_t)rs1 & 0xF) << 12;
    insOp |= ((uint8_t)rs2 & 0xF) << 16;
    insOp |= (imm& 0xF) << 20;
    insOp |= ((imm >> 4) & 0xFF) << 24;

    return (AssembledOperation){.value = insOp, .hasValue = true};
}

AssembledOperation assemble_jal(const Instruction *instruction, const char *asmLine) {
    char rdStr[5], immStr[10];

    int count = sscanf(asmLine, "%*s %[^,], %s", rdStr, immStr);
    if (count != 2) {
        return (AssembledOperation){.hasValue = false};
    }

    int8_t rd = register_to_byte(rdStr);
    if (rd < 0) {
        return (AssembledOperation){.hasValue = false};
    }

    uint32_t imm;
    if (is_label(immStr)) {
        imm = resolve_label(immStr, 0, false);
        if (imm == 0xFFFFFFFF) {
            return (AssembledOperation){.hasValue = false};
        }
    } else {
        uint16_t imm_val;
        if (!parse_immediate_unsigned(immStr, 0xFFFF, &imm_val)) {
            return (AssembledOperation){.hasValue = false};
        }
        imm = imm_val;
    }

    uint32_t insOp = 0;

    insOp |= (instruction->funct3 & 0x7);
    insOp |= (instruction->opcode& 0x1F) << 3;
    insOp |= (instruction->funct4 & 0xF) << 8;
    insOp |= ((uint8_t)rd & 0xF) << 12;
    insOp |= (imm & 0xF) << 16;
    insOp |= ((imm >> 4)& 0xFF) << 20;
    insOp |= ((imm >> 12)& 0xF) << 28;

    return (AssembledOperation){.value = insOp, .hasValue = true};
}


AssembledOperation assemble_jalr(const Instruction *instruction, const char *asmLine) {
    char rdStr[5], rs1Str[5], immStr[10];

    int count = sscanf(asmLine, "%*s %[^,], %[^,], %s", rdStr, rs1Str, immStr);
    if (count != 3) {
        return (AssembledOperation){.hasValue = false};
    }

    int8_t rd = register_to_byte(rdStr);
    int8_t rs1 = register_to_byte(rs1Str);
    if (rd < 0 || rs1 < 0) {
        return (AssembledOperation){.hasValue = false};
    }

    uint32_t imm;
    if (is_label(immStr)) {
        imm = resolve_label(immStr, 0, false);
        if (imm == 0xFFFFFFFF) {
            return (AssembledOperation){.hasValue = false};
        }
    } else {
        uint16_t imm_val;
        if (!parse_immediate_unsigned(immStr, 0xFFFF,&imm_val)) {
            return (AssembledOperation){.hasValue = false};
        }
        imm = imm_val;
    }

    uint32_t insOp = 0;

    insOp |= (instruction->funct3 & 0x7);
    insOp |= (instruction->opcode& 0x1F) << 3;
    insOp |= (instruction->funct4 & 0xF) << 8;
    insOp |= ((uint8_t)rd & 0xF) << 12;
    insOp |= ((uint8_t)rs1 & 0xF) << 16;
    insOp |= (imm & 0xF) << 20;
    insOp |= ((imm >> 4)& 0xFF) << 24;

    return (AssembledOperation){.value = insOp, .hasValue = true};
}


AssembledOperation assemble_lui(const Instruction *instruction, const char *asmLine) {
    char rdStr[5], immStr[10];

    int count = sscanf(asmLine, "%*s %[^,], %s", rdStr, immStr);
    if (count != 2) {
        return (AssembledOperation){.hasValue = false};
    }

    int8_t rd = register_to_byte(rdStr);
    if (rd < 0) {
        return (AssembledOperation){.hasValue = false};
    }

    uint16_t imm;
    if (!parse_immediate_unsigned(immStr, 0xFFFF, &imm)) {
        return (AssembledOperation){.hasValue = false};
    }

    uint32_t insOp = 0;

    insOp |= (instruction->funct3 & 0x7);
    insOp |= (instruction->opcode& 0x1F) << 3;
    insOp |= (instruction->funct4 & 0xF) << 8;
    insOp |= ((uint8_t)rd & 0xF) << 12;
    insOp |= (imm& 0xF) << 16;
    insOp |= ((imm >> 4)& 0xFF) << 20;
    insOp |= ((imm >> 12)& 0xF) << 28;

    return (AssembledOperation){.value = insOp, .hasValue = true};
}


AssembledOperation assemble_sys(const Instruction *instruction, const char *asmLine) {
    char instructionStr[20];
    char immStr[10];

    int count = sscanf(asmLine, "%s %s", instructionStr, immStr);
    if (count != 2) {
        return (AssembledOperation){.hasValue = false};
    }

    uint32_t imm;
    if (is_label(immStr)) {
        imm = resolve_label(immStr, 0, false);
        if (imm == 0xFFFFFFFF) {
            return (AssembledOperation){.hasValue = false};
        }
    } else {
        uint16_t imm_val;
        if (!parse_immediate_unsigned(immStr, 0xFFFF, &imm_val)) {
            return (AssembledOperation){.hasValue = false};
        }
        imm = imm_val;
    }

    uint32_t insOp = 0;

    insOp |= (instruction->funct3 & 0x7);
    insOp |= (instruction->opcode& 0x1F) << 3;
    insOp |= (instruction->funct4 & 0xF) << 8;
    insOp |= (imm & 0xFF) << 24;

    return (AssembledOperation){.value = insOp, .hasValue = true};
}


AssembledOperation assemble_nop(const Instruction *instruction, const char *asmLine) {
    (void)asmLine;
    uint32_t insOp = 0;
    insOp |= (instruction->funct4& 0xF) << 8;
    return (AssembledOperation){.value = insOp, .hasValue = true};
}