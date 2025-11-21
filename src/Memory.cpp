#include "Memory.h"

RAM::RAM(size_t size_bytes) { data.resize(size_bytes, 0); }

void RAM::read(uint64_t addr, uint8_t *buffer, size_t size) {
  if (addr + size > data.size()) {
    memset(buffer, 0, size);
    return;
  }
  memcpy(buffer, &data[addr], size);
}

void RAM::write(uint64_t addr, const uint8_t *buffer, size_t size) {
  if (addr + size > data.size())
    return;
  memcpy(&data[addr], buffer, size);
}
