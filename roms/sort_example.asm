; Bubble Sort Algorithm
; Demonstrates: Nested loops, branches, comparisons, memory operations

ARRAY_SIZE = 4
ARRAY_ADDR = 0x100

main:
    ; Initialize array with values
    ADDI r1, r0, 5
    SW r1, r0, ARRAY_ADDR
    ADDI r1, r0, 2
    SW r1, r0, ARRAY_ADDR_plus_4
    ADDI r1, r0, 8
    SW r1, r0, ARRAY_ADDR_plus_8
    ADDI r1, r0, 1
    SW r1, r0, ARRAY_ADDR_plus_12
    
    ; Setup outer loop counter: i = ARRAY_SIZE - 1
    ADDI r4, r0, ARRAY_SIZE
    SUBI r4, r4, 1            ; r4 = i = 3

outer_loop:
    ; Check if i < 0, exit
    BLT r4, r0, sort_done
    
    ; Inner loop counter: j = 0
    ADDI r5, r0, 0            ; r5 = j = 0

inner_loop:
    ; Check if j >= i, exit inner loop
    BGE r5, r4, next_outer
    
    ; Load array[j] and array[j+1]
    ADD r6, r0, r5            ; r6 = j
    SLLI r6, r6, 2            ; r6 = j * 4 (word offset)
    ADD r6, r6, r0            ; r6 = ARRAY_ADDR + j*4
    LW r7, r0, r6             ; r7 = array[j]
    
    ADDI r8, r5, 1            ; r8 = j + 1
    SLLI r8, r8, 2            ; r8 = (j+1) * 4
    ADD r8, r8, r0            ; r8 = ARRAY_ADDR + (j+1)*4
    LW r9, r0, r8             ; r9 = array[j+1]
    
    ; If array[j] > array[j+1], swap
    BLE r7, r9, no_swap
    SW r9, r0, r6             ; array[j] = array[j+1]
    SW r7, r0, r8             ; array[j+1] = array[j]

no_swap:
    ; Increment j and continue inner loop
    ADDI r5, r5, 1
    JAL r1, inner_loop

next_outer:
    ; Decrement i and continue outer loop
    SUBI r4, r4, 1
    JAL r1, outer_loop

sort_done:
    HALT

ARRAY_ADDR_plus_4 = ARRAY_ADDR + 4
ARRAY_ADDR_plus_8 = ARRAY_ADDR + 8
ARRAY_ADDR_plus_12 = ARRAY_ADDR + 12