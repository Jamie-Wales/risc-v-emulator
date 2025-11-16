#include "CPU.h"
#include "Decoder.h"

uint32_t CPU::get_pc() const { return pc; }

uint32_t &CPU::x(const uint32_t n) {
    if (n == 0) {
        static uint32_t zero = 0;
        return zero;
    }
    return registers[n];
}
void CPU::step() {
    uint32_t raw_inst = bus->read(pc);
#ifdef DEBUG
    std::cout << "[0x" << std::hex << pc << "] "
              << "Inst: 0x" << raw_inst << " ";
#endif
    uint32_t next_pc = pc + 4;
    DecodedInstruction d = decode(raw_inst);
    uint32_t val1 = x(d.rs1_addr);
    uint32_t val2;
    if (d.use_immediate) {
        val2 = d.immediate;
    } else {
        val2 = x(d.rs2_addr);
    }
    uint32_t result = alu.execute(val1, val2, d.alu_op);

    if (d.mem_write) {
        uint32_t data_to_store = x(d.rs2_addr);
        bus->write(result, data_to_store);
    } else if (d.mem_read) {
        result = bus->read(result);
    }

    if (d.reg_write && d.rd_addr != 0) {
        x(d.rd_addr) = result;
    }

    pc = next_pc;
}
