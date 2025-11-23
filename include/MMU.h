#pragma once

#include "Memory.h"
#include "Types.h"
#include <cstdint>

namespace PTE {
const uint64_t V = 1 << 0; // Valid
const uint64_t R = 1 << 1; // Read
const uint64_t W = 1 << 2; // Write
const uint64_t X = 1 << 3; // Execute
const uint64_t U = 1 << 4; // User Mode accessible
const uint64_t G = 1 << 5; // Global
const uint64_t A = 1 << 6; // Accessed
const uint64_t D = 1 << 7; // Dirty
} // namespace PTE

struct TranslationResult {
  uint64_t physical_address;
  bool exception;
};

struct TLBEntry {
  uint64_t vpn;
  uint64_t ppn;
  uint64_t permissions;
  bool valid = false;
};

class MMU {
private:
  Memory *memory;
  uint64_t satp_v = 0;

  TLBEntry tlb[64];

  PrivilegeMode current_mode = PrivilegeMode::MACHINE;

public:
  MMU(Memory *mem);
  void set_satp(uint64_t val);
  void set_mode(PrivilegeMode mode);
  void flush_tlb();
  TranslationResult translate(uint64_t va, AccessType type);
};
