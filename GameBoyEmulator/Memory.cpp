#include "Memory.hpp"
#include "Rom.hpp"

#include <iostream>

Memory::Memory() {
	memset(memory.totalMemory, 0, sizeof(memory.totalMemory));
}

void Memory::copy(unsigned short destination, unsigned short source, size_t length) {
	unsigned int i;
	for (i = 0; i < length; i++) {
		writeByte(destination + i, readByte(source + i));
	}
}

void Memory::writeByte(unsigned short address, unsigned char value) {
	if (address >= Address::Echo && address < Address::Oam - 1) {
		memory.wram[address - Address::Echo] = value;
	}
	if (addressOnCartridge(address)) {
		cartridge->writeByte(address, value);
		return;
	}
	memory.totalMemory[address] = value;
	std::cout << std::hex << memory.totalMemory[address];
}

void Memory::writeShort(unsigned short address, unsigned short value) {
	writeByte(address, (unsigned char)(value & 0x00FF));
	writeByte(address + 1, (unsigned char)((value & 0xFF00) >> 8));
}

void Memory::writeShortToStack(unsigned short value, unsigned short* spRegister) {
	(*spRegister) -= 2;
	writeShort(*spRegister, value);
}

unsigned char Memory::readByte(unsigned short address) {
	if (address >= Address::Echo && address < Address::Oam - 1) {
		return memory.wram[address - Address::Echo];
	}
	if (addressOnCartridge(address))
		return cartridge->readByte(address);
	return memory.totalMemory[address];
}

unsigned short Memory::readShort(unsigned short address) {
	return readByte(address) | (readByte(address + 1) << 8);
}

unsigned short Memory::readShortFromStack(unsigned short* spRegister) {
	unsigned short value = readShort(*spRegister);
	(*spRegister) += 2;
	return value;
}

bool Memory::addressOnCartridge(unsigned short address) {
	if (address <= 0x7FFF)//Rom banks
		return true;
	if (address >= 0xA000 && address <= 0xBFFF)//External ram
		return true;
	return false;
}