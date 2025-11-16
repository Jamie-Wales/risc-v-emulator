.global _start
_start:
    # 1. Setup Argument
    addi x10, x0, 5     # x10 = 5

    # 2. Function Call
    # jal x1, label: Jumps to 'double_val' and saves PC+4 into x1 (Return Address)
    jal x1, double_val  

    # 3. Return Point
    # We expect x10 to be 10 now.
    addi x11, x0, 0xDD  # x11 = 0xDD (Marker that we returned successfully)
    
stop:
    beq x0, x0, stop    # Infinite loop

# --- The Function ---
double_val:
    add x10, x10, x10   # x10 = x10 + x10 (Doubling)
    
    # Return
    # jalr x0, 0(x1): Jump to the address stored in x1
    jalr x0, 0(x1)
