#include <string>
#include <Types.h>
DecodedInstruction decode(uint32_t raw_value);
static std::string get_alu_name(ALUControl op) {
    switch(op) {
        case ALUControl::ADD: return "ADD";
        case ALUControl::SUB: return "SUB";
        case ALUControl::SLT: return "SLT";
        case ALUControl::SLTU: return "SLTU";
        default: return "???";
    }
}

static std::string get_branch_name(BranchFunc op) {
    switch(op) {
        case BranchFunc::BEQ: return "BEQ";
        case BranchFunc::BNE: return "BNE";
        case BranchFunc::BLT: return "BLT";
        case BranchFunc::BGE: return "BGE";
        case BranchFunc::BLTU: return "BLTU";
        case BranchFunc::BGEU: return "BGEU";
        default: return "???";
    }
}