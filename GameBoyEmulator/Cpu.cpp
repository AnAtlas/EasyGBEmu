#include "Cpu.hpp"
#include <iostream>
#include <string>
#include <sstream>

Cpu::Cpu(){
	clearRegisters();
	generateInstructions();
}


void Cpu::runCommand(std::string command) {
	std::cout << command;

	//Break apart command
	std::vector <std::string> ps;
	std::stringstream ss;
	ss.str(command);
	std::string item;
	while (std::getline(ss, item, ' ')) {
		ps.push_back(item);
	}
	if (ps[0] == "p")
		printRegisters();
	else {
		unsigned char op = (unsigned char)(std::stoi(ps[0]));
		std::vector<unsigned char> parms;
		for (int i = 1; i < ps.size(); i++) {
			parms.push_back((unsigned char)(std::stoi(ps[i])));
		}
		(this->*(instructions[op].fp))(parms);
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
	std::cout << std::hex << "F: 0x" << (unsigned short)registers.f << std::endl;
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
	registers.b = parms[0];

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
