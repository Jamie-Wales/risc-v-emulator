#include <Common.h>
#include <fstream>
#include <iostream>

bool load_binary(RAM &memory, const std::string &filename,
                 uint32_t start_address) {

  std::ifstream file(filename, std::ios::binary | std::ios::ate);

  if (!file) {
    std::cerr << "Error: Could not open file " << filename << std::endl;
    return false;
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  if (size == 0)
    return false;

  std::vector<char> buffer(size);
  if (file.read(buffer.data(), size)) {
    for (size_t i = 0; i < size; ++i) {
      memory.write_byte(start_address + i, (uint8_t)buffer[i]);
    }
    std::cout << "Loaded " << size << " bytes into memory at 0x" << std::hex
              << start_address << std::endl;
    return true;
  }

  return false;
}
