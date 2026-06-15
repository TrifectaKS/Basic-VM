; Subroutine/Function Example
; Demonstrates: CALL, RET, PUSH, POP, stack usage

main:
    ; Save initial values
    ADDI r1, r0, 5
    ADDI r2, r0, 3
    
    ; Call subroutine to add them
    CALL add_values
    
    ; Result is now in r1
    HALT

; Subroutine: add_values
; Adds r1 and r2, returns result in r1
add_values:
    PUSH r1              ; Save r1 (first argument)
    PUSH r2              ; Save r2 (second argument)
    
    ; Perform addition
    ADD r3, r1, r2       ; r3 = r1 + r2
    
    ; Restore registers
    POP r2               ; Restore r2
    POP r1                ; Restore r1
    
    ; Move result to return register (r1)
    ADDI r1, r3, 0
    
    RET