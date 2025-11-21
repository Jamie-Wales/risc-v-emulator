.global _start
_start:
    # 1. Store 'H' (72) at address 0x100
    addi x1, x0, 72
    addi x2, x0, 0x100
    sb   x1, 0(x2)

    # 2. Store 'i' (105) at address 0x101
    addi x1, x0, 105
    sb   x1, 1(x2)
    
    # 3. Store '\n' (10) at address 0x102
    addi x1, x0, 10
    sb   x1, 2(x2)

    # 4. Setup Write Syscall
    addi x10, x0, 1      # a0 = 1 (stdout)
    addi x11, x0, 0x100  # a1 = Address of string
    addi x12, x0, 3      # a2 = Length (3 bytes)
    addi x17, x0, 64     # a7 = 64 (Syscall ID for write)
    ecall                # Trigger System Call

    # 5. Setup Exit Syscall
    addi x10, x0, 0      # a0 = 0 (Exit code success)
    addi x17, x0, 93     # a7 = 93 (Syscall ID for exit)
    ecall
