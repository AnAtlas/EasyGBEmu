#include "Cpu.hpp"
#include <iostream>
#include <string>

Cpu::Cpu() {
	clearRegisters();
	generateInstructions();
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
	//Name string, number of parameters, number of cycles, function
	instructions.push_back(Instruction("NOP", 0, 4, &Cpu::nop));					//0x00
	instructions.push_back(Instruction("LD BC, 0xXXXX", 2, 12, &Cpu::ld_bc_nn));	//0x01	
	instructions.push_back(Instruction("LD_BC_A", 0, 8, &Cpu::ld_bcp_a));			//0x02
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
	clock.m = 1;
	clock.t = 4;
}

//0x01 Store 2 byte value, nn, into the bc register
void Cpu::ld_bc_nn(std::vector<unsigned char> parms) {
	unsigned short t = (parms[0] << 8) | parms[1];
	registers.bc = t;
	clock.m = 3;
	clock.t = 12;
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

//0x07 Rotate register a n bits left
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

void Cpu::stop(std::vector<unsigned char> parms);
void Cpu::ld_de_nn(std::vector<unsigned char> parms);
void Cpu::ld_dep_a(std::vector<unsigned char> parms);
void Cpu::inc_de(std::vector<unsigned char> parms);
void Cpu::inc_d(std::vector<unsigned char> parms);
