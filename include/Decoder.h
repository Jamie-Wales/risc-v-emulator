#pragma once
#include "Types.h"
#include <cstdint>

DecodedInstruction decode(const uint32_t raw_value);

// Helpers for debug output
const char *get_alu_name(ALUControl op);
const char *get_branch_name(BranchFunc op);
