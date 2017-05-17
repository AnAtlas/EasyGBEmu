#pragma once

class Rom;
class Gpu;

enum Address {
	IntVBlank = 0x40,
	IntLCDState = 0x48,
	IntTimer = 0x50,
	IntJoypad = 0x60,
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
	LYC = 0xFF45,
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
			unsigned char vRam[0x2000]; //0x8000
			unsigned char sRam[0x2000]; //0xA000
			unsigned char wram[0x2000];
			unsigned char echo[0x1E00];
			unsigned char oam[0xA0];	//0xFE00
			unsigned char blank[0x60];
			unsigned char ioPorts[0x4C]; //0xFF00
			unsigned char blank2[0x34];
			unsigned char hRam[0x80];
			unsigned char ieRegister[0x01];
		};
		unsigned char totalMemory[0xFFFF];
	};
	Rom* cartridge;
	Gpu* gpu;
	Mem memory;
	bool inBios;
	void copy(unsigned short destination, unsigned short source, size_t length);
	bool addressOnCartridge(unsigned short address);
public:
	Memory(bool runBios);
	void linkRom(Rom* rom) { this->cartridge = rom; }
	void linkGpu(Gpu* gpu) { this->gpu = gpu; }
	void resetIO();
	void biosFinished();
	void writeByte(unsigned short address, unsigned char value);
	void writeShort(unsigned short address, unsigned short value);
	void writeShortToStack(unsigned short value, unsigned short* spRegister);

	unsigned char readByte(unsigned short address);
	unsigned short readShort(unsigned short address);
	unsigned short readShortFromStack(unsigned short* spRegister);

	unsigned char* getBytePointer(unsigned short address);
};