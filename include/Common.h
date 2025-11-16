#pragma once
#include "Memory.h"

bool load_binary(RAM &memory, const std::string &filename,
                 uint32_t start_address);
