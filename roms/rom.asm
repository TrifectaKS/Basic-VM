; Basic-VM ROM Example
; Demonstrates all instruction types including ASM variables

; ASM Variables - Named constants for immediates
BANK_ADDR = 0x100
CONFIG_VAL = 0xFF
SHIFT_AMT = 0x1F

; NOP
NOP

; Arithmetic R-type (REG)
start:
ADD r1, r2, r3
SUB r1, r2, r3
MUL r1, r2, r3
DIV r1, r2, r3

; Arithmetic I-type (IMM) - 12-bit immediates
ADDI r1, r2, 0x123
SUBI r1, r2, 0x123
MULI r1, r2, 0x123
DIVI r1, r2, 0x123

; Using variables for immediates
ADDI r1, r2, CONFIG_VAL
SUBI r1, r2, BANK_ADDR

; Upper Immediates - 16-bit immediates
LUI r1, 0x1234
AUIPC r1, 0x1234

; Stores - 12-bit offsets
SB r1, r2, 0xFF
SH r1, r2, 0xFFF
SW r1, r2, 0xFFF

; Using variables for store offsets
SW r1, r2, BANK_ADDR

; Branches - 12-bit offsets
loop:
BEQ r1, r2, loop
BNE r1, r2, loop
BLT r1, r2, loop
BGT r1, r2, loop
BLE r1, r2, loop
BGE r1, r2, loop

; Using variables for branch offsets
BEQ r1, r2, CONFIG_VAL

; Jumps
JAL r1, end
JALR r1, r2, 0xFFF

; Using variable for JALR offset
JALR r1, r2, BANK_ADDR

; Loads - 12-bit offsets
LW r1, r2, 0xFFF
LH r1, r2, 0xFFF
LB r1, r2, 0xFFF

; Using variable for load offset
LW r1, r2, BANK_ADDR

; Bitwise R-type (REG)
AND r1, r2, r3
OR r1, r2, r3
XOR r1, r2, r3

; Bitwise I-type (IMM) - 12-bit immediates
ANDI r1, r2, 0xFFF
ORI r1, r2, 0xFFF
XORI r1, r2, 0xFFF

; Using variables for bitwise immediates
ANDI r1, r2, CONFIG_VAL
ORI r1, r2, BANK_ADDR

; Shifts
SLL r1, r2, r3
SRL r1, r2, r3
SLLI r1, r2, 0x1F
SRLI r1, r2, 0x1F

; Using variable for shift amount
SLLI r1, r2, SHIFT_AMT
SRLI r1, r2, SHIFT_AMT

; System
end:
SYS 0xFF

; Using variable for SYS
SYS BANK_ADDR