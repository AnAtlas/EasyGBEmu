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
	cbMode = false;
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

unsigned char Cpu::step() {
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
	clock.m = nextIns.parameterLength + 1;
	clock.t = nextIns.ticks;
	if (cbMode) {
		(this->*(extInstructions[nextOp].fp))(parms);
		cbMode = false;
	}
	else
		(this->*(instructions[nextOp].fp))(parms);
	registers.pc += clock.m;
	return clock.t;
}

void Cpu::setRegisters() {
	if (gameboyMode == GameboyModes::DMG) {
		registers.af = 0x01B0;
		registers.bc = 0x0013;
		registers.de = 0x00D8;
		registers.hl = 0x014D;
		registers.sp = 0xFFFE;
		registers.pc = 0x0000;
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

unsigned char Cpu::or(unsigned char a, unsigned char b) {
	clearAllFlags();
	if (!(a | b))
		setFlag(Flags::Zero);
	return a | b;
}

//Rotate byte left 1, over carry bit
unsigned char Cpu::rlc(unsigned char byte) {
	int carry = (byte & 0b10000000) >> 7;

	if (carry)
		setFlag(Flags::Carry);
	else
		clearFlag(Flags::Carry);

	byte <<= 1;
	byte += carry;

	if (byte > 0)
		clearFlag(Flags::Zero);
	else
		setFlag(Flags::Zero);

	clearFlag(Flags::Subtract);
	clearFlag(Flags::HalfCarry);

	return byte;
}

//Rotate byte right 1, over carry bit
unsigned char Cpu::rrc(unsigned char byte) {
	int carry = byte & 0b1;

	if (carry)
		setFlag(Flags::Carry);
	else
		clearFlag(Flags::Carry);

	byte >>= 1;
	byte += (carry << 7);

	if (byte > 0)
		clearFlag(Flags::Zero);
	else
		setFlag(Flags::Zero);

	clearFlag(Flags::Subtract);
	clearFlag(Flags::HalfCarry);

	return byte;
}

//Rotate byte left 1, with carry bit
unsigned char Cpu::rl(unsigned char byte) {
	unsigned char carry = checkFlag(Flags::Carry) ? 1 : 0;

	if (byte & 0b10000000)
		setFlag(Flags::Carry);
	else
		clearFlag(Flags::Carry);
	byte <<= 1;
	byte += carry;

	if (byte)
		clearFlag(Flags::Zero);
	else
		setFlag(Flags::Zero);

	clearFlag(Flags::Subtract);
	clearFlag(Flags::HalfCarry);
	return byte;
}

//Rotate byte right 1, with carry bit
unsigned char Cpu::rr(unsigned char byte) {
	unsigned char carry = checkFlag(Flags::Carry) ? 0b10000000 : 0;

	if (byte & 0b1)
		setFlag(Flags::Carry);
	else
		clearFlag(Flags::Carry);
	byte >>= 1;
	byte += carry;

	if (byte)
		clearFlag(Flags::Zero);
	else
		setFlag(Flags::Zero);

	clearFlag(Flags::Subtract);
	clearFlag(Flags::HalfCarry);
	return byte;
}

//Shift byte left into Carry, LSB set to 0
unsigned char Cpu::sla(unsigned char byte) {
	if (byte & 0b10000000)
		setFlag(Flags::Carry);
	else
		clearFlag(Flags::Carry);
	byte <<= 1;

	if (byte)
		clearFlag(Flags::Zero);
	else
		setFlag(Flags::Zero);

	clearFlag(Flags::Subtract);
	clearFlag(Flags::HalfCarry);

	return byte;
}

//Shift byte right into Carry, MSB doesn't change
unsigned char Cpu::sra(unsigned char byte) {
	unsigned char msb = byte & 0b10000000;
	if (byte & 0b1)
		setFlag(Flags::Carry);
	else
		clearFlag(Flags::Carry);
	byte >>= 1;
	byte |= msb;

	if (byte)
		clearFlag(Flags::Zero);
	else
		setFlag(Flags::Zero);

	clearFlag(Flags::Subtract);
	clearFlag(Flags::HalfCarry);

	return byte;
}

//Shift byte right into Carry, MSB set to 0
unsigned char Cpu::srl(unsigned char byte) {
	if (byte & 0b1)
		setFlag(Flags::Carry);
	else
		clearFlag(Flags::Carry);
	byte >>= 1;

	if (byte)
		clearFlag(Flags::Zero);
	else
		setFlag(Flags::Zero);

	clearFlag(Flags::Subtract);
	clearFlag(Flags::HalfCarry);

	return byte;
}

//Swap the upper and lower nibbles of byte
unsigned char Cpu::swap(unsigned char byte) {
	unsigned char lower = byte & 0xF;

	byte >>= 4;
	byte |= (lower << 4);
	clearAllFlags();
	if (!byte)
		setFlag(Flags::Zero);

	return byte;
}

//Test bit b in byte, set zero flag if bit b in byte = 0
void Cpu::bit(unsigned char bit, unsigned char byte) {
	if (bit & byte)
		clearFlag(Flags::Zero);
	else
		setFlag(Flags::Zero);
	clearFlag(Flags::Subtract);
	setFlag(Flags::HalfCarry);
}

//Reset bit b in byte
unsigned char Cpu::res(unsigned char bit, unsigned char byte) {
	return byte & (~bit);
}

//Turn on the bit in position bit, in byte
unsigned char Cpu::set(unsigned char bit, unsigned char byte) {
	byte |= bit;
	return byte;
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
	extInstructions = Opcodes::getExtendedInstructionVector();
}
//NULL NOT IMPLEMENTED OPCODE
void Cpu::null(std::vector<unsigned char> parms) {
}
//0x00 Do nothing
void Cpu::nop(std::vector<unsigned char> parms) {
	std::cout << "NOP" << std::endl;
}
//0x01 Store 2 byte value, nn, into the bc register
void Cpu::ld_bc_nn(std::vector<unsigned char> parms) {
	unsigned short t = (parms[1] << 8) | parms[0];
	registers.bc = t;
}
//0x02 Store the value in register a, into the address pointed to by register bc
void Cpu::ld_bcp_a(std::vector<unsigned char> parms) {
	memory->writeByte(registers.bc, registers.a);
}
//0x03 Increment the bc Register, does NOT affect flags
void Cpu::inc_bc(std::vector<unsigned char> parms) {
	registers.bc++;
}
//0x04 Increment the b register, affects flags
void Cpu::inc_b(std::vector<unsigned char> parms) {
	registers.b = inc(registers.b);
}
//0x05 Decrement the b register, affects flags
void Cpu::dec_b(std::vector<unsigned char> parms) {
	registers.b = dec(registers.b);
}
//0x06 Store byte value, into b register
void Cpu::ld_b_n(std::vector<unsigned char> parms) {
	registers.b = parms[0];
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
}
//0x08 Store sp into address at nn, nn = a16
void Cpu::ld_nnp_sp(std::vector<unsigned char> parms) {
	unsigned short address = parms[1] << 8 | parms[0];
	memory->writeShort(address, registers.sp);
}
//0x09 add bc into hl
void Cpu::add_hl_bc(std::vector<unsigned char> parms) {
	clearFlag(Flags::Subtract);
	registers.hl = addShorts(registers.hl, registers.bc);
}
//0x0A Store memory at address bc into a
void Cpu::ld_a_bcp(std::vector<unsigned char> parms) {
	registers.a = memory->readByte(registers.bc);
}
//0x0B Decrement register bc
void Cpu::dec_bc(std::vector<unsigned char> parms) {
	registers.bc--;
}
//0x0C Increment register c
void Cpu::inc_c(std::vector<unsigned char> parms) {
	registers.c = inc(registers.c);
}
//0x0D Decrement register c
void Cpu::dec_c(std::vector<unsigned char> parms) {
	registers.c = dec(registers.c);
}
//0x0E Store byte value into c register
void Cpu::ld_c_n(std::vector<unsigned char> parms) {
	registers.c = parms[0];
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
}
//0x10 Stop, hald cpu and lcd display until button pressed
void Cpu::stop(std::vector<unsigned char> parms) {
	stopped = true;
}
//0x11 Store short into register de
void Cpu::ld_de_nn(std::vector<unsigned char> parms) {
	unsigned short a = (parms[1] << 8) | parms[0];
	registers.de = a;
}
//0x12 Store register a into memory location de
void Cpu::ld_dep_a(std::vector<unsigned char> parms) {
	memory->writeByte(registers.de, registers.a);
}
//0x13 increment de register
void Cpu::inc_de(std::vector<unsigned char> parms) {
	registers.de++;
}
//0x14 increment d register, Flags(Z,0,H)
void Cpu::inc_d(std::vector<unsigned char> parms) {
	registers.d = inc(registers.d);
}
//0x15 decrement d register, Flags(Z,1,H,-)
void Cpu::dec_d(std::vector<unsigned char> parms) {
	registers.d = dec(registers.d);
}
//0x16 load byte into register d, Flags(-,-,-,-)
void Cpu::ld_d_n(std::vector<unsigned char> parms) {
	registers.d = parms[0];
}
//0x17 Rotate A left through carry flag, new bit 0 = old carry flag, Flags(0,0,0,C)
void Cpu::rla(std::vector<unsigned char> parms) {
	unsigned char carry = checkFlag(Flags::Carry);
	clearAllFlags();
	if (registers.a & 0x80)
		setFlag(Flags::Carry);
	registers.a = registers.a << 1;
	registers.a += carry;
}
//0x18 Jump to current address + n
void Cpu::jr_n(std::vector<unsigned char> parms) {
	registers.pc += (signed char)parms[0];
}
//0x19 Add de register to hl register, Flags(-,0,H,C)
void Cpu::add_hl_de(std::vector<unsigned char> parms) {
	clearFlag(Flags::Subtract);
	registers.hl = addShorts(registers.hl, registers.de);
}
//0x1A Store byte at address (de) into register a, Flags(-,-,-,-)
void Cpu::ld_a_dep(std::vector<unsigned char> parms) {
	registers.a = memory->readByte(registers.de);
}
//0x1B Decrement register de, Flags(-,-,-,-)
void Cpu::dec_de(std::vector<unsigned char> parms) {
	registers.de--;
}
//0x1C Increment register e, Flags(Z,0,H,-)
void Cpu::inc_e(std::vector<unsigned char> parms) {
	registers.e = inc(registers.e);
}
//0x1D Decrement register e, Flags(Z,1,H,-)
void Cpu::dec_e(std::vector<unsigned char> parms) {
	registers.e = dec(registers.e);
}
//0x1E Store byte n into register e, Flags(-,-,-,-)
void Cpu::ld_e_n(std::vector<unsigned char> parms) {
	registers.e = parms[0];
}
//0x1F Rotate register a right through carry flag, Flags(0,0,0,C)
void Cpu::rra(std::vector<unsigned char> parms) {
	unsigned char carry = checkFlag(Flags::Carry);
	clearAllFlags();
	if (registers.a & 0x01)
		setFlag(Flags::Carry);
	registers.a >>= 1;
	registers.a += (carry << 7);
}
//0x20 Jump to address sp + (signed)n if zero flag is not set, Flags(-,-,-,-)
void Cpu::jr_nz_n(std::vector<unsigned char> parms) {
	signed char addressOffset = (signed char)parms[0];
	if (!checkFlag(Flags::Zero)) 
		registers.pc += addressOffset;
	else
		clock.t = 8;
}
//0x21 Store unsigned short into register hl
void Cpu::ld_hl_nn(std::vector<unsigned char> parms) {
	unsigned short value = (parms[1] << 8) + parms[0];
	registers.hl = value;
}
//0x22 Store register a into memory at hl, then increment HL, Flags(-,-,-,-)
void Cpu::ldi_hl_a(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.a);
	registers.hl++;
}
//0x23 Increment the hl register, Flags(-,-,-,-)
void Cpu::inc_hl(std::vector<unsigned char> parms) {
	registers.hl++;
}
//0x24 Increment the h register, Flags(Z,0,H,-)
void Cpu::inc_h(std::vector<unsigned char> parms) {
	registers.h = inc(registers.h);
}
//0x25 Decrement the h register, Flags(Z,1,H,-)
void Cpu::dec_h(std::vector<unsigned char> parms) {
	registers.h = dec(registers.h);
}
//0x26 Store byte n into register h
void Cpu::ld_h_n(std::vector<unsigned char> parms) {
	registers.h = parms[0];
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
	if (checkFlag(Flags::Zero)) 
		registers.pc += addressOffset;
	else 
		clock.t = 8;
}
//0x29 Add register hl to register hl, Flags(-,0,H,C)
void Cpu::add_hl_hl(std::vector<unsigned char> parms) {
	registers.hl = addShorts(registers.hl, registers.hl);
}
//0x2A Set register a to memory location hl, then increment hl, Flags(-,-,-,-)
void Cpu::ld_a_hlpi(std::vector<unsigned char> parms) {
	registers.a = memory->readByte(registers.hl);
	registers.hl++;
}
//0x2B Decrement hl register, Flags(-,-,-,-)
void Cpu::dec_hl(std::vector<unsigned char> parms) {
	registers.hl--;
}
//0x2C Increment l register, Flags(Z,0,H,-)
void Cpu::inc_l(std::vector<unsigned char> parms) {
	registers.l = inc(registers.l);
}
//0x2D Decrement l register, Flags(Z,1,H,-)
void Cpu::dec_l(std::vector<unsigned char> parms) {
	registers.l = dec(registers.l);
}
//0x2E Store byte n into register l, Flags(-,-,-,-)
void Cpu::ld_l_n(std::vector<unsigned char> parms) {
	registers.l = parms[0];
}
//0x2F Complement the a register, Flags(-,1,1,-)
void Cpu::cpl(std::vector<unsigned char> parms) {
	registers.a = ~registers.a;
	setFlag(Flags::Subtract);
	setFlag(Flags::HalfCarry);
}
//0x30 Jump relative if carry flag not set, n = signed char, Flags(-,-,-,-)
void Cpu::jr_nc_n(std::vector<unsigned char> parms) {
	signed char addressOffset = (signed char)parms[0];
	if (checkFlag(Flags::Carry)) 
		registers.pc += addressOffset;
	else 
		clock.t = 8;
}
//0x31 Store unsigned short nn into register sp, Flags(-,-,-,-)
void Cpu::ld_sp_nn(std::vector<unsigned char> parms) {
	unsigned short value = (parms[1] << 8) | parms[0];
	registers.sp = value;
}
//0x32 Load a into memory at hl, then decrement hl, Flags(-,-,-,-)
void Cpu::ldd_hlp_a(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.a);
	registers.hl--;
}
//0x33 Increment sp, Flags(-,-,-,-)
void Cpu::inc_sp(std::vector<unsigned char> parms) {
	registers.sp++;
}
//0x34 Get Value at memory hl, then increment it, Flags(Z,0,H,-)
void Cpu::inc_hlp(std::vector<unsigned char> parms) {
	unsigned char value = memory->readByte(registers.hl);
	memory->writeByte(registers.hl, inc(value));
}
//0x35 Get Value at memory hl, then decrement it, Flags(Z,1,H,-)
void Cpu::dec_hlp(std::vector<unsigned char> parms) {
	unsigned char value = memory->readByte(registers.hl);
	memory->writeByte(registers.hl, dec(value));
}
//0x36 Set memory at hl equal to unsigned byte n, Flags(-,-,-,-)
void Cpu::ld_hlp_n(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, parms[0]);
}
//0x37 Set Carry Flag, Flags(-,0,0,1)
void Cpu::scf(std::vector<unsigned char> parms) {
	setFlag(Flags::Carry);
	clearFlag(Flags::Subtract);
	clearFlag(Flags::HalfCarry);
}
//0x38 Jump relative by n, if carry flag is set, Flags(-,-,-,-)
void Cpu::jr_c_n(std::vector<unsigned char> parms) {
	signed char addressOffset = (signed char)parms[0];
	if (checkFlag(Flags::Carry))
		registers.pc += addressOffset;
	else 
		clock.t = 8;
}
//0x39 Add sp to hl register, Flags(-,0,H,C)
void Cpu::add_hl_sp(std::vector<unsigned char> parms) {
	registers.hl = addShorts(registers.hl, registers.sp);
}
//0x3A Load memory at hl into register a, then decrement hl, Flags(-,-,-,-)
void Cpu::ld_a_hlpd(std::vector<unsigned char> parms) {
	registers.a = memory->readByte(registers.hl);
	registers.hl--;
}
//0x3B Decrement sp register, Flags(-,-,-,-)
void Cpu::dec_sp(std::vector<unsigned char> parms) {
	registers.sp--;
}
//0x3C Increment a register, Flags(Z,0,H,-)
void Cpu::inc_a(std::vector<unsigned char> parms) {
	registers.a = inc(registers.a);
}
//0x3D Decrement a register, Flags(Z,1,H,-)
void Cpu::dec_a(std::vector<unsigned char> parms) {
	registers.a = dec(registers.a);
}
//0x3E Store unsigned byte n into register a, Flags(-,-,-,-)
void Cpu::ld_a_n(std::vector<unsigned char> parms) {
	registers.a = parms[0];
}
//0x3F Complement Carry Flag, Flags(-,0,0,C)
void Cpu::ccf(std::vector<unsigned char> parms) {
	if (checkFlag(Flags::Carry))
		clearFlag(Flags::Carry);
	else
		setFlag(Flags::Carry);
	clearFlag(Flags::Subtract);
	clearFlag(Flags::HalfCarry);
}
//0x40 Store register b into register b, Flags(-,-,-,-)
void Cpu::ld_b_b(std::vector<unsigned char> parms) {
	registers.b = registers.b;
}
//0x41 Store register c into register b, Flags(-,-,-,-)
void Cpu::ld_b_c(std::vector<unsigned char> parms) {
	registers.b = registers.c;
}
//0x42 Store register d into register b, Flags(-,-,-,-)
void Cpu::ld_b_d(std::vector<unsigned char> parms) {
	registers.b = registers.d;
}
//0x43 Store register e into register b, Flags(-,-,-,-)
void Cpu::ld_b_e(std::vector<unsigned char> parms) {
	registers.b = registers.e;
}
//0x44 Store register h into register b, Flags(-,-,-,-)
void Cpu::ld_b_h(std::vector<unsigned char> parms) {
	registers.b = registers.h;
}
//0x45 Store register l into register b, Flags(-,-,-,-)
void Cpu::ld_b_l(std::vector<unsigned char> parms) {
	registers.b = registers.l;
}
//0x46 Store memory value hl into register b, Flags(-,-,-,-)
void Cpu::ld_b_hlp(std::vector<unsigned char> parms) {
	registers.b = memory->readByte(registers.hl);
}
//0x47 Store register a into register b, Flags(-,-,-,-)
void Cpu::ld_b_a(std::vector<unsigned char> parms) {
	registers.b = registers.a;
}
//0x48 Store register b into register c, Flags(-,-,-,-)
void Cpu::ld_c_b(std::vector<unsigned char> parms) {
	registers.c = registers.b;
}
//0x49 Store register c into register c, Flags(-,-,-,-)
void Cpu::ld_c_c(std::vector<unsigned char> parms) {
	registers.c = registers.c;
}
//0x4A Store register d into register c, Flags(-,-,-,-)
void Cpu::ld_c_d(std::vector<unsigned char> parms) {
	registers.c = registers.d;
}
//0x4B Store register e into register c, Flags(-,-,-,-)//0x4A
void Cpu::ld_c_e(std::vector<unsigned char> parms) {
	registers.c = registers.e;
}
//0x4C Store register h into register c, Flags(-,-,-,-)
void Cpu::ld_c_h(std::vector<unsigned char> parms) {
	registers.c = registers.h;
}
//0x4D Store register l into register c, Flags(-,-,-,-)
void Cpu::ld_c_l(std::vector<unsigned char> parms) {
	registers.c = registers.l;
}
//0x4E Store memory location hl into register b, Flags(-,-,-,-)
void Cpu::ld_c_hlp(std::vector<unsigned char> parms) {
	registers.c = memory->readByte(registers.hl);
}
//0x4F Store register a into register c, Flags(-,-,-,-)
void Cpu::ld_c_a(std::vector<unsigned char> parms) {
	registers.c = registers.a;
}
//0x50 Store register b into register d, Flags(-,-,-,-)
void Cpu::ld_d_b(std::vector<unsigned char> parms) {
	registers.d = registers.b;
}
//0x51 Store register c into register d, Flags(-,-,-,-)
void Cpu::ld_d_c(std::vector<unsigned char> parms) {
	registers.d = registers.c;
}
//0x52 Store register d into register d, Flags(-,-,-,-)
void Cpu::ld_d_d(std::vector<unsigned char> parms) {
	registers.d = registers.d;
}
//0x53 Store register e into register d, Flags(-,-,-,-)
void Cpu::ld_d_e(std::vector<unsigned char> parms) {
	registers.d = registers.e;
}
//0x54 Store register h into register d, Flags(-,-,-,-)
void Cpu::ld_d_h(std::vector<unsigned char> parms) {
	registers.d = registers.h;
}
//0x55 Store register l into register d, Flags(-,-,-,-)
void Cpu::ld_d_l(std::vector<unsigned char> parms) {
	registers.d = registers.l;
}
//0x56 Store memory value hl into register d, Flags(-,-,-,-)
void Cpu::ld_d_hlp(std::vector<unsigned char> parms) {
	registers.d = memory->readByte(registers.hl);
}
//0x57 Store register a into register d, Flags(-,-,-,-)
void Cpu::ld_d_a(std::vector<unsigned char> parms) {
	registers.d = registers.a;
}
//0x58 Store register b into register e, Flags(-,-,-,-)
void Cpu::ld_e_b(std::vector<unsigned char> parms) {
	registers.e = registers.b;
}
//0x59 Store register c into register e, Flags(-,-,-,-)
void Cpu::ld_e_c(std::vector<unsigned char> parms) {
	registers.e = registers.c;
}
//0x5A Store register d into register e, Flags(-,-,-,-)
void Cpu::ld_e_d(std::vector<unsigned char> parms) {
	registers.e = registers.d;
}
//0x5B Store register e into register e, Flags(-,-,-,-)//0x4A
void Cpu::ld_e_e(std::vector<unsigned char> parms) {
	registers.e = registers.e;
}
//0x5C Store register h into register e, Flags(-,-,-,-)
void Cpu::ld_e_h(std::vector<unsigned char> parms) {
	registers.e = registers.h;
}
//0x5D Store register l into register e, Flags(-,-,-,-)
void Cpu::ld_e_l(std::vector<unsigned char> parms) {
	registers.e = registers.l;
}
//0x5E Store memory value hl into register e, Flags(-,-,-,-)
void Cpu::ld_e_hlp(std::vector<unsigned char> parms) {
	registers.e = memory->readByte(registers.hl);
}
//0x5F Store register a into register e, Flags(-,-,-,-)
void Cpu::ld_e_a(std::vector<unsigned char> parms) {
	registers.e = registers.a;
}
//0x60 Store register b into register h, Flags(-,-,-,-)
void Cpu::ld_h_b(std::vector<unsigned char> parms) {
	registers.h = registers.b;
}
//0x61 Store register c into register h, Flags(-,-,-,-)//0x60
void Cpu::ld_h_c(std::vector<unsigned char> parms) {
	registers.h = registers.c;
}
//0x62 Store register d into register h, Flags(-,-,-,-)
void Cpu::ld_h_d(std::vector<unsigned char> parms) {
	registers.h = registers.d;
}
//0x63 Store register e into register h, Flags(-,-,-,-)
void Cpu::ld_h_e(std::vector<unsigned char> parms) {
	registers.h = registers.e;
}
//0x64 Store register h into register h, Flags(-,-,-,-)
void Cpu::ld_h_h(std::vector<unsigned char> parms) {
	registers.h = registers.h;
}
//0x65 Store register l into register h, Flags(-,-,-,-)
void Cpu::ld_h_l(std::vector<unsigned char> parms) {
	registers.h = registers.l;
}
//0x66 Store memory value at hl into register h, Flags(-,-,-,-)
void Cpu::ld_h_hlp(std::vector<unsigned char> parms) {
	registers.h = memory->readByte(registers.hl);
}
//0x67 Store register b into register h, Flags(-,-,-,-)
void Cpu::ld_h_a(std::vector<unsigned char> parms) {
	registers.h = registers.a;
}
//0x68 Store register b into register l, Flags(-,-,-,-)
void Cpu::ld_l_b(std::vector<unsigned char> parms) {
	registers.l = registers.b;
}
//0x69 Store register c into register l, Flags(-,-,-,-)
void Cpu::ld_l_c(std::vector<unsigned char> parms){
	registers.l = registers.c;
}
//0x6A Store register d into register l, Flags(-,-,-,-)
void Cpu::ld_l_d(std::vector<unsigned char> parms) {
	registers.l = registers.d;
}
//0x6B Store register e into register l, Flags(-,-,-,-)
void Cpu::ld_l_e(std::vector<unsigned char> parms) {
	registers.l = registers.e;
}
//0x6C Store register h into register l, Flags(-,-,-,-)
void Cpu::ld_l_h(std::vector<unsigned char> parms) {
	registers.l = registers.h;
}
//0x6D Store register l into register l, Flags(-,-,-,-)
void Cpu::ld_l_l(std::vector<unsigned char> parms) {
	registers.l = registers.l;
}
//0x6E Store memory value at hl into register l, Flags(-,-,-,-)
void Cpu::ld_l_hlp(std::vector<unsigned char> parms) {
	registers.l = memory->readByte(registers.hl);
}
//0x6F Store register a into register b, Flags(-,-,-,-)
void Cpu::ld_l_a(std::vector<unsigned char> parms) {
	registers.l = registers.a;
}
//0x70 Store register b into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_b(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.b);
}
//0x71 Store register c into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_c(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.c);
}
//0x72 Store register d into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_d(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.d);
}
//0x73 Store register e into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_e(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.e);
}
//0x74 Store register h into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_h(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.h);
}
//0x75 Store register l into register b, Flags(-,-,-,-)
void Cpu::ld_hlp_l(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.l);
}
//0x76 Halt, Power down CPU until interrupt occurs
void Cpu::halt(std::vector<unsigned char> parms) {
	std::cout << "ERROR, 0x76 HALT not implemented";
}
//0x77 Store register a into memory at hl, Flags(-,-,-,-)
void Cpu::ld_hlp_a(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, registers.a);
}
//0x78 Store register b into register a, Flags(-,-,-,-)
void Cpu::ld_a_b(std::vector<unsigned char> parms) {
	registers.a = registers.b;
}
//0x79 Store register c into register a, Flags(-,-,-,-)
void Cpu::ld_a_c(std::vector<unsigned char> parms) {
	registers.a = registers.c;
}
//0x7A Store register d into register a, Flags(-,-,-,-)
void Cpu::ld_a_d(std::vector<unsigned char> parms) {
	registers.a = registers.d;
}
//0x7B Store register e into register a, Flags(-,-,-,-)
void Cpu::ld_a_e(std::vector<unsigned char> parms) {
	registers.a = registers.e;
}
//0x7C Store register h into register a, Flags(-,-,-,-)
void Cpu::ld_a_h(std::vector<unsigned char> parms) {
	registers.a = registers.h;
}
//0x7D Store register l into register a, Flags(-,-,-,-)
void Cpu::ld_a_l(std::vector<unsigned char> parms) {
	registers.a = registers.l;
}
//0x7E Store memory value at hl into register a, Flags(-,-,-,-)
void Cpu::ld_a_hlp(std::vector<unsigned char> parms) {
	registers.a = memory->readByte(registers.hl);
}
//0x7F Store register a into register a, Flags(-,-,-,-)
void Cpu::ld_a_a(std::vector<unsigned char> parms) {
	registers.a = registers.a;
}
//0x80 Add register b into a, Flags(Z,0,H,C)
void Cpu::add_a_b(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, registers.b);
}
//0x81 Add register b into a, Flags(Z,0,H,C)
void Cpu::add_a_c(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, registers.c);
}
//0x82 Add register d into a, Flags(Z,0,H,C)
void Cpu::add_a_d(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, registers.d);
}
//0x83 Add register e into a, Flags(Z,0,H,C)
void Cpu::add_a_e(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, registers.e);
}
//0x84 Add register h into a, Flags(Z,0,H,C)
void Cpu::add_a_h(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, registers.h);
}
//0x85 Add register l into a, Flags(Z,0,H,C)
void Cpu::add_a_l(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, registers.l);
}
//0x86 Add memory value hl into a, Flags(Z,0,H,C)
void Cpu::add_a_hlp(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, memory->readByte(registers.hl));
}
//0x87 Add register a into a, Flags(Z,0,H,C)
void Cpu::add_a_a(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, registers.a);
}
//0x88 Add register c + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_b(std::vector<unsigned char> parms) {
	registers.a = addCarry(registers.b);
}
//0x89 Add register c + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_c(std::vector<unsigned char> parms) {
	registers.a = addCarry(registers.c);
}
//0x8A Add register d + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_d(std::vector<unsigned char> parms) {
	registers.a = addCarry(registers.d);
}
//0x8B Add register e + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_e(std::vector<unsigned char> parms) {
	registers.a = addCarry(registers.e);
}
//0x8C Add register h + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_h(std::vector<unsigned char> parms) {
	registers.a = addCarry(registers.h);
}
//0x8D Add register l + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_l(std::vector<unsigned char> parms) {
	registers.a = addCarry(registers.l);
}
//0x8E Add memory value at hl + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_hlp(std::vector<unsigned char> parms) {
	registers.a = addCarry(memory->readByte(registers.hl));
}
//0x8F Add register a + carry into register a, Flags(Z,0,H,C)
void Cpu::adc_a_a(std::vector<unsigned char> parms) {
	registers.a = addCarry(registers.a);
}
//0x90 Subtract register b from/into register a, Flags(Z,1,H,C)
void Cpu::sub_b(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, registers.b);
}
//0x91 Subtract register c from/into register a, Flags(Z,1,H,C)
void Cpu::sub_c(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, registers.c);
}
//0x92 Subtract register d from/into register a, Flags(Z,1,H,C)
void Cpu::sub_d(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, registers.d);
}
//0x93 Subtract register e from/into register a, Flags(Z,1,H,C)
void Cpu::sub_e(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, registers.e);
}
//0x94 Subtract register h from/into register a, Flags(Z,1,H,C)
void Cpu::sub_h(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, registers.h);
}
//0x95 Subtract register l from/into register a, Flags(Z,1,H,C)
void Cpu::sub_l(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, registers.l);
}
//0x96 Subtract memory value at hl from/into register a, Flags(Z,1,H,C)
void Cpu::sub_hlp(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, memory->readByte(registers.hl));
}
//0x97 Subtract register b from/into register a, Flags(Z,1,H,C)
void Cpu::sub_a(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, registers.a);
}
//0x98 Subtract register b + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_b(std::vector<unsigned char> parms) {
	registers.a = subCarry(registers.b);
}
//0x99 Subtract register c + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_c(std::vector<unsigned char> parms) {
	registers.a = subCarry(registers.c);
}
//0x9A Subtract register d + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_d(std::vector<unsigned char> parms) {
	registers.a = subCarry(registers.d);
}
//0x9B Subtract register e + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_e(std::vector<unsigned char> parms) {
	registers.a = subCarry(registers.e);
}
//0x9C Subtract register h + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_h(std::vector<unsigned char> parms) {
	registers.a = subCarry(registers.h);
}
//0x9D Subtract register l + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_l(std::vector<unsigned char> parms) {
	registers.a = subCarry(registers.l);
}
//0x9E Subtract memory value at hl + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_hlp(std::vector<unsigned char> parms) {
	registers.a = subCarry(memory->readByte(registers.hl));
}
//0x9F Subtract register a + carry from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_a(std::vector<unsigned char> parms) {
	registers.a = subCarry(registers.a);
}
//0xA0 AND register b with a into register a, Flags(Z,0,1,0)
void Cpu::and_b(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, registers.b);
}
//0xA1 AND register c with a into register a, Flags(Z,0,1,0)
void Cpu::and_c(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, registers.c);
}
//0xA2 AND register d with a into register a, Flags(Z,0,1,0)
void Cpu::and_d(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, registers.d);
}
//0xA3 AND register e with a into register a, Flags(Z,0,1,0)
void Cpu::and_e(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, registers.e);
}
//0xA4 AND register h with a into register a, Flags(Z,0,1,0)
void Cpu::and_h(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, registers.h);
}
//0xA5 AND register l with a into register a, Flags(Z,0,1,0)
void Cpu::and_l(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, registers.l);
}
//0xA6 AND memory value at hl with a into register a, Flags(Z,0,1,0)
void Cpu::and_hlp(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, memory->readByte(registers.hl));
}
//0xA7 AND register a with a into register a, Flags(Z,0,1,0)
void Cpu::and_a(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, registers.a);
}
//0xA8 XOR register b with a into register a, Flags(Z,0,0,0)
void Cpu::xor_b(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, registers.b);
}
//0xA9 XOR register c with a into register a, Flags(Z,0,0,0)
void Cpu::xor_c(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, registers.c);
}
//0xAA XOR register d with a into register a, Flags(Z,0,0,0)
void Cpu::xor_d(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, registers.d);
}
//0xAB XOR register e with a into register a, Flags(Z,0,0,0)
void Cpu::xor_e(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, registers.e);
}
//0xAC XOR register h with a into register a, Flags(Z,0,0,0)
void Cpu::xor_h(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, registers.h);
}
//0xAD XOR register l with a into register a, Flags(Z,0,0,0)
void Cpu::xor_l(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, registers.l);
}
//0xAE XOR memory value at hl with a into register a, Flags(Z,0,0,0)
void Cpu::xor_hlp(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, memory->readByte(registers.hl));
}
//0xAF XOR register a with a into register a, Flags(Z,0,0,0)
void Cpu::xor_a(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, registers.a);
}
//0xB0 OR register a with b into register a, Flags(Z,0,0,0)
void Cpu::or_b(std::vector<unsigned char> parms) {
	registers.a = or(registers.a, registers.b);
}
//0xB1 OR register a with c into register a, Flags(Z,0,0,0)
void Cpu::or_c(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, registers.c);
}
//0xB2 OR register a with d into register a, Flags(Z,0,0,0)
void Cpu::or_d(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, registers.d);
}
//0xB3 OR register a with e into register a, Flags(Z,0,0,0)
void Cpu::or_e(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, registers.e);
}
//0xB4 OR register a with h into register a, Flags(Z,0,0,0)
void Cpu::or_h(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, registers.h);
}
//0xB5 OR register a with l into register a, Flags(Z,0,0,0)
void Cpu::or_l(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, registers.l);
}
//0xB6 OR register a memory value at hl into register a, Flags(Z,0,0,0)
void Cpu::or_hlp(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, memory->readByte(registers.hl));
}
//0xB7 OR register a with a into register a, Flags(Z,0,0,0)
void Cpu::or_a(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, registers.a);
}
//0xB8 Compare register a with register b, Flags(Z,1,H,C)
void Cpu::cp_b(std::vector<unsigned char> parms) {
	compare(registers.b);
}
//0xB9 Compare register a with register c, Flags(Z,1,H,C)
void Cpu::cp_c(std::vector<unsigned char> parms) {
	compare(registers.c);
}
//0xBA Compare register a with register d, Flags(Z,1,H,C)
void Cpu::cp_d(std::vector<unsigned char> parms) {
	compare(registers.d);
}
//0xBB Compare register a with register e, Flags(Z,1,H,C)
void Cpu::cp_e(std::vector<unsigned char> parms) {
	compare(registers.e);
}
//0xBC Compare register a with register h, Flags(Z,1,H,C)
void Cpu::cp_h(std::vector<unsigned char> parms) {
	compare(registers.h);
}
//0xBD Compare register a with register l, Flags(Z,1,H,C)
void Cpu::cp_l(std::vector<unsigned char> parms) {
	compare(registers.l);
}
//0xBE Compare register a with memory value at hl, Flags(Z,1,H,C)
void Cpu::cp_hlp(std::vector<unsigned char> parms) {
	compare(memory->readByte(registers.hl));
}
//0xBF Compare register a with register a, Flags(Z,1,H,C)
void Cpu::cp_a(std::vector<unsigned char> parms) {
	compare(registers.a);
}
//0xC0 Return if zero flag is not set, Flags(-,-,-,-)
void Cpu::ret_nz(std::vector<unsigned char> parms) {
	if (!checkFlag(Flags::Zero))
		registers.pc = memory->readShortFromStack(&registers.sp);
	else
		clock.t = 8;
}
//0xC1 Pop top of Stack into register bc, Flags(-,-,-,-)
void Cpu::pop_bc(std::vector<unsigned char> parms) {
	registers.bc = memory->readShortFromStack(&registers.sp);
}
//0xC2 Jump to address nn if not zero, Flags(-,-,-,-)
void Cpu::jp_nz_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (!checkFlag(Flags::Zero))
		registers.pc = address;
	else
		clock.t = 12;
}
//0xC3 Jump to address nn, Flags(-,-,-,-)
void Cpu::jp_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	registers.pc = address;
}
//0xC4 Store pc on stack, jump to address if not zero, Flags(-,-,-,-)
void Cpu::call_nz_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (!checkFlag(Flags::Zero)) {
		memory->writeShortToStack(registers.pc, &registers.sp);
		registers.pc = address;
	}
	else
		clock.t = 12;
}
//0xC5 Push register bc onto stack Flags(-,-,-,-)
void Cpu::push_bc(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.bc, &registers.sp);
}
//0xC6 Add value n into register a, Flags(Z,0,H,C)
void Cpu::add_a_n(std::vector<unsigned char> parms) {
	registers.a = addBytes(registers.a, parms[0]);
}
//0xC7 Restart, Push pc onto stack, jump to address 0x00 + n, Flags(-,-,-,-)
void Cpu::rst_0(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x00;
}
//0xC8 Return to address off stack if zero flag is set, Flags(-,-,-,-)
void Cpu::ret_z(std::vector<unsigned char> parms) {
	if (checkFlag(Flags::Zero))
		registers.pc = memory->readShortFromStack(&registers.sp);
	else
		clock.t = 8;
}
//0xC9 Return to address off stack, Flags(-,-,-,-)
void Cpu::ret(std::vector<unsigned char> parms) {
	registers.pc = memory->readShortFromStack(&registers.sp);
}
//0xCA Jump to address nn if zero flag is set, Flags(-,-,-,-)
void Cpu::jp_z_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (checkFlag(Flags::Zero))
		registers.pc = address;
	else 
		clock.t = 12;
}
//0xCB Access point to extended opcode table, Flags(-,-,-,-)
void Cpu::cb(std::vector<unsigned char> parms) {
	cbMode = true;
}
//0xCC Store pc on stack, jump to address if zero flag set, Flags(-,-,-,-)
void Cpu::call_z_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (checkFlag(Flags::Zero)) {
		memory->writeShortToStack(registers.pc, &registers.sp);
		registers.pc = address;
	}
	else
		clock.t = 12;
}
//0xCD Store pc on stack, jump to address, Flags(-,-,-,-)
void Cpu::call_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = address;
}
//0xCE Carry add n into register a, Flags(Z,0,H,C)
void Cpu::adc_a_n(std::vector<unsigned char> parms) {
	registers.a = addCarry(parms[0]);
}
//0xCF Restart, Push pc onto stack, jump to address 0x08, Flags(-,-,-,-)
void Cpu::rst_08(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x08;
}
//0xD0 Return to address on stack, if carry flag not set, Flags(-,-,-,-)
void Cpu::ret_nc(std::vector<unsigned char> parms) {
	if (!checkFlag(Flags::Carry)) 
		registers.pc = memory->readShortFromStack(&registers.sp);
	else
		clock.t = 8;
}
//0xD1 Pop short off stack into register de, Flags(-,-,-,-)
void Cpu::pop_de(std::vector<unsigned char> parms) {
	registers.de = memory->readShortFromStack(&registers.sp);
	clock.t = 12;
}
//0xD2 Jump to address nn if carry flag not set, Flags(-,-,-,-)
void Cpu::jp_nc_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (!checkFlag(Flags::Carry))
		registers.pc = address;
	else
		clock.t = 12;
}
//0xD4 If carry flag is not set, push pc onto stack and jump to address nn, Flags(-,-,-,-)
void Cpu::call_nc_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (!checkFlag(Flags::Carry)) {
		memory->writeShortToStack(registers.pc, &registers.sp);
		registers.pc = address;
	}
	else
		clock.t = 12;
}
//0xD5 Push register de onto the stack, Flags(-,-,-,-)
void Cpu::push_de(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.de, &registers.sp);
}
//0xD6 Subtract value n from register a, Flags(Z,1,H,C)
void Cpu::sub_n(std::vector<unsigned char> parms) {
	registers.a = subBytes(registers.a, parms[0]);
}
//0xD7 Restart, push pc onto stack and jump to address 0x10, Flags(-,-,-,-)
void Cpu::rst_10(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x10;
}
//0xD8 Return to address from stack if Carry flag set, Flags(-,-,-,-)
void Cpu::ret_c(std::vector<unsigned char> parms) {
	if (checkFlag(Flags::Carry))
		registers.pc = memory->readShortFromStack(&registers.sp);
	else
		clock.t = 8;
}
//0xD9 Pop short off of stack, jump to that address, enable interrupts, Flags(-,-,-,-)
void Cpu::reti(std::vector<unsigned char> parms) {
	registers.pc = memory->readShortFromStack(&registers.sp);
	enableInterrupts();
}
//0xDA Jump to address nn if carry flag set, Flags(-,-,-,-)
void Cpu::jp_c_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (checkFlag(Flags::Carry))
		registers.pc = address;
	else
		clock.t = 12;
}
//0xDC If carry flag is set, push pc onto stack and jump to address nn, Flags(-,-,-,-)
void Cpu::call_c_nn(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	if (checkFlag(Flags::Carry)) {
		memory->writeShortToStack(registers.pc, &registers.sp);
		registers.pc = address;
	}
	else
		clock.t = 12;
}
//0xDE Sub Carry n from/into register a, Flags(Z,1,H,C)
void Cpu::sbc_a_n(std::vector<unsigned char> parms) {
	registers.a = subCarry(parms[0]);
}
//0xDF Restart, push pc onto stack and jump to address 0x18, Flags(-,-,-,-)
void Cpu::rst_18(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x18;
}
//0xE0 Put register a into memory address 0xFF00 + n, Flags(-,-,-,-)
void Cpu::ld_ffnp_a(std::vector<unsigned char> parms) {
	memory->writeByte(0xFF00 + parms[0], registers.a);
}
//0xE1 Pop short from stack into register hl, Flags(-,-,-,-)
void Cpu::pop_hl(std::vector<unsigned char> parms) {
	registers.hl = memory->readShortFromStack(&registers.sp);
}
//0xE2 Store register a into memory at address 0xFF00 + register c, Flags(-,-,-,-)
void Cpu::ld_cp_a(std::vector<unsigned char> parms) {
	memory->writeByte(0xFF00 + registers.c, registers.a);
}
//0xE5 Push register hl onto stack, Flags(-,-,-,-)
void Cpu::push_hl(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.hl, &registers.sp);
}
//0xE6 AND register a with value n, Flags(Z,0,1,0)
void Cpu::and_n(std::vector<unsigned char> parms) {
	registers.a = and(registers.a, parms[0]);
}
//0xE7 Restart, push pc onto stack and jump to address 0x20, Flags(-,-,-,-)
void Cpu::rst_20(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x20;
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
}
//0xE9 Jump to address in memory pointed at hl, Flags(-,-,-,-)
void Cpu::jp_hlp(std::vector<unsigned char> parms) {
	registers.pc = memory->readShort(registers.hl);
}
//0xEA Store register a into memory address nn, Flags(-,-,-,-)
void Cpu::ld_nnp_a(std::vector<unsigned char> parms) {
	unsigned short address = (parms[1] << 8) | parms[0];
	memory->writeByte(address, registers.a);
}
//0xEE XOR value n into register a, Flags(Z,-,-,-)
void Cpu::xor_n(std::vector<unsigned char> parms) {
	registers.a = xor(registers.a, parms[0]);
}
//0xEF Restart, push pc onto stack and jump to address 0x28, Flags(-,-,-,-)
void Cpu::rst_28(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x28;
}
//0xF0 Load memory location 0xFF00 + n into register a, Flags(-,-,-,-)//0xEF
void Cpu::ld_a_ffnp(std::vector<unsigned char> parms) {
	registers.a = memory->readByte(0xFF00 + parms[0]);
}
//0xF1 Pop short off stack into register af, Flags(Z,N,H,C)
void Cpu::pop_af(std::vector<unsigned char> parms) {
	registers.af = memory->readShortFromStack(&registers.sp);
	clock.t = 12;
}
//0xF2 Load memory location 0xFF00 + c into register a, Flags(-,-,-,-)
void Cpu::ld_a_cp(std::vector<unsigned char> parms) {
	registers.a = memory->readByte(0xFF00 + registers.c);
	
}
//0xF3 Disable interrupts, not immediate, after instruction after this is executed, Flags(-,-,-,-)
void Cpu::di(std::vector<unsigned char> parms) {
	disableInterrupts();
}
//0xF5 Push register af onto stack, Flags(-,-,-,-)
void Cpu::push_af(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.af, &registers.sp);
	clock.t = 16;
}
//0xF6 OR value n into register a, Flags(Z,0,0,0)
void Cpu::or_n(std::vector<unsigned char> parms) {
	registers.a = or (registers.a, parms[0]);
	
}
//0xF7 Restart, push pc onto stack and jump to address 0x30, Flags(-,-,-,-)
void Cpu::rst_30(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x30;
	clock.t = 16;
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
}
//0xF9 Store register hl into register sp, Flags(-,-,-,-)
void Cpu::ld_sp_hl(std::vector<unsigned char> parms) {
	registers.sp = registers.hl;
}
//0xFA Store byte at memory location nn into register a, Flags(-,-,-,-)
void Cpu::ld_a_nnp(std::vector<unsigned char> parms) {
	unsigned char address = (parms[1] << 8) | parms[0];
	registers.a = memory->readByte(address);
	clock.t = 16;
}
//0xFB Enable interrupts, not immediate, enabled after instruction after this is executed, Flags(-,-,-,-)
void Cpu::ei(std::vector<unsigned char> parms) {
	enableInterrupts();
}
//0xFE Compare value n with register a, Flags(Z,1,H,C)
void Cpu::cp_n(std::vector<unsigned char> parms) {
	compare(parms[0]);
}
//0xFF Restart, push pc onto stack and jump to address 0x38, Flags(-,-,-,-)
void Cpu::rst_38(std::vector<unsigned char> parms) {
	memory->writeShortToStack(registers.pc, &registers.sp);
	registers.pc = 0x38;
	clock.t = 16;
}

//Extended OpCode instructions

//0x00 Rotate Left Carry, Flags(Z,0,0,C)
void Cpu::rlc_b(std::vector<unsigned char> parms) {
	registers.b = rlc(registers.b);
}
//0x01 Rotate Left Carry, Flags(Z,0,0,C)
void Cpu::rlc_c(std::vector<unsigned char> parms){
	registers.c = rlc(registers.c);
}
//0x02 Rotate Left Carry, Flags(Z,0,0,C)
void Cpu::rlc_d(std::vector<unsigned char> parms) {
	registers.d = rlc(registers.d);
}
//0x03 Rotate Left Carry, Flags(Z,0,0,C)
void Cpu::rlc_e(std::vector<unsigned char> parms) {
	registers.e = rlc(registers.e);
}
//0x04 Rotate Left Carry, Flags(Z,0,0,C)
void Cpu::rlc_h(std::vector<unsigned char> parms) {
	registers.h = rlc(registers.h);
}
//0x05 Rotate Left Carry, Flags(Z,0,0,C)
void Cpu::rlc_l(std::vector<unsigned char> parms) {
	registers.l = rlc(registers.l);
}
//0x06 Rotate Left Carry, Flags(Z,0,0,C)
void Cpu::rlc_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, memory->readByte(registers.hl));
}
//0x07 Rotate Left Carry, Flags(Z,0,0,C)
void Cpu::rlc_a(std::vector<unsigned char> parms) {
	registers.a = rlc(registers.a);
}
//0x08 Rotate Right Carry, Flags(Z,0,0,C)
void Cpu::rrc_b(std::vector<unsigned char> parms) {
	registers.b = rrc(registers.b);
}
//0x09 Rotate Right Carry, Flags(Z,0,0,C)
void Cpu::rrc_c(std::vector<unsigned char> parms) {
	registers.c = rrc(registers.c);
}
//0x0A Rotate Right Carry, Flags(Z,0,0,C)
void Cpu::rrc_d(std::vector<unsigned char> parms) {
	registers.d = rrc(registers.d);
}
//0x0B Rotate Right Carry, Flags(Z,0,0,C)
void Cpu::rrc_e(std::vector<unsigned char> parms) {
	registers.e = rrc(registers.e);
}
//0x0C Rotate Right Carry, Flags(Z,0,0,C)
void Cpu::rrc_h(std::vector<unsigned char> parms) {
	registers.h = rrc(registers.h);
}
//0x0D Rotate Right Carry, Flags(Z,0,0,C)
void Cpu::rrc_l(std::vector<unsigned char> parms) {
	registers.l = rrc(registers.l);
}
//0x0E Rotate Right Carry, Flags(Z,0,0,C)
void Cpu::rrc_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, rrc(memory->readByte(registers.hl)));
}
//0x0F Rotate Right Carry, Flags(Z,0,0,C)
void Cpu::rrc_a(std::vector<unsigned char> parms) {
	registers.a = rrc(registers.a);
}
//0x10 Rotate Left, Flags(Z,0,0,C)
void Cpu::rl_b(std::vector<unsigned char> parms) {
	registers.b = rl(registers.b);
}
//0x11 Rotate Left, Flags(Z,0,0,C)
void Cpu::rl_c(std::vector<unsigned char> parms) {
	registers.c = rl(registers.c);
}
//0x12 Rotate Left, Flags(Z,0,0,C)
void Cpu::rl_d(std::vector<unsigned char> parms) {
	registers.d = rl(registers.d);
}
//0x13 Rotate Left, Flags(Z,0,0,C)
void Cpu::rl_e(std::vector<unsigned char> parms) {
	registers.e = rl(registers.e);
}
//0x14 Rotate Left, Flags(Z,0,0,C)
void Cpu::rl_h(std::vector<unsigned char> parms) {
	registers.h = rl(registers.h);
}
//0x15 Rotate Left, Flags(Z,0,0,C)
void Cpu::rl_l(std::vector<unsigned char> parms) {
	registers.l = rl(registers.l);
}
//0x16 Rotate Left, Flags(Z,0,0,C)
void Cpu::rl_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, rl(memory->readByte(registers.hl)));
}
//0x17 Rotate Left, Flags(Z,0,0,C)
void Cpu::rl_a(std::vector<unsigned char> parms) {
	registers.a = rl(registers.a);
}
//0x18 Rotate Right, Flags(Z,0,0,C)
void Cpu::rr_b(std::vector<unsigned char> parms) {
	registers.b = rr(registers.b);
}
//0x19 Rotate Right, Flags(Z,0,0,C)
void Cpu::rr_c(std::vector<unsigned char> parms) {
	registers.c = rr(registers.c);
}
//0x1A Rotate Right, Flags(Z,0,0,C)
void Cpu::rr_d(std::vector<unsigned char> parms) {
	registers.d = rr(registers.d);
}
//0x1B Rotate Right, Flags(Z,0,0,C)
void Cpu::rr_e(std::vector<unsigned char> parms) {
	registers.e = rr(registers.e);
}
//0x1C Rotate Right, Flags(Z,0,0,C)
void Cpu::rr_h(std::vector<unsigned char> parms) {
	registers.h = rr(registers.h);
}
//0x1D Rotate Right, Flags(Z,0,0,C)
void Cpu::rr_l(std::vector<unsigned char> parms) {
	registers.l = rr(registers.l);
}
//0x1E Rotate Right, Flags(Z,0,0,C)
void Cpu::rr_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, rr(memory->readByte(registers.hl)));
}
//0x1F Rotate Right, Flags(Z,0,0,C)
void Cpu::rr_a(std::vector<unsigned char> parms) {
	registers.a = rr(registers.a);
}

//0x20 Shift Left, into carry, lsb set to 0, Flags(Z,0,0,C)
void Cpu::sla_b(std::vector<unsigned char> parms) {
	registers.b = sla(registers.b);
}
//0x21 Shift Left, into carry, lsb set to 0, Flags(Z,0,0,C)
void Cpu::sla_c(std::vector<unsigned char> parms){
	registers.c = sla(registers.c);
}
//0x22 Shift Left, into carry, lsb set to 0, Flags(Z,0,0,C)
void Cpu::sla_d(std::vector<unsigned char> parms) {
	registers.d = sla(registers.d);
}
//0x23 Shift Left, into carry, lsb set to 0, Flags(Z,0,0,C)
void Cpu::sla_e(std::vector<unsigned char> parms) {
	registers.e = sla(registers.e);
}
//0x24 Shift Left, into carry, lsb set to 0, Flags(Z,0,0,C)
void Cpu::sla_h(std::vector<unsigned char> parms) {
	registers.h = sla(registers.h);
}
//0x25 Shift Left, into carry, lsb set to 0, Flags(Z,0,0,C)
void Cpu::sla_l(std::vector<unsigned char> parms) {
	registers.l = sla(registers.l);
}
//0x26 Shift Left, into carry, lsb set to 0, Flags(Z,0,0,C)
void Cpu::sla_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, sla(memory->readByte(registers.hl)));
}
//0x27 Shift Left, into carry, lsb set to 0, Flags(Z,0,0,C)
void Cpu::sla_a(std::vector<unsigned char> parms){
	registers.a = sla(registers.a);
}
//0x28 Shift Right, into carry, msb doesn't change, Flags(Z,0,0,C)
void Cpu::sra_b(std::vector<unsigned char> parms) {
	registers.b = sra(registers.b);
}
//0x29 Shift Right, into carry, msb doesn't change, Flags(Z,0,0,C)
void Cpu::sra_c(std::vector<unsigned char> parms) {
	registers.c = sra(registers.c);
}
//0x2A Shift Right, into carry, msb doesn't change, Flags(Z,0,0,C)
void Cpu::sra_d(std::vector<unsigned char> parms) {
	registers.d = sra(registers.d);
}
//0x2B Shift Right, into carry, msb doesn't change, Flags(Z,0,0,C)
void Cpu::sra_e(std::vector<unsigned char> parms) {
	registers.e = sra(registers.e);
}
//0x2C Shift Right, into carry, msb doesn't change, Flags(Z,0,0,C)
void Cpu::sra_h(std::vector<unsigned char> parms) {
	registers.h = sra(registers.h);
}
//0x2D Shift Right, into carry, msb doesn't change, Flags(Z,0,0,C)
void Cpu::sra_l(std::vector<unsigned char> parms) {
	registers.l = sra(registers.l);
}
//0x2E Shift Right, into carry, msb doesn't change, Flags(Z,0,0,C)
void Cpu::sra_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, sra(memory->readByte(registers.hl)));
}
//0x2F Shift Right, into carry, msb doesn't change, Flags(Z,0,0,C)
void Cpu::sra_a(std::vector<unsigned char> parms) {
	registers.a = sra(registers.a);
}
//0x30 Swap upper and lower nibbles, Flags(Z,0,0,0)
void Cpu::swap_b(std::vector<unsigned char> parms) {
	registers.b = swap(registers.b);
}
//0x31 Swap upper and lower nibbles, Flags(Z,0,0,0)
void Cpu::swap_c(std::vector<unsigned char> parms) {
	registers.c = swap(registers.c);
}
//0x32 Swap upper and lower nibbles, Flags(Z,0,0,0)
void Cpu::swap_d(std::vector<unsigned char> parms) {
	registers.d = swap(registers.d);
}
//0x33 Swap upper and lower nibbles, Flags(Z,0,0,0)
void Cpu::swap_e(std::vector<unsigned char> parms) {
	registers.e = swap(registers.e);
}
//0x34 Swap upper and lower nibbles, Flags(Z,0,0,0)
void Cpu::swap_h(std::vector<unsigned char> parms) {
	registers.h = swap(registers.h);
}
//0x35 Swap upper and lower nibbles, Flags(Z,0,0,0)
void Cpu::swap_l(std::vector<unsigned char> parms) {
	registers.l = swap(registers.l);
}
//0x36 Swap upper and lower nibbles, Flags(Z,0,0,0)
void Cpu::swap_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, swap(memory->readByte(registers.hl)));
}
//0x37 Swap upper and lower nibbles, Flags(Z,0,0,0)
void Cpu::swap_a(std::vector<unsigned char> parms) {
	registers.a = swap(registers.a);
}
//0x38 Shift byte right into Carry, MSB set to 0, Flags(Z,0,0,C)
void Cpu::srl_b(std::vector<unsigned char> parms) {
	registers.b = srl(registers.b);
}
//0x39 Shift byte right into Carry, MSB set to 0, Flags(Z,0,0,C)
void Cpu::srl_c(std::vector<unsigned char> parms) {
	registers.c = srl(registers.c);
}
//0x3A Shift byte right into Carry, MSB set to 0, Flags(Z,0,0,C)
void Cpu::srl_d(std::vector<unsigned char> parms) {
	registers.d = srl(registers.d);
}
//0x3B Shift byte right into Carry, MSB set to 0, Flags(Z,0,0,C)
void Cpu::srl_e(std::vector<unsigned char> parms) {
	registers.e = srl(registers.e);
}
//0x3C Shift byte right into Carry, MSB set to 0, Flags(Z,0,0,C)
void Cpu::srl_h(std::vector<unsigned char> parms) {
	registers.h = srl(registers.h);
}
//0x3D Shift byte right into Carry, MSB set to 0, Flags(Z,0,0,C)
void Cpu::srl_l(std::vector<unsigned char> parms) {
	registers.l = srl(registers.l);
}
//0x3E Shift byte right into Carry, MSB set to 0, Flags(Z,0,0,C)
void Cpu::srl_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, srl(memory->readByte(registers.hl)));
}
//0x3F Shift byte right into Carry, MSB set to 0, Flags(Z,0,0,C)
void Cpu::srl_a(std::vector<unsigned char> parms) {
	registers.a = srl(registers.a);
}
//0x40 Test bit 0, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit0_b(std::vector<unsigned char> parms) {
	bit(0, registers.b);
}
//0x41 Test bit 0, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit0_c(std::vector<unsigned char> parms) {
	bit(0, registers.c);
}
//0x42 Test bit 0, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit0_d(std::vector<unsigned char> parms) {
	bit(0, registers.d);
}
//0x43 Test bit 0, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit0_e(std::vector<unsigned char> parms) {
	bit(0, registers.e);
}
//0x44 Test bit 0, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit0_h(std::vector<unsigned char> parms) {
	bit(0, registers.h);
}
//0x45 Test bit 0, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit0_l(std::vector<unsigned char> parms) {
	bit(0, registers.l);
}
//0x46 Test bit 0, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit0_hlp(std::vector<unsigned char> parms) {
	bit(0, memory->readByte(registers.hl));
}
//0x47 Test bit 0, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit0_a(std::vector<unsigned char> parms) {
	bit(0, registers.a);
}
//0x48 Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit1_b(std::vector<unsigned char> parms) {
	bit(1, registers.b);
}
//0x49 Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit1_c(std::vector<unsigned char> parms) {
	bit(1, registers.c);
}
//0x4A Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit1_d(std::vector<unsigned char> parms) {
	bit(1, registers.d);
}
//0x4B Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit1_e(std::vector<unsigned char> parms) {
	bit(1, registers.e);
}
//0x4C Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit1_h(std::vector<unsigned char> parms) {
	bit(1, registers.h);
}
//0x4D Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit1_l(std::vector<unsigned char> parms) {
	bit(1, registers.l);
}
//0x4E Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit1_hlp(std::vector<unsigned char> parms) {
	bit(1, memory->readByte(registers.hl));
}
//0x4F Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit1_a(std::vector<unsigned char> parms) {
	bit(1, registers.a);
}
//0x50 Test bit 2, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit2_b(std::vector<unsigned char> parms) {
	bit(2, registers.b);
}
//0x51 Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit2_c(std::vector<unsigned char> parms) {
	bit(2, registers.c);
}
//0x52 Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit2_d(std::vector<unsigned char> parms) {
	bit(2, registers.d);
}
//0x53 Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit2_e(std::vector<unsigned char> parms) {
	bit(2, registers.e);
}
//0x54 Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit2_h(std::vector<unsigned char> parms) {
	bit(2, registers.h);
}
//0x55 Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit2_l(std::vector<unsigned char> parms) {
	bit(2, registers.l);
}
//0x56 Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit2_hlp(std::vector<unsigned char> parms) {
	bit(2, memory->readByte(registers.hl));
}
//0x57 Test bit 1, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit2_a(std::vector<unsigned char> parms) {
	bit(2, registers.a);
}
//0x58 Test bit 3, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit3_b(std::vector<unsigned char> parms) {
	bit(3, registers.b);
}
//0x59 Test bit 3, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit3_c(std::vector<unsigned char> parms) {
	bit(3, registers.c);
}
//0x5A Test bit 3, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit3_d(std::vector<unsigned char> parms) {
	bit(3, registers.d);
}
//0x5B Test bit 3, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit3_e(std::vector<unsigned char> parms) {
	bit(3, registers.e);
}
//0x5C Test bit 3, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit3_h(std::vector<unsigned char> parms) {
	bit(3, registers.h);
}
//0x5D Test bit 3, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit3_l(std::vector<unsigned char> parms) {
	bit(3, registers.l);
}
//0x5E Test bit 3, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit3_hlp(std::vector<unsigned char> parms) {
	bit(3, memory->readByte(registers.hl));
}
//0x5F Test bit 3, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit3_a(std::vector<unsigned char> parms) {
	bit(3, registers.a);
}
//0x60 Test bit 4, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit4_b(std::vector<unsigned char> parms) {
	bit(4, registers.b);
}
//0x61 Test bit 4, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit4_c(std::vector<unsigned char> parms) {
	bit(4, registers.c);
}
//0x62 Test bit 4, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit4_d(std::vector<unsigned char> parms) {
	bit(4, registers.d);
}
//0x63 Test bit 4, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit4_e(std::vector<unsigned char> parms) {
	bit(4, registers.e);
}
//0x64 Test bit 4, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit4_h(std::vector<unsigned char> parms) {
	bit(4, registers.h);
}
//0x65 Test bit 4, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit4_l(std::vector<unsigned char> parms) {
	bit(4, registers.l);
}
//0x66 Test bit 4, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit4_hlp(std::vector<unsigned char> parms) {
	bit(4, memory->readByte(registers.hl));
}
//0x67 Test bit 4, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit4_a(std::vector<unsigned char> parms) {
	bit(4, registers.a);
}
//0x68 Test bit 5, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit5_b(std::vector<unsigned char> parms) {
	bit(5, registers.b);
}
//0x69 Test bit 5, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit5_c(std::vector<unsigned char> parms) {
	bit(5, registers.c);
}
//0x6A Test bit 5, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit5_d(std::vector<unsigned char> parms) {
	bit(5, registers.d);
}
//0x6B Test bit 5, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit5_e(std::vector<unsigned char> parms) {
	bit(5, registers.e);
}
//0x6C Test bit 5, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit5_h(std::vector<unsigned char> parms) {
	bit(5, registers.h);
}
//0x6D Test bit 5, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit5_l(std::vector<unsigned char> parms) {
	bit(5, registers.l);
}
//0x6E Test bit 5, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit5_hlp(std::vector<unsigned char> parms) {
	bit(5, memory->readByte(registers.hl));
}
//0x6F Test bit 5, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit5_a(std::vector<unsigned char> parms) {
	bit(5, registers.a);
}
//0x70 Test bit 6, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit6_b(std::vector<unsigned char> parms) {
	bit(6, registers.b);
}
//0x71 Test bit 6, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit6_c(std::vector<unsigned char> parms) {
	bit(6, registers.c);
}
//0x72 Test bit 6, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit6_d(std::vector<unsigned char> parms) {
	bit(6, registers.d);
}
//0x73 Test bit 6, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit6_e(std::vector<unsigned char> parms) {
	bit(6, registers.e);
}
//0x74 Test bit 6, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit6_h(std::vector<unsigned char> parms) {
	bit(6, registers.h);
}
//0x75 Test bit 6, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit6_l(std::vector<unsigned char> parms) {
	bit(6, registers.l);
}
//0x76 Test bit 6, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit6_hlp(std::vector<unsigned char> parms) {
	bit(6, memory->readByte(registers.hl));
}
//0x77 Test bit 6, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit6_a(std::vector<unsigned char> parms) {
	bit(6, registers.a);
}
//0x78 Test bit 7, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit7_b(std::vector<unsigned char> parms) {
	bit(7, registers.b);
}
//0x79 Test bit 7, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit7_c(std::vector<unsigned char> parms) {
	bit(7, registers.c);
}
//0x7A Test bit 7, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit7_d(std::vector<unsigned char> parms) {
	bit(7, registers.d);
}
//0x7B Test bit 7, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit7_e(std::vector<unsigned char> parms) {
	bit(7, registers.e);
}
//0x7C Test bit 7, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit7_h(std::vector<unsigned char> parms) {
	bit(7, registers.h);
}
//0x7D Test bit 7, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit7_l(std::vector<unsigned char> parms) {
	bit(7, registers.l);
}
//0x7E Test bit 7, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit7_hlp(std::vector<unsigned char> parms) {
	bit(7, memory->readByte(registers.hl));
}
//0x7F Test bit 7, set Zero flag if bit is 0, Flags(Z,0,1,-)
void Cpu::bit7_a(std::vector<unsigned char> parms) {
	bit(7, registers.a);
}
//0x80 Reset bit 0 in register b, Flags(-,-,-,-)
void Cpu::res0_b(std::vector<unsigned char> parms) {
	registers.b = res(0b1, registers.b);
}
//0x81 Reset bit 0 in register c, Flags(-,-,-,-)
void Cpu::res0_c(std::vector<unsigned char> parms) {
	registers.c = res(0b1, registers.c);
}
//0x82 Reset bit 0 in register d, Flags(-,-,-,-)
void Cpu::res0_d(std::vector<unsigned char> parms) {
	registers.d = res(0b1, registers.d);
}
//0x83 Reset bit 0 in register e, Flags(-,-,-,-)     
void Cpu::res0_e(std::vector<unsigned char> parms) {
	registers.e = res(0b1, registers.e);
}
//0x84 Reset bit 0 in register h, Flags(-,-,-,-)     
void Cpu::res0_h(std::vector<unsigned char> parms) {
	registers.h = res(0b1, registers.h);
}
//0x85 Reset bit 0 in register bl, Flags(-,-,-,-)     
void Cpu::res0_l(std::vector<unsigned char> parms) {
	registers.l = res(0b1, registers.l);
}
//0x86 Reset bit 0 in memory at register hl, Flags(-,-,-,-)     
void Cpu::res0_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, res(0b1, memory->readByte(registers.hl)));
}
//0x87 Reset bit 0 in register a, Flags(-,-,-,-)     
void Cpu::1es0_a(std::vector<unsigned char> parms) {
	registers.a = res(0b1, registers.a);
}
//0x88 Reset bit 1 in register b, Flags(-,-,-,-)     
void Cpu::res1_b(std::vector<unsigned char> parms) {
	registers.b = res(0b10, registers.b);
}
//0x89 Reset bit 1 in register c, Flags(-,-,-,-)     
void Cpu::res1_c(std::vector<unsigned char> parms) {
	registers.c = res(0b10, registers.c);
}
//0x8A Reset bit 1 in register d, Flags(-,-,-,-)     
void Cpu::res1_d(std::vector<unsigned char> parms) {
	registers.d = res(0b10, registers.d);
}
//0x8B Reset bit 1 in register e, Flags(-,-,-,-)     
void Cpu::res1_e(std::vector<unsigned char> parms) {
	registers.e = res(0b10, registers.e);
}
//0x8C Reset bit 1 in register h, Flags(-,-,-,-)     
void Cpu::res1_h(std::vector<unsigned char> parms) {
	registers.h = res(0b10, registers.h);
}
//0x8D Reset bit 1 in register l, Flags(-,-,-,-)     
void Cpu::res1_l(std::vector<unsigned char> parms) {
	registers.l = res(0b10, registers.l);
}
//0x8E Reset bit 1 in memory at register hl, Flags(-,-,-,-)     
void Cpu::res1_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, res(0b10, memory->readByte(registers.hl)));
}
//0x8F Reset bit 1 in register a, Flags(-,-,-,-)     
void Cpu::res1_a(std::vector<unsigned char> parms) {
	registers.a = res(0b10, registers.a);
}
//0x90 Reset bit 2 in register b, Flags(-,-,-,-)     
void Cpu::res2_b(std::vector<unsigned char> parms) {
	registers. = res(0b100, registers.);
}
//0x91 Reset bit 2 in register c, Flags(-,-,-,-)     
void Cpu::res2_c(std::vector<unsigned char> parms) {
	registers.c = res(0b100, registers.c);
}
//0x92 Reset bit 2 in register d, Flags(-,-,-,-)     
void Cpu::res2_d(std::vector<unsigned char> parms) {
	registers.d = res(0b100, registers.d);
}
//0x93 Reset bit 2 in register e, Flags(-,-,-,-)     
void Cpu::res2_e(std::vector<unsigned char> parms) {
	registers.e = res(0b100, registers.e);
}
//0x94 Reset bit 2 in register h, Flags(-,-,-,-)     
void Cpu::res2_h(std::vector<unsigned char> parms) {
	registers.h = res(0b100, registers.h);
}
//0x95 Reset bit 2 in register l, Flags(-,-,-,-)     
void Cpu::res2_l(std::vector<unsigned char> parms) {
	registers.l = res(0b100, registers.l);
}
//0x96 Reset bit 2 in memory at register hl, Flags(-,-,-,-)     
void Cpu::res2_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, res(0b100, memory->readByte(registers.hl)));
}
//0x97 Reset bit 2 in register a, Flags(-,-,-,-)     
void Cpu::res2_a(std::vector<unsigned char> parms) {
	registers.a = res(0b100, registers.a);
}
//0x98 Reset bit 3 in register b, Flags(-,-,-,-)     
void Cpu::res3_b(std::vector<unsigned char> parms){
	registers. = res(0b1000, registers.);
}
//0x99 Reset bit 3 in register c, Flags(-,-,-,-)     
void Cpu::res3_c(std::vector<unsigned char> parms) {
	registers. = res(0b1000, registers.);
}
//0x9A Reset bit 3 in register d, Flags(-,-,-,-)     
void Cpu::res3_d(std::vector<unsigned char> parms) {
	registers. = res(0b1000, registers.);
}
//0x9B Reset bit 3 in register e, Flags(-,-,-,-)     
void Cpu::res3_e(std::vector<unsigned char> parms) {
	registers. = res(0b1000, registers.);
}
//0x9C Reset bit 3 in register h, Flags(-,-,-,-)     
void Cpu::res3_h(std::vector<unsigned char> parms) {
	registers. = res(0b1000, registers.);
}
//0x9D Reset bit 3 in register l, Flags(-,-,-,-)     
void Cpu::res3_l(std::vector<unsigned char> parms) {
	registers. = res(0b1000, registers.);
}
//0x9E Reset bit 3 in memory at register hl, Flags(-,-,-,-)     
void Cpu::res3_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, res(0b1000, memory->readByte(registers.hl)));
}
//0x9F Reset bit 3 in register a, Flags(-,-,-,-)     
void Cpu::res3_a(std::vector<unsigned char> parms) {
	registers. = res(0b1000, registers.);
}
//0xA0 Reset bit 4 in register b, Flags(-,-,-,-)     
void Cpu::res4_b(std::vector<unsigned char> parms) {
	registers.b = res(0b10000, registers.b);
}
//0xA1 Reset bit 4 in register c, Flags(-,-,-,-)     
void Cpu::res4_c(std::vector<unsigned char> parms) {
	registers.c = res(0b10000, registers.c);
}
//0xA2 Reset bit 4 in register d, Flags(-,-,-,-)     
void Cpu::res4_d(std::vector<unsigned char> parms) {
	registers.d = res(0b10000, registers.d);
}
//0xA3 Reset bit 4 in register e, Flags(-,-,-,-)     
void Cpu::res4_e(std::vector<unsigned char> parms) {
	registers.e = res(0b10000, registers.e);
}
//0xA4 Reset bit 4 in register h, Flags(-,-,-,-)     
void Cpu::res4_h(std::vector<unsigned char> parms) {
	registers.h = res(0b10000, registers.h);
}
//0xA5 Reset bit 4 in register l, Flags(-,-,-,-)     
void Cpu::res4_l(std::vector<unsigned char> parms) {
	registers.l = res(0b10000, registers.l);
}
//0xA6 Reset bit 4 in memory at register hl, Flags(-,-,-,-)     
void Cpu::res4_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, res(0b10000, memory->readByte(registers.hl)));
}
//0xA7 Reset bit 4 in register a, Flags(-,-,-,-)     
void Cpu::res4_a(std::vector<unsigned char> parms) {
	registers.a = res(0b10000, registers.a);
}
//0xA8 Reset bit 5 in register b, Flags(-,-,-,-)     
void Cpu::res5_b(std::vector<unsigned char> parms) {
	registers.b = res(0b100000, registers.b);
}
//0xA9 Reset bit 5 in register c, Flags(-,-,-,-)     
void Cpu::res5_c(std::vector<unsigned char> parms) {
	registers.c = res(0b100000, registers.c);
}
//0xAA Reset bit 5 in register d, Flags(-,-,-,-)     
void Cpu::res5_d(std::vector<unsigned char> parms) {
	registers.d = res(0b100000, registers.d);
}
//0xAB Reset bit 5 in register e, Flags(-,-,-,-)     
void Cpu::res5_e(std::vector<unsigned char> parms) {
	registers.e = res(0b100000, registers.e);
}
//0xAC Reset bit 5 in register h, Flags(-,-,-,-)     
void Cpu::res5_h(std::vector<unsigned char> parms) {
	registers.h = res(0b100000, registers.h);
}
//0xAD Reset bit 5 in register l, Flags(-,-,-,-)     
void Cpu::res5_l(std::vector<unsigned char> parms) {
	registers.l = res(0b100000, registers.l);
}
//0xAE Reset bit 5 in memory at register hl, Flags(-,-,-,-)     
void Cpu::res5_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, res(0b100000, memory->readByte(registers.hl)));
}
//0xAF Reset bit 5 in register a, Flags(-,-,-,-)     
void Cpu::res5_a(std::vector<unsigned char> parms) {
	registers.a = res(0b100000, registers.a);
}
//0xB0 Reset bit 6 in register b, Flags(-,-,-,-)    
void Cpu::res6_b(std::vector<unsigned char> parms) {
	registers.b = res(0b1000000, registers.b);
}
//0xB1 Reset bit 6 in register c, Flags(-,-,-,-)    
void Cpu::res6_c(std::vector<unsigned char> parms) {
	registers.c = res(0b1000000, registers.c);
}
//0xB2 Reset bit 6 in register d, Flags(-,-,-,-)    
void Cpu::res6_d(std::vector<unsigned char> parms) {
	registers.d = res(0b1000000, registers.d);
}
//0xB3 Reset bit 6 in register e, Flags(-,-,-,-)    
void Cpu::res6_e(std::vector<unsigned char> parms) {
	registers.e = res(0b1000000, registers.e);
}
//0xB4 Reset bit 6 in register h, Flags(-,-,-,-)    
void Cpu::res6_h(std::vector<unsigned char> parms) {
	registers.h = res(0b1000000, registers.h);
}
//0xB5 Reset bit 6 in register l, Flags(-,-,-,-)    
void Cpu::res6_l(std::vector<unsigned char> parms) {
	registers.l = res(0b1000000, registers.l);
}
//0xB6 Reset bit 6 in memory at register hl, Flags(-,-,-,-)    
void Cpu::res6_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, res(0b1000000, memory->readByte(registers.hl)));
}
//0xB7 Reset bit 6 in register a, Flags(-,-,-,-)    
void Cpu::res6_a(std::vector<unsigned char> parms) {
	registers.a = res(0b1000000, registers.a);
}
//0xB8 Reset bit 7 in register b, Flags(-,-,-,-)     
void Cpu::res7_b(std::vector<unsigned char> parms) {
	registers.b = res(0b10000000, registers.b);
}
//0xB9 Reset bit 7 in register c, Flags(-,-,-,-)     
void Cpu::res7_c(std::vector<unsigned char> parms) {
	registers.c = res(0b10000000, registers.c);
}
//0xBA Reset bit 7 in register d, Flags(-,-,-,-)     
void Cpu::res7_d(std::vector<unsigned char> parms) {
	registers.d = res(0b10000000, registers.d);
}
//0xBB Reset bit 7 in register e, Flags(-,-,-,-)     
void Cpu::res7_e(std::vector<unsigned char> parms) {
	registers.e = res(0b10000000, registers.e);
}
//0xBC Reset bit 7 in register h, Flags(-,-,-,-)     
void Cpu::res7_h(std::vector<unsigned char> parms) {
	registers.h = res(0b10000000, registers.h);
}
//0xBD Reset bit 7 in register l, Flags(-,-,-,-)     
void Cpu::res7_l(std::vector<unsigned char> parms) {
	registers.l = res(0b10000000, registers.l);
}
//0xBE Reset bit 7 in memory at register hl, Flags(-,-,-,-)     
void Cpu::res7_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, res(0b10000000, memory->readByte(registers.hl)));
}
//0xBF Reset bit 7 in register a, Flags(-,-,-,-)     
void Cpu::res7_a(std::vector<unsigned char> parms) {
	registers.a = res(0b10000000, registers.a);
}
//0xC0 Set bit 0 in register b, Flags(-,-,-,-)
void Cpu::set0_b(std::vector<unsigned char> parms) {
	registers.b = set(0b1, registers.b);
}
//0xC1 Set bit 0 in register c, Flags(-,-,-,-)
void Cpu::set0_c(std::vector<unsigned char> parms) {
	registers.c = set(0b1, registers.c);
}
//0xC2 Set bit 0 in register d, Flags(-,-,-,-)
void Cpu::set0_d(std::vector<unsigned char> parms) {
	registers.d = set(0b1, registers.d);
}
//0xC3 Set bit 0 in register e, Flags(-,-,-,-)     
void Cpu::set0_e(std::vector<unsigned char> parms) {
	registers.e = set(0b1, registers.e);
}
//0xC4 Set bit 0 in register h, Flags(-,-,-,-)     
void Cpu::set0_h(std::vector<unsigned char> parms) {
	registers.h = set(0b1, registers.h);
}
//0xC5 Set bit 0 in register bl, Flags(-,-,-,-)     
void Cpu::set0_l(std::vector<unsigned char> parms) {
	registers.l = set(0b1, registers.l);
}
//0xC6 Set bit 0 in memory at register hl, Flags(-,-,-,-)     
void Cpu::set0_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, set(0b1, memory->readByte(registers.hl)));
}
//0xC7 Set bit 0 in register a, Flags(-,-,-,-)     
void Cpu::1es0_a(std::vector<unsigned char> parms) {
	registers.a = set(0b1, registers.a);
}
//0xC8 Set bit 1 in register b, Flags(-,-,-,-)     
void Cpu::set1_b(std::vector<unsigned char> parms) {
	registers.b = set(0b10, registers.b);
}
//0xC9 Set bit 1 in register c, Flags(-,-,-,-)     
void Cpu::set1_c(std::vector<unsigned char> parms) {
	registers.c = set(0b10, registers.c);
}
//0xCA Set bit 1 in register d, Flags(-,-,-,-)     
void Cpu::set1_d(std::vector<unsigned char> parms) {
	registers.d = set(0b10, registers.d);
}
//0xCB Set bit 1 in register e, Flags(-,-,-,-)     
void Cpu::set1_e(std::vector<unsigned char> parms) {
	registers.e = set(0b10, registers.e);
}
//0xCC Set bit 1 in register h, Flags(-,-,-,-)     
void Cpu::set1_h(std::vector<unsigned char> parms) {
	registers.h = set(0b10, registers.h);
}
//0xCD Set bit 1 in register l, Flags(-,-,-,-)     
void Cpu::set1_l(std::vector<unsigned char> parms) {
	registers.l = set(0b10, registers.l);
}
//0xCE Set bit 1 in memory at register hl, Flags(-,-,-,-)     
void Cpu::set1_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, set(0b10, memory->readByte(registers.hl)));
}
//0xCF Set bit 1 in register a, Flags(-,-,-,-)     
void Cpu::set1_a(std::vector<unsigned char> parms) {
	registers.a = set(0b10, registers.a);
}
//0xD0 Set bit 2 in register b, Flags(-,-,-,-)     
void Cpu::set2_b(std::vector<unsigned char> parms) {
	registers. = set(0b100, registers.);
}
//0xD1 Set bit 2 in register c, Flags(-,-,-,-)     
void Cpu::set2_c(std::vector<unsigned char> parms) {
	registers.c = set(0b100, registers.c);
}
//0xD2 Set bit 2 in register d, Flags(-,-,-,-)     
void Cpu::set2_d(std::vector<unsigned char> parms) {
	registers.d = set(0b100, registers.d);
}
//0xD3 Set bit 2 in register e, Flags(-,-,-,-)     
void Cpu::set2_e(std::vector<unsigned char> parms) {
	registers.e = set(0b100, registers.e);
}
//0xD4 Set bit 2 in register h, Flags(-,-,-,-)     
void Cpu::set2_h(std::vector<unsigned char> parms) {
	registers.h = set(0b100, registers.h);
}
//0xD5 Set bit 2 in register l, Flags(-,-,-,-)     
void Cpu::set2_l(std::vector<unsigned char> parms) {
	registers.l = set(0b100, registers.l);
}
//0xD6 Set bit 2 in memory at register hl, Flags(-,-,-,-)     
void Cpu::set2_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, set(0b100, memory->readByte(registers.hl)));
}
//0xD7 Set bit 2 in register a, Flags(-,-,-,-)     
void Cpu::set2_a(std::vector<unsigned char> parms) {
	registers.a = set(0b100, registers.a);
}
//0xD8 Set bit 3 in register b, Flags(-,-,-,-)     
void Cpu::set3_b(std::vector<unsigned char> parms) {
	registers. = set(0b1000, registers.);
}
//0xD9 Set bit 3 in register c, Flags(-,-,-,-)     
void Cpu::set3_c(std::vector<unsigned char> parms) {
	registers. = set(0b1000, registers.);
}
//0xDA Set bit 3 in register d, Flags(-,-,-,-)     
void Cpu::set3_d(std::vector<unsigned char> parms) {
	registers. = set(0b1000, registers.);
}
//0xDB Set bit 3 in register e, Flags(-,-,-,-)     
void Cpu::set3_e(std::vector<unsigned char> parms) {
	registers. = set(0b1000, registers.);
}
//0xDC Set bit 3 in register h, Flags(-,-,-,-)     
void Cpu::set3_h(std::vector<unsigned char> parms) {
	registers. = set(0b1000, registers.);
}
//0xDD Set bit 3 in register l, Flags(-,-,-,-)     
void Cpu::set3_l(std::vector<unsigned char> parms) {
	registers. = set(0b1000, registers.);
}
//0xDE Set bit 3 in memory at register hl, Flags(-,-,-,-)     
void Cpu::set3_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, set(0b1000, memory->readByte(registers.hl)));
}
//0xDF Set bit 3 in register a, Flags(-,-,-,-)     
void Cpu::set3_a(std::vector<unsigned char> parms) {
	registers. = set(0b1000, registers.);
}
//0xE0 Set bit 4 in register b, Flags(-,-,-,-)     
void Cpu::set4_b(std::vector<unsigned char> parms) {
	registers.b = set(0b10000, registers.b);
}
//0xE1 Set bit 4 in register c, Flags(-,-,-,-)     
void Cpu::set4_c(std::vector<unsigned char> parms) {
	registers.c = set(0b10000, registers.c);
}
//0xE2 Set bit 4 in register d, Flags(-,-,-,-)     
void Cpu::set4_d(std::vector<unsigned char> parms) {
	registers.d = set(0b10000, registers.d);
}
//0xE3 Set bit 4 in register e, Flags(-,-,-,-)     
void Cpu::set4_e(std::vector<unsigned char> parms) {
	registers.e = set(0b10000, registers.e);
}
//0xE4 Set bit 4 in register h, Flags(-,-,-,-)     
void Cpu::set4_h(std::vector<unsigned char> parms) {
	registers.h = set(0b10000, registers.h);
}
//0xE5 Set bit 4 in register l, Flags(-,-,-,-)     
void Cpu::set4_l(std::vector<unsigned char> parms) {
	registers.l = set(0b10000, registers.l);
}
//0xE6 Set bit 4 in memory at register hl, Flags(-,-,-,-)     
void Cpu::set4_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, set(0b10000, memory->readByte(registers.hl)));
}
//0xE7 Set bit 4 in register a, Flags(-,-,-,-)     
void Cpu::set4_a(std::vector<unsigned char> parms) {
	registers.a = set(0b10000, registers.a);
}
//0xE8 Set bit 5 in register b, Flags(-,-,-,-)     
void Cpu::set5_b(std::vector<unsigned char> parms) {
	registers.b = set(0b100000, registers.b);
}
//0xE9 Set bit 5 in register c, Flags(-,-,-,-)     
void Cpu::set5_c(std::vector<unsigned char> parms) {
	registers.c = set(0b100000, registers.c);
}
//0xEA Set bit 5 in register d, Flags(-,-,-,-)     
void Cpu::set5_d(std::vector<unsigned char> parms) {
	registers.d = set(0b100000, registers.d);
}
//0xEB Set bit 5 in register e, Flags(-,-,-,-)     
void Cpu::set5_e(std::vector<unsigned char> parms) {
	registers.e = set(0b100000, registers.e);
}
//0xEC Set bit 5 in register h, Flags(-,-,-,-)     
void Cpu::set5_h(std::vector<unsigned char> parms) {
	registers.h = set(0b100000, registers.h);
}
//0xED Set bit 5 in register l, Flags(-,-,-,-)     
void Cpu::set5_l(std::vector<unsigned char> parms) {
	registers.l = set(0b100000, registers.l);
}
//0xEE Set bit 5 in memory at register hl, Flags(-,-,-,-)     
void Cpu::set5_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, set(0b100000, memory->readByte(registers.hl)));
}
//0xEF Set bit 5 in register a, Flags(-,-,-,-)     
void Cpu::set5_a(std::vector<unsigned char> parms) {
	registers.a = set(0b100000, registers.a);
}
//0xF0 Set bit 6 in register b, Flags(-,-,-,-)    
void Cpu::set6_b(std::vector<unsigned char> parms) {
	registers.b = set(0b1000000, registers.b);
}
//0xF1 Set bit 6 in register c, Flags(-,-,-,-)    
void Cpu::set6_c(std::vector<unsigned char> parms) {
	registers.c = set(0b1000000, registers.c);
}
//0xF2 Set bit 6 in register d, Flags(-,-,-,-)    
void Cpu::set6_d(std::vector<unsigned char> parms) {
	registers.d = set(0b1000000, registers.d);
}
//0xF3 Set bit 6 in register e, Flags(-,-,-,-)    
void Cpu::set6_e(std::vector<unsigned char> parms) {
	registers.e = set(0b1000000, registers.e);
}
//0xF4 Set bit 6 in register h, Flags(-,-,-,-)    
void Cpu::set6_h(std::vector<unsigned char> parms) {
	registers.h = set(0b1000000, registers.h);
}
//0xF5 Set bit 6 in register l, Flags(-,-,-,-)    
void Cpu::set6_l(std::vector<unsigned char> parms) {
	registers.l = set(0b1000000, registers.l);
}
//0xF6 Set bit 6 in memory at register hl, Flags(-,-,-,-)    
void Cpu::set6_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, set(0b1000000, memory->readByte(registers.hl)));
}
//0xF7 Set bit 6 in register a, Flags(-,-,-,-)    
void Cpu::set6_a(std::vector<unsigned char> parms) {
	registers.a = set(0b1000000, registers.a);
}
//0xF8 Set bit 7 in register b, Flags(-,-,-,-)     
void Cpu::set7_b(std::vector<unsigned char> parms) {
	registers.b = set(0b10000000, registers.b);
}
//0xF9 Set bit 7 in register c, Flags(-,-,-,-)     
void Cpu::set7_c(std::vector<unsigned char> parms) {
	registers.c = set(0b10000000, registers.c);
}
//0xFA Set bit 7 in register d, Flags(-,-,-,-)     
void Cpu::set7_d(std::vector<unsigned char> parms) {
	registers.d = set(0b10000000, registers.d);
}
//0xFB Set bit 7 in register e, Flags(-,-,-,-)     
void Cpu::set7_e(std::vector<unsigned char> parms) {
	registers.e = set(0b10000000, registers.e);
}
//0xFC Set bit 7 in register h, Flags(-,-,-,-)     
void Cpu::set7_h(std::vector<unsigned char> parms) {
	registers.h = set(0b10000000, registers.h);
}
//0xFD Set bit 7 in register l, Flags(-,-,-,-)     
void Cpu::set7_l(std::vector<unsigned char> parms) {
	registers.l = set(0b10000000, registers.l);
}
//0xFE Set bit 7 in memory at register hl, Flags(-,-,-,-)     
void Cpu::set7_hlp(std::vector<unsigned char> parms) {
	memory->writeByte(registers.hl, set(0b10000000, memory->readByte(registers.hl)));
}
//0xFF Set bit 7 in register a, Flags(-,-,-,-)     
void Cpu::set7_a(std::vector<unsigned char> parms) {
	registers.a = set(0b10000000, registers.a);
}