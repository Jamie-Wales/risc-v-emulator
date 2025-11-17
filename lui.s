.global _start
_start:
    # --- TEST 1: LUI (Load Upper Immediate) ---
    # Load 0xDEADB into the top 20 bits.
    # Expected Result: x1 = 0xDEADB000
    lui x1, 0xDEADB

    # --- TEST 2: 32-bit Constant Construction ---
    # Add 0x123 to x1 to make a full 32-bit number.
    # Expected Result: x1 = 0xDEADB123
    addi x1, x1, 0x123

    # --- TEST 3: AUIPC (Add Upper Immediate to PC) ---
    # Current PC is 0x8. 
    # Immediate is 1 (which becomes 1 << 12 = 0x1000 / 4096).
    # Expected Result: x2 = 0x8 + 0x1000 = 0x1008
    auipc x2, 1

    # --- TEST 4: High Memory Access ---
    # We want to write to address 0x10000 (64KB mark).
    # 0x10000 is (16 << 12), so we use LUI with 16.
    lui x3, 16          # x3 = 0x10000
    
    addi x4, x0, 88     # Data to store (88)
    sw   x4, 0(x3)      # Store 88 at [0x10000]
    
    lw   x5, 0(x3)      # Load back from [0x10000]
    # x5 should be 88 (0x58)

stop:
    beq x0, x0, stop
