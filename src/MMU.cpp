#include "MMU.h"

MMU::MMU(Memory *mem) : memory(mem) {}

void MMU::set_satp(uint64_t val) {
  if (satp_v != val) {
    satp_v = val;
    flush_tlb();
  }
}
void MMU::set_mode(PrivilegeMode mode) { current_mode = mode; }

void MMU::flush_tlb() {
  for (auto &entry : tlb)
    entry.valid = false;
}

TranslationResult MMU::translate(uint64_t va, AccessType type) {
  uint64_t mode_field = satp_v >> 60;
  if (mode_field == 0 || current_mode == PrivilegeMode::MACHINE) {
    return {va, false};
  }

  uint64_t vpn = va >> 12;
  uint64_t tlb_index = vpn % 64;
  TLBEntry &entry = tlb[tlb_index];
  if (entry.valid && entry.vpn == vpn) {
    bool permitted = false;
    switch (type) {
    case AccessType::LOAD:
      permitted = (entry.permissions & PTE::R) ||
                  (entry.permissions & PTE::R && (satp_v & (1 << 19)));
      permitted = (entry.permissions & PTE::R);
      break;
    case AccessType::STORE:
      permitted = (entry.permissions & PTE::W);
      break;
    case AccessType::FETCH:
      permitted = (entry.permissions & PTE::X);
      break;
    }

    if (!permitted) {
      return {0, true};
    }

    uint64_t offset = va & 0xFFF;
    return {(entry.ppn << 12) | offset, false};
  }

  uint64_t table_base = (satp_v & 0xFFFFFFFFFFF) << 12;
  for (int level = 2; level >= 0; level--) {
    uint64_t vpn_index = (va >> (12 + level * 9)) & 0x1FF;

    uint64_t pte_addr = table_base + (vpn_index * 8);
    uint64_t pte = memory->read64(pte_addr);

    if (!(pte & PTE::V)) {
      return {0, true};
    }

    if ((pte & (PTE::R | PTE::W | PTE::X)) != 0) {
      bool permitted = false;
      switch (type) {
      case AccessType::LOAD:
        // Readable?
        permitted = (pte & PTE::R);
        // #NOTE: RISC-V MXR bit allows reading Execute-only pages, ignored for
        // simplicity here
        break;
      case AccessType::STORE:
        // Writable?
        permitted = (pte & PTE::W);
        break;
      case AccessType::FETCH:
        permitted = (pte & PTE::X);
        break;
      }

      if (!permitted) {
        return {0, true}; // Page Fault (Permission Denied)
      }

      // Update TLB
      uint64_t ppn = (pte >> 10) & 0xFFFFFFFFFFF;
      entry.vpn = vpn;
      entry.ppn = ppn;
      entry.permissions = pte & 0xFF;
      entry.valid = true;

      if (!(pte & PTE::A) || (type == AccessType::STORE && !(pte & PTE::D))) {
        // #NOTE: In a full emulator we would write back to memory here to set
        // A/D for now just pagefault
        return {0, true};
      }

      uint64_t offset = va & 0xFFF;
      return {(ppn << 12) | offset, false};
    }

    table_base = ((pte >> 10) & 0xFFFFFFFFFFF) << 12;
  }
  return {0, true};
}
