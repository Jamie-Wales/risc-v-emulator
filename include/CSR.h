#pragma once

#include <cstdint>

namespace CSR_Addr {
const uint64_t SSTATUS = 0x100;
const uint64_t SIE = 0x104;
const uint64_t STVEC = 0x105;
const uint64_t SATCH = 0x140;
const uint64_t SEPC = 0x141;
const uint64_t SCAUSE = 0x142;
const uint64_t CSR_STVAL = 0x143;
const uint64_t SIP = 0x144;
const uint64_t SATP = 0x180;
const uint64_t MSTATUS = 0x300;
const uint64_t MIE = 0x304;
const uint64_t MTVEC = 0x305;
const uint64_t MEPC = 0x341;
const uint16_t MCAUSE = 0x342;
const uint16_t MTVAL = 0x343;
const uint16_t MIP = 0x344;

} // namespace CSR_Addr
class CSR {
private:
  uint64_t csrs[4096];

public:
  CSR() { csrs[CSR_Addr::MSTATUS] = 0; }
  uint64_t read(uint16_t addr) {
    // #TODO: Check mode
    return csrs[addr];
  }

  void write(uint16_t addr, uint64_t value) { csrs[addr] = value; }
  uint64_t get_satp() { return csrs[CSR_Addr::SATP]; }
  uint64_t get_mtvec() { return csrs[CSR_Addr::MTVEC]; }
};
