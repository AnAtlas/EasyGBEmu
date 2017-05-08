#pragma once

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
	
	Mem memory;
	void copy(unsigned short destination, unsigned short source, size_t length);
public:
	Memory();
	void writeByte(unsigned short address, unsigned char value);
	void writeShort(unsigned short address, unsigned short value);
	void writeShortToStack(unsigned short value, unsigned short* spRegister);

	unsigned char readByte(unsigned short address);
	unsigned short readShort(unsigned short address);
	unsigned short readShortFromStack(unsigned short* spRegister);
};