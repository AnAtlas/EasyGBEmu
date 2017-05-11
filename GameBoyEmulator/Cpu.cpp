#include "Cpu.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>

#include "Gameboy.hpp"
#include "Opcodes.hpp"

Cpu::Cpu(GameboyModes gameboyMode) : jumped(false),gameboyMode(gameboyMode){
	setRegisters();
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
	unsigned char nextOp = memory->readByte(registers.pc);
	Instruction nextIns = instructions[nextOp];
	std::vector<unsigned char> parms;
	if (nextIns.parameterLength > 0)
		parms.push_back(memory->readByte(registers.pc + 1));
	if (nextIns.parameterLength > 1)
		parms.push_back(memory->readByte(registers.pc + 2));
	std::cout <<"\n"<< nextIns.instruction << " : Parms <- ";
	if (parms.size() > 0) {
		for (unsigned char i : parms) {
			std::cout <<  std::hex << "0x" << static_cast<unsigned>(i) << ", ";
		}
	}
	clock.m = 0;
	clock.t = 0;
	(this->*(instructions[nextOp].fp))(parms);
	registers.pc += clock.m;
}

void Cpu::setRegisters() {
	if (gameboyMode == GameboyModes::DMG) {
		registers.af = 0x01B0;
		registers.bc = 0x0013;
		registers.de = 0x00D8;
		registers.hl = 0x014D;
		registers.sp = 0xFFFE;
		registers.pc = 0x0100;
	}
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
	instructions = Opcodes::getInstructionVector();
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
		unsigned short t = (parms[1] << 8) | parms[0];
		registers.bc = t;
		clock.m = 3;
		clock.t = 12;
	}
}

//0x02 Store the value in register a, into the address pointed to by register bc
void Cpu::ld_bcp_a(std::vector<unsigned char> parms) {
	memory->writeByte(registers.bc, registers.a);
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
	unsigned short address = parms[1] << 8 | parms[0];
	memory->writeShort(address, registers.sp);

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
	registers.a = memory->readByte(registers.bc);

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
	unsigned short a = (parms[1] << 8) | parms[0];
	registers.de = a;
	
	clock.m = 3;
	clock.t = 12;
}

//0x12 Store register a into memory location de
void Cpu::ld_dep_a(std::vector<unsigned char> parms) {
	memory->writeByte(registers.de, registers.a);

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

	clock.t = 12;
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
	registers.a = memory->readByte(registers.de);

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
	else {
		clock.t = 8;
		clock.m = 2;
	}	
}

//0x21 Store unsigned short into register hl
void Cpu::ld_hl_nn(std::vector<unsigned char> parms) {
	unsigned short value = (parms[1] << 8) + parms[0];
	registers.hl = value;

	clock.m = 3;
	clock.t = 12;
}

//0x22 Store register a into memory at hl, then increment HL, Flags(-,-,-,-)
void Cpu::ldi_hl_a(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.a);
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
	else {
		clock.t = 8;
		clock.m = 2;
	}
}

//0x29 Add register hl to register hl, Flags(-,0,H,C)
void Cpu::add_hl_hl(std::vector<unsigned char> parms) {
	registers.hl = addShorts(registers.hl, registers.hl);

	clock.m = 1;
	clock.t = 8;
}

//0x2A Set register a to memory location hl, then increment hl, Flags(-,-,-,-)
void Cpu::ld_a_hlpi(std::vector<unsigned char> parms) {
	registers.a = memory->readByte(registers.hl);
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
	else {
		clock.t = 8;
		clock.m = 2;
	}
		
}

//0x31 Store unsigned short nn into register sp, Flags(-,-,-,-)
void Cpu::ld_sp_nn(std::vector<unsigned char> parms) {
	unsigned short value = (parms[1] << 8) | parms[0];
	registers.sp = value;

	clock.m = 3;
	clock.t = 12;
}

//0x32 Load a into memory at hl, then decrement hl, Flags(-,-,-,-)
void Cpu::ldd_hlp_a(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.a);
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
	unsigned char value = memory->readByte(registers.hl);
	memory->writeByte(registers.hl, inc(value));

	clock.m = 1;
	clock.t = 12;
}

//0x35 Get Value at memory hl, then decrement it, Flags(Z,1,H,-)
void Cpu::dec_hlp(std::vector<unsigned char> parms) {
	unsigned char value = memory->readByte(registers.hl);
	memory->writeByte(registers.hl, dec(value));

	clock.m = 1;
	clock.t = 12;
}

//0x36 Set memory at hl equal to unsigned byte n, Flags(-,-,-,-)
void Cpu::ld_hlp_n(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, parms[0]);

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
	else {
		clock.t = 8;
		clock.m = 2;
	}
}

//0x39 Add sp to hl register, Flags(-,0,H,C)
void Cpu::add_hl_sp(std::vector<unsigned char> parms) {
	registers.hl = addShorts(registers.hl, registers.sp);

	clock.m = 1;
	clock.t = 8;
}

//0x3A Load memory at hl into register a, then decrement hl, Flags(-,-,-,-)
void Cpu::ld_a_hlpd(std::vector<unsigned char> parms) {
	registers.a = memory->readByte(registers.hl);
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
	registers.b = memory->readByte(registers.hl);

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
	registers.c = memory->readByte(registers.hl);

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
	registers.d = memory->readByte(registers.hl);

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
	registers.e = memory->readByte(registers.hl);

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
	registers.h = memory->readByte(registers.hl);

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
	registers.l = memory->readByte(registers.hl);

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
	memory->writeByte(registers.hl, registers.b);

	clock.m = 1;
	clock.t = 8;
}

//0x71 Store register c into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_c(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.c);

	clock.m = 1;
	clock.t = 8;
}

//0x72 Store register d into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_d(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.d);

	clock.m = 1;
	clock.t = 8;
}

//0x73 Store register e into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_e(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.e);

	clock.m = 1;
	clock.t = 8;
}

//0x74 Store register h into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_h(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.h);

	clock.m = 1;
	clock.t = 8;
}

//0x75 Store register l into register b, Flags(-,-,-,-)
void Cpu::ld_hlp_l(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.l);

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
	memory->writeByte(registers.hl, registers.a);

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
	registers.a = memory->readByte(registers.hl);

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
	registers.a = addBytes(registers.a, memory->readByte(registers.hl));

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
	registers.a = addCarry(memory->readByte(registers.hl));

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
	registers.a = subBytes(registers.a, memory->readByte(registers.hl));

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
	registers.a = subCarry(memory->readByte(registers.hl));

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
	registers.a = and(registers.a, memory->readByte(registers.hl));

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
	registers.a = xor(registers.a, memory->readByte(registers.hl));

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
	registers.a = or (registers.a, memory->readByte(registers.hl));

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
	compare(memory->readByte(registers.hl));

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
		registers.pc = memory->readShortFromStack(&registers.sp);
		clock.t = 20;
	}
	else
		clock.t = 8;
	clock.m = 1;
}

//0xC1 Pop top of Stack into register bc, Flags(-,-,-,-)
void Cpu::pop_bc(std::vector<unsigned char> parms) {
	registers.bc = memory->readShortFromStack(&registers.sp);
	clock.m = 1;
	clock.t = 12;
}

//0xC2 Jump to address nn if not zero, Flags(-,-,-,-)
void Cpu::jp_nz_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
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
	unsigned short address = (parms[1] << 8) | parms[0];
	registers.pc = address;
	clock.t = 16;
	clock.m = 0;
}

//0xC4 Store pc on stack, jump to address if not zero, Flags(-,-,-,-)
void Cpu::call_nz_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (!checkFlag(Flags::Zero)) {
		memory->writeShortToStack(registers.pc, &registers.sp);
		registers.pc = address;
		clock.t = 24;
	}
	else
		clock.t = 12;
	clock.m = 3;
}

//0xC5 Push register bc onto stack Flags(-,-,-,-)
void Cpu::push_bc(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.bc, &registers.sp);
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
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x00;

	clock.t = 16;
	clock.m = 1;
}

//0xC8 Return to address off stack if zero flag is set, Flags(-,-,-,-)
void Cpu::ret_z(std::vector<unsigned char> parms) {
	if (checkFlag(Flags::Zero)) {
		registers.pc = memory->readShortFromStack(&registers.sp);
		clock.t = 20;
	}
	else
		clock.t = 8;
	clock.m = 1;
}

//0xC9 Return to address off stack, Flags(-,-,-,-)
void Cpu::ret(std::vector<unsigned char> parms) {
	registers.pc = memory->readShortFromStack(&registers.sp);
	
	clock.t = 20;
	clock.m = 1;

}

//0xCA Jump to address nn if zero flag is set, Flags(-,-,-,-)
void Cpu::jp_z_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (checkFlag(Flags::Zero)) {
		registers.pc = address;
		clock.t = 16;
	}
	else {
		clock.t = 12;
		clock.m = 3;
	}
}

//0xCB Access point to extended opcode table, Flags(-,-,-,-)
void Cpu::cb(std::vector<unsigned char> parms) {
	
	for (int i = 0; i < 10; i++) {
		std::cout << "IN CB, THESE ARE NEEDED";
		Sleep(1000);
	}
	clock.t = 4;
	clock.m = 1;
}

//0xCC Store pc on stack, jump to address if zero flag set, Flags(-,-,-,-)
void Cpu::call_z_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (checkFlag(Flags::Zero)) {
		memory->writeShortToStack(registers.pc, &registers.sp);
		registers.pc = address;
		clock.t = 24;
	}
	else
		clock.t = 12;
	clock.m = 3;
}

//0xCD Store pc on stack, jump to address, Flags(-,-,-,-)
void Cpu::call_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	memory->writeShortToStack(registers.pc, &registers.sp);
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
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x08;

	clock.t = 16;
	clock.m = 1;
}


//0xD0 Return to address on stack, if carry flag not set, Flags(-,-,-,-)
void Cpu::ret_nc(std::vector<unsigned char> parms) {
	if (!checkFlag(Flags::Carry)) {
		registers.pc = memory->readShortFromStack(&registers.sp);
		clock.t = 20;
	}
	else
		clock.t = 8;
	clock.m = 1;
}

//0xD1 Pop short off stack into register de, Flags(-,-,-,-)
void Cpu::pop_de(std::vector<unsigned char> parms) {
	registers.de = memory->readShortFromStack(&registers.sp);

	clock.t = 12;
	clock.m = 1;
}

//0xD2 Jump to address nn if carry flag not set, Flags(-,-,-,-)
void Cpu::jp_nc_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (!checkFlag(Flags::Carry)) {
		registers.pc = address;
		clock.t = 16;
	}
	else {
		clock.t = 12;
		clock.m = 3;
	}
}

//0xD4 If carry flag is not set, push pc onto stack and jump to address nn, Flags(-,-,-,-)
void Cpu::call_nc_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (!checkFlag(Flags::Carry)) {
		memory->writeShortToStack(registers.pc, &registers.sp);
		registers.pc = address;
		clock.t = 24;
	}
	else
		clock.t = 12;
	clock.m = 3;
}

//0xD5 Push register de onto the stack, Flags(-,-,-,-)
void Cpu::push_de(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.de, &registers.sp);

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
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x10;
	clock.t = 16;
	clock.m = 1;
}

//0xD8 Return to address from stack if Carry flag set, Flags(-,-,-,-)
void Cpu::ret_c(std::vector<unsigned char> parms) {
	if (checkFlag(Flags::Carry)) {
		registers.pc = memory->readShortFromStack(&registers.sp);
		clock.t = 20;
	}
	else
		clock.t = 8;
	clock.m = 1;
}

//0xD9 Pop short off of stack, jump to that address, enable interrupts, Flags(-,-,-,-)
void Cpu::reti(std::vector<unsigned char> parms) {
	registers.pc = memory->readShortFromStack(&registers.sp);
	enableInterrupts();

	clock.t = 16;
	clock.m = 1;
}

//0xDA Jump to address nn if carry flag set, Flags(-,-,-,-)
void Cpu::jp_c_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (checkFlag(Flags::Carry)) {
		registers.pc = address;
		clock.t = 16;
	}
	else {
		clock.t = 12;
		clock.m = 3;
	}
}

//0xDC If carry flag is set, push pc onto stack and jump to address nn, Flags(-,-,-,-)
void Cpu::call_c_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (checkFlag(Flags::Carry)) {
		memory->writeShortToStack(registers.pc, &registers.sp);
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
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x18;
	clock.t = 16;
	clock.m = 1;
}

//0xE0 Put register a into memory address 0xFF00 + n, Flags(-,-,-,-)
void Cpu::ld_ffnp_a(std::vector<unsigned char> parms) {
	memory->writeByte(0xFF00 + parms[0], registers.a);

	clock.t = 12;
	clock.m = 2;
}

//0xE1 Pop short from stack into register hl, Flags(-,-,-,-)
void Cpu::pop_hl(std::vector<unsigned char> parms) {
	registers.hl = memory->readShortFromStack(&registers.sp);

	clock.t = 12;
	clock.m = 1;
}

//0xE2 Store register a into memory at address 0xFF00 + register c, Flags(-,-,-,-)
void Cpu::ld_cp_a(std::vector<unsigned char> parms) {
	memory->writeByte(0xFF00 + registers.c, registers.a);

	clock.t = 8;
	clock.m = 2;
}

//0xE5 Push register hl onto stack, Flags(-,-,-,-)
void Cpu::push_hl(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.hl, &registers.sp);

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
	memory->writeShortToStack(registers.pc, &registers.sp);
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
	registers.pc = memory->readShort(registers.hl);

	clock.t = 4;
}

//0xEA Store register a into memory address nn, Flags(-,-,-,-)
void Cpu::ld_nnp_a(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	memory->writeByte(address, registers.a);

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
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x28;
	clock.t = 16;
	clock.m = 1;
}

//0xF0 Load memory location 0xFF00 + n into register a, Flags(-,-,-,-)//0xEF
void Cpu::ld_a_ffnp(std::vector<unsigned char> parms) {
	registers.a = memory->readByte(0xFF00 + parms[0]);

	clock.t = 8;
	clock.m = 2;
}

//0xF1 Pop short off stack into register af, Flags(Z,N,H,C)
void Cpu::pop_af(std::vector<unsigned char> parms) {
	registers.af = memory->readShortFromStack(&registers.sp);

	clock.t = 12;
	clock.m = 1;
}

//0xF2 Load memory location 0xFF00 + c into register a, Flags(-,-,-,-)
void Cpu::ld_a_cp(std::vector<unsigned char> parms) {
	registers.a = memory->readByte(0xFF00 + registers.c);

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
	memory->writeShortToStack(registers.af, &registers.sp);

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
	memory->writeShortToStack(registers.pc, &registers.sp);
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
	unsigned char address = (parms[1] << 8) | parms[0];
	registers.a = memory->readByte(address);

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
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x38;
	clock.t = 16;
	clock.m = 1;
}
