#include "Cache.h"
#include <iostream>

Cache::Cache(Memory *next, uint32_t size_bytes, uint32_t num_ways,
             std::string name)
    : next_level(next), num_ways(num_ways), name(name) {

  num_sets = size_bytes / (num_ways * 64);
  sets.resize(num_sets);
  for (auto &set : sets) {
    set.ways.resize(num_ways);
  }
}

CacheLine &Cache::ensure_line_loaded(uint64_t addr, Set &set, uint64_t tag) {
  int target_index = -1;
  uint64_t min_time = UINT64_MAX;

  for (int i = 0; i < num_ways; i++) {
    if (!set.ways[i].valid) {
      target_index = i;
      break;
    }
    if (set.ways[i].last_access < min_time) {
      min_time = set.ways[i].last_access;
      target_index = i;
    }
  }

  CacheLine &target = set.ways[target_index];

  if (target.valid && target.dirty) {
    uint64_t old_addr = (target.tag << 6);
#ifdef DEBUG_CACHE
    std::cout << "[" << name << "] EVICT: Way " << target_index
              << " Dirty! Writing back to 0x" << std::hex << old_addr
              << std::dec << "\n";
#endif
    next_level->write(old_addr, target.data.data(), 64);
  }
#ifdef DEBUG_CACHE
  else if (target.valid) {
    std::cout << "[" << name << "] EVICT: Way " << target_index
              << " (Clean, dropped)\n";
  }
#endif

  uint64_t block_addr = addr & ~0x3F;
#ifdef DEBUG_CACHE
  std::cout << "[" << name << "] FILL : Loading block 0x" << std::hex
            << block_addr << " into Way " << std::dec << target_index << "\n";
#endif
  next_level->read(block_addr, target.data.data(), 64);

  target.valid = true;
  target.dirty = false;
  target.tag = tag;
  target.last_access = access_counter;

  return target;
}
void Cache::read(uint64_t addr, uint8_t *buffer, size_t size) {
  access_counter++;
  uint64_t index = (addr >> 6) % num_sets;
  uint64_t tag = (addr >> 6);
  uint64_t offset = addr & 0x3F;

  Set &set = sets[index];

#ifdef DEBUG_CACHE
  std::cout << "[" << name << "] READ : 0x" << std::hex << addr
            << " (Set: " << std::dec << index << ", Tag: 0x" << std::hex << tag
            << ")\n";
#endif

  for (int i = 0; i < num_ways; ++i) {
    CacheLine &line = set.ways[i];
    if (line.valid && line.tag == tag) {
#ifdef DEBUG_CACHE
      std::cout << "[" << name << "] HIT  : Way " << std::dec << i << "\n";
#endif
      line.last_access = access_counter;
      memcpy(buffer, &line.data[offset], size);
      return;
    }
  }

#ifdef DEBUG_CACHE
  std::cout << "[" << name << "] MISS \n";
#endif
  CacheLine &line = ensure_line_loaded(addr, set, tag);
  memcpy(buffer, &line.data[offset], size);
}

void Cache::write(uint64_t addr, const uint8_t *buffer, size_t size) {
  access_counter++;
  uint64_t index = (addr >> 6) % num_sets;
  uint64_t tag = addr >> 6;
  uint64_t offset = addr & 0x3F;

  Set &set = sets[index];

#ifdef DEBUG_CACHE
  std::cout << "[" << name << "] WRITE: 0x" << std::hex << addr
            << " (Set: " << std::dec << index << ")\n";
#endif

  for (int i = 0; i < num_ways; ++i) {
    CacheLine &line = set.ways[i];
    if (line.valid && line.tag == tag) {
#ifdef DEBUG_CACHE
      std::cout << "[" << name << "] HIT  : Way " << std::dec << i
                << " (Marking Dirty)\n";
#endif
      memcpy(&line.data[offset], buffer, size);
      line.dirty = true;
      line.last_access = access_counter;
      return;
    }
  }

#ifdef DEBUG_CACHE
  std::cout << "[" << name << "] MISS \n";
#endif
  CacheLine &line = ensure_line_loaded(addr, set, tag);
  memcpy(&line.data[offset], buffer, size);
  line.dirty = true;
}
