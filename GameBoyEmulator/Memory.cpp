#include "Memory.hpp"
#include "Rom.hpp"
#include "Gpu.hpp"
#include "Debug.hpp"
#include <iostream>
#include <stdio.h>

unsigned char bios [] = {
	0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
	0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
	0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
	0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
	0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
	0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
	0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
	0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
	0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xF2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
	0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
	0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
	0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
	0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
	0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3c, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x4C,
	0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20,
	0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50
};

Memory::Memory(bool runBios) {
	memset(memory.totalMemory, 0, sizeof(memory.totalMemory));
	memcpy(memory.totalMemory, bios, sizeof(bios));
	inBios = runBios;
	logFileOpen = false;
	inputRow1 = 0xFF; // Start Select B A
	inputRow2 = 0xFF; // Down Up Left Right
}

void Memory::copy(unsigned short destination, unsigned short source, size_t length) {
	unsigned int i;
	for (i = 0; i < length; i++) {
		writeByte(destination + i, readByte(source + i));
	}
}

void Memory::biosFinished() {
	inBios = false;
}

void Memory::requestInterrupt(int bit) {
	unsigned char reqFlags = readByte(Address::IntFlags);
	reqFlags |= (1 << bit);
	writeByte(Address::IntFlags, reqFlags);
}

void Memory::writeByte(unsigned short address, unsigned char value) {
	if (address >= Address::Echo && address < Address::Oam) {
		memory.wram[address - Address::Echo] = value;
		memory.echo[address - Address::Echo] = value;
		return;
	}
	if ((inBios && address >= 0x100 && addressOnCartridge(address)) || (!inBios && addressOnCartridge(address))) {
		cartridge->writeByte(address, value);
		return;
	}

	//FF44 shows horizontal scanline being drawn, writing here resets to 0
	if (address == Address::LineY) {
		memory.totalMemory[Address::LineY] = 0;
		return;
	}
	//Writing to div address, resets timer to 0
	if (address == Address::DivReg) {
		memory.totalMemory[Address::DivReg] = 0;
		return;
	}
	
	//You can only write to bits 4 and 5, 6 and 7 always return 1
	if (address == Address::P1) {
		value &= 0x30;
		value |= 0xC0; //turn on bits 6 and 7
		memory.totalMemory[Address::P1] &= 0b00001111;
		memory.totalMemory[Address::P1] |= value;
		return;
	}

	if (inBios && address == Address::ExitBios)
		inBios = false;
	memory.totalMemory[address] = value;
}

void Memory::writeByteTimer(unsigned short address, unsigned char value) {
	if (address == Address::DivReg) {
		memory.totalMemory[Address::DivReg] = value;
		return;
	}
	if (address == Address::TimerCounter) {
		memory.totalMemory[Address::TimerCounter] = value;
		return;
	}
	std::cout << "writeByteTimer ERROR" << std::endl;
}

void Memory::writeByteGpu(unsigned short address, unsigned char value) {
	if (address == Address::LineY) {
		memory.totalMemory[Address::LineY] = value;
		return;
	}
	std::cout << "writeByteGpu ERROR" << std::endl;
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
	if ((inBios && address >= 0x100 && addressOnCartridge(address)) || (!inBios && addressOnCartridge(address)))
		return cartridge->readByte(address);

	if (address == Address::P1) {
		unsigned char ret = 0b11000000;
		unsigned char keyReq = memory.totalMemory[Address::P1];
		ret |= (keyReq & 0b00110000);
		if (!(keyReq & (1 << 5))) { // Directionals selected, row 2
			ret |= (inputRow2 & 0xF);
		}
		if (!(keyReq & (1 << 4))) {
			ret |= (inputRow1 & 0xF);
		}
		if ((inputRow1 & 0xF) != 0xF || (inputRow2 & 0xF) != 0xF)
			int a = 0;
		return ret;
	}

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

unsigned char* Memory::getBytePointer(unsigned short address) {
	return &(memory.totalMemory[address]);
}

void Memory::resetIO() {
	if (inBios) {
		memory.totalMemory[0xFF00] = 0xFF;
		memory.totalMemory[0xFF05] = 0x00;
		memory.totalMemory[0xFF06] = 0x00;
		memory.totalMemory[0xFF07] = 0x00;
		memory.totalMemory[0xFF10] = 0x80;
		memory.totalMemory[0xFF11] = 0x00;
		memory.totalMemory[0xFF12] = 0x00;
		memory.totalMemory[0xFF14] = 0x00;
		memory.totalMemory[0xFF16] = 0x00;
		memory.totalMemory[0xFF17] = 0x00;
		memory.totalMemory[0xFF19] = 0x00;
		memory.totalMemory[0xFF1A] = 0x00;
		memory.totalMemory[0xFF1B] = 0x00;
		memory.totalMemory[0xFF1C] = 0x00;
		memory.totalMemory[0xFF1E] = 0x00;
		memory.totalMemory[0xFF20] = 0x00;
		memory.totalMemory[0xFF21] = 0x00;
		memory.totalMemory[0xFF22] = 0x00;
		memory.totalMemory[0xFF23] = 0x00;
		memory.totalMemory[0xFF24] = 0x00;
		memory.totalMemory[0xFF25] = 0x00;
		memory.totalMemory[0xFF26] = 0x00;
		memory.totalMemory[0xFF40] = 0x00; //LCDC
		memory.totalMemory[0xFF42] = 0x00; //SCY
		memory.totalMemory[0xFF43] = 0x00; //SCX
		memory.totalMemory[0xFF45] = 0x00; //LYC
		memory.totalMemory[0xFF47] = 0x00; //BGP
		memory.totalMemory[0xFF48] = 0x00; //OBP0
		memory.totalMemory[0xFF49] = 0x00; //OBP1
		memory.totalMemory[0xFF4A] = 0x00; //WY
		memory.totalMemory[0xFF4B] = 0x00; //WX
		memory.totalMemory[0xFFFF] = 0x00; //IE
	}
	else {
		memory.totalMemory[0xFF00] = 0xFF;
		memory.totalMemory[0xFF05] = 0x00;
		memory.totalMemory[0xFF06] = 0x00;
		memory.totalMemory[0xFF07] = 0x00;
		memory.totalMemory[0xFF10] = 0x80;
		memory.totalMemory[0xFF11] = 0xBF;
		memory.totalMemory[0xFF12] = 0xF3;
		memory.totalMemory[0xFF14] = 0xBF;
		memory.totalMemory[0xFF16] = 0x3F;
		memory.totalMemory[0xFF17] = 0x00;
		memory.totalMemory[0xFF19] = 0xBF;
		memory.totalMemory[0xFF1A] = 0x7F;
		memory.totalMemory[0xFF1B] = 0xFF;
		memory.totalMemory[0xFF1C] = 0x9F;
		memory.totalMemory[0xFF1E] = 0xBF;
		memory.totalMemory[0xFF20] = 0xFF;
		memory.totalMemory[0xFF21] = 0x00;
		memory.totalMemory[0xFF22] = 0x00;
		memory.totalMemory[0xFF23] = 0xBF;
		memory.totalMemory[0xFF24] = 0x77;
		memory.totalMemory[0xFF25] = 0xF3;
		memory.totalMemory[0xFF26] = 0xF1;
		memory.totalMemory[0xFF40] = 0x91; //LCDC
		memory.totalMemory[0xFF42] = 0x00; //SCY
		memory.totalMemory[0xFF43] = 0x00; //SCX
		memory.totalMemory[0xFF45] = 0x00; //LYC
		memory.totalMemory[0xFF47] = 0xFC; //BGP
		memory.totalMemory[0xFF48] = 0xFF; //OBP0
		memory.totalMemory[0xFF49] = 0xFF; //OBP1
		memory.totalMemory[0xFF4A] = 0x00; //WY
		memory.totalMemory[0xFF4B] = 0x00; //WX
		memory.totalMemory[0xFFFF] = 0x00; //IE
	}
}

void Memory::printMemory(unsigned char arrayIndex) {
	//FILE* logFile;
	if (!logFileOpen) {
		logFile = std::fopen("memory.log", "w");
		logFileOpen = true;
	}
	
	std::fwrite(memory.vRam, sizeof(unsigned char), sizeof(memory.vRam), logFile);
	std::fclose(logFile);
}