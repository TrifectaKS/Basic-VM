; Bitwise Operations and Arithmetic
; Demonstrates: AND, OR, XOR, SLL, SRL, ADD, SUB, MUL, DIV

main:
    ; Bitwise operations
    ADDI r1, r0, 0xF0        ; r1 = 0xF0
    ADDI r2, r0, 0x0F        ; r2 = 0x0F
    
    AND r3, r1, r2           ; r3 = 0x00 (AND)
    OR r4, r1, r2            ; r4 = 0xFF (OR)
    XOR r5, r1, r2           ; r5 = 0xFF (XOR)
    
    ; Shift operations
    ADDI r1, r0, 1           ; r1 = 1
    SLLI r2, r1, 4           ; r2 = 16 (shift left by 4)
    SRLI r3, r2, 2           ; r3 = 4 (shift right by 2)
    
    ; Arithmetic operations
    ADDI r1, r0, 10
    ADDI r2, r0, 3
    
    ADD r3, r1, r2           ; r3 = 13
    SUB r4, r1, r2           ; r4 = 7
    MUL r5, r1, r2           ; r5 = 30
    DIV r6, r1, r2           ; r6 = 3
    
    ; Immediate arithmetic
    ADDI r1, r0, 100
    SUBI r1, r1, 25          ; r1 = 75
    MULI r1, r1, 2           ; r1 = 150
    DIVI r1, r1, 3           ; r1 = 50
    
    HALT