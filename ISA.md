# Basic-VM Instruction Set Architecture

## Overview

This is a custom RISC-like architecture with 16 general-purpose 8-bit registers.

## Registers

- **r0-r15**: 16 general-purpose registers (8-bit each)
- **PC**: Program Counter (16-bit)
- **SP**: Stack Pointer (8-bit, 16-level stack)
- **I**: Index Register (16-bit)
- **Timers**: delay_timer, sound_timer (8-bit each)

## Memory

- **4096 bytes** of addressable memory

---

## Instruction Formats

### 24-bit R-type (Arithmetic, Bitwise, Shifts)
```
 opcode(5) | funct3(3) | funct4(4) | rd(4) | rs1(4) | rs2(4)
```
- Byte 0: `funct3(3) | opcode(5)` = `opcode_funct3`
- Byte 1: `funct4(4) | rd(4)`
- Byte 2: `rs1(4) | rs2(4)`

### 32-bit I-type (Arithmetic Immediates, Loads, Bitwise Immediates)
```
 opcode(5) | funct3(3) | rd(4) | rs1(4) | imm(16)
```
- Byte 0: `funct3(3) | opcode(5)` = `opcode_funct3`
- Byte 1: `rd(4) | rs1(4)`
- Byte 2-3: `imm(16)`

### 32-bit U-type (LUI, AUIPC)
```
 opcode(5) | funct3(3) | funct4(4) | rd(4) | imm(16)
```
- Byte 0: `funct3(3) | opcode(5)`
- Byte 1: `funct4(4) | rd(4)`
- Byte 2-3: `imm(16)`

### 32-bit S-type (Stores)
```
 opcode(5) | funct3(3) | rs1(4) | rs2(4) | imm(16)
```
- Byte 0: `funct3(3) | opcode(5)`
- Byte 1: `rs1(4) | rs2(4)`
- Byte 2-3: `imm(16)`

### 32-bit B-type (Branches)
```
 opcode(5) | funct3(3) | rs1(4) | rs2(4) | imm(16)
```
- Same encoding as S-type

### 40-bit J-type (JAL - Jump and Link)
```
 opcode(5) | funct3(3) | rd(4) | imm(24)
```
- Byte 0: `funct3(3) | opcode(5)`
- Byte 1: `rd(4) | imm[23:20](4)`
- Byte 2-4: `imm(20)`

### 32-bit I-type (JALR - Jump and Link Register)
```
 opcode(5) | funct3(3) | rd(4) | rs1(4) | imm(16)
```
- Same encoding as other I-type instructions

### 32-bit Shift Immediate (SLLI, SRLI)
```
 opcode(5) | funct3(3) | funct4(4) | rd(4) | rs1(4) | imm(8)
```
- Byte 0: `funct3(3) | opcode(5)`
- Byte 1: `funct4(4) | rd(4)`
- Byte 2: `imm(8)`
- Byte 3: `rs1(4) | unused(4)`

### 32-bit HALT
```
 opcode(5) | funct3(3) | rd(4) | rs1(4) | imm(16)
```
- All fields zero except opcode=0x0F, funct3=0

---

## Instruction Set Reference

### Arithmetic Instructions (R-type, 24-bit)

| Instruction | opcode | funct3 | funct4 | Format | Description |
|------------|--------|--------|--------|--------|-------------|
| ADD | 0x01 | 0x00 | 0x00 | rd, rs1, rs2 | rd = rs1 + rs2 |
| SUB | 0x01 | 0x00 | 0x01 | rd, rs1, rs2 | rd = rs1 - rs2 |
| MUL | 0x01 | 0x00 | 0x02 | rd, rs1, rs2 | rd = rs1 * rs2 |
| DIV | 0x01 | 0x00 | 0x03 | rd, rs1, rs2 | rd = rs1 / rs2 |

**Example:**
```asm
ADD r1, r2, r3   ; r1 = r2 + r3
SUB r4, r5, r6   ; r4 = r5 - r6
MUL r7, r8, r9   ; r7 = r8 * r9
DIV r10, r11, r12 ; r10 = r11 / r12
```

---

### Arithmetic Immediate Instructions (I-type, 32-bit)

| Instruction | opcode | funct3 | Format | Description |
|------------|--------|--------|--------|-------------|
| ADDI | 0x02 | 0x00 | rd, rs1, imm | rd = rs1 + imm (8-bit) |
| SUBI | 0x02 | 0x01 | rd, rs1, imm | rd = rs1 - imm (8-bit) |
| MULI | 0x02 | 0x02 | rd, rs1, imm | rd = rs1 * imm (8-bit) |
| DIVI | 0x02 | 0x03 | rd, rs1, imm | rd = rs1 / imm (8-bit) |

**Example:**
```asm
ADDI r1, r2, 0xFF   ; r1 = r2 + 0xFF
SUBI r3, r4, 0x1A   ; r3 = r4 - 0x1A
```

---

### Upper Immediate Instructions (U-type, 32-bit)

| Instruction | opcode | funct3 | funct4 | Format | Description |
|------------|--------|--------|--------|--------|-------------|
| LUI | 0x03 | 0x00 | 0x00 | rd, imm | rd = imm >> 8 (load upper 16 bits) |
| AUIPC | 0x03 | 0x00 | 0x01 | rd, imm | rd = PC + (imm >> 8) |

**Example:**
```asm
LUI r1, 0x1234     ; r1 = 0x12 (upper 8 bits of 0x1234)
AUIPC r2, 0x5678   ; r2 = PC + 0x56
```

---

### Store Instructions (S-type, 32-bit)

| Instruction | opcode | funct3 | Format | Description |
|------------|--------|--------|--------|-------------|
| SB | 0x04 | 0x00 | rs2, imm(rs1) | Store byte |
| SH | 0x04 | 0x01 | rs2, imm(rs1) | Store halfword (2 bytes) |
| SW | 0x04 | 0x02 | rs2, imm(rs1) | Store word (4 bytes) |

**Example:**
```asm
SB r1, 0x100(r2)   ; MEM[r2 + 0x100] = r1
SH r3, 0x200(r4)   ; MEM[r4 + 0x200] = r3 (byte), MEM[r4 + 0x201] = r3>>8
SW r5, 0x300(r6)   ; Store 4 bytes starting at r6 + 0x300
```

---

### Branch Instructions (B-type, 32-bit)

| Instruction | opcode | funct3 | Format | Description |
|------------|--------|--------|--------|-------------|
| BEQ | 0x05 | 0x00 | rs1, rs2, imm | Branch if rs1 == rs2 |
| BNE | 0x05 | 0x01 | rs1, rs2, imm | Branch if rs1 != rs2 |
| BLT | 0x05 | 0x02 | rs1, rs2, imm | Branch if rs1 < rs2 (signed) |
| BGT | 0x05 | 0x03 | rs1, rs2, imm | Branch if rs1 > rs2 (signed) |
| BLE | 0x05 | 0x04 | rs1, rs2, imm | Branch if rs1 <= rs2 (signed) |
| BGE | 0x05 | 0x05 | rs1, rs2, imm | Branch if rs1 >= rs2 (signed) |

**Example:**
```asm
BEQ r1, r2, 0x100   ; If r1 == r2, PC = PC + 0x100
BNE r3, r4, -0x50   ; If r3 != r4, PC = PC - 0x50
BLT r5, r6, label   ; If r5 < r6, branch to label
```

---

### Jump Instructions

#### JAL (J-type, 40-bit)
| Instruction | opcode | funct3 | Format | Description |
|------------|--------|--------|--------|-------------|
| JAL | 0x06 | 0x01 | rd, imm | rd = PC + 4, PC = PC + imm (24-bit) |

#### JALR (I-type, 32-bit)
| Instruction | opcode | funct3 | Format | Description |
|------------|--------|--------|--------|-------------|
| JALR | 0x06 | 0x02 | rd, rs1, imm | rd = PC + 4, PC = (rs1 + imm) & ~1 |

**Example:**
```asm
JAL r1, 0x1000      ; r1 = return addr, PC = PC + 0x1000
JALR r2, r3, 0x100  ; r2 = return addr, PC = (r3 + 0x100) & ~1
```

---

### Load Instructions (I-type, 32-bit)

| Instruction | opcode | funct3 | Format | Description |
|------------|--------|--------|--------|-------------|
| LW | 0x07 | 0x00 | rd, imm(rs1) | Load word (4 bytes) |
| LH | 0x07 | 0x01 | rd, imm(rs1) | Load halfword (2 bytes, zero-extended) |
| LB | 0x07 | 0x02 | rd, imm(rs1) | Load byte (zero-extended) |

**Example:**
```asm
LW r1, 0x100(r2)   ; r1 = MEM[r2+0x100] | (MEM[r2+0x101] << 8)
LH r3, 0x200(r4)   ; r3 = MEM[r4+0x200] | (MEM[r4+0x201] << 8)
LB r5, 0x300(r6)   ; r5 = MEM[r6+0x300]
```

---

### Bitwise Instructions (R-type, 24-bit)

| Instruction | opcode | funct3 | funct4 | Format | Description |
|------------|--------|--------|--------|--------|-------------|
| AND | 0x08 | 0x00 | 0x00 | rd, rs1, rs2 | rd = rs1 & rs2 |
| OR | 0x08 | 0x00 | 0x01 | rd, rs1, rs2 | rd = rs1 \| rs2 |
| XOR | 0x08 | 0x00 | 0x02 | rd, rs1, rs2 | rd = rs1 ^ rs2 |

**Example:**
```asm
AND r1, r2, r3   ; r1 = r2 & r3
OR r4, r5, r6    ; r4 = r5 | r6
XOR r7, r8, r9    ; r7 = r8 ^ r9
```

---

### Bitwise Immediate Instructions (I-type, 32-bit)

| Instruction | opcode | funct3 | Format | Description |
|------------|--------|--------|--------|-------------|
| ANDI | 0x09 | 0x00 | rd, rs1, imm | rd = rs1 & imm |
| ORI | 0x09 | 0x01 | rd, rs1, imm | rd = rs1 \| imm |
| XORI | 0x09 | 0x02 | rd, rs1, imm | rd = rs1 ^ imm |

**Example:**
```asm
ANDI r1, r2, 0xFF   ; r1 = r2 & 0xFF
ORI r3, r4, 0x0F    ; r3 = r4 | 0x0F
```

---

### Shift Instructions

#### Register Shifts (R-type, 24-bit)
| Instruction | opcode | funct3 | funct4 | Format | Description |
|------------|--------|--------|--------|--------|-------------|
| SLL | 0x0A | 0x00 | 0x00 | rd, rs1, rs2 | rd = rs1 << rs2 |
| SRL | 0x0A | 0x00 | 0x01 | rd, rs1, rs2 | rd = rs1 >> rs2 |

#### Immediate Shifts (32-bit)
| Instruction | opcode | funct3 | funct4 | Format | Description |
|------------|--------|--------|--------|--------|-------------|
| SLLI | 0x0A | 0x01 | 0x00 | rd, rs1, imm | rd = rs1 << imm (8-bit) |
| SRLI | 0x0A | 0x01 | 0x01 | rd, rs1, imm | rd = rs1 >> imm (8-bit) |

**Example:**
```asm
SLL r1, r2, r3    ; r1 = r2 << r3
SRL r4, r5, r6    ; r4 = r5 >> r6
SLLI r7, r8, 0xF  ; r7 = r8 << 0xF
SRLI r9, r10, 0x5 ; r9 = r10 >> 0x5
```

---

### System Instructions

| Instruction | opcode | funct3 | funct4 | Format | Description |
|------------|--------|--------|--------|--------|-------------|
| HALT | 0x0F | 0x00 | 0x00 | (none) | Stop execution |

**Example:**
```asm
HALT   ; Stop the VM
```

---

## Complete Instruction Encoding Table

| Name | opcode | funct3 | opcode_funct3 | funct4 | Length |
|------|--------|--------|---------------|--------|--------|
| NULL | 0x00 | 0x00 | 0x00 | 0x00 | 0 |
| ADD | 0x01 | 0x00 | 0x08 | 0x00 | 24 |
| SUB | 0x01 | 0x00 | 0x08 | 0x01 | 24 |
| MUL | 0x01 | 0x00 | 0x08 | 0x02 | 24 |
| DIV | 0x01 | 0x00 | 0x08 | 0x03 | 24 |
| ADDI | 0x02 | 0x00 | 0x10 | 0x00 | 32 |
| SUBI | 0x02 | 0x01 | 0x11 | 0x00 | 32 |
| MULI | 0x02 | 0x02 | 0x12 | 0x00 | 32 |
| DIVI | 0x02 | 0x03 | 0x13 | 0x00 | 32 |
| LUI | 0x03 | 0x00 | 0x18 | 0x00 | 32 |
| AUIPC | 0x03 | 0x00 | 0x18 | 0x01 | 32 |
| SB | 0x04 | 0x00 | 0x20 | 0x00 | 32 |
| SH | 0x04 | 0x01 | 0x21 | 0x00 | 32 |
| SW | 0x04 | 0x02 | 0x22 | 0x00 | 32 |
| BEQ | 0x05 | 0x00 | 0x28 | 0x00 | 32 |
| BNE | 0x05 | 0x01 | 0x29 | 0x00 | 32 |
| BLT | 0x05 | 0x02 | 0x2A | 0x00 | 32 |
| BGT | 0x05 | 0x03 | 0x2B | 0x00 | 32 |
| BLE | 0x05 | 0x04 | 0x2C | 0x00 | 32 |
| BGE | 0x05 | 0x05 | 0x2D | 0x00 | 32 |
| JAL | 0x06 | 0x01 | 0x31 | 0x00 | 40 |
| JALR | 0x06 | 0x02 | 0x32 | 0x00 | 32 |
| LW | 0x07 | 0x00 | 0x38 | 0x00 | 32 |
| LH | 0x07 | 0x01 | 0x39 | 0x00 | 32 |
| LB | 0x07 | 0x02 | 0x3A | 0x00 | 32 |
| AND | 0x08 | 0x00 | 0x40 | 0x00 | 24 |
| OR | 0x08 | 0x00 | 0x40 | 0x01 | 24 |
| XOR | 0x08 | 0x00 | 0x40 | 0x02 | 24 |
| ANDI | 0x09 | 0x00 | 0x49 | 0x00 | 32 |
| ORI | 0x09 | 0x01 | 0x4A | 0x00 | 32 |
| XORI | 0x09 | 0x02 | 0x4B | 0x00 | 32 |
| SLL | 0x0A | 0x00 | 0x50 | 0x00 | 24 |
| SRL | 0x0A | 0x00 | 0x50 | 0x01 | 24 |
| SLLI | 0x0A | 0x01 | 0x51 | 0x00 | 32 |
| SRLI | 0x0A | 0x01 | 0x51 | 0x01 | 32 |
| HALT | 0x0F | 0x00 | 0x78 | 0x00 | 32 |

---

## Assembler Syntax

Comments start with `;`:
```asm
ADD r1, r2, r3   ; This is a comment
HALT             ; Stop execution
```

Hexadecimal immediates use `0x` prefix:
```asm
ADDI r1, r2, 0xFF
JAL r0, 0x1000
```

---

## Building and Running

```bash
make asm   ; Assemble roms/rom.asm to roms/rom.bin
make run   ; Run the VM with rom.bin
make clean  ; Clean build artifacts
```
