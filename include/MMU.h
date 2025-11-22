#pragma once

#include "Memory.h"
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

enum class AccessType { LOAD, STORE, FETCH };
enum class PrivilegeMode { USER = 0, SUPERVISOR = 1, MACHINE = 3 };

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
  MMU(Memory *mem) : memory(mem) {}

  void set_satp(uint64_t val) {
    if (satp_v != val) {
      satp_v = val;
      flush_tlb();
    }
  }

  void set_mode(PrivilegeMode mode) { current_mode = mode; }

  void flush_tlb() {
    for (auto &entry : tlb)
      entry.valid = false;
  }

  TranslationResult translate(uint64_t va, AccessType type) {
    uint64_t mode_field = satp_v >> 60;
    if (mode_field == 0 || current_mode == PrivilegeMode::MACHINE) {
      return {va, false};
    }
    uint64_t vpn = va >> 12;
    uint64_t tlb_index = vpn % 64;
    TLBEntry &entry = tlb[tlb_index];
    if (entry.valid && entry.vpn == vpn) {
      // TODO: Check permissions
      // assume success on hit
      uint64_t offset = va & 0xFFF;
      return {(entry.ppn << 12) | offset, false};
    }
    uint64_t table_base = (satp_v & 0xFFFFFFFFFFF) << 12;
    for (int level = 2; level >= 0; level--) {
      uint64_t vpn_index = (va >> (12 + level * 9)) & 0x1FF;

      uint64_t pte_addr = table_base + (vpn_index * 8);
      uint64_t pte = memory->read64(pte_addr);

      if (!(pte & PTE::V)) {
        return {0, true}; // Page Fault
      }

      if ((pte & (PTE::R | PTE::W | PTE::X)) != 0) {
        uint64_t ppn = (pte >> 10) & 0xFFFFFFFFFFF;

        entry.vpn = vpn;
        entry.ppn = ppn;
        entry.permissions = pte & 0xFF;
        entry.valid = true;

        uint64_t offset = va & 0xFFF;
        return {(ppn << 12) | offset, false};
      }

      table_base = ((pte >> 10) & 0xFFFFFFFFFFF) << 12;
    }

    return {0, true};
  }
};
