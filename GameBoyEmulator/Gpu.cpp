#include "Gpu.hpp"
#include "Memory.hpp"
#include <iostream>

const unsigned char SCREEN_WIDTH = 160;
const unsigned char SCREEN_HEIGHT = 144;

const RGB palette[4] = {
	{ 255,255,255, 255 },
	{ 192,192,192, 255 },
	{  96, 96,96, 255  },
	{  0 , 0 , 0, 255  }
};

Gpu::Gpu(sf::RenderWindow* window): window(window) {
	modeClock = 0;
	gpuMode = GPUMode::HBlank;
	int pos = 0;
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			frameBuffer[i * SCREEN_WIDTH + j] = palette[Color::White];
		}
	}

	for (int i = 0; i < 384; i++) {
		for (int j = 0; j < 8; j++)
			for (int k = 0; k < 8; k++)
				tiles[i][j][k] = 0;
	}
	//memcpy(tFrameBuffer, frameBuffer, SCREEN_HEIGHT * SCREEN_WIDTH * 4);
	renderTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	renderTexture.setSmooth(false);
	renderTexture.update(tFrameBuffer, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
	draw.setTexture(renderTexture);
}

void Gpu::linkMemory(Memory* memory) {
	this->memory = memory;
}

void Gpu::step(unsigned char ticks) {
	unsigned char lcdControlRegister = memory->readByte(Address::LcdControl);
	unsigned char lineY = memory->readByte(Address::LineY);

	if (!(lcdControlRegister && LCDControlFlags::DisplayEnable))
		return;
	modeClock += ticks;

	switch (gpuMode) {
		//HBlank after last hblank, push screen to texture
	case GPUMode::HBlank:
		if (modeClock >= GPUTimings::HBlank) {
			modeClock = 0;
			memory->writeByte(Address::LineY, memory->readByte(Address::LineY) + 1);
			if (memory->readByte(Address::LineY) == 143) {
				//Enter VBlank
				gpuMode = GPUMode::VBlank;

				//Push framebuffer to screen
				memcpy(tFrameBuffer, frameBuffer, SCREEN_HEIGHT * SCREEN_WIDTH * 4);
				renderTexture.update(tFrameBuffer, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
				
				window->clear();
				window->draw(draw);
				window->display();
			}
			else
				gpuMode = GPUMode::OAM;
		}
		break;

	case GPUMode::VBlank:
		if (modeClock >= GPUTimings::VBlank) {
			modeClock = 0;
			memory->writeByte(Address::LineY, memory->readByte(Address::LineY) + 1);
			if (memory->readByte(Address::LineY) > 153) {
				gpuMode = GPUMode::OAM;
				memory->writeByte(Address::LineY, 0);
			}
		}
		break;

	case GPUMode::OAM:
		if (modeClock >= GPUTimings::AccessOAM) {
			modeClock = 0;
			gpuMode = GPUMode::VRAM;
		}
		break;

	case GPUMode::VRAM:
		if (modeClock >= GPUTimings::AccessVRAM) {
			modeClock = 0;
			gpuMode = GPUMode::HBlank;

			renderScanLine();
		}
	}
	memory->writeByte(Address::LcdStatus, (memory->readByte(Address::LcdStatus) & 0xFC) | (gpuMode & 0x3));
}

void Gpu::updateTile(unsigned short address, unsigned char value) {
	address &= 0x1FFE;

	unsigned short tile = (address >> 4) & 551;
	unsigned short y = (address >> 1) & 7;

	unsigned char x, bitIndex;
	for (x = 0; x < 8; x++) {
		bitIndex = 1 << (7 - x);

		tiles[tile][y][x] = ((memory->readByte(address) & bitIndex) ? 1 : 0) + ((memory->readByte(address + 1) & bitIndex) ? 2 : 0);
	}
}

Color Gpu::getBackgroundPaletteShade(Color color) {
	unsigned char bgPaletteData = memory->readByte(Address::BackgroundPalette);
	if (color == Color::White)
		return (Color)((bgPaletteData) & 0x3);
	if (color == Color::LightGray)
		return (Color)(((bgPaletteData) & 0xC) >> 2);
	if (color == Color::DarkGray)
		return (Color)(((bgPaletteData) & 0x30) >> 4);
	if (color == Color::Black)
		return (Color)(((bgPaletteData) & 0xC0) >> 6);

	return Color::White;
}

void Gpu::renderScanLine() {
	/*//Check LCDControl Register for which offset the tile map is using
	unsigned char mapOffset = (*lcdControlRegister & (unsigned char)LCDControlFlags::BackgroundTileMapSelect) ? 0x1C00 : 0x1800;

	//Check which line of tiles we're at
	mapOffset += ((*lineY + *bgScrollY) & 0xFF) >> 3;

	//Which tile to start with on the x coordinate
	unsigned char lineOffset = ((*bgScrollX) >> 3);

	//which line of pixels to use in the tiles
	unsigned char y = (*lineY + *bgScrollY) & 7;

	//Where in the tileline to start
	unsigned char x = *bgScrollX & 7;

	//Where to draw on the texture
	unsigned int pixelOffset = *lineY * 160;

	//Read tile index from background map
	RGB color;
	unsigned short tile = memory->readShort(mapOffset + lineOffset + Address::Vram);

	//If the tile data set in use is 1, the indeces are signed; calculate tile offset
	if (*lcdControlRegister & LCDControlFlags::BackgroundWindowTileDataSelect)
		if (tile < 128)
			tile += 256;


	for (int i = 0; i < 160; i++) {
		Color color = (Color)(tiles[tile][y][x]);
		//Color color = Color::Black;
		frameBuffer[i + pixelOffset] = palette[getBackgroundPaletteShade(color)];
		x++;
		if (x == 8) {
			x = 0;
			lineOffset = (lineOffset + 1) & 31;
			tile = memory->readShort(mapOffset + lineOffset + Address::Vram);
		}
	}*/
	bool unsignedIndex = true;
	bool usingWindow = false;
	unsigned short tileData = 0;
	unsigned short backgroundMemory = 0;

	unsigned char lcdControl = memory->readByte(Address::LcdControl);
	unsigned char scrollX = memory->readByte(Address::ScrollX);
	unsigned char scrollY = memory->readByte(Address::ScrollY);
	unsigned char windowX = (memory->readByte(Address::WindowX)) - 7;
	unsigned char windowY = memory->readByte(Address::WindowY);
	unsigned char lineY = memory->readByte(Address::LineY);

	//std::cout << std::hex << scrollY << std::endl;
	if (scrollY == 0x4e)
		int a = 4;

	if (lcdControl & LCDControlFlags::WindowDisplayEnable) {
		if (windowY <= lineY)
			usingWindow = true;
	}
	else
		usingWindow = false;

	//Which tile data are we using?
	if (lcdControl & LCDControlFlags::BackgroundWindowTileDataSelect)
		tileData = 0x8000;
	else {
		tileData = 0x8800;
		unsignedIndex = false;
	}

	//WHich background memory?
	if (usingWindow == false) {
		if (lcdControl & LCDControlFlags::BackgroundTileMapSelect)
			backgroundMemory = 0x9C00;
		else
			backgroundMemory = 0x9800;
	}
	else {
		if (LcdControl & LCDControlFlags::WindowTileMapSelect)
			backgroundMemory = 0x9C00;
		else
			backgroundMemory = 0x9800;
	}

	//yPos = yPosition on the TileMap
	unsigned char yPos = 0;

	if (!usingWindow)
		yPos = scrollY + lineY;
	else
		yPos = lineY - windowY;

	unsigned short tileRow = (((unsigned char)(yPos / 8)) * 32);

	for (int pixel = 0; pixel < 160; pixel++) {
		unsigned char xPos = pixel + scrollX;

		if (usingWindow) {
			if (pixel >= windowX)
				xPos = pixel - windowX;
		}

		unsigned short tileCol = (xPos / 8);
		short tileNum;

		if (unsignedIndex)
			tileNum = (unsigned char)(memory->readByte(backgroundMemory + tileRow + tileCol));
		else
			tileNum = (signed char)(memory->readByte(backgroundMemory + tileCol + tileRow));

		unsigned short tileLocation = tileData;

		if (unsignedIndex)
			tileLocation += (tileNum * 16);
		else
			tileLocation += ((tileNum + 128) * 16);

		unsigned char line = yPos % 8;
		line *= 2;
		unsigned char data1 = memory->readByte(tileLocation + line);
		unsigned char data2 = memory->readByte(tileLocation + line + 1);

		int colorBit = xPos % 8;
		colorBit -= 7;
		colorBit *= -1;

		int colorNum = data2 & (1 << colorBit);
		colorNum <<= 1;
		colorNum |= data1 & (1 << colorBit);

		Color color = (Color)(colorNum);

		if (getBackgroundPaletteShade(color) != Color::White)
			int a = 0;

		frameBuffer[lineY * 160  + pixel ] = palette[getBackgroundPaletteShade(color)];
	}
}