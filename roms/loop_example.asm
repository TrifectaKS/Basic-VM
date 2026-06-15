; Simple Countdown Loop
; Demonstrates: Branch instructions, labels, variables

COUNT_MAX = 10

main:
    ; Initialize counter
    ADDI r1, r0, COUNT_MAX    ; r1 = 10 (counter)

loop:
    ; Decrement and check
    SUBI r1, r1, 1            ; r1 = r1 - 1
    BLT r1, r0, done          ; if r1 < 0, exit loop
    BEQ r1, r0, done          ; if r1 == 0, exit loop
    JAL r1, loop              ; Otherwise, loop again

done:
    HALT