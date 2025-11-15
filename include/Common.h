#pragma once
#include "Memory.h"
#include <cstdint>

bool load_binary(RAM &memory, const std::string &filename,
                 uint32_t start_address);
