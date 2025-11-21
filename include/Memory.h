#pragma once
#include <cstddef> // for size_t
#include <cstdint>
#include <vector>

class Memory {
public:
  virtual ~Memory() = default;

  virtual void read(uint64_t addr, uint8_t *buffer, size_t size) = 0;

  virtual void write(uint64_t addr, const uint8_t *buffer, size_t size) = 0;

  uint8_t read8(uint64_t addr) {
    uint8_t val;
    read(addr, &val, 1);
    return val;
  }

  uint16_t read16(uint64_t addr) {
    uint16_t val = 0;
    read(addr, (uint8_t *)&val, 2);
    return val;
  }

  uint32_t read32(uint64_t addr) {
    uint32_t val = 0;
    read(addr, (uint8_t *)&val, 4);
    return val;
  }

  uint64_t read64(uint64_t addr) {
    uint64_t val = 0;
    read(addr, (uint8_t *)&val, 8);
    return val;
  }

  void write8(uint64_t addr, uint8_t val) { write(addr, &val, 1); }
  void write16(uint64_t addr, uint16_t val) { write(addr, (uint8_t *)&val, 2); }
  void write32(uint64_t addr, uint32_t val) { write(addr, (uint8_t *)&val, 4); }
  void write64(uint64_t addr, uint64_t val) { write(addr, (uint8_t *)&val, 8); }
};
class RAM : public Memory {
private:
  std::vector<uint8_t> data;

public:
  explicit RAM(size_t size_bytes);
  void read(uint64_t addr, uint8_t *buffer, size_t size) override;
  void write(uint64_t addr, const uint8_t *buffer, size_t size) override;
};
