#include <Memory.h>
#include <cstdint>

uint32_t RAM::read(uint32_t address) {
  if (address + 3 >= data.size()) {
    throw std::runtime_error("Bus error address larger than data size");
  }

  uint32_t byte0 = data[address];
  uint32_t byte1 = data[address + 1];
  uint32_t byte2 = data[address + 2];
  uint32_t byte3 = data[address + 3];

  return byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24);
}

void RAM::write(uint32_t address, uint32_t value) {
  if (address + 3 >= data.size())
    return;

  data[address] = value & 0xFF;
  data[address + 1] = (value >> 8) & 0xFF;
  data[address + 2] = (value >> 16) & 0xFF;
  data[address + 3] = (value >> 24) & 0xFF;
}

void RAM::write_byte(uint32_t address, uint8_t val) {
  if (address < data.size()) {
    data[address] = val;
  }
}
RAM::RAM(size_t size_bytes) { data.resize(size_bytes, 0); }
