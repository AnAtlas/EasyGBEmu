#pragma once
class Memory {
private:


public:
	Memory();
	void writeByte(unsigned short address, unsigned char value);
	void writeShort(unsigned short address, unsigned short value);

	unsigned char readByte(unsigned short address);
	unsigned short readShort(unsigned short address);
};