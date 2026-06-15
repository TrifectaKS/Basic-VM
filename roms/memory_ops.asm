; Memory Operations
; Demonstrates: LW, LH, LB, SW, SH, SB with offsets

; Memory layout假设 (简化示例)
DATA_ADDR = 0x100
SRC_ADDR = 0x110
DST_ADDR = 0x120

main:
    ; Store values to memory
    ADDI r1, r0, 0xABCD      ; Test value
    SW r1, r0, DATA_ADDR     ; Store word at DATA_ADDR
    
    ADDI r1, r0, 0x12        ; Byte value
    SB r1, r0, DATA_ADDR     ; Store byte at DATA_ADDR
    
    ; Load values from memory
    LW r2, r0, DATA_ADDR     ; Load word into r2
    LB r3, r0, DATA_ADDR     ; Load byte into r3
    LH r4, r0, DATA_ADDR     ; Load halfword into r4
    
    ; Memory copy example (word)
    LW r5, r0, SRC_ADDR       ; Load from source
    SW r5, r0, DST_ADDR       ; Store to destination
    
    HALT