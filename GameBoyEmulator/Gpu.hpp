#pragma once

struct Sprite {
	union {
		struct {
			unsigned char y;
			unsigned char x;
			unsigned char patternNumber;
			unsigned char flags;
		};
		unsigned int block;
	};
	
	enum Flags {
		Priority = (1 << 7),
		Yflip = (1 << 6),
		Xflip = (1 << 5),
		PaletteNumber = (1 << 4)
	};
};

enum LCDStatus {
	HBlank = 0x00,
	VBlank = 0x01,
	OAM = 0x02,
	Busy = 0x03
};

enum Color {
	White,
	LightGray,
	DarkGray,
	Black
};

class Memory;

class Gpu {
private:
	LCDStatus lCDStatus;
	Memory* memory;

public:
	Gpu();
	void linkMemory(Memory* memory) { this->memory = memory; }
};