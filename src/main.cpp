#include "CPU.h"
#include "Common.h"
#include "Memory.h"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ./riscv_emu <binary_file>" << std::endl;
    return 1;
  }

  RAM ram = RAM(1024 * 1024);
  if (!load_binary(ram, argv[1], 0))
    return 1;

  CPU cpu;
  cpu.connect_bus(&ram);

  while (true) {
    cpu.step();
  }
}
