.global _start
_start:
    addi x1, x0, 100    # x1 = 100 (Base Address)
    addi x2, x0, 42     # x2 = 42 (Data)
    sw   x2, 4(x1)
    lw   x3, 4(x1)