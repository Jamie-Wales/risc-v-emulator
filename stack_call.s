.global _start
_start:
    # 1. Initialize Stack Pointer (sp/x2) to top of RAM
    # 1024 fits in 12 bits (Max is 2047)
    addi x2, x0, 1024   
    
    # 2. Initialize Argument
    addi x10, x0, 3     
    
    # 3. Call 'my_func'
    # JAL saves Return Address (PC+4) into x1
    jal x1, my_func

    # 4. Success!
    # FIX: Changed 0xBAD to 0x0AD (Fits in 12 bits)
    addi x20, x0, 0x0AD 
    
    # FIX: Changed 0xACE to 0x0CE (Fits in 12 bits)
    addi x20, x0, 0x0CE 
    
hang:
    beq x0, x0, hang

# --- A "Proper" Function ---
my_func:
    # Prologue: Save Return Address (ra/x1) to Stack
    addi x2, x2, -4     # Move Stack Pointer down
    sw   x1, 0(x2)      # Store x1 onto stack
    
    # Body: Modify x1 (corrupting the register to prove we restored it later)
    addi x10, x10, 1    # x10 = 3 + 1 = 4
    addi x1, x0, 111    # TRASH x1! If we don't restore, JALR will crash.

    # Epilogue: Restore Return Address from Stack
    lw   x1, 0(x2)      # Load x1 back from stack (Original Return Address)
    addi x2, x2, 4      # Restore Stack Pointer
    
    # Return
    jalr x0, 0(x1)      # Jump to address in x1
