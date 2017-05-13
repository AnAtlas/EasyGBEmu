#include "Opcodes.hpp"
#include "Cpu.hpp"

namespace Opcodes {
	std::vector<Instruction> getInstructionVector() {
		std::vector<Instruction> instructions;
		instructions.push_back(Instruction("NOP", 0, 4, &Cpu::nop));			//0x00	
		instructions.push_back(Instruction("LD BC, 0xXXXX", 2, 12, &Cpu::ld_bc_nn));		//0x01	
		instructions.push_back(Instruction("LD (BC), A", 0, 8, &Cpu::ld_bcp_a));		//0x02	
		instructions.push_back(Instruction("INC BC", 0, 8, &Cpu::inc_bc));		//0x03	
		instructions.push_back(Instruction("INC B", 0, 4, &Cpu::inc_b));		//0x04
		instructions.push_back(Instruction("DEC B", 0, 4, &Cpu::dec_b));		//0x05
		instructions.push_back(Instruction("LD B, 0xXX", 1, 8, &Cpu::ld_b_n));		//0x06
		instructions.push_back(Instruction("RLCA", 0, 4, &Cpu::rlca));			//0x07
		instructions.push_back(Instruction("LD (0xXXXX), SP", 2, 20, &Cpu::ld_nnp_sp));	//0x08
		instructions.push_back(Instruction("ADD HL, BC", 0, 8, &Cpu::add_hl_bc));	//0x09
		instructions.push_back(Instruction("LD A, (BC)", 0, 8, &Cpu::ld_a_bcp));		//0x0A
		instructions.push_back(Instruction("DEC BC", 0, 8, &Cpu::dec_bc));		//0x0B
		instructions.push_back(Instruction("INC C", 0, 4, &Cpu::inc_c));		//0x0C
		instructions.push_back(Instruction("DEC C", 0, 4, &Cpu::dec_c));		//0x0D
		instructions.push_back(Instruction("LD C, 0xXX", 1, 8, &Cpu::ld_c_n));		//0x0E
		instructions.push_back(Instruction("RRCA", 0, 4, &Cpu::rrca));			//0x0F
		instructions.push_back(Instruction("STOP", 1, 4, &Cpu::stop));			//0x10
		instructions.push_back(Instruction("LD DE, 0xXXXX", 2, 12, &Cpu::ld_de_nn));		//0x11
		instructions.push_back(Instruction("LD (DE), A", 0, 8, &Cpu::ld_dep_a));		//0x12
		instructions.push_back(Instruction("INC DE", 0, 8, &Cpu::inc_de));		//0x13
		instructions.push_back(Instruction("INC D", 0, 4, &Cpu::inc_d));		//0x14
		instructions.push_back(Instruction("DEC D", 0, 4, &Cpu::dec_d));		//0x15
		instructions.push_back(Instruction("LD D, 0xXX", 1, 8, &Cpu::ld_d_n));		//0x16
		instructions.push_back(Instruction("RLA", 0, 4, &Cpu::rla));			//0x17
		instructions.push_back(Instruction("JR, 0xXX", 1, 12, &Cpu::jr_n));			//0x18
		instructions.push_back(Instruction("ADD HL, DE", 0, 8, &Cpu::add_hl_de));	//0x19
		instructions.push_back(Instruction("LD A, (DE)", 0, 8, &Cpu::ld_a_dep));		//0x1A
		instructions.push_back(Instruction("DEC DE", 0, 8, &Cpu::dec_de));		//0x1B
		instructions.push_back(Instruction("INC E", 0, 4, &Cpu::inc_e));		//0x1C
		instructions.push_back(Instruction("DEC E", 0, 4, &Cpu::dec_e));		//0x1D
		instructions.push_back(Instruction("LD E, 0xXX", 1, 8, &Cpu::ld_e_n));		//0x1E
		instructions.push_back(Instruction("RRA", 0, 4, &Cpu::rra));			//0x1F
		instructions.push_back(Instruction("JR NZ, 0xXX", 1, 12, &Cpu::jr_nz_n));		//0x20
		instructions.push_back(Instruction("LD HL, 0xXXXX", 2, 12, &Cpu::ld_hl_nn));		//0x21
		instructions.push_back(Instruction("LD (HL+), A", 0, 8, &Cpu::ldi_hl_a));		//0x22
		instructions.push_back(Instruction("INC HL", 0, 8, &Cpu::inc_hl));		//0x23
		instructions.push_back(Instruction("INC H", 0, 4, &Cpu::inc_h));		//0x24
		instructions.push_back(Instruction("DEC H", 0, 4, &Cpu::dec_h));		//0x25
		instructions.push_back(Instruction("LD H, 0xXX", 1, 8, &Cpu::ld_h_n));		//0x26
		instructions.push_back(Instruction("DAA", 1, 4, &Cpu::daa));			//0x27
		instructions.push_back(Instruction("JR Z, 0xXX", 1, 12, &Cpu::jr_z_n));		//0x28
		instructions.push_back(Instruction("ADD HL, HL", 1, 8, &Cpu::add_hl_hl));	//0x29
		instructions.push_back(Instruction("LD A, (HL+)", 0, 8, &Cpu::ld_a_hlpi));	//0x2A
		instructions.push_back(Instruction("DEC HL", 0, 8, &Cpu::dec_hl));		//0x2B
		instructions.push_back(Instruction("INC HL", 0, 8, &Cpu::inc_hl));		//0x2C
		instructions.push_back(Instruction("DEC L", 0, 4, &Cpu::dec_l));		//0x2D
		instructions.push_back(Instruction("LD L, 0xXX", 1, 8, &Cpu::ld_l_n));		//0x2E
		instructions.push_back(Instruction("CPL", 0, 4, &Cpu::cpl));			//0x2F
		instructions.push_back(Instruction("JR NC, 0xXX", 1, 12, &Cpu::jr_nc_n));		//0x30
		instructions.push_back(Instruction("LD SP, 0xXXXX", 2, 12, &Cpu::ld_sp_nn));		//0x31
		instructions.push_back(Instruction("LD (HL-), A", 0, 8, &Cpu::ldd_hlp_a));	//0x32
		instructions.push_back(Instruction("INC_SP", 0, 8, &Cpu::inc_sp));		//0x33
		instructions.push_back(Instruction("INC (HL)", 0, 12, &Cpu::inc_hlp));		//0x34
		instructions.push_back(Instruction("DEC (HL)", 0, 12, &Cpu::dec_hlp));		//0x35
		instructions.push_back(Instruction("LD (HL), 0xXX", 1, 12, &Cpu::ld_hlp_n));		//0x36
		instructions.push_back(Instruction("SCF", 0, 4, &Cpu::scf));			//0x37
		instructions.push_back(Instruction("JR C, 0xXX", 1, 12, &Cpu::jr_c_n));		//0x38
		instructions.push_back(Instruction("ADD HL, SP", 0, 8, &Cpu::add_hl_sp));	//0x39
		instructions.push_back(Instruction("LD A, (HL-)", 0, 8, &Cpu::ld_a_hlpd));	//0x3A
		instructions.push_back(Instruction("DEC SP", 0, 8, &Cpu::dec_sp));		//0x3B
		instructions.push_back(Instruction("INC A", 0, 4, &Cpu::inc_a));		//0x3C
		instructions.push_back(Instruction("DEC A", 0, 4, &Cpu::dec_a));		//0x3D
		instructions.push_back(Instruction("LD A, 0xXX", 1, 8, &Cpu::ld_a_n));		//0x3E
		instructions.push_back(Instruction("CCF", 0, 4, &Cpu::ccf));			//0x3F
		instructions.push_back(Instruction("LD B, B", 0, 4, &Cpu::ld_b_b));		//0x40
		instructions.push_back(Instruction("LD B, C", 0, 4, &Cpu::ld_b_c));		//0x41
		instructions.push_back(Instruction("LD B, D", 0, 4, &Cpu::ld_b_d));		//0x42
		instructions.push_back(Instruction("LD B, E", 0, 4, &Cpu::ld_b_e));		//0x43
		instructions.push_back(Instruction("LD B, H", 0, 4, &Cpu::ld_b_h));		//0x44
		instructions.push_back(Instruction("LD B, L", 0, 4, &Cpu::ld_b_l));		//0x45
		instructions.push_back(Instruction("LD B, (HL)", 0, 8, &Cpu::ld_b_hlp));		//0x46
		instructions.push_back(Instruction("LD B, A", 0, 4, &Cpu::ld_b_a));		//0x47
		instructions.push_back(Instruction("LD C, B", 0, 4, &Cpu::ld_c_b));		//0x48
		instructions.push_back(Instruction("LD C, C", 0, 8, &Cpu::ld_c_c));		//0x49
		instructions.push_back(Instruction("LD C, D", 0, 8, &Cpu::ld_c_d));		//0x4A
		instructions.push_back(Instruction("LD C, E", 0, 8, &Cpu::ld_c_e));		//0x4B
		instructions.push_back(Instruction("LD C, H", 0, 4, &Cpu::ld_c_h));		//0x4C
		instructions.push_back(Instruction("LD C, L", 0, 4, &Cpu::ld_c_l));		//0x4D
		instructions.push_back(Instruction("LD C, (HL)", 0, 8, &Cpu::ld_c_hlp));		//0x4E
		instructions.push_back(Instruction("LD C, A", 0, 4, &Cpu::ld_c_a));		//0x4F
		instructions.push_back(Instruction("LD D, B", 0, 4, &Cpu::ld_d_b));		//0x50
		instructions.push_back(Instruction("LD D, C", 0, 4, &Cpu::ld_d_c));		//0x51
		instructions.push_back(Instruction("LD D, D", 0, 4, &Cpu::ld_d_d));		//0x52
		instructions.push_back(Instruction("LD D, E", 0, 4, &Cpu::ld_d_e));		//0x53
		instructions.push_back(Instruction("LD D, H", 0, 4, &Cpu::ld_d_h));		//0x54
		instructions.push_back(Instruction("LD D, L", 0, 4, &Cpu::ld_d_l));		//0x55
		instructions.push_back(Instruction("LD D, (HL)", 0, 8, &Cpu::ld_d_hlp));		//0x56
		instructions.push_back(Instruction("LD D, A", 0, 4, &Cpu::ld_d_a));		//0x57
		instructions.push_back(Instruction("LD E, B", 0, 4, &Cpu::ld_e_b));		//0x58
		instructions.push_back(Instruction("LD E, C", 0, 4, &Cpu::ld_e_c));		//0x59
		instructions.push_back(Instruction("LD E, D", 0, 4, &Cpu::ld_e_d));		//0x5A
		instructions.push_back(Instruction("LD E, E", 0, 4, &Cpu::ld_e_e));		//0x5B
		instructions.push_back(Instruction("LD E, H", 0, 4, &Cpu::ld_e_h));		//0x5C
		instructions.push_back(Instruction("LD E, L", 0, 4, &Cpu::ld_e_l));		//0x5D
		instructions.push_back(Instruction("LD E, (HL)", 0, 8, &Cpu::ld_e_hlp));		//0x5E
		instructions.push_back(Instruction("LD E, A", 0, 4, &Cpu::ld_e_a));		//0x5F
		instructions.push_back(Instruction("LD H, B", 0, 4, &Cpu::ld_h_b));		//0x60
		instructions.push_back(Instruction("LD H, C", 0, 4, &Cpu::ld_h_c));		//0x61
		instructions.push_back(Instruction("LD H, D", 0, 4, &Cpu::ld_h_d));		//0x62
		instructions.push_back(Instruction("LD H, E", 0, 4, &Cpu::ld_h_e));		//0x63
		instructions.push_back(Instruction("LD H, H", 0, 4, &Cpu::ld_h_h));		//0x64
		instructions.push_back(Instruction("LD H, L", 0, 4, &Cpu::ld_h_l));		//0x65
		instructions.push_back(Instruction("LD H, (HL)", 0, 8, &Cpu::ld_h_hlp));		//0x66
		instructions.push_back(Instruction("LD H, A", 0, 4, &Cpu::ld_h_a));		//0x67
		instructions.push_back(Instruction("LD L, B", 0, 4, &Cpu::ld_l_b));		//0x68
		instructions.push_back(Instruction("LD L, C", 0, 4, &Cpu::ld_l_c));		//0x69
		instructions.push_back(Instruction("LD L, D", 0, 4, &Cpu::ld_l_d));		//0x6A
		instructions.push_back(Instruction("LD L, E", 0, 4, &Cpu::ld_l_e));		//0x6B
		instructions.push_back(Instruction("LD L, H", 0, 4, &Cpu::ld_l_h));		//0x6C
		instructions.push_back(Instruction("LD L, L", 0, 4, &Cpu::ld_l_l));		//0x6D
		instructions.push_back(Instruction("LD L, (HL)", 0, 8, &Cpu::ld_l_hlp));		//0x6E
		instructions.push_back(Instruction("LD L, A", 0, 4, &Cpu::ld_l_a));		//0x6F
		instructions.push_back(Instruction("LD (HL), B", 0, 8, &Cpu::ld_hlp_b));		//0x70
		instructions.push_back(Instruction("LD (HL), C", 0, 8, &Cpu::ld_hlp_c));		//0x71
		instructions.push_back(Instruction("LD (HL), D", 0, 8, &Cpu::ld_hlp_d));		//0x72
		instructions.push_back(Instruction("LD (HL), E", 0, 8, &Cpu::ld_hlp_e));		//0x73
		instructions.push_back(Instruction("LD (HL), H", 0, 8, &Cpu::ld_hlp_h));		//0x74
		instructions.push_back(Instruction("LD (HL), L", 0, 8, &Cpu::ld_hlp_l));		//0x75
		instructions.push_back(Instruction("HALT", 0, 4, &Cpu::halt));			//0x76
		instructions.push_back(Instruction("LD (HL), A", 0, 8, &Cpu::ld_hlp_a));		//0x77
		instructions.push_back(Instruction("LD A, B", 0, 4, &Cpu::ld_a_b));		//0x78
		instructions.push_back(Instruction("LD A, C", 0, 4, &Cpu::ld_a_c));		//0x79
		instructions.push_back(Instruction("LD A, D", 0, 4, &Cpu::ld_a_d));		//0x7A
		instructions.push_back(Instruction("LD A, E", 0, 4, &Cpu::ld_a_e));		//0x7B
		instructions.push_back(Instruction("LD A, H", 0, 4, &Cpu::ld_a_h));		//0x7C
		instructions.push_back(Instruction("LD A, L", 0, 4, &Cpu::ld_a_l));		//0x7D
		instructions.push_back(Instruction("LD A, (HL)", 0, 8, &Cpu::ld_a_hlp));		//0x7E
		instructions.push_back(Instruction("LD A, A", 0, 4, &Cpu::ld_a_a));		//0x7F
		instructions.push_back(Instruction("ADD A, B", 0, 4, &Cpu::add_a_b));		//0x80
		instructions.push_back(Instruction("ADD A, C", 0, 4, &Cpu::add_a_c));		//0x81
		instructions.push_back(Instruction("ADD A, D", 0, 4, &Cpu::add_a_d));		//0x82
		instructions.push_back(Instruction("ADD A, E", 0, 4, &Cpu::add_a_e));		//0x83
		instructions.push_back(Instruction("ADD A, H", 0, 4, &Cpu::add_a_h));		//0x84
		instructions.push_back(Instruction("ADD A, L", 0, 4, &Cpu::add_a_l));		//0x85
		instructions.push_back(Instruction("ADD A, (HL)", 0, 8, &Cpu::add_a_hlp));	//0x86
		instructions.push_back(Instruction("ADD A, A", 0, 4, &Cpu::add_a_a));		//0x87
		instructions.push_back(Instruction("ADC A, B", 0, 4, &Cpu::adc_a_b));		//0x88
		instructions.push_back(Instruction("ADC A, C", 0, 4, &Cpu::adc_a_c));		//0x89
		instructions.push_back(Instruction("ADC A, D", 0, 4, &Cpu::adc_a_d));		//0x8A
		instructions.push_back(Instruction("ADC A, E", 0, 4, &Cpu::adc_a_e));		//0x8B
		instructions.push_back(Instruction("ADC A, H", 0, 4, &Cpu::adc_a_h));		//0x8C
		instructions.push_back(Instruction("ADC A, L", 0, 4, &Cpu::adc_a_l));		//0x8D
		instructions.push_back(Instruction("ADC A, (HL)", 0, 8, &Cpu::adc_a_hlp));	//0x8E
		instructions.push_back(Instruction("ADC A, A", 0, 4, &Cpu::adc_a_a));		//0x8F
		instructions.push_back(Instruction("SUB B", 0, 4, &Cpu::sub_b));		//0x90
		instructions.push_back(Instruction("SUB C", 0, 4, &Cpu::sub_c));		//0x91
		instructions.push_back(Instruction("SUB D", 0, 4, &Cpu::sub_d));		//0x92
		instructions.push_back(Instruction("SUB E", 0, 4, &Cpu::sub_e));		//0x93
		instructions.push_back(Instruction("SUB H", 0, 4, &Cpu::sub_h));		//0x94
		instructions.push_back(Instruction("SUB L", 0, 4, &Cpu::sub_l));		//0x95
		instructions.push_back(Instruction("SUB (HL)", 0, 8, &Cpu::sub_hlp));		//0x96
		instructions.push_back(Instruction("SUB A", 0, 4, &Cpu::sub_a));		//0x97
		instructions.push_back(Instruction("SBC A, B", 0, 4, &Cpu::sbc_a_b));		//0x98
		instructions.push_back(Instruction("SBC A, C", 0, 4, &Cpu::sbc_a_c));		//0x99
		instructions.push_back(Instruction("SBC A, D", 0, 4, &Cpu::sbc_a_d));		//0x9A
		instructions.push_back(Instruction("SBC A, E", 0, 4, &Cpu::sbc_a_e));		//0x9B
		instructions.push_back(Instruction("SBC A, H", 0, 4, &Cpu::sbc_a_h));		//0x9C
		instructions.push_back(Instruction("SBC A, L", 0, 4, &Cpu::sbc_a_l));		//0x9D
		instructions.push_back(Instruction("SBC A, (HL)", 0, 8, &Cpu::sbc_a_hlp));	//0x9E
		instructions.push_back(Instruction("SBC A, A", 0, 4, &Cpu::sbc_a_a));		//0x9F
		instructions.push_back(Instruction("AND B", 0, 4, &Cpu::and_b));		//0xA0
		instructions.push_back(Instruction("AND C", 0, 4, &Cpu::and_c));		//0xA1
		instructions.push_back(Instruction("AND D", 0, 4, &Cpu::and_d));		//0xA2
		instructions.push_back(Instruction("AND E", 0, 4, &Cpu::and_e));		//0xA3
		instructions.push_back(Instruction("AND H", 0, 4, &Cpu::and_h));		//0xA4
		instructions.push_back(Instruction("AND L", 0, 4, &Cpu::and_l));		//0xA5
		instructions.push_back(Instruction("AND (HL)", 0, 8, &Cpu::and_hlp));		//0xA6
		instructions.push_back(Instruction("AND A", 0, 4, &Cpu::and_a));		//0xA7
		instructions.push_back(Instruction("XOR B", 0, 4, &Cpu::xor_b));		//0xA8
		instructions.push_back(Instruction("XOR C", 0, 4, &Cpu::xor_c));		//0xA9
		instructions.push_back(Instruction("XOR D", 0, 4, &Cpu::xor_d));		//0xAA
		instructions.push_back(Instruction("XOR E", 0, 4, &Cpu::xor_e));		//0xAB
		instructions.push_back(Instruction("XOR H", 0, 4, &Cpu::xor_h));		//0xAC
		instructions.push_back(Instruction("XOR L", 0, 4, &Cpu::xor_l));		//0xAD
		instructions.push_back(Instruction("XOR (HL)", 0, 8, &Cpu::xor_hlp));		//0xAE
		instructions.push_back(Instruction("XOR A", 0, 4, &Cpu::xor_a));		//0xAF
		instructions.push_back(Instruction("OR B", 0, 4, &Cpu::or_b));			//0xB0
		instructions.push_back(Instruction("OR C", 0, 4, &Cpu::or_c));			//0xB1
		instructions.push_back(Instruction("OR D", 0, 4, &Cpu::or_d));			//0xB2
		instructions.push_back(Instruction("OR E", 0, 4, &Cpu::or_e));			//0xB3
		instructions.push_back(Instruction("OR H", 0, 4, &Cpu::or_h));			//0xB4
		instructions.push_back(Instruction("OR L", 0, 4, &Cpu::or_l));			//0xB5
		instructions.push_back(Instruction("OR (HL)", 0, 8, &Cpu::or_hlp));		//0xB6
		instructions.push_back(Instruction("OR A", 0, 4, &Cpu::or_a));			//0xB7
		instructions.push_back(Instruction("CP B", 0, 4, &Cpu::cp_b));			//0xB8
		instructions.push_back(Instruction("CP C", 0, 4, &Cpu::cp_c));			//0xB9
		instructions.push_back(Instruction("CP D", 0, 4, &Cpu::cp_d));			//0xBA
		instructions.push_back(Instruction("CP E", 0, 4, &Cpu::cp_e));			//0xBB
		instructions.push_back(Instruction("CP H", 0, 4, &Cpu::cp_h));			//0xBC
		instructions.push_back(Instruction("CP L", 0, 4, &Cpu::cp_l));			//0xBD
		instructions.push_back(Instruction("CP (HL)", 0, 8, &Cpu::cp_hlp));		//0xBE
		instructions.push_back(Instruction("CP A", 0, 4, &Cpu::cp_a));			//0xBF
		instructions.push_back(Instruction("RET NZ", 0, 20, &Cpu::ret_nz));		//0xC0
		instructions.push_back(Instruction("POP BC", 0, 12, &Cpu::pop_bc));		//0xC1
		instructions.push_back(Instruction("JP NZ, 0xXXXX", 2, 16, &Cpu::jp_nz_nn));		//0xC2
		instructions.push_back(Instruction("JP 0xXXXX", 2, 16, &Cpu::jp_nn));		//0xC3
		instructions.push_back(Instruction("CALL NZ, 0xXXXX", 2, 24, &Cpu::call_nz_nn));	//0xC4
		instructions.push_back(Instruction("PUSH BC", 0, 16, &Cpu::push_bc));		//0xC5
		instructions.push_back(Instruction("ADD A, 0xXX", 1, 8, &Cpu::add_a_n));		//0xC6
		instructions.push_back(Instruction("RST 00H", 0, 16, &Cpu::rst_0));		//0xC7
		instructions.push_back(Instruction("RET Z", 0, 20, &Cpu::ret_z));		//0xC8
		instructions.push_back(Instruction("RET", 0, 16, &Cpu::ret));			//0xC9
		instructions.push_back(Instruction("JP Z, 0xXXXX", 2, 16, &Cpu::jp_z_nn));		//0xCA
		instructions.push_back(Instruction("CB", 0, 4, &Cpu::cb));			//0xCB
		instructions.push_back(Instruction("CALL Z, 0xXXXX", 2, 24, &Cpu::call_z_nn));	//0xCC
		instructions.push_back(Instruction("CALL 0xXXXX", 2, 24, &Cpu::call_nn));		//0xCD
		instructions.push_back(Instruction("ADC A, 0xXX", 1, 8, &Cpu::adc_a_n));		//0xCE
		instructions.push_back(Instruction("RST 08H", 0, 16, &Cpu::rst_08));		//0xCF
		instructions.push_back(Instruction("RET NC", 0, 20, &Cpu::ret_nc));		//0xD0
		instructions.push_back(Instruction("POP DE", 0, 12, &Cpu::pop_de));		//0xD1
		instructions.push_back(Instruction("JP NC, 0xXXXX", 2, 16, &Cpu::jp_nc_nn));		//0xD2
		instructions.push_back(Instruction("NULL", 0, 4, &Cpu::null));			//0xD3
		instructions.push_back(Instruction("CALL NC, 0xXXXX", 2, 24, &Cpu::call_nc_nn));	//0xD4
		instructions.push_back(Instruction("PUSH DE", 0, 16, &Cpu::push_de));		//0xD5
		instructions.push_back(Instruction("SUB 0xXX", 1, 8, &Cpu::sub_n));		//0xD6
		instructions.push_back(Instruction("RST 10H", 0, 16, &Cpu::rst_10));		//0xD7
		instructions.push_back(Instruction("RET C", 0, 20, &Cpu::ret_c));		//0xD8
		instructions.push_back(Instruction("RETI", 0, 16, &Cpu::reti));			//0xD9
		instructions.push_back(Instruction("JP C, 0xXXXX", 2, 16, &Cpu::jp_c_nn));		//0xDA
		instructions.push_back(Instruction("NULL", 0, 4, &Cpu::null));			//0xDB
		instructions.push_back(Instruction("CALL C, 0xXXXX", 2, 24, &Cpu::call_c_nn));	//0xDC
		instructions.push_back(Instruction("NULL", 0, 4, &Cpu::null));			//0xDD
		instructions.push_back(Instruction("SBC A, 0xXX", 1, 8, &Cpu::sbc_a_n));		//0xDE
		instructions.push_back(Instruction("RST 18H", 0, 16, &Cpu::rst_18));		//0xDF
		instructions.push_back(Instruction("LD (0xFFXX), A", 1, 12, &Cpu::ld_ffnp_a));	//0xE0
		instructions.push_back(Instruction("POP HL", 0, 12, &Cpu::pop_hl));		//0xE1
		instructions.push_back(Instruction("LD (C), A", 1, 8, &Cpu::ld_cp_a));		//0xE2
		instructions.push_back(Instruction("NULL", 0, 4, &Cpu::null));			//0xE3
		instructions.push_back(Instruction("NULL", 0, 4, &Cpu::null));			//0xE4
		instructions.push_back(Instruction("PUSH HL", 0, 16, &Cpu::push_hl));		//0xE5
		instructions.push_back(Instruction("AND 0xXX", 1, 8, &Cpu::and_n));		//0xE6
		instructions.push_back(Instruction("RST 20H", 0, 16, &Cpu::rst_20));		//0xE7
		instructions.push_back(Instruction("ADD SP, 0xXX", 1, 16, &Cpu::add_sp_n));		//0xE8
		instructions.push_back(Instruction("JP (HL)", 0, 4, &Cpu::jp_hlp));		//0xE9
		instructions.push_back(Instruction("LD (0xXXXX), A", 2, 16, &Cpu::ld_nnp_a));		//0xEA
		instructions.push_back(Instruction("NULL", 0, 4, &Cpu::null));			//0xEB
		instructions.push_back(Instruction("NULL", 0, 4, &Cpu::null));			//0xEC
		instructions.push_back(Instruction("NULL", 0, 4, &Cpu::null));			//0xED
		instructions.push_back(Instruction("XOR 0xXX", 1, 8, &Cpu::xor_n));		//0xEE
		instructions.push_back(Instruction("RST 28H", 0, 16, &Cpu::rst_28));		//0xEF
		instructions.push_back(Instruction("LD A, (0xFFXX)", 1, 12, &Cpu::ld_a_ffnp));	//0xF0
		instructions.push_back(Instruction("POP AF", 0, 12, &Cpu::pop_af));		//0xF1
		instructions.push_back(Instruction("LD A, (C)", 1, 8, &Cpu::ld_a_cp));		//0xF2
		instructions.push_back(Instruction("DI", 0, 4, &Cpu::di));			//0xF3
		instructions.push_back(Instruction("NULL", 0, 4, &Cpu::null));			//0xF4
		instructions.push_back(Instruction("PUSH AF", 0, 16, &Cpu::push_af));		//0xF5
		instructions.push_back(Instruction("OR 0xXX", 1, 8, &Cpu::or_n));			//0xF6
		instructions.push_back(Instruction("RST 30H", 0, 16, &Cpu::rst_30));		//0xF7
		instructions.push_back(Instruction("LD HL, SP+0xXX", 1, 12, &Cpu::ld_hl_sp_n));	//0xF8
		instructions.push_back(Instruction("LD SP, HL", 0, 8, &Cpu::ld_sp_hl));		//0xF9
		instructions.push_back(Instruction("LD A, (0xXXXX)", 2, 16, &Cpu::ld_a_nnp));	//0xFA
		instructions.push_back(Instruction("EI", 0, 4, &Cpu::ei));			//0xFB
		instructions.push_back(Instruction("NULL", 0, 4, &Cpu::null));			//0xFC
		instructions.push_back(Instruction("NULL", 0, 4, &Cpu::null));			//0xFD
		instructions.push_back(Instruction("CP 0xXX", 1, 8, &Cpu::cp_n));			//0xFE
		instructions.push_back(Instruction("RST 38H", 0, 16, &Cpu::rst_38));		//0xFF
		return instructions;
	}

	std::vector<Instruction> getExtendedInstructionVector() {
		std::vector<Instruction> ins;
		ins.push_back(Instruction("RLC B", 1, 8, &Cpu::rlc_b));
		ins.push_back(Instruction("RLC C", 1, 8, &Cpu::rlc_c));
		ins.push_back(Instruction("RLC D", 1, 8, &Cpu::rlc_d));
		ins.push_back(Instruction("RLC E", 1, 8, &Cpu::rlc_e));
		ins.push_back(Instruction("RLC H", 1, 8, &Cpu::rlc_h));
		ins.push_back(Instruction("RLC L", 1, 8, &Cpu::rlc_l));
		ins.push_back(Instruction("RLC (HL)", 1, 16, &Cpu::rlc_hlp));
		ins.push_back(Instruction("RLC A", 1, 8, &Cpu::rlc_a));
		ins.push_back(Instruction("RRC B", 1, 8, &Cpu::rrc_b));
		ins.push_back(Instruction("RRC C", 1, 8, &Cpu::rrc_c));
		ins.push_back(Instruction("RRC D", 1, 8, &Cpu::rrc_d));
		ins.push_back(Instruction("RRC E", 1, 8, &Cpu::rrc_e));
		ins.push_back(Instruction("RRC H", 1, 8, &Cpu::rrc_h));
		ins.push_back(Instruction("RRC L", 1, 8, &Cpu::rrc_l));
		ins.push_back(Instruction("RRC (HL)", 1, 16, &Cpu::rrc_hlp));
		ins.push_back(Instruction("RRC A", 1, 8, &Cpu::rrc_a));
		ins.push_back(Instruction("RL B", 1, 8, &Cpu::rl_b));
		ins.push_back(Instruction("RL C", 1, 8, &Cpu::rl_c));
		ins.push_back(Instruction("RL D", 1, 8, &Cpu::rl_d));
		ins.push_back(Instruction("RL E", 1, 8, &Cpu::rl_e));
		ins.push_back(Instruction("RL H", 1, 8, &Cpu::rl_h));
		ins.push_back(Instruction("RL L", 1, 8, &Cpu::rl_l));
		ins.push_back(Instruction("RL (HL)", 1, 16, &Cpu::rl_hlp));
		ins.push_back(Instruction("RL A", 1, 8, &Cpu::rl_a));
		ins.push_back(Instruction("RR B", 1, 8, &Cpu::rr_b));
		ins.push_back(Instruction("RR C", 1, 8, &Cpu::rr_c));
		ins.push_back(Instruction("RR D", 1, 8, &Cpu::rr_d));
		ins.push_back(Instruction("RR E", 1, 8, &Cpu::rr_e));
		ins.push_back(Instruction("RR H", 1, 8, &Cpu::rr_h));
		ins.push_back(Instruction("RR L", 1, 8, &Cpu::rr_l));
		ins.push_back(Instruction("RR (HL)", 1, 16, &Cpu::rr_hlp));
		ins.push_back(Instruction("RR A", 1, 8, &Cpu::rr_a));
		ins.push_back(Instruction("SLA B", 1, 8, &Cpu::sla_b));
		ins.push_back(Instruction("SLA C", 1, 8, &Cpu::sla_c));
		ins.push_back(Instruction("SLA D", 1, 8, &Cpu::sla_d));
		ins.push_back(Instruction("SLA E", 1, 8, &Cpu::sla_e));
		ins.push_back(Instruction("SLA H", 1, 8, &Cpu::sla_h));
		ins.push_back(Instruction("SLA L", 1, 8, &Cpu::sla_l));
		ins.push_back(Instruction("SLA (HL)", 1, 16, &Cpu::sla_hlp));
		ins.push_back(Instruction("SLA A", 1, 8, &Cpu::sla_a));
		ins.push_back(Instruction("SRA B", 1, 8, &Cpu::sra_b));
		ins.push_back(Instruction("SRA C", 1, 8, &Cpu::sra_c));
		ins.push_back(Instruction("SRA D", 1, 8, &Cpu::sra_d));
		ins.push_back(Instruction("SRA E", 1, 8, &Cpu::sra_e));
		ins.push_back(Instruction("SRA H", 1, 8, &Cpu::sra_h));
		ins.push_back(Instruction("SRA L", 1, 8, &Cpu::sra_l));
		ins.push_back(Instruction("SRA (HL)", 1, 16, &Cpu::sra_hlp));
		ins.push_back(Instruction("SRA A", 1, 8, &Cpu::sra_a));
		ins.push_back(Instruction("SWAP B", 1, 8, &Cpu::swap_b));
		ins.push_back(Instruction("SWAP C", 1, 8, &Cpu::swap_c));
		ins.push_back(Instruction("SWAP D", 1, 8, &Cpu::swap_d));
		ins.push_back(Instruction("SWAP E", 1, 8, &Cpu::swap_e));
		ins.push_back(Instruction("SWAP H", 1, 8, &Cpu::swap_h));
		ins.push_back(Instruction("SWAP L", 1, 8, &Cpu::swap_l));
		ins.push_back(Instruction("SWAP (HL)", 1, 16, &Cpu::swap_hlp));
		ins.push_back(Instruction("SWAP A", 1, 8, &Cpu::swap_a));
		ins.push_back(Instruction("SRL B", 1, 8, &Cpu::srl_b));
		ins.push_back(Instruction("SRL C", 1, 8, &Cpu::srl_c));
		ins.push_back(Instruction("SRL D", 1, 8, &Cpu::srl_d));
		ins.push_back(Instruction("SRL E", 1, 8, &Cpu::srl_e));
		ins.push_back(Instruction("SRL H", 1, 8, &Cpu::srl_h));
		ins.push_back(Instruction("SRL L", 1, 8, &Cpu::srl_l));
		ins.push_back(Instruction("SRL (HL)", 1, 16, &Cpu::srl_hlp));
		ins.push_back(Instruction("SRL A", 1, 8, &Cpu::srl_a));
		ins.push_back(Instruction("BIT 0, B", 1, 8, &Cpu::bit0_b));
		ins.push_back(Instruction("BIT 0, C", 1, 8, &Cpu::bit0_c));
		ins.push_back(Instruction("BIT 0, D", 1, 8, &Cpu::bit0_d));
		ins.push_back(Instruction("BIT 0, E", 1, 8, &Cpu::bit0_e));
		ins.push_back(Instruction("BIT 0, H", 1, 8, &Cpu::bit0_h));
		ins.push_back(Instruction("BIT 0, L", 1, 8, &Cpu::bit0_l));
		ins.push_back(Instruction("BIT 0, (HL)", 1, 16, &Cpu::bit0_hlp));
		ins.push_back(Instruction("BIT 0, A", 1, 8, &Cpu::bit0_a));
		ins.push_back(Instruction("BIT 1, B", 1, 8, &Cpu::bit1_b));
		ins.push_back(Instruction("BIT 1, C", 1, 8, &Cpu::bit1_c));
		ins.push_back(Instruction("BIT 1, D", 1, 8, &Cpu::bit1_d));
		ins.push_back(Instruction("BIT 1, E", 1, 8, &Cpu::bit1_e));
		ins.push_back(Instruction("BIT 1, H", 1, 8, &Cpu::bit1_h));
		ins.push_back(Instruction("BIT 1, L", 1, 8, &Cpu::bit1_l));
		ins.push_back(Instruction("BIT 1, (HL)", 1, 16, &Cpu::bit1_hlp));
		ins.push_back(Instruction("BIT 1, A", 1, 8, &Cpu::bit1_a));
		ins.push_back(Instruction("BIT 2, B", 1, 8, &Cpu::bit2_b));
		ins.push_back(Instruction("BIT 2, C", 1, 8, &Cpu::bit2_c));
		ins.push_back(Instruction("BIT 2, D", 1, 8, &Cpu::bit2_d));
		ins.push_back(Instruction("BIT 2, E", 1, 8, &Cpu::bit2_e));
		ins.push_back(Instruction("BIT 2, H", 1, 8, &Cpu::bit2_h));
		ins.push_back(Instruction("BIT 2, L", 1, 8, &Cpu::bit2_l));
		ins.push_back(Instruction("BIT 2, (HL)", 1, 16, &Cpu::bit2_hlp));
		ins.push_back(Instruction("BIT 2, A", 1, 8, &Cpu::bit2_a));
		ins.push_back(Instruction("BIT 3, B", 1, 8, &Cpu::bit3_b));
		ins.push_back(Instruction("BIT 3, C", 1, 8, &Cpu::bit3_c));
		ins.push_back(Instruction("BIT 3, D", 1, 8, &Cpu::bit3_d));
		ins.push_back(Instruction("BIT 3, E", 1, 8, &Cpu::bit3_e));
		ins.push_back(Instruction("BIT 3, H", 1, 8, &Cpu::bit3_h));
		ins.push_back(Instruction("BIT 3, L", 1, 8, &Cpu::bit3_l));
		ins.push_back(Instruction("BIT 3, (HL)", 1, 16, &Cpu::bit3_hlp));
		ins.push_back(Instruction("BIT 3, A", 1, 8, &Cpu::bit3_a));
		ins.push_back(Instruction("BIT 4, B", 1, 8, &Cpu::bit4_b));
		ins.push_back(Instruction("BIT 4, C", 1, 8, &Cpu::bit4_c));
		ins.push_back(Instruction("BIT 4, D", 1, 8, &Cpu::bit4_d));
		ins.push_back(Instruction("BIT 4, E", 1, 8, &Cpu::bit4_e));
		ins.push_back(Instruction("BIT 4, H", 1, 8, &Cpu::bit4_h));
		ins.push_back(Instruction("BIT 4, L", 1, 8, &Cpu::bit4_l));
		ins.push_back(Instruction("BIT 4, (HL)", 1, 16, &Cpu::bit4_hlp));
		ins.push_back(Instruction("BIT 4, A", 1, 8, &Cpu::bit4_a));
		ins.push_back(Instruction("BIT 5, B", 1, 8, &Cpu::bit5_b));
		ins.push_back(Instruction("BIT 5, C", 1, 8, &Cpu::bit5_c));
		ins.push_back(Instruction("BIT 5, D", 1, 8, &Cpu::bit5_d));
		ins.push_back(Instruction("BIT 5, E", 1, 8, &Cpu::bit5_e));
		ins.push_back(Instruction("BIT 5, H", 1, 8, &Cpu::bit5_h));
		ins.push_back(Instruction("BIT 5, L", 1, 8, &Cpu::bit5_l));
		ins.push_back(Instruction("BIT 5, (HL)", 1, 16, &Cpu::bit5_hlp));
		ins.push_back(Instruction("BIT 5, A", 1, 8, &Cpu::bit5_a));
		ins.push_back(Instruction("BIT 6, B", 1, 8, &Cpu::bit6_b));
		ins.push_back(Instruction("BIT 6, C", 1, 8, &Cpu::bit6_c));
		ins.push_back(Instruction("BIT 6, D", 1, 8, &Cpu::bit6_d));
		ins.push_back(Instruction("BIT 6, E", 1, 8, &Cpu::bit6_e));
		ins.push_back(Instruction("BIT 6, H", 1, 8, &Cpu::bit6_h));
		ins.push_back(Instruction("BIT 6, L", 1, 8, &Cpu::bit6_l));
		ins.push_back(Instruction("BIT 6, (HL)", 1, 16, &Cpu::bit6_hlp));
		ins.push_back(Instruction("BIT 6, A", 1, 8, &Cpu::bit6_a));
		ins.push_back(Instruction("BIT 7, B", 1, 8, &Cpu::bit7_b));
		ins.push_back(Instruction("BIT 7, C", 1, 8, &Cpu::bit7_c));
		ins.push_back(Instruction("BIT 7, D", 1, 8, &Cpu::bit7_d));
		ins.push_back(Instruction("BIT 7, E", 1, 8, &Cpu::bit7_e));
		ins.push_back(Instruction("BIT 7, H", 1, 8, &Cpu::bit7_h));
		ins.push_back(Instruction("BIT 7, L", 1, 8, &Cpu::bit7_l));
		ins.push_back(Instruction("BIT 7, (HL)", 1, 16, &Cpu::bit7_hlp));
		ins.push_back(Instruction("BIT 7, A", 1, 8, &Cpu::bit7_a));
		ins.push_back(Instruction("RES 0, B", 1, 8, &Cpu::res0_b));
		ins.push_back(Instruction("RES 0, C", 1, 8, &Cpu::res0_c));
		ins.push_back(Instruction("RES 0, D", 1, 8, &Cpu::res0_d));
		ins.push_back(Instruction("RES 0, E", 1, 8, &Cpu::res0_e));
		ins.push_back(Instruction("RES 0, H", 1, 8, &Cpu::res0_h));
		ins.push_back(Instruction("RES 0, L", 1, 8, &Cpu::res0_l));
		ins.push_back(Instruction("RES 0, (HL)", 1, 16, &Cpu::res0_hlp));
		ins.push_back(Instruction("RES 0, A", 1, 8, &Cpu::res0_a));
		ins.push_back(Instruction("RES 1, B", 1, 8, &Cpu::res1_b));
		ins.push_back(Instruction("RES 1, C", 1, 8, &Cpu::res1_c));
		ins.push_back(Instruction("RES 1, D", 1, 8, &Cpu::res1_d));
		ins.push_back(Instruction("RES 1, E", 1, 8, &Cpu::res1_e));
		ins.push_back(Instruction("RES 1, H", 1, 8, &Cpu::res1_h));
		ins.push_back(Instruction("RES 1, L", 1, 8, &Cpu::res1_l));
		ins.push_back(Instruction("RES 1, (HL)", 1, 16, &Cpu::res1_hlp));
		ins.push_back(Instruction("RES 1, A", 1, 8, &Cpu::res1_a));
		ins.push_back(Instruction("RES 2, B", 1, 8, &Cpu::res2_b));
		ins.push_back(Instruction("RES 2, C", 1, 8, &Cpu::res2_c));
		ins.push_back(Instruction("RES 2, D", 1, 8, &Cpu::res2_d));
		ins.push_back(Instruction("RES 2, E", 1, 8, &Cpu::res2_e));
		ins.push_back(Instruction("RES 2, H", 1, 8, &Cpu::res2_h));
		ins.push_back(Instruction("RES 2, L", 1, 8, &Cpu::res2_l));
		ins.push_back(Instruction("RES 2, (HL)", 1, 16, &Cpu::res2_hlp));
		ins.push_back(Instruction("RES 2, A", 1, 8, &Cpu::res2_a));
		ins.push_back(Instruction("RES 3, B", 1, 8, &Cpu::res3_b));
		ins.push_back(Instruction("RES 3, C", 1, 8, &Cpu::res3_c));
		ins.push_back(Instruction("RES 3, D", 1, 8, &Cpu::res3_d));
		ins.push_back(Instruction("RES 3, E", 1, 8, &Cpu::res3_e));
		ins.push_back(Instruction("RES 3, H", 1, 8, &Cpu::res3_h));
		ins.push_back(Instruction("RES 3, L", 1, 8, &Cpu::res3_l));
		ins.push_back(Instruction("RES 3, (HL)", 1, 16, &Cpu::res3_hlp));
		ins.push_back(Instruction("RES 3, A", 1, 8, &Cpu::res3_a));
		ins.push_back(Instruction("RES 4, B", 1, 8, &Cpu::res4_b));
		ins.push_back(Instruction("RES 4, C", 1, 8, &Cpu::res4_c));
		ins.push_back(Instruction("RES 4, D", 1, 8, &Cpu::res4_d));
		ins.push_back(Instruction("RES 4, E", 1, 8, &Cpu::res4_e));
		ins.push_back(Instruction("RES 4, H", 1, 8, &Cpu::res4_h));
		ins.push_back(Instruction("RES 4, L", 1, 8, &Cpu::res4_l));
		ins.push_back(Instruction("RES 4, (HL)", 1, 16, &Cpu::res4_hlp));
		ins.push_back(Instruction("RES 4, A", 1, 8, &Cpu::res4_a));
		ins.push_back(Instruction("RES 5, B", 1, 8, &Cpu::res5_b));
		ins.push_back(Instruction("RES 5, C", 1, 8, &Cpu::res5_c));
		ins.push_back(Instruction("RES 5, D", 1, 8, &Cpu::res5_d));
		ins.push_back(Instruction("RES 5, E", 1, 8, &Cpu::res5_e));
		ins.push_back(Instruction("RES 5, H", 1, 8, &Cpu::res5_h));
		ins.push_back(Instruction("RES 5, L", 1, 8, &Cpu::res5_l));
		ins.push_back(Instruction("RES 5, (HL)", 1, 16, &Cpu::res5_hlp));
		ins.push_back(Instruction("RES 5, A", 1, 8, &Cpu::res5_a));
		ins.push_back(Instruction("RES 6, B", 1, 8, &Cpu::res6_b));
		ins.push_back(Instruction("RES 6, C", 1, 8, &Cpu::res6_c));
		ins.push_back(Instruction("RES 6, D", 1, 8, &Cpu::res6_d));
		ins.push_back(Instruction("RES 6, E", 1, 8, &Cpu::res6_e));
		ins.push_back(Instruction("RES 6, H", 1, 8, &Cpu::res6_h));
		ins.push_back(Instruction("RES 6, L", 1, 8, &Cpu::res6_l));
		ins.push_back(Instruction("RES 6, (HL)", 1, 16, &Cpu::res6_hlp));
		ins.push_back(Instruction("RES 6, A", 1, 8, &Cpu::res6_a));
		ins.push_back(Instruction("RES 7, B", 1, 8, &Cpu::res7_b));
		ins.push_back(Instruction("RES 7, C", 1, 8, &Cpu::res7_c));
		ins.push_back(Instruction("RES 7, D", 1, 8, &Cpu::res7_d));
		ins.push_back(Instruction("RES 7, E", 1, 8, &Cpu::res7_e));
		ins.push_back(Instruction("RES 7, H", 1, 8, &Cpu::res7_h));
		ins.push_back(Instruction("RES 7, L", 1, 8, &Cpu::res7_l));
		ins.push_back(Instruction("RES 7, (HL)", 1, 16, &Cpu::res7_hlp));
		ins.push_back(Instruction("RES 7, A", 1, 8, &Cpu::res7_a));
		ins.push_back(Instruction("SET 0, B", 1, 8, &Cpu::set0_b));
		ins.push_back(Instruction("SET 0, C", 1, 8, &Cpu::set0_c));
		ins.push_back(Instruction("SET 0, D", 1, 8, &Cpu::set0_d));
		ins.push_back(Instruction("SET 0, E", 1, 8, &Cpu::set0_e));
		ins.push_back(Instruction("SET 0, H", 1, 8, &Cpu::set0_h));
		ins.push_back(Instruction("SET 0, L", 1, 8, &Cpu::set0_l));
		ins.push_back(Instruction("SET 0, (HL)", 1, 16, &Cpu::set0_hlp));
		ins.push_back(Instruction("SET 0, A", 1, 8, &Cpu::set0_a));
		ins.push_back(Instruction("SET 1, B", 1, 8, &Cpu::set1_b));
		ins.push_back(Instruction("SET 1, C", 1, 8, &Cpu::set1_c));
		ins.push_back(Instruction("SET 1, D", 1, 8, &Cpu::set1_d));
		ins.push_back(Instruction("SET 1, E", 1, 8, &Cpu::set1_e));
		ins.push_back(Instruction("SET 1, H", 1, 8, &Cpu::set1_h));
		ins.push_back(Instruction("SET 1, L", 1, 8, &Cpu::set1_l));
		ins.push_back(Instruction("SET 1, (HL)", 1, 16, &Cpu::set1_hlp));
		ins.push_back(Instruction("SET 1, A", 1, 8, &Cpu::set1_a));
		ins.push_back(Instruction("SET 2, B", 1, 8, &Cpu::set2_b));
		ins.push_back(Instruction("SET 2, C", 1, 8, &Cpu::set2_c));
		ins.push_back(Instruction("SET 2, D", 1, 8, &Cpu::set2_d));
		ins.push_back(Instruction("SET 2, E", 1, 8, &Cpu::set2_e));
		ins.push_back(Instruction("SET 2, H", 1, 8, &Cpu::set2_h));
		ins.push_back(Instruction("SET 2, L", 1, 8, &Cpu::set2_l));
		ins.push_back(Instruction("SET 2, (HL)", 1, 16, &Cpu::set2_hlp));
		ins.push_back(Instruction("SET 2, A", 1, 8, &Cpu::set2_a));
		ins.push_back(Instruction("SET 3, B", 1, 8, &Cpu::set3_b));
		ins.push_back(Instruction("SET 3, C", 1, 8, &Cpu::set3_c));
		ins.push_back(Instruction("SET 3, D", 1, 8, &Cpu::set3_d));
		ins.push_back(Instruction("SET 3, E", 1, 8, &Cpu::set3_e));
		ins.push_back(Instruction("SET 3, H", 1, 8, &Cpu::set3_h));
		ins.push_back(Instruction("SET 3, L", 1, 8, &Cpu::set3_l));
		ins.push_back(Instruction("SET 3, (HL)", 1, 16, &Cpu::set3_hlp));
		ins.push_back(Instruction("SET 3, A", 1, 8, &Cpu::set3_a));
		ins.push_back(Instruction("SET 4, B", 1, 8, &Cpu::set4_b));
		ins.push_back(Instruction("SET 4, C", 1, 8, &Cpu::set4_c));
		ins.push_back(Instruction("SET 4, D", 1, 8, &Cpu::set4_d));
		ins.push_back(Instruction("SET 4, E", 1, 8, &Cpu::set4_e));
		ins.push_back(Instruction("SET 4, H", 1, 8, &Cpu::set4_h));
		ins.push_back(Instruction("SET 4, L", 1, 8, &Cpu::set4_l));
		ins.push_back(Instruction("SET 4, (HL)", 1, 16, &Cpu::set4_hlp));
		ins.push_back(Instruction("SET 4, A", 1, 8, &Cpu::set4_a));
		ins.push_back(Instruction("SET 5, B", 1, 8, &Cpu::set5_b));
		ins.push_back(Instruction("SET 5, C", 1, 8, &Cpu::set5_c));
		ins.push_back(Instruction("SET 5, D", 1, 8, &Cpu::set5_d));
		ins.push_back(Instruction("SET 5, E", 1, 8, &Cpu::set5_e));
		ins.push_back(Instruction("SET 5, H", 1, 8, &Cpu::set5_h));
		ins.push_back(Instruction("SET 5, L", 1, 8, &Cpu::set5_l));
		ins.push_back(Instruction("SET 5, (HL)", 1, 16, &Cpu::set5_hlp));
		ins.push_back(Instruction("SET 5, A", 1, 8, &Cpu::set5_a));
		ins.push_back(Instruction("SET 6, B", 1, 8, &Cpu::set6_b));
		ins.push_back(Instruction("SET 6, C", 1, 8, &Cpu::set6_c));
		ins.push_back(Instruction("SET 6, D", 1, 8, &Cpu::set6_d));
		ins.push_back(Instruction("SET 6, E", 1, 8, &Cpu::set6_e));
		ins.push_back(Instruction("SET 6, H", 1, 8, &Cpu::set6_h));
		ins.push_back(Instruction("SET 6, L", 1, 8, &Cpu::set6_l));
		ins.push_back(Instruction("SET 6, (HL)", 1, 16, &Cpu::set6_hlp));
		ins.push_back(Instruction("SET 6, A", 1, 8, &Cpu::set6_a));
		ins.push_back(Instruction("SET 7, B", 1, 8, &Cpu::set7_b));
		ins.push_back(Instruction("SET 7, C", 1, 8, &Cpu::set7_c));
		ins.push_back(Instruction("SET 7, D", 1, 8, &Cpu::set7_d));
		ins.push_back(Instruction("SET 7, E", 1, 8, &Cpu::set7_e));
		ins.push_back(Instruction("SET 7, H", 1, 8, &Cpu::set7_h));
		ins.push_back(Instruction("SET 7, L", 1, 8, &Cpu::set7_l));
		ins.push_back(Instruction("SET 7, (HL)", 1, 16, &Cpu::set7_hlp));
		ins.push_back(Instruction("SET 7, A", 1, 8, &Cpu::set7_a));
	}
}
