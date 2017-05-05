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

void Cpu::clearRegisters() {
	registers.af = 0;
	registers.bc = 0;
	registers.de = 0;
	registers.hl = 0;
	registers.sp = 0;
	registers.pc = 0;
}

void Cpu::generateInstructions() {
	//Name string, number of parameters(in bytes, not counting opcode byte), number of cycles, function
	instructions.push_back(Instruction("NOP",				0, 4, &Cpu::nop));			//0x00	
	instructions.push_back(Instruction("LD BC, 0xXXXX",		2,12, &Cpu::ld_bc_nn));		//0x01	
	instructions.push_back(Instruction("LD (BC), A",		0, 8, &Cpu::ld_bcp_a));		//0x02	
	instructions.push_back(Instruction("INC BC",			0, 8, &Cpu::inc_bc));		//0x03	
	instructions.push_back(Instruction("INC B",				0, 4, &Cpu::inc_b));		//0x04
	instructions.push_back(Instruction("DEC B",				0, 4, &Cpu::dec_b));		//0x05
	instructions.push_back(Instruction("LD B, 0xXX",		1, 8, &Cpu::ld_b_n));		//0x06
	instructions.push_back(Instruction("RLCA",				0, 4, &Cpu::rlca));			//0x07
	instructions.push_back(Instruction("LD (0xXXXX), SP",	2,20, &Cpu::ld_nnp_sp));	//0x08
	instructions.push_back(Instruction("ADD HL, BC",		0, 8, &Cpu::add_hl_bc));	//0x09
	instructions.push_back(Instruction("LD A, (BC)",		0, 8, &Cpu::ld_a_bcp));		//0x0A
	instructions.push_back(Instruction("DEC BC",			0, 8, &Cpu::dec_bc));		//0x0B
	instructions.push_back(Instruction("INC C",				0, 4, &Cpu::inc_c));		//0x0C
	instructions.push_back(Instruction("DEC C",				0, 4, &Cpu::dec_c));		//0x0D
	instructions.push_back(Instruction("LD C, 0xXX",		1, 8, &Cpu::ld_c_n));		//0x0E
	instructions.push_back(Instruction("RRCA",				0, 4, &Cpu::rrca));			//0x0F
	instructions.push_back(Instruction("STOP",				1, 4, &Cpu::stop));			//0x10
	instructions.push_back(Instruction("LD DE, 0xXXXX",		2,12, &Cpu::ld_de_nn));		//0x11
	instructions.push_back(Instruction("LD (DE), A",		0, 8, &Cpu::ld_dep_a));		//0x12
	instructions.push_back(Instruction("INC DE",			0, 8, &Cpu::inc_de));		//0x13
	instructions.push_back(Instruction("INC D",				0, 4, &Cpu::inc_d));		//0x14
	instructions.push_back(Instruction("DEC D",				0, 4, &Cpu::dec_d));		//0x15
	instructions.push_back(Instruction("LD D, 0xXX",		1, 8, &Cpu::ld_d_n));		//0x16
	instructions.push_back(Instruction("RLA",				0, 4, &Cpu::rla));			//0x17
	instructions.push_back(Instruction("JR, 0xXX",			1,12, &Cpu::jr_n));			//0x18
	instructions.push_back(Instruction("ADD HL, DE",		0, 8, &Cpu::add_hl_de));	//0x19
	instructions.push_back(Instruction("LD A, (DE)",		0, 8, &Cpu::ld_a_dep));		//0x1A
	instructions.push_back(Instruction("DEC DE",			0, 8, &Cpu::dec_de));		//0x1B
	instructions.push_back(Instruction("INC E",				0, 4, &Cpu::inc_e));		//0x1C
	instructions.push_back(Instruction("DEC E",				0, 4, &Cpu::dec_e));		//0x1D
	instructions.push_back(Instruction("LD E, 0xXX",		1, 8, &Cpu::ld_e_n));		//0x1E
	instructions.push_back(Instruction("RRA",				0, 4, &Cpu::rra));			//0x1F
	instructions.push_back(Instruction("JR NZ, 0xXX",		1,12, &Cpu::jr_nz_n));		//0x20
	instructions.push_back(Instruction("LD HL, 0xXXXX",		2,12, &Cpu::ld_hl_nn));		//0x21
	instructions.push_back(Instruction("LD (HL+), A",		0, 8, &Cpu::ldi_hl_a));		//0x22
	instructions.push_back(Instruction("INC HL",			0, 8, &Cpu::inc_hl));		//0x23
	instructions.push_back(Instruction("INC H",				0, 4, &Cpu::inc_h));		//0x24
	instructions.push_back(Instruction("DEC H",				0, 4, &Cpu::dec_h));		//0x25
	instructions.push_back(Instruction("LD H, 0xXX",		1, 8, &Cpu::ld_h_n));		//0x26
	instructions.push_back(Instruction("DAA",				1, 4, &Cpu::daa));			//0x27
	instructions.push_back(Instruction("JR Z, 0xXX",		1, 12, &Cpu::jr_z_n));		//0x28
	instructions.push_back(Instruction("ADD HL, HL",		1, 8, &Cpu::add_hl_hl));	//0x29
	instructions.push_back(Instruction("LD A, (HL+)",		0, 8, &Cpu::ld_a_hlpi));	//0x2A
	instructions.push_back(Instruction("DEC HL",			0, 8, &Cpu::dec_hl));		//0x2B
	instructions.push_back(Instruction("INC HL",			0, 8, &Cpu::inc_hl));		//0x2C
	instructions.push_back(Instruction("DEC L",				0, 4, &Cpu::dec_l));		//0x2D
	instructions.push_back(Instruction("LD L, 0xXX",		1, 8, &Cpu::ld_l_n));		//0x2E
	instructions.push_back(Instruction("CPL",				0, 4, &Cpu::cpl));			//0x2F
	instructions.push_back(Instruction("JR NC, 0xXX",		1,12, &Cpu::jr_nc_n));		//0x30
	instructions.push_back(Instruction("LD SP, 0xXXXX",		2,12, &Cpu::ld_sp_nn));		//0x31
	instructions.push_back(Instruction("LD (HL-), A",		0, 8, &Cpu::ldd_hlp_a));	//0x32
	instructions.push_back(Instruction("INC_SP",			0, 8, &Cpu::inc_sp));		//0x33
	instructions.push_back(Instruction("INC (HL)",			0,12, &Cpu::inc_hlp));		//0x34
	instructions.push_back(Instruction("DEC (HL)",			0,12, &Cpu::dec_hlp));		//0x35
	instructions.push_back(Instruction("LD (HL), 0xXX",		1,12, &Cpu::ld_hlp_n));		//0x36
	instructions.push_back(Instruction("SCF",				0, 4, &Cpu::scf));			//0x37
	instructions.push_back(Instruction("JR C, 0xXX",		1,12, &Cpu::jr_c_n));		//0x38
	instructions.push_back(Instruction("ADD HL, SP",		0, 8, &Cpu::add_hl_sp));	//0x39
	instructions.push_back(Instruction("LD A, (HL-)",		0, 8, &Cpu::ld_a_hlpd));	//0x3A
	instructions.push_back(Instruction("DEC SP",			0, 8, &Cpu::dec_sp));		//0x3B
	instructions.push_back(Instruction("INC A",				0, 4, &Cpu::inc_a));		//0x3C
	instructions.push_back(Instruction("DEC A",				0, 4, &Cpu::dec_a));		//0x3D
	instructions.push_back(Instruction("LD A, 0xXX",		1, 8, &Cpu::ld_a_n));		//0x3E
	instructions.push_back(Instruction("CCF",				0, 4, &Cpu::ccf));			//0x3F
}

bool Cpu::checkFlag(unsigned char flag) {
	return registers.f & flag;
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

void Cpu::step() {

}


unsigned char Cpu::inc(unsigned char val) {
	if (val & 0x0F == 0x0F)
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
	if (val & 0x10 == 0x10)
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

unsigned short Cpu::add2(unsigned short v1, unsigned short v2) {
	if ((v1 & 0xfff) + (v2 & 0xfff) & 0x1000)
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
	registers.hl = add2(registers.hl, registers.bc);

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

	registers.hl = add2(registers.hl, registers.de);

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

	registers.a = s;
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
	registers.hl = add2(registers.hl, registers.hl);

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
	registers.hl = add2(registers.hl, registers.sp);

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
