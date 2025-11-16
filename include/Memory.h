#pragma once
#include <vector>

class Memory {
public:
  virtual ~Memory() = default;

  virtual uint32_t read(uint32_t address) = 0;
  virtual void write(uint32_t address, uint32_t data) = 0;
};

class RAM : public Memory {

private:
  std::vector<uint8_t> data;

public:
  explicit RAM(size_t size_bytes);
  uint32_t read(uint32_t address) override;
  void write(uint32_t address, uint32_t value) override;
  void write_byte(uint32_t address, uint8_t val);
};
