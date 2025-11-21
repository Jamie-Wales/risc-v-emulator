#pragma once
#include "Memory.h"
#include <cstdint>
#include <string>
#include <vector>

#define DEBUG_CACHE 1

struct CacheLine {
  bool valid = false;
  bool dirty = false;
  uint64_t tag = 0;
  uint64_t last_access = 0;
  std::vector<uint8_t> data;

  CacheLine() { data.resize(64, 0); }
};

struct Set {
  std::vector<CacheLine> ways;
};

class Cache : public Memory {
  Memory *next_level;
  std::vector<Set> sets;
  uint32_t num_sets;
  uint32_t num_ways;
  uint64_t access_counter = 0;
  std::string name;

public:
  Cache(Memory *next, uint32_t size_bytes, uint32_t num_ways,
        std::string name = "Cache");

  CacheLine &ensure_line_loaded(uint64_t addr, Set &set, uint64_t tag);

  void read(uint64_t addr, uint8_t *buffer, size_t size) override;

  void write(uint64_t addr, const uint8_t *buffer, size_t size) override;
};
