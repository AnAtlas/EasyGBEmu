#pragma once

class Rom;

enum Address {
	Cart1 = 0x0000,
	Cart2 = 0x4000,
	Vram = 0x8000,
	Sram = 0xA000,
	Wram = 0xC000,
	Echo = 0xE000,
	Oam = 0xFE00,
	Blank = 0xFEA0,
	IoPorts = 0xFF00,
	IntFlags = 0xFF0F,
	Blank2 = 0xFF4C,
	Hram = 0xFF80,
	IeRegister = 0xFFFF
};

class Memory {
private:
	union Mem {
		struct {
			unsigned char cart1[0x4000];
			unsigned char cart2[0x4000];
			unsigned char vRam[0x2000];
			unsigned char sRam[0x2000];
			unsigned char wram[0x2000];
			unsigned char echo[0x1E00];
			unsigned char oam[0xA0];
			unsigned char blank[0x60];
			unsigned char ioPorts[0x4C];
			unsigned char blank2[0x34];
			unsigned char hRam[0x80];
			unsigned char ieRegister[0x01];
		};
		unsigned char totalMemory[0xFFFF];
	};
	Rom* cartridge;
	Mem memory;
	void copy(unsigned short destination, unsigned short source, size_t length);
	bool addressOnCartridge(unsigned short address);
public:
	Memory();
	void linkRom(Rom* rom) { this->cartridge = rom; }
	void writeByte(unsigned short address, unsigned char value);
	void writeShort(unsigned short address, unsigned short value);
	void writeShortToStack(unsigned short value, unsigned short* spRegister);

	unsigned char readByte(unsigned short address);
	unsigned short readShort(unsigned short address);
	unsigned short readShortFromStack(unsigned short* spRegister);
};