#include "Memory.hpp"
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
	memory.totalMemory[address] = value;
	std::cout << memory.totalMemory[address];
}

void Memory::writeShort(unsigned short address, unsigned short value) {
	writeByte(address, (unsigned char)(value & 0x00FF));
	writeByte(address, (unsigned char)((value & 0xFF00) >> 8));
}

void Memory::writeShortToStack(unsigned short value, unsigned short* spRegister) {
	(*spRegister) -= 2;
	writeShort(*spRegister, value);
}

unsigned char Memory::readByte(unsigned short address) {
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