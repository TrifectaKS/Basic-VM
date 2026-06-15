; Stack Operations
; Demonstrates: PUSH, POP for saving/restoring registers

main:
    ; Setup initial values
    ADDI r1, r0, 100
    ADDI r2, r0, 200
    ADDI r3, r0, 300
    
    ; Save registers before subroutine call
    PUSH r1
    PUSH r2
    PUSH r3
    
    ; Call subroutine that uses these registers
    CALL process_values
    
    ; Restore registers after return
    POP r3
    POP r2
    POP r1
    
    ; r1, r2, r3 now contain original values
    HALT

process_values:
    ; Do some work with registers
    ADD r1, r1, r2
    SUB r1, r1, r3
    
    RET