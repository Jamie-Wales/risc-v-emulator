#pragma once
#include "Memory.h"

bool load_binary(Memory &memory, const std::string &filename,
                 uint64_t start_address);
