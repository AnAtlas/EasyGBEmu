#include "Cpu.hpp"
#include <iostream>
#include <string>
#include <sstream>

Cpu::Cpu(){
	clearRegisters();
	generateInstructions();
}


void Cpu::runCommand(std::string command) {

	//Break apart command
	std::vector <unsigned char> parms;
	std::stringstream ss;
	ss.str(command);
	std::string item;
	while (std::getline(ss, item, ' ')) {
		unsigned char temp1 = std::stoul(item, nullptr, 16);

		parms.push_back(temp1);
	}
	if (parms[0] == 0xff)
		printRegisters();
	else {
		unsigned char op = parms[0];
		parms.erase(parms.begin());
		(this->*(instructions[op].fp))(parms);
		printRegisters();
	}
}

void Cpu::step() {

}

void Cpu::clearRegisters() {
	registers.af = 0;
	registers.bc = 0;
	registers.de = 0;
	registers.hl = 0;
	registers.sp = 0;
	registers.pc = 0;
}

bool Cpu::checkFlag(unsigned char flag) {
	return (registers.f & flag) ? true : false;
}

void Cpu::setFlag(unsigned char flag) {
	registers.f |= flag;
}

void Cpu::clearFlag(unsigned char flag) {
	registers.f &= ~flag;
}

void Cpu::clearAllFlags() {
	registers.f = 0;
}

void Cpu::reset() {
	stopped = false;
}


unsigned char Cpu::inc(unsigned char val) {
	if ((val & 0x0F) == 0x0F)
		setFlag(Flags::HalfCarry);
	else
		clearFlag(Flags::HalfCarry);
	val++;
	if (val == 0x00)
		setFlag(Flags::Zero);
	else
		clearFlag(Flags::Zero);

	clearFlag(Flags::Subtract);
	return val;
}

unsigned char Cpu::dec(unsigned char val) {
	if ((val & 0x10) == 0x10)
		setFlag(Flags::HalfCarry);
	else
		clearFlag(Flags::HalfCarry);
	val--;
	if (val == 0x00)
		setFlag(Flags::Zero);
	else
		clearFlag(Flags::Zero);

	setFlag(Flags::Subtract);
	return val;
}

unsigned char Cpu::and(unsigned char a, unsigned char b) {
	clearFlag(Flags::Subtract);
	clearFlag(Flags::Carry);
	setFlag(Flags::HalfCarry);

	if (a & b)
		clearFlag(Flags::Zero);
	else
		setFlag(Flags::Zero);

	return a & b;
}

unsigned char Cpu::xor(unsigned char a, unsigned char b) {
	clearAllFlags();
	if (!(a ^ b))
		setFlag(Flags::Zero);
	return a ^ b;
}

unsigned char Cpu:: or(unsigned char a, unsigned char b) {
	clearAllFlags();
	if (!(a | b))
		setFlag(Flags::Zero);
	return a | b;
}

//Compare given char to register a
void Cpu::compare(unsigned char b) {
	setFlag(Flags::Subtract);
	if (registers.a < b)
		setFlag(Flags::Carry);
	else
		clearFlag(Flags::Carry);

	if ((registers.a & 0xf) < (b & 0xf))
		setFlag(Flags::HalfCarry);
	else
		clearFlag(Flags::HalfCarry);
	if (registers.a == b)
		setFlag(Flags::Zero);
	else
		clearFlag(Flags::Zero);
}

unsigned char Cpu::addBytes(unsigned char a, unsigned char b) {
	if (((a & 0xf) + (b & 0xf)) & 0x10)
		setFlag(Flags::HalfCarry);
	else
		clearFlag(Flags::HalfCarry);
	if ((a + b) & 0x100)
		setFlag(Flags::Carry);
	else
		clearFlag(Flags::Carry);

	if (a + b)
		clearFlag(Flags::Zero);
	else
		setFlag(Flags::Zero);

	clearFlag(Flags::Subtract);
	return a + b;
}

unsigned char Cpu::subBytes(unsigned char a, unsigned char b) {
	if (b > a)
		setFlag(Flags::Carry);
	else
		clearFlag(Flags::Carry);

	if ((b & 0xf) > (a & 0xf))
		setFlag(Flags::HalfCarry);
	else
		clearFlag(Flags::HalfCarry);

	if (a == b)
		setFlag(Flags::Zero);
	else
		clearFlag(Flags::Zero);

	setFlag(Flags::Subtract);

	return a - b;
}

unsigned short Cpu::addShorts(unsigned short v1, unsigned short v2) {
	if (((v1 & 0xfff) + (v2 & 0xfff)) & 0x1000)
		setFlag(Flags::HalfCarry);
	else
		clearFlag(Flags::HalfCarry);
	if ((v1 + v2) & 0x10000)
		setFlag(Flags::Carry);
	else
		clearFlag(Flags::Carry);

	clearFlag(Flags::Subtract);
	return v1 + v2;
}

unsigned char Cpu::addCarry(unsigned char b) {
	if (checkFlag(Flags::Carry))
		b++;
	return addBytes(registers.a, b);
}

unsigned char Cpu::subCarry(unsigned char b) {
	if (checkFlag(Flags::Carry))
		b++;
	return subBytes(registers.a, b);
}

void Cpu::enableInterrupts() {

}

void Cpu::disableInterrupts() {

}

void Cpu::printRegisters() {
	system("cls");
	std::cout << std::hex << "A: 0x" << (unsigned short)registers.a << std::endl;
	std::cout << std::hex << "B: 0x" << (unsigned short)registers.b << std::endl;
	std::cout << std::hex << "C: 0x" << (unsigned short)registers.c << std::endl;
	std::cout << std::hex << "D: 0x" << (unsigned short)registers.d << std::endl;
	std::cout << std::hex << "E: 0x" << (unsigned short)registers.e << std::endl;
	std::cout << std::hex << "F: 0x" << (unsigned short)registers.f << std::endl << "  Z:" << (bool)(registers.f & Flags::Zero) << " S:" << (bool)(registers.f & Flags::Subtract) << " H:" << (bool)(registers.f & Flags::HalfCarry) << " C:" << (bool)(registers.f & Flags::Carry) << std::endl;
	std::cout << std::hex << "H: 0x" << (unsigned short)registers.h << std::endl;
	std::cout << std::hex << "L: 0x" << (unsigned short)registers.l << std::endl;
	std::cout << std::hex << "AF: 0x" << registers.af << std::endl;
	std::cout << std::hex << "BC: 0x" << registers.bc << std::endl;
	std::cout << std::hex << "DE: 0x" << registers.de << std::endl;
	std::cout << std::hex << "HL: 0x" << registers.hl << std::endl;
	std::cout << std::hex << "SP: 0x" << registers.sp << std::endl;
	std::cout << std::hex << "PC: 0x" << registers.pc << std::endl;
}

void Cpu::generateInstructions() {
	//Name string, number of parameters(in bytes, not counting opcode byte), number of cycles, function
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
}

//NULL NOT IMPLEMENTED OPCODE
void Cpu::null(std::vector<unsigned char> parms) {

}

//0x00 Do nothing
void Cpu::nop(std::vector<unsigned char> parms) {
	std::cout << "NOP" << std::endl;
	clock.m = 1;
	clock.t = 4;
}

//0x01 Store 2 byte value, nn, into the bc register
void Cpu::ld_bc_nn(std::vector<unsigned char> parms) {
	if (parms.size() != 2)
		std::cout << "0x01:LD_BC_NN invalid parameter amount";
	else {
		unsigned short t = (parms[0] << 8) | parms[1];
		registers.bc = t;
		clock.m = 3;
		clock.t = 12;
	}
}

//0x02 Store the value in register a, into the address pointed to by register bc
void Cpu::ld_bcp_a(std::vector<unsigned char> parms) {
	memory.writeByte(registers.bc, registers.a);
	clock.m = 1;
	clock.t = 8;
}

//0x03 Increment the bc Register, does NOT affect flags
void Cpu::inc_bc(std::vector<unsigned char> parms) {
	registers.bc++;
	clock.m = 1;
	clock.t = 4;
}

//0x04 Increment the b register, affects flags
void Cpu::inc_b(std::vector<unsigned char> parms) {
	registers.b = inc(registers.b);
	clock.m = 1;
	clock.t = 4;
}

//0x05 Decrement the b register, affects flags
void Cpu::dec_b(std::vector<unsigned char> parms) {
	registers.b = dec(registers.b);
	clock.m = 1;
	clock.t = 4;
}

//0x06 Store byte value, into b register
void Cpu::ld_b_n(std::vector<unsigned char> parms) {
	registers.b = parms[0];
	clock.m = 2;
	clock.t = 8;
}

//0x07 Rotate register a n bits left, old bit 7 to Carry flag
void Cpu::rlca(std::vector<unsigned char> parms) {
	clearAllFlags();

	unsigned char carry = (registers.a >> 7) & 0x1;
	if (carry)
		setFlag(Flags::Carry);

	registers.a <<= 1;
	registers.a += carry;

	//TODO:Zero flag might not be set, not sure
	if (registers.a)
		setFlag(Flags::Zero);

	clock.m = 1;
	clock.t = 4;
}

//0x08 Store sp into address at nn, nn = a16
void Cpu::ld_nnp_sp(std::vector<unsigned char> parms) {
	unsigned short address = parms[0] << 8 | parms[1];
	memory.writeShort(address, registers.sp);

	clock.m = 3;
	clock.t = 20;
}

//0x09 add bc into hl
void Cpu::add_hl_bc(std::vector<unsigned char> parms) {
	clearFlag(Flags::Subtract);
	registers.hl = addShorts(registers.hl, registers.bc);

	clock.m = 1;
	clock.t = 8;
}

//0x0A Store memory at address bc into a
void Cpu::ld_a_bcp(std::vector<unsigned char> parms) {
	registers.a = memory.readByte(registers.bc);

	clock.m = 1;
	clock.t = 8;
}

//0x0B Decrement register bc
void Cpu::dec_bc(std::vector<unsigned char> parms) {
	registers.bc--;

	clock.m = 1;
	clock.t = 8;
}

//0x0C Increment register c
void Cpu::inc_c(std::vector<unsigned char> parms) {
	registers.c = inc(registers.c);

	clock.m = 1;
	clock.t = 4;
}

//0x0D Decrement register c
void Cpu::dec_c(std::vector<unsigned char> parms) {
	registers.c = dec(registers.c);

	clock.m = 1;
	clock.t = 4;
}

//0x0E Store byte value into c register
void Cpu::ld_c_n(std::vector<unsigned char> parms) {
	registers.c = parms[0];

	clock.m = 2;
	clock.t = 8;
}

//0x0F Rotate A right, set old bit 0 to carry flag
void Cpu::rrca(std::vector<unsigned char> parms) {
	unsigned char carry = registers.a & 0x01;
	if (carry)
		setFlag(Flags::Carry);
	else
		clearFlag(Flags::Carry);

	registers.a >>= 1;

	if (carry)
		registers.a |= 0x80;

	if (registers.a)
		clearFlag(Flags::Zero);
	else
		setFlag(Flags::Zero);

	clearFlag(Flags::HalfCarry);
	clearFlag(Flags::Subtract);

	clock.m = 1;
	clock.t = 4;
}

//0x10 Stop, hald cpu and lcd display until button pressed
void Cpu::stop(std::vector<unsigned char> parms) {
	stopped = true;

	clock.m = 2;
	clock.t = 4;
}

//0x11 Store short into register de
void Cpu::ld_de_nn(std::vector<unsigned char> parms) {
	unsigned short a = (parms[0] << 8) | parms[1];
	registers.de = a;
	
	clock.m = 3;
	clock.t = 12;
}

//0x12 Store register a into memory location de
void Cpu::ld_dep_a(std::vector<unsigned char> parms) {
	memory.writeByte(registers.de, registers.a);

	clock.m = 1;
	clock.t = 8;
}

//0x13 increment de register
void Cpu::inc_de(std::vector<unsigned char> parms) {
	registers.de++;

	clock.m = 1;
	clock.t = 8;
}

//0x14 increment d register, Flags(Z,0,H)
void Cpu::inc_d(std::vector<unsigned char> parms) {
	registers.d = inc(registers.d);

	clock.m = 1;
	clock.t = 4;
}

//0x15 decrement d register, Flags(Z,1,H,-)
void Cpu::dec_d(std::vector<unsigned char> parms) {
	registers.d = dec(registers.d);

	clock.m = 1;
	clock.t = 4;
}

//0x16 load byte into register d, Flags(-,-,-,-)
void Cpu::ld_d_n(std::vector<unsigned char> parms) {
	registers.d = parms[0];

	clock.m = 2;
	clock.t = 8;
}

//0x17 Rotate A left through carry flag, new bit 0 = old carry flag, Flags(0,0,0,C)
void Cpu::rla(std::vector<unsigned char> parms) {
	unsigned char carry = checkFlag(Flags::Carry);
	clearAllFlags();

	if (registers.a & 0x80)
		setFlag(Flags::Carry);
	registers.a = registers.a << 1;
	registers.a += carry;

	clock.m = 1;
	clock.t = 4;
}

//0x18 Jump to current address + n
void Cpu::jr_n(std::vector<unsigned char> parms) {
	registers.pc += (signed char)parms[0];

}

//0x19 Add de register to hl register, Flags(-,0,H,C)
void Cpu::add_hl_de(std::vector<unsigned char> parms) {
	clearFlag(Flags::Subtract);

	registers.hl = addShorts(registers.hl, registers.de);

	clock.m = 1;
	clock.t = 8;
}

//0x1A Store byte at address (de) into register a, Flags(-,-,-,-)
void Cpu::ld_a_dep(std::vector<unsigned char> parms) {
	registers.a = memory.readByte(registers.de);

	clock.m = 1;
	clock.t = 8;
}

//0x1B Decrement register de, Flags(-,-,-,-)
void Cpu::dec_de(std::vector<unsigned char> parms) {
	registers.de--;

	clock.m = 1;
	clock.t = 8;
}

//0x1C Increment register e, Flags(Z,0,H,-)
void Cpu::inc_e(std::vector<unsigned char> parms) {
	registers.e = inc(registers.e);

	clock.m = 1;
	clock.t = 4;
}

//0x1D Decrement register e, Flags(Z,1,H,-)
void Cpu::dec_e(std::vector<unsigned char> parms) {
	registers.e = dec(registers.e);

	clock.m = 1;
	clock.t = 4;
}

//0x1E Store byte n into register e, Flags(-,-,-,-)
void Cpu::ld_e_n(std::vector<unsigned char> parms) {
	registers.e = parms[0];

	clock.m = 1;
	clock.t = 8;
}

//0x1F Rotate register a right through carry flag, Flags(0,0,0,C)
void Cpu::rra(std::vector<unsigned char> parms) {
	unsigned char carry = checkFlag(Flags::Carry);
	clearAllFlags();

	if (registers.a & 0x01)
		setFlag(Flags::Carry);
	registers.a >>= 1;

	registers.a += (carry << 7);

	clock.m = 1;
	clock.t = 4;
}

//0x20 Jump to address sp + (signed)n if zero flag is not set, Flags(-,-,-,-)
void Cpu::jr_nz_n(std::vector<unsigned char> parms) {
	signed char addressOffset = (signed char)parms[0];
	if (!checkFlag(Flags::Zero)) {
		registers.pc += addressOffset;
		clock.t = 12;
	}
	else
		clock.t = 8;
	clock.m = 2;
}

//0x21 Store unsigned short into register hl
void Cpu::ld_hl_nn(std::vector<unsigned char> parms) {
	unsigned short value = (parms[0] << 8) + parms[1];
	registers.hl = value;

	clock.m = 3;
	clock.t = 12;
}

//0x22 Store register a into memory at hl, then increment HL, Flags(-,-,-,-)
void Cpu::ldi_hl_a(std::vector<unsigned char> parms) {
	memory.writeByte(registers.hl, registers.a);
	registers.hl++;

	clock.m = 1;
	clock.t = 8;
}

//0x23 Increment the hl register, Flags(-,-,-,-)
void Cpu::inc_hl(std::vector<unsigned char> parms) {
	registers.hl++;

	clock.m = 1;
	clock.t = 8;
}

//0x24 Increment the h register, Flags(Z,0,H,-)
void Cpu::inc_h(std::vector<unsigned char> parms) {
	registers.h = inc(registers.h);

	clock.m = 1;
	clock.t = 4;
}

//0x25 Decrement the h register, Flags(Z,1,H,-)
void Cpu::dec_h(std::vector<unsigned char> parms) {
	registers.h = dec(registers.h);

	clock.m = 1;
	clock.t = 4;
}

//0x26 Store byte n into register h
void Cpu::ld_h_n(std::vector<unsigned char> parms) {
	registers.h = parms[0];

	clock.m = 2;
	clock.t = 8;
}

//0x27 Decimal Adjust register a, Flags(Z,-,0,C)
void Cpu::daa(std::vector<unsigned char> parms) {
	unsigned short s = registers.a;
	
	if (checkFlag(Flags::Subtract)) {
		if (checkFlag(Flags::HalfCarry)) s = (s - 0x06) & 0xFF;
		if (checkFlag(Flags::Carry)) s -= 0x60;
	}
	else {
		if (checkFlag(Flags::HalfCarry) || (s & 0xF) > 9) s += 0x06;
		if (checkFlag(Flags::Carry) || s > 0x9F) s += 0x60;
	}

	registers.a = (unsigned char)s;
	clearFlag(Flags::HalfCarry);

	if (registers.a)
		clearFlag(Flags::Zero);
	else
		setFlag(Flags::Zero);

	if (s >= 0x100)
		setFlag(Flags::Carry);
}

//0x28 Jump to address sp + (signed char)n if zero flag is set, Flags(-,-,-,-)
void Cpu::jr_z_n(std::vector<unsigned char> parms) {
	signed char addressOffset = (signed char)parms[0];
	if (checkFlag(Flags::Zero)) {
		registers.pc += addressOffset;
		clock.t = 12;
	}
	else
		clock.t = 8;
	clock.m = 2;
}

//0x29 Add register hl to register hl, Flags(-,0,H,C)
void Cpu::add_hl_hl(std::vector<unsigned char> parms) {
	registers.hl = addShorts(registers.hl, registers.hl);

	clock.m = 1;
	clock.t = 8;
}

//0x2A Set register a to memory location hl, then increment hl, Flags(-,-,-,-)
void Cpu::ld_a_hlpi(std::vector<unsigned char> parms) {
	registers.a = memory.readByte(registers.hl);
	registers.hl++;

	clock.m = 1;
	clock.t = 8;
}

//0x2B Decrement hl register, Flags(-,-,-,-)
void Cpu::dec_hl(std::vector<unsigned char> parms) {
	registers.hl--;

	clock.m = 1;
	clock.t = 8;
}

//0x2C Increment l register, Flags(Z,0,H,-)
void Cpu::inc_l(std::vector<unsigned char> parms) {
	registers.l = inc(registers.l);

	clock.m = 1;
	clock.t = 4;
}

//0x2D Decrement l register, Flags(Z,1,H,-)
void Cpu::dec_l(std::vector<unsigned char> parms) {
	registers.l = dec(registers.l);

	clock.m = 1;
	clock.t = 4;
}

//0x2E Store byte n into register l, Flags(-,-,-,-)
void Cpu::ld_l_n(std::vector<unsigned char> parms) {
	registers.l = parms[0];

	clock.m = 2;
	clock.t = 8;
}

//0x2F Complement the a register, Flags(-,1,1,-)
void Cpu::cpl(std::vector<unsigned char> parms) {
	registers.a = ~registers.a;
	
	setFlag(Flags::Subtract);
	setFlag(Flags::HalfCarry);

	clock.m = 1;
	clock.t = 4;
}

//0x30 Jump relative if carry flag not set, n = signed char, Flags(-,-,-,-)
void Cpu::jr_nc_n(std::vector<unsigned char> parms) {
	signed char addressOffset = (signed char)parms[0];
	if (checkFlag(Flags::Carry)) {
		registers.pc += addressOffset;
		clock.t = 12;
	}
	else
		clock.t = 8;

	clock.m = 2;
}

//0x31 Store unsigned short nn into register sp, Flags(-,-,-,-)
void Cpu::ld_sp_nn(std::vector<unsigned char> parms) {
	unsigned short value = (parms[0] << 8) | parms[1];
	registers.sp = value;

	clock.m = 3;
	clock.t = 12;
}

//0x32 Load a into memory at hl, then decrement hl, Flags(-,-,-,-)
void Cpu::ldd_hlp_a(std::vector<unsigned char> parms) {
	memory.writeByte(registers.hl, registers.a);
	registers.hl--;

	clock.m = 1;
	clock.t = 8;
}

//0x33 Increment sp, Flags(-,-,-,-)
void Cpu::inc_sp(std::vector<unsigned char> parms) {
	registers.sp++;

	clock.m = 1;
	clock.t = 8;
}

//0x34 Get Value at memory hl, then increment it, Flags(Z,0,H,-)
void Cpu::inc_hlp(std::vector<unsigned char> parms) {
	unsigned char value = memory.readByte(registers.hl);
	memory.writeByte(registers.hl, inc(value));

	clock.m = 1;
	clock.t = 12;
}

//0x35 Get Value at memory hl, then decrement it, Flags(Z,1,H,-)
void Cpu::dec_hlp(std::vector<unsigned char> parms) {
	unsigned char value = memory.readByte(registers.hl);
	memory.writeByte(registers.hl, dec(value));

	clock.m = 1;
	clock.t = 12;
}

//0x36 Set memory at hl equal to unsigned byte n, Flags(-,-,-,-)
void Cpu::ld_hlp_n(std::vector<unsigned char> parms) {
	memory.writeByte(registers.hl, parms[0]);

	clock.m = 2;
	clock.t = 12;
}

//0x37 Set Carry Flag, Flags(-,0,0,1)
void Cpu::scf(std::vector<unsigned char> parms) {
	setFlag(Flags::Carry);
	clearFlag(Flags::Subtract);
	clearFlag(Flags::HalfCarry);

	clock.m = 1;
	clock.t = 4;
}

//0x38 Jump relative by n, if carry flag is set, Flags(-,-,-,-)
void Cpu::jr_c_n(std::vector<unsigned char> parms) {
	signed char addressOffset = (signed char)parms[0];

	if (checkFlag(Flags::Carry)) {
		registers.pc += addressOffset;
		clock.t = 12;
	}
	else
		clock.t = 8;
	clock.m = 2;
}

//0x39 Add sp to hl register, Flags(-,0,H,C)
void Cpu::add_hl_sp(std::vector<unsigned char> parms) {
	registers.hl = addShorts(registers.hl, registers.sp);

	clock.m = 1;
	clock.t = 8;
}

//0x3A Load memory at hl into register a, then decrement hl, Flags(-,-,-,-)
void Cpu::ld_a_hlpd(std::vector<unsigned char> parms) {
	registers.a = memory.readByte(registers.hl);
	registers.hl--;

	clock.m = 1;
	clock.t = 8;
}

//0x3B Decrement sp register, Flags(-,-,-,-)
void Cpu::dec_sp(std::vector<unsigned char> parms) {
	registers.sp--;

	clock.m = 1;
	clock.t = 8;
}

//0x3C Increment a register, Flags(Z,0,H,-)
void Cpu::inc_a(std::vector<unsigned char> parms) {
	registers.a = inc(registers.a);

	clock.m = 1;
	clock.t = 4;
}

//0x3D Decrement a register, Flags(Z,1,H,-)
void Cpu::dec_a(std::vector<unsigned char> parms) {
	registers.a = dec(registers.a);

	clock.m = 1;
	clock.t = 4;
}

//0x3E Store unsigned byte n into register a, Flags(-,-,-,-)
void Cpu::ld_a_n(std::vector<unsigned char> parms) {
	registers.a = parms[0];

	clock.m = 2;
	clock.t = 8;
}

//0x3F Complement Carry Flag, Flags(-,0,0,C)
void Cpu::ccf(std::vector<unsigned char> parms) {
	if (checkFlag(Flags::Carry))
		clearFlag(Flags::Carry);
	else
		setFlag(Flags::Carry);

	clearFlag(Flags::Subtract);
	clearFlag(Flags::HalfCarry);

	clock.m = 1;
	clock.t = 4;
}

//0x40 Store register b into register b, Flags(-,-,-,-)
void Cpu::ld_b_b(std::vector<unsigned char> parms) {
	registers.b = registers.b;

	clock.m = 1;
	clock.t = 4;
}

//0x41 Store register c into register b, Flags(-,-,-,-)
void Cpu::ld_b_c(std::vector<unsigned char> parms) {
	registers.b = registers.c;

	clock.m = 1;
	clock.t = 4;
}

//0x42 Store register d into register b, Flags(-,-,-,-)
void Cpu::ld_b_d(std::vector<unsigned char> parms) {
	registers.b = registers.d;

	clock.m = 1;
	clock.t = 4;
}

//0x43 Store register e into register b, Flags(-,-,-,-)
void Cpu::ld_b_e(std::vector<unsigned char> parms) {
	registers.b = registers.e;

	clock.m = 1;
	clock.t = 4;
}

//0x44 Store register h into register b, Flags(-,-,-,-)
void Cpu::ld_b_h(std::vector<unsigned char> parms) {
	registers.b = registers.h;

	clock.m = 1;
	clock.t = 4;
}

//0x45 Store register l into register b, Flags(-,-,-,-)
void Cpu::ld_b_l(std::vector<unsigned char> parms) {
	registers.b = registers.l;

	clock.m = 1;
	clock.t = 4;
}

//0x46 Store memory value hl into register b, Flags(-,-,-,-)
void Cpu::ld_b_hlp(std::vector<unsigned char> parms) {
	registers.b = memory.readByte(registers.hl);

	clock.m = 1;
	clock.t = 8;
}

//0x47 Store register a into register b, Flags(-,-,-,-)
void Cpu::ld_b_a(std::vector<unsigned char> parms) {
	registers.b = registers.a;

	clock.m = 1;
	clock.t = 4;
}

//0x48 Store register b into register c, Flags(-,-,-,-)
void Cpu::ld_c_b(std::vector<unsigned char> parms) {
	registers.c = registers.b;

	clock.m = 1;
	clock.t = 4;
}

//0x49 Store register c into register c, Flags(-,-,-,-)
void Cpu::ld_c_c(std::vector<unsigned char> parms) {
	registers.c = registers.c;

	clock.m = 1;
	clock.t = 4;
}

//0x4A Store register d into register c, Flags(-,-,-,-)
void Cpu::ld_c_d(std::vector<unsigned char> parms) {
	registers.c = registers.d;

	clock.m = 1;
	clock.t = 4;
}

//0x4B Store register e into register c, Flags(-,-,-,-)//0x4A
void Cpu::ld_c_e(std::vector<unsigned char> parms) {
	registers.c = registers.e;

	clock.m = 1;
	clock.t = 4;
}

//0x4C Store register h into register c, Flags(-,-,-,-)
void Cpu::ld_c_h(std::vector<unsigned char> parms) {
	registers.c = registers.h;

	clock.m = 1;
	clock.t = 4;
}

//0x4D Store register l into register c, Flags(-,-,-,-)
void Cpu::ld_c_l(std::vector<unsigned char> parms) {
	registers.c = registers.l;

	clock.m = 1;
	clock.t = 4;
}

//0x4E Store memory location hl into register b, Flags(-,-,-,-)
void Cpu::ld_c_hlp(std::vector<unsigned char> parms) {
	registers.c = memory.readByte(registers.hl);

	clock.m = 1;
	clock.t = 8;
}

//0x4F Store register a into register c, Flags(-,-,-,-)
void Cpu::ld_c_a(std::vector<unsigned char> parms) {
	registers.c = registers.a;

	clock.m = 1;
	clock.t = 4;
}

//0x50 Store register b into register d, Flags(-,-,-,-)
void Cpu::ld_d_b(std::vector<unsigned char> parms) {
	registers.d = registers.b;

	clock.m = 1;
	clock.t = 4;
}

//0x51 Store register c into register d, Flags(-,-,-,-)
void Cpu::ld_d_c(std::vector<unsigned char> parms) {
	registers.d = registers.c;

	clock.m = 1;
	clock.t = 4;
}

//0x52 Store register d into register d, Flags(-,-,-,-)
void Cpu::ld_d_d(std::vector<unsigned char> parms) {
	registers.d = registers.d;

	clock.m = 1;
	clock.t = 4;
}

//0x53 Store register e into register d, Flags(-,-,-,-)
void Cpu::ld_d_e(std::vector<unsigned char> parms) {
	registers.d = registers.e;

	clock.m = 1;
	clock.t = 4;
}

//0x54 Store register h into register d, Flags(-,-,-,-)
void Cpu::ld_d_h(std::vector<unsigned char> parms) {
	registers.d = registers.h;

	clock.m = 1;
	clock.t = 4;
}

//0x55 Store register l into register d, Flags(-,-,-,-)
void Cpu::ld_d_l(std::vector<unsigned char> parms) {
	registers.d = registers.l;

	clock.m = 1;
	clock.t = 4;
}

//0x56 Store memory value hl into register d, Flags(-,-,-,-)
void Cpu::ld_d_hlp(std::vector<unsigned char> parms) {
	registers.d = memory.readByte(registers.hl);

	clock.m = 1;
	clock.t = 8;
}

//0x57 Store register a into register d, Flags(-,-,-,-)
void Cpu::ld_d_a(std::vector<unsigned char> parms) {
	registers.d = registers.a;

	clock.m = 1;
	clock.t = 4;
}

//0x58 Store register b into register e, Flags(-,-,-,-)
void Cpu::ld_e_b(std::vector<unsigned char> parms) {
	registers.e = registers.b;

	clock.m = 1;
	clock.t = 4;
}

//0x59 Store register c into register e, Flags(-,-,-,-)
void Cpu::ld_e_c(std::vector<unsigned char> parms) {
	registers.e = registers.c;

	clock.m = 1;
	clock.t = 4;
}

//0x5A Store register d into register e, Flags(-,-,-,-)
void Cpu::ld_e_d(std::vector<unsigned char> parms) {
	registers.e = registers.d;

	clock.m = 1;
	clock.t = 4;
}

//0x5B Store register e into register e, Flags(-,-,-,-)//0x4A
void Cpu::ld_e_e(std::vector<unsigned char> parms) {
	registers.e = registers.e;

	clock.m = 1;
	clock.t = 4;
}

//0x5C Store register h into register e, Flags(-,-,-,-)
void Cpu::ld_e_h(std::vector<unsigned char> parms) {
	registers.e = registers.h;

	clock.m = 1;
	clock.t = 4;
}

//0x5D Store register l into register e, Flags(-,-,-,-)
void Cpu::ld_e_l(std::vector<unsigned char> parms) {
	registers.e = registers.l;

	clock.m = 1;
	clock.t = 4;
}

//0x5E Store memory value hl into register e, Flags(-,-,-,-)
void Cpu::ld_e_hlp(std::vector<unsigned char> parms) {
	registers.e = memory.readByte(registers.hl);

	clock.m = 1;
	clock.t = 8;
}

//0x5F Store register a into register e, Flags(-,-,-,-)
void Cpu::ld_e_a(std::vector<unsigned char> parms) {
	registers.e = registers.a;

	clock.m = 1;
	clock.t = 4;
}

//0x60 Store register b into register h, Flags(-,-,-,-)
void Cpu::ld_h_b(std::vector<unsigned char> parms) {
	registers.h = registers.b;

	clock.m = 1;
	clock.t = 4;
}

//0x61 Store register c into register h, Flags(-,-,-,-)//0x60
void Cpu::ld_h_c(std::vector<unsigned char> parms) {
	registers.h = registers.c;

	clock.m = 1;
	clock.t = 4;
}

//0x62 Store register d into register h, Flags(-,-,-,-)
void Cpu::ld_h_d(std::vector<unsigned char> parms) {
	registers.h = registers.d;

	clock.m = 1;
	clock.t = 4;
}

//0x63 Store register e into register h, Flags(-,-,-,-)
void Cpu::ld_h_e(std::vector<unsigned char> parms) {
	registers.h = registers.e;

	clock.m = 1;
	clock.t = 4;
}

//0x64 Store register h into register h, Flags(-,-,-,-)
void Cpu::ld_h_h(std::vector<unsigned char> parms) {
	registers.h = registers.h;

	clock.m = 1;
	clock.t = 4;
}

//0x65 Store register l into register h, Flags(-,-,-,-)
void Cpu::ld_h_l(std::vector<unsigned char> parms) {
	registers.h = registers.l;

	clock.m = 1;
	clock.t = 4;
}

//0x66 Store memory value at hl into register h, Flags(-,-,-,-)
void Cpu::ld_h_hlp(std::vector<unsigned char> parms) {
	registers.h = memory.readByte(registers.hl);

	clock.m = 1;
	clock.t = 4;
}

//0x67 Store register b into register h, Flags(-,-,-,-)
void Cpu::ld_h_a(std::vector<unsigned char> parms) {
	registers.h = registers.a;

	clock.m = 1;
	clock.t = 4;
}

//0x68 Store register b into register l, Flags(-,-,-,-)
void Cpu::ld_l_b(std::vector<unsigned char> parms) {
	registers.l = registers.b;

	clock.m = 1;
	clock.t = 4;
}

//0x69 Store register c into register l, Flags(-,-,-,-)
void Cpu::ld_l_c(std::vector<unsigned char> parms){
	registers.l = registers.c;

	clock.m = 1;
	clock.t = 4;
}

//0x6A Store register d into register l, Flags(-,-,-,-)
void Cpu::ld_l_d(std::vector<unsigned char> parms) {
	registers.l = registers.d;

	clock.m = 1;
	clock.t = 4;
}

//0x6B Store register e into register l, Flags(-,-,-,-)
void Cpu::ld_l_e(std::vector<unsigned char> parms) {
	registers.l = registers.e;

	clock.m = 1;
	clock.t = 4;
}

//0x6C Store register h into register l, Flags(-,-,-,-)
void Cpu::ld_l_h(std::vector<unsigned char> parms) {
	registers.l = registers.h;

	clock.m = 1;
	clock.t = 4;
}

//0x6D Store register l into register l, Flags(-,-,-,-)
void Cpu::ld_l_l(std::vector<unsigned char> parms) {
	registers.l = registers.l;

	clock.m = 1;
	clock.t = 4;
}

//0x6E Store memory value at hl into register l, Flags(-,-,-,-)
void Cpu::ld_l_hlp(std::vector<unsigned char> parms) {
	registers.l = memory.readByte(registers.hl);

	clock.m = 1;
	clock.t = 4;
}

//0x6F Store register a into register b, Flags(-,-,-,-)
void Cpu::ld_l_a(std::vector<unsigned char> parms) {
	registers.l = registers.a;

	clock.m = 1;
	clock.t = 4;
}

//0x70 Store register b into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_b(std::vector<unsigned char> parms) {
	memory.writeByte(registers.hl, registers.b);

	clock.m = 1;
	clock.t = 8;
}

//0x71 Store register c into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_c(std::vector<unsigned char> parms) {
	memory.writeByte(registers.hl, registers.c);

	clock.m = 1;
	clock.t = 8;
}

//0x72 Store register d into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_d(std::vector<unsigned char> parms) {
	memory.writeByte(registers.hl, registers.d);

	clock.m = 1;
	clock.t = 8;
}

//0x73 Store register e into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_e(std::vector<unsigned char> parms) {
	memory.writeByte(registers.hl, registers.e);

	clock.m = 1;
	clock.t = 8;
}

//0x74 Store register h into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_h(std::vector<unsigned char> parms) {
	memory.writeByte(registers.hl, registers.h);

	clock.m = 1;
	clock.t = 8;
}

//0x75 Store register l into register b, Flags(-,-,-,-)
void Cpu::ld_hlp_l(std::vector<unsigned char> parms) {
	memory.writeByte(registers.hl, registers.l);

	clock.m = 1;
	clock.t = 8;
}

//0x76 Halt, Power down CPU until interrupt occurs
void Cpu::halt(std::vector<unsigned char> parms) {
	std::cout << "ERROR, 0x76 HALT not implemented";

	clock.m = 1;
	clock.t = 4;
}

//0x77 Store register a into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_a(std::vector<unsigned char> parms) {
	memory.writeByte(registers.hl, registers.a);

	clock.m = 1;
	clock.t = 8;
}

//0x78 Store register b into register a, Flags(-,-,-,-)
void Cpu::ld_a_b(std::vector<unsigned char> parms) {
	registers.a = registers.b;

	clock.m = 1;
	clock.t = 4;
}

//0x79 Store register c into register a, Flags(-,-,-,-)
void Cpu::ld_a_c(std::vector<unsigned char> parms) {
	registers.a = registers.c;

	clock.m = 1;
	clock.t = 4;
}

//0x7A Store register d into register a, Flags(-,-,-,-)
void Cpu::ld_a_d(std::vector<unsigned char> parms) {
	registers.a = registers.d;

	clock.m = 1;
	clock.t = 4;
}

//0x7B Store register e into register a, Flags(-,-,-,-)
void Cpu::ld_a_e(std::vector<unsigned char> parms) {
	registers.a = registers.e;

	clock.m = 1;
	clock.t = 4;
}

//0x7C Store register h into register a, Flags(-,-,-,-)
void Cpu::ld_a_h(std::vector<unsigned char> parms) {
	registers.a = registers.h;

	clock.m = 1;
	clock.t = 4;
}

//0x7D Store register l into register a, Flags(-,-,-,-)
void Cpu::ld_a_l(std::vector<unsigned char> parms) {
	registers.a = registers.l;

	clock.m = 1;
	clock.t = 4;
}

//0x7E Store memory value at hl into register a, Flags(-,-,-,-)
void Cpu::ld_a_hlp(std::vector<unsigned char> parms) {
	registers.a = memory.readByte(registers.hl);

	clock.m = 1;
	clock.t = 8;
}

//0x7F Store register a into register a, Flags(-,-,-,-)
void Cpu::ld_a_a(std::vector<unsigned char> parms) {
	registers.a = registers.a;

	clock.m = 1;
	clock.t = 4;
}

//0x80 Add register b into a, Flags(Z,0,H,C)
void Cpu::add_a_b(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, registers.b);

	clock.m = 1;
	clock.t = 4;
}

//0x81 Add register b into a, Flags(Z,0,H,C)
void Cpu::add_a_c(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, registers.c);

	clock.m = 1;
	clock.t = 4;
}

//0x82 Add register d into a, Flags(Z,0,H,C)
void Cpu::add_a_d(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, registers.d);

	clock.m = 1;
	clock.t = 4;
}

//0x83 Add register e into a, Flags(Z,0,H,C)
void Cpu::add_a_e(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, registers.e);

	clock.m = 1;
	clock.t = 4;
}

//0x84 Add register h into a, Flags(Z,0,H,C)
void Cpu::add_a_h(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, registers.h);

	clock.m = 1;
	clock.t = 4;
}

//0x85 Add register l into a, Flags(Z,0,H,C)
void Cpu::add_a_l(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, registers.l);

	clock.m = 1;
	clock.t = 4;
}

//0x86 Add memory value hl into a, Flags(Z,0,H,C)
void Cpu::add_a_hlp(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, memory.readByte(registers.hl));

	clock.m = 1;
	clock.t = 8;
}

//0x87 Add register a into a, Flags(Z,0,H,C)
void Cpu::add_a_a(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, registers.a);

	clock.m = 1;
	clock.t = 4;
}

//0x88 Add register c + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_b(std::vector<unsigned char> parms) {
	registers.a = addCarry(registers.b);

	clock.m = 1;
	clock.t = 4;
}

//0x89 Add register c + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_c(std::vector<unsigned char> parms) {
	registers.a = addCarry(registers.c);

	clock.m = 1;
	clock.t = 4;
}

//0x8A Add register d + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_d(std::vector<unsigned char> parms) {
	registers.a = addCarry(registers.d);

	clock.m = 1;
	clock.t = 4;
}

//0x8B Add register e + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_e(std::vector<unsigned char> parms) {
	registers.a = addCarry(registers.e);

	clock.m = 1;
	clock.t = 4;
}

//0x8C Add register h + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_h(std::vector<unsigned char> parms) {
	registers.a = addCarry(registers.h);

	clock.m = 1;
	clock.t = 4;
}

//0x8D Add register l + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_l(std::vector<unsigned char> parms) {
	registers.a = addCarry(registers.l);

	clock.m = 1;
	clock.t = 4;
}

//0x8E Add memory value at hl + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_hlp(std::vector<unsigned char> parms) {
	registers.a = addCarry(memory.readByte(registers.hl));

	clock.m = 1;
	clock.t = 8;
}

//0x8F Add register a + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_a(std::vector<unsigned char> parms) {
	registers.a = addCarry(registers.a);

	clock.m = 1;
	clock.t = 4;
}

//0x90 Subtract register b from/into register a, Flags(Z,1,H,C)
void Cpu::sub_b(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, registers.b);

	clock.m = 1;
	clock.t = 4;
}

//0x91 Subtract register c from/into register a, Flags(Z,1,H,C)
void Cpu::sub_c(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, registers.c);

	clock.m = 1;
	clock.t = 4;
}

//0x92 Subtract register d from/into register a, Flags(Z,1,H,C)
void Cpu::sub_d(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, registers.d);

	clock.m = 1;
	clock.t = 4;
}

//0x93 Subtract register e from/into register a, Flags(Z,1,H,C)
void Cpu::sub_e(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, registers.e);

	clock.m = 1;
	clock.t = 4;
}

//0x94 Subtract register h from/into register a, Flags(Z,1,H,C)
void Cpu::sub_h(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, registers.h);

	clock.m = 1;
	clock.t = 4;
}

//0x95 Subtract register l from/into register a, Flags(Z,1,H,C)
void Cpu::sub_l(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, registers.l);

	clock.m = 1;
	clock.t = 4;
}

//0x96 Subtract memory value at hl from/into register a, Flags(Z,1,H,C)
void Cpu::sub_hlp(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, memory.readByte(registers.hl));

	clock.m = 1;
	clock.t = 8;
}

//0x97 Subtract register b from/into register a, Flags(Z,1,H,C)
void Cpu::sub_a(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, registers.a);

	clock.m = 1;
	clock.t = 4;
}

//0x98 Subtract register b + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_b(std::vector<unsigned char> parms) {
	registers.a = subCarry(registers.b);

	clock.m = 1;
	clock.t = 4;
}

//0x99 Subtract register c + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_c(std::vector<unsigned char> parms) {
	registers.a = subCarry(registers.c);

	clock.m = 1;
	clock.t = 4;
}

//0x9A Subtract register d + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_d(std::vector<unsigned char> parms) {
	registers.a = subCarry(registers.d);

	clock.m = 1;
	clock.t = 4;
}

//0x9B Subtract register e + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_e(std::vector<unsigned char> parms) {
	registers.a = subCarry(registers.e);

	clock.m = 1;
	clock.t = 4;
}

//0x9C Subtract register h + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_h(std::vector<unsigned char> parms) {
	registers.a = subCarry(registers.h);

	clock.m = 1;
	clock.t = 4;
}

//0x9D Subtract register l + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_l(std::vector<unsigned char> parms) {
	registers.a = subCarry(registers.l);

	clock.m = 1;
	clock.t = 4;
}

//0x9E Subtract memory value at hl + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_hlp(std::vector<unsigned char> parms) {
	registers.a = subCarry(memory.readByte(registers.hl));

	clock.m = 1;
	clock.t = 8;
}

//0x9F Subtract register a + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_a(std::vector<unsigned char> parms) {
	registers.a = subCarry(registers.a);

	clock.m = 1;
	clock.t = 4;
}

//0xA0 AND register b with a into register a, Flags(Z,0,1,0)
void Cpu::and_b(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, registers.b);

	clock.m = 1;
	clock.t = 4;
}

//0xA1 AND register c with a into register a, Flags(Z,0,1,0)
void Cpu::and_c(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, registers.c);

	clock.m = 1;
	clock.t = 4;
}

//0xA2 AND register d with a into register a, Flags(Z,0,1,0)
void Cpu::and_d(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, registers.d);

	clock.m = 1;
	clock.t = 4;
}

//0xA3 AND register e with a into register a, Flags(Z,0,1,0)
void Cpu::and_e(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, registers.e);

	clock.m = 1;
	clock.t = 4;
}

//0xA4 AND register h with a into register a, Flags(Z,0,1,0)
void Cpu::and_h(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, registers.h);

	clock.m = 1;
	clock.t = 4;
}

//0xA5 AND register l with a into register a, Flags(Z,0,1,0)
void Cpu::and_l(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, registers.l);

	clock.m = 1;
	clock.t = 4;
}

//0xA6 AND memory value at hl with a into register a, Flags(Z,0,1,0)
void Cpu::and_hlp(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, memory.readByte(registers.hl));

	clock.m = 1;
	clock.t = 8;
}

//0xA7 AND register a with a into register a, Flags(Z,0,1,0)
void Cpu::and_a(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, registers.a);

	clock.m = 1;
	clock.t = 4;
}

//0xA8 XOR register b with a into register a, Flags(Z,0,0,0)
void Cpu::xor_b(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, registers.b);

	clock.m = 1;
	clock.t = 4;
}

//0xA9 XOR register c with a into register a, Flags(Z,0,0,0)
void Cpu::xor_c(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, registers.c);

	clock.m = 1;
	clock.t = 4;
}

//0xAA XOR register d with a into register a, Flags(Z,0,0,0)
void Cpu::xor_d(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, registers.d);

	clock.m = 1;
	clock.t = 4;
}

//0xAB XOR register e with a into register a, Flags(Z,0,0,0)
void Cpu::xor_e(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, registers.e);

	clock.m = 1;
	clock.t = 4;
}

//0xAC XOR register h with a into register a, Flags(Z,0,0,0)
void Cpu::xor_h(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, registers.h);

	clock.m = 1;
	clock.t = 4;
}

//0xAD XOR register l with a into register a, Flags(Z,0,0,0)
void Cpu::xor_l(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, registers.l);

	clock.m = 1;
	clock.t = 4;
}

//0xAE XOR memory value at hl with a into register a, Flags(Z,0,0,0)
void Cpu::xor_hlp(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, memory.readByte(registers.hl));

	clock.m = 1;
	clock.t = 8;
}

//0xAF XOR register a with a into register a, Flags(Z,0,0,0)
void Cpu::xor_a(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, registers.a);

	clock.m = 1;
	clock.t = 4;
}



//0xB0 OR register a with b into register a, Flags(Z,0,0,0)
void Cpu::or_b(std::vector<unsigned char> parms) {
	registers.a = or(registers.a, registers.b);

	clock.m = 1;
	clock.t = 4;
}

//0xB1 OR register a with c into register a, Flags(Z,0,0,0)
void Cpu::or_c(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, registers.c);

	clock.m = 1;
	clock.t = 4;
}

//0xB2 OR register a with d into register a, Flags(Z,0,0,0)
void Cpu::or_d(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, registers.d);

	clock.m = 1;
	clock.t = 4;
}

//0xB3 OR register a with e into register a, Flags(Z,0,0,0)
void Cpu::or_e(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, registers.e);

	clock.m = 1;
	clock.t = 4;
}

//0xB4 OR register a with h into register a, Flags(Z,0,0,0)
void Cpu::or_h(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, registers.h);

	clock.m = 1;
	clock.t = 4;
}

//0xB5 OR register a with l into register a, Flags(Z,0,0,0)
void Cpu::or_l(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, registers.l);

	clock.m = 1;
	clock.t = 4;
}

//0xB6 OR register a memory value at hl into register a, Flags(Z,0,0,0)
void Cpu::or_hlp(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, memory.readByte(registers.hl));

	clock.m = 1;
	clock.t = 8;
}

//0xB7 OR register a with a into register a, Flags(Z,0,0,0)
void Cpu::or_a(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, registers.a);

	clock.m = 1;
	clock.t = 4;
}

//0xB8 Compare register a with register b, Flags(Z,1,H,C)
void Cpu::cp_b(std::vector<unsigned char> parms) {
	compare(registers.b);

	clock.m = 1;
	clock.t = 4;
}

//0xB9 Compare register a with register c, Flags(Z,1,H,C)
void Cpu::cp_c(std::vector<unsigned char> parms) {
	compare(registers.c);

	clock.m = 1;
	clock.t = 4;
}

//0xBA Compare register a with register d, Flags(Z,1,H,C)
void Cpu::cp_d(std::vector<unsigned char> parms) {
	compare(registers.d);

	clock.m = 1;
	clock.t = 4;
}
//0xBB Compare register a with register e, Flags(Z,1,H,C)
void Cpu::cp_e(std::vector<unsigned char> parms) {
	compare(registers.e);

	clock.m = 1;
	clock.t = 4;
}

//0xBC Compare register a with register h, Flags(Z,1,H,C)
void Cpu::cp_h(std::vector<unsigned char> parms) {
	compare(registers.h);

	clock.m = 1;
	clock.t = 4;
}

//0xBD Compare register a with register l, Flags(Z,1,H,C)
void Cpu::cp_l(std::vector<unsigned char> parms) {
	compare(registers.l);

	clock.m = 1;
	clock.t = 4;
}

//0xBE Compare register a with memory value at hl, Flags(Z,1,H,C)
void Cpu::cp_hlp(std::vector<unsigned char> parms) {
	compare(memory.readByte(registers.hl));

	clock.m = 1;
	clock.t = 8;
}

//0xBF Compare register a with register a, Flags(Z,1,H,C)
void Cpu::cp_a(std::vector<unsigned char> parms) {
	compare(registers.a);

	clock.m = 1;
	clock.t = 4;
}


//0xC0 Return if zero flag is not set, Flags(-,-,-,-)
void Cpu::ret_nz(std::vector<unsigned char> parms) {
	if (!checkFlag(Flags::Zero)) {
		registers.pc = memory.readShortFromStack(&registers.sp);
		clock.t = 20;
	}
	else
		clock.t = 8;
	clock.m = 1;
}

//0xC1 Pop top of Stack into register bc, Flags(-,-,-,-)
void Cpu::pop_bc(std::vector<unsigned char> parms) {
	registers.bc = memory.readShortFromStack(&registers.sp);
	clock.m = 1;
	clock.t = 12;
}

//0xC2 Jump to address nn if not zero, Flags(-,-,-,-)
void Cpu::jp_nz_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[0] << 8) | parms[1];
	if (!checkFlag(Flags::Zero)) {
		registers.pc = address;
		clock.t = 16;
	}
	else
		clock.t = 12;
	clock.m = 3;
}

//0xC3 Jump to address nn, Flags(-,-,-,-)
void Cpu::jp_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[0] << 8) | parms[1];
	registers.pc = address;
	clock.t = 16;
	clock.m = 3;
}

//0xC4 Store pc on stack, jump to address if not zero, Flags(-,-,-,-)
void Cpu::call_nz_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[0] << 8) | parms[1];
	if (!checkFlag(Flags::Zero)) {
		memory.writeShortToStack(registers.pc, &registers.sp);
		registers.pc = address;
		clock.t = 24;
	}
	else
		clock.t = 12;
	clock.m = 3;
}

//0xC5 Push register bc onto stack Flags(-,-,-,-)
void Cpu::push_bc(std::vector<unsigned char> parms) {
	memory.writeShortToStack(registers.bc, &registers.sp);
	clock.t = 16;
	clock.m = 1;
}

//0xC6 Add value n into register a, Flags(Z,0,H,C)
void Cpu::add_a_n(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, parms[0]);

	clock.t = 8;
	clock.m = 2;
}

//0xC7 Restart, Push pc onto stack, jump to address 0x00 + n, Flags(-,-,-,-)
void Cpu::rst_0(std::vector<unsigned char> parms) {
	memory.writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x00;

	clock.t = 16;
	clock.m = 1;
}

//0xC8 Return to address off stack if zero flag is set, Flags(-,-,-,-)
void Cpu::ret_z(std::vector<unsigned char> parms) {
	if (checkFlag(Flags::Zero)) {
		registers.pc = memory.readShortFromStack(&registers.sp);
		clock.t = 20;
	}
	else
		clock.t = 8;
	clock.m = 1;
}

//0xC9 Return to address off stack, Flags(-,-,-,-)
void Cpu::ret(std::vector<unsigned char> parms) {
	registers.pc = memory.readShortFromStack(&registers.sp);
	
	clock.t = 20;
	clock.m = 1;

}

//0xCA Jump to address nn if zero flag is set, Flags(-,-,-,-)
void Cpu::jp_z_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[0] << 8) | parms[1];
	if (checkFlag(Flags::Zero)) {
		registers.pc = address;
		clock.t = 16;
	}
	else
		clock.t = 12;
	clock.m = 3;
}

//0xCB Access point to extended opcode table, Flags(-,-,-,-)
void Cpu::cb(std::vector<unsigned char> parms) {
	
	clock.t = 4;
	clock.m = 1;
}

//0xCC Store pc on stack, jump to address if zero flag set, Flags(-,-,-,-)
void Cpu::call_z_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[0] << 8) | parms[1];
	if (checkFlag(Flags::Zero)) {
		memory.writeShortToStack(registers.pc, &registers.sp);
		registers.pc = address;
		clock.t = 24;
	}
	else
		clock.t = 12;
	clock.m = 3;
}

//0xCD Store pc on stack, jump to address, Flags(-,-,-,-)
void Cpu::call_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[0] << 8) | parms[1];
	memory.writeShortToStack(registers.pc, &registers.sp);
	registers.pc = address;

	clock.t = 24;
	clock.m = 3;
}

//0xCE Carry add n into register a, Flags(Z,0,H,C)
void Cpu::adc_a_n(std::vector<unsigned char> parms) {
	registers.a = addCarry(parms[0]);

	clock.t = 8;
	clock.m = 2;
}

//0xCF Restart, Push pc onto stack, jump to address 0x08, Flags(-,-,-,-)
void Cpu::rst_08(std::vector<unsigned char> parms) {
	memory.writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x08;

	clock.t = 16;
	clock.m = 1;
}


//0xD0 Return to address on stack, if carry flag not set, Flags(-,-,-,-)
void Cpu::ret_nc(std::vector<unsigned char> parms) {
	if (!checkFlag(Flags::Carry)) {
		registers.pc = memory.readShortFromStack(&registers.sp);
		clock.t = 20;
	}
	else
		clock.t = 8;
	clock.m = 1;
}

//0xD1 Pop short off stack into register de, Flags(-,-,-,-)
void Cpu::pop_de(std::vector<unsigned char> parms) {
	registers.de = memory.readShortFromStack(&registers.sp);

	clock.t = 12;
	clock.m = 1;
}

//0xD2 Jump to address nn if carry flag not set, Flags(-,-,-,-)
void Cpu::jp_nc_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[0] << 7) | parms[1];
	if (!checkFlag(Flags::Carry)) {
		registers.pc = address;
		clock.t = 16;
	}
	else
		clock.t = 12;
	clock.m = 3;
}

//0xD4 If carry flag is not set, push pc onto stack and jump to address nn, Flags(-,-,-,-)
void Cpu::call_nc_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[0] << 7) | parms[1];
	if (!checkFlag(Flags::Carry)) {
		memory.writeShortToStack(registers.pc, &registers.sp);
		registers.pc = address;
		clock.t = 24;
	}
	else
		clock.t = 12;
	clock.m = 3;
}

//0xD5 Push register de onto the stack, Flags(-,-,-,-)
void Cpu::push_de(std::vector<unsigned char> parms) {
	memory.writeShortToStack(registers.de, &registers.sp);

	clock.t = 16;
	clock.m = 1;
}

//0xD6 Subtract value n from register a, Flags(Z,1,H,C)
void Cpu::sub_n(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, parms[0]);

	clock.t = 8;
	clock.m = 2;
}

//0xD7 Restart, push pc onto stack and jump to address 0x10, Flags(-,-,-,-)
void Cpu::rst_10(std::vector<unsigned char> parms) {
	memory.writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x10;
	clock.t = 16;
	clock.m = 1;
}

//0xD8 Return to address from stack if Carry flag set, Flags(-,-,-,-)
void Cpu::ret_c(std::vector<unsigned char> parms) {
	if (checkFlag(Flags::Carry)) {
		registers.pc = memory.readShortFromStack(&registers.sp);
		clock.t = 20;
	}
	else
		clock.t = 8;
	clock.m = 1;
}

//0xD9 Pop short off of stack, jump to that address, enable interrupts, Flags(-,-,-,-)
void Cpu::reti(std::vector<unsigned char> parms) {
	registers.pc = memory.readShortFromStack(&registers.sp);
	enableInterrupts();

	clock.t = 16;
	clock.m = 1;
}

//0xDA Jump to address nn if carry flag set, Flags(-,-,-,-)
void Cpu::jp_c_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[0] << 7) | parms[1];
	if (checkFlag(Flags::Carry)) {
		registers.pc = address;
		clock.t = 16;
	}
	else
		clock.t = 12;
	clock.m = 3;
}

//0xDC If carry flag is set, push pc onto stack and jump to address nn, Flags(-,-,-,-)
void Cpu::call_c_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[0] << 7) | parms[1];
	if (checkFlag(Flags::Carry)) {
		memory.writeShortToStack(registers.pc, &registers.sp);
		registers.pc = address;
		clock.t = 24;
	}
	else
		clock.t = 12;
	clock.m = 3;
}

//0xDE Sub Carry n from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_n(std::vector<unsigned char> parms) {
	registers.a = subCarry(parms[0]);

	clock.m = 1;
	clock.t = 4;
}

//0xDF Restart, push pc onto stack and jump to address 0x18, Flags(-,-,-,-)
void Cpu::rst_18(std::vector<unsigned char> parms) {
	memory.writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x18;
	clock.t = 16;
	clock.m = 1;
}

//0xE0 Put register a into memory address 0xFF00 + n, Flags(-,-,-,-)
void Cpu::ld_ffnp_a(std::vector<unsigned char> parms) {
	memory.writeByte(0xFF00 + parms[0], registers.a);

	clock.t = 12;
	clock.m = 2;
}

//0xE1 Pop short from stack into register hl, Flags(-,-,-,-)
void Cpu::pop_hl(std::vector<unsigned char> parms) {
	registers.hl = memory.readShortFromStack(&registers.sp);

	clock.t = 12;
	clock.m = 1;
}

//0xE2 Store register a into memory at address 0xFF00 + register c, Flags(-,-,-,-)
void Cpu::ld_cp_a(std::vector<unsigned char> parms) {
	memory.writeByte(0xFF00 + registers.c, registers.a);

	clock.t = 8;
	clock.m = 2;
}

//0xE5 Push register hl onto stack, Flags(-,-,-,-)
void Cpu::push_hl(std::vector<unsigned char> parms) {
	memory.writeShortToStack(registers.hl, &registers.sp);

	clock.t = 16;
	clock.m = 1;
}

//0xE6 AND register a with value n, Flags(Z,0,1,0)
void Cpu::and_n(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, parms[0]);

	clock.t = 8;
	clock.m = 2;
}

//0xE7 Restart, push pc onto stack and jump to address 0x20, Flags(-,-,-,-)
void Cpu::rst_20(std::vector<unsigned char> parms) {
	memory.writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x20;
	clock.t = 16;
	clock.m = 1;
}

//0xE8 Add signed byte value n into register sp, Flags(0,0,H,C)
void Cpu::add_sp_n(std::vector<unsigned char> parms) {
	char byte = (signed char)parms[0];
	
	if ((registers.sp & 0xF) + (byte & 0xF) >= 0x10)
		setFlag(Flags::HalfCarry);
	else
		clearFlag(Flags::HalfCarry);

	if (registers.sp + byte >= 0x10000)
		setFlag(Flags::Carry);
	else
		clearFlag(Flags::HalfCarry);

	clearFlag(Flags::Zero);
	clearFlag(Flags::Subtract);

	clock.t = 16;
	clock.m = 2;
}

//0xE9 Jump to address in memory pointed at hl, Flags(-,-,-,-)
void Cpu::jp_hlp(std::vector<unsigned char> parms) {
	registers.pc = memory.readShort(registers.hl);

	clock.t = 4;
	clock.m = 1;
}

//0xEA Store register a into memory address nn, Flags(-,-,-,-)
void Cpu::ld_nnp_a(std::vector<unsigned char> parms) {
	unsigned short address = (parms[0] << 8) | parms[1];
	memory.writeByte(address, registers.a);

	clock.t = 16;
	clock.m = 3;
}

//0xEE XOR value n into register a, Flags(Z,-,-,-)
void Cpu::xor_n(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, parms[0]);

	clock.t = 8;
	clock.m = 2;
}

//0xEF Restart, push pc onto stack and jump to address 0x28, Flags(-,-,-,-)
void Cpu::rst_28(std::vector<unsigned char> parms) {
	memory.writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x28;
	clock.t = 16;
	clock.m = 1;
}

//0xF0 Load memory location 0xFF00 + n into register a, Flags(-,-,-,-)//0xEF
void Cpu::ld_a_ffnp(std::vector<unsigned char> parms) {
	registers.a = memory.readByte(0xFF00 + parms[0]);

	clock.t = 8;
	clock.m = 2;
}

//0xF1 Pop short off stack into register af, Flags(Z,N,H,C)
void Cpu::pop_af(std::vector<unsigned char> parms) {
	registers.af = memory.readShortFromStack(&registers.sp);

	clock.t = 12;
	clock.m = 1;
}

//0xF2 Load memory location 0xFF00 + c into register a, Flags(-,-,-,-)
void Cpu::ld_a_cp(std::vector<unsigned char> parms) {
	registers.a = memory.readByte(0xFF00 + registers.c);

	clock.t = 8;
	clock.m = 2;
}

//0xF3 Disable interrupts, not immediate, after instruction after this is executed, Flags(-,-,-,-)
void Cpu::di(std::vector<unsigned char> parms) {
	disableInterrupts();

	clock.t = 4;
	clock.m = 1;
}

//0xF5 Push register af onto stack, Flags(-,-,-,-)
void Cpu::push_af(std::vector<unsigned char> parms) {
	memory.writeShortToStack(registers.af, &registers.sp);

	clock.t = 16;
	clock.m = 1;
}

//0xF6 OR value n into register a, Flags(Z,0,0,0)
void Cpu::or_n(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, parms[0]);

	clock.t = 8;
	clock.m = 2;
}

//0xF7 Restart, push pc onto stack and jump to address 0x30, Flags(-,-,-,-)
void Cpu::rst_30(std::vector<unsigned char> parms) {
	memory.writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x30;
	clock.t = 16;
	clock.m = 1;
}

//0xF8 Store register sp + (signed byte)n into register hl, Flags(0,0,H,C)
void Cpu::ld_hl_sp_n(std::vector<unsigned char> parms) {
	signed char value = (signed char)parms[0];
	clearAllFlags();
	if ((registers.sp & 0x0F) + (value & 0x0F) >= 0x10)
		setFlag(Flags::HalfCarry);
	if (registers.sp + value >= 0x10000)
		setFlag(Flags::Carry);

	clock.t = 12;
	clock.m = 2;
}

//0xF9 Store register hl into register sp, Flags(-,-,-,-)
void Cpu::ld_sp_hl(std::vector<unsigned char> parms) {
	registers.sp = registers.hl;

	clock.t = 8;
	clock.m = 1;
}

//0xFA Store byte at memory location nn into register a, Flags(-,-,-,-)
void Cpu::ld_a_nnp(std::vector<unsigned char> parms) {
	unsigned char address = (parms[0] << 8) | parms[1];
	registers.a = memory.readByte(address);

	clock.t = 16;
	clock.m = 3;
}

//0xFB Enable interrupts, not immediate, enabled after instruction after this is executed, Flags(-,-,-,-)
void Cpu::ei(std::vector<unsigned char> parms) {
	enableInterrupts();

	clock.t = 4;
	clock.m = 1;
}

//0xFE Compare value n with register a, Flags(Z,1,H,C)
void Cpu::cp_n(std::vector<unsigned char> parms) {
	compare(parms[0]);

	clock.t = 8;
	clock.m = 2;
}

//0xFF Restart, push pc onto stack and jump to address 0x38, Flags(-,-,-,-)
void Cpu::rst_38(std::vector<unsigned char> parms) {
	memory.writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x38;
	clock.t = 16;
	clock.m = 1;
}
