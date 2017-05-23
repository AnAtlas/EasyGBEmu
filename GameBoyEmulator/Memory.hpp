#pragma once
#include <stdio.h>

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
	P1 = 0xFF00,			//P1 Register for reading joypad info
	DivReg = 0xFF04,		//DIV
	TimerCounter = 0xFF05,	//TIMA
	TimerModulo = 0xFF06,	//TMA
	TimerControl = 0xFF07,	//TAC
	IntFlags = 0xFF0F,
	LcdControl = 0xFF40,
	LcdStatus = 0xFF41,
	ScrollY = 0xFF42,
	ScrollX = 0xFF43,
	LineY = 0xFF44,
	LYC = 0xFF45,
	DMA = 0xFF46,
	BackgroundPalette = 0xFF47,
	ObjectPalette0 = 0xFF48,
	ObjectPalette1 = 0xFF49,
	ExitBios = 0xFF50,
	WindowY = 0xFF4A,
	WindowX = 0xFF4B,
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
	FILE* logFile;
	bool logFileOpen;
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

	//Timer specific functions
	void writeByteTimer(unsigned short address, unsigned char value);

	//Gpu specific functions
	void writeByteGpu(unsigned short address, unsigned char value);

	unsigned char* getBytePointer(unsigned short address);
	void printMemory(unsigned char arrayIndex);
};