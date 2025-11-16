.global _start
_start:
    addi x1, x0, 3      # x1 = 3 (The Counter)
    addi x2, x0, 1      # x2 = 1 (The Decrementer)

loop:
    sub  x1, x1, x2     # x1 = x1 - 1
    bne  x1, x0, loop   # If x1 != 0, jump back to 'loop'

done:
    addi x10, x0, 55    # Success! Write 55 to x10
    
    # Infinite loop at the end to stop PC from crashing
stop:
    beq x0, x0, stop
