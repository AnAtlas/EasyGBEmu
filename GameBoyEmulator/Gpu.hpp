#pragma once
#include "SFML/Graphics.hpp"

struct Sprite {
	union {
		struct {
			unsigned char y;
			unsigned char x;
			unsigned char patternNumber;
			unsigned char attributes;
		};
		unsigned int block;
	};
	
	enum AttributeFlags {
		Priority = (1 << 7),
		Yflip = (1 << 6),
		Xflip = (1 << 5),
		PaletteNumber = (1 << 4)
	};
};
struct GPUTimings {
	enum {
		AccessOAM = 0x50,
		AccessVRAM = 0xAC,
		HBlank = 0xCC,
		VBlank = 0x1C8 // This happens for 10 lines
	};
};
enum GPUMode {
	HBlank = 0x00,
	VBlank = 0x01,
	OAM = 0x02,
	VRAM = 0x03
};

enum Color {
	White,
	LightGray,
	DarkGray,
	Black
};

struct RGB {
	unsigned char r, g, b, a;
};

enum LCDControlFlags {
	DisplayEnable = (1 << 7), //(0=Off, 1=On)
	WindowTileMapSelect = (1 << 6), //(0=9800-9BFF, 1=9C00-9FFF)
	WindowDisplayEnable = (1 << 5), //(0=Off, 1=On)
	BackgroundWindowTileDataSelect = (1 << 4), //(0=8800-97FF, 1=8000-8FFF)
	BackgroundTileMapSelect = (1 << 3), //(0=9800-9BFF, 1=9C00-9FFF)
	ObjectSize = (1 << 2), //Sprite Size (0=8x8, 1=8x16)
	ObjectDisplayEnable = (1 << 1), //(0=Off, 1=On)
	BackgroundDisplay = (1 << 0) //(0=Off, 1=On)
};
class Memory;

class Gpu {
private:
	GPUMode gpuMode;
	Memory* memory;
	RGB frameBuffer[160 * 144];
	unsigned char tFrameBuffer[160 * 144 * 4];
	unsigned char tiles[384][8][8];

	sf::RenderWindow* window;
	sf::Texture renderTexture;
	sf::Sprite draw;

	unsigned char* dmaTransfer; //0xFF46 Writing to this launches DMA transfer from ROM or RAM to OAM memory

	unsigned short modeClock;
	unsigned char currentLine;

	void renderScanLine();
	void setLCDMode(unsigned char mode);
	GPUMode getLCDMode();
	void requestInterrupt(int bit);
	void renderBackground();
	void renderSprites();
	void compareLYToLYC();
	void updateLCDStatus();
	Color getBackgroundPaletteShade(Color color);
	Color getObjectPaletteShade(Color color, bool paletteIndex);
public:
	Gpu(sf::RenderWindow* window);
	void linkMemory(Memory* memory);
	void step(unsigned char ticks);
	void updateTile(unsigned short address, unsigned char value);
};