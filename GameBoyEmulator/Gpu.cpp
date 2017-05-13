#include "Gpu.hpp"
#include "Memory.hpp"

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
	memcpy(tFrameBuffer, frameBuffer, SCREEN_HEIGHT * SCREEN_WIDTH * 4);
	renderTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	renderTexture.setSmooth(false);
	renderTexture.update(tFrameBuffer, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
	draw.setTexture(renderTexture);
}

void Gpu::linkMemory(Memory* memory) {
	this->memory = memory;

	lcdControlRegister = memory->getBytePointer(0xFF40);
	lcdStatusRegister = memory->getBytePointer(0xFF41);
	bgScrollY = memory->getBytePointer(0xFF42);
	bgScrollX = memory->getBytePointer(0xFF43);
	lineY = memory->getBytePointer(0xFF44);
	lineYCompare = memory->getBytePointer(0xFF45);
	windowY = memory->getBytePointer(0xFF4A);
	windowX = memory->getBytePointer(0xFF4B);

	bgPaletteData = memory->getBytePointer(0xFF47);
	obPalette1Data = memory->getBytePointer(0xFF48);
	obPalette2Data = memory->getBytePointer(0xFF49);

	dmaTransfer = memory->getBytePointer(0xFF46);
}

void Gpu::step(unsigned char ticks) {
	modeClock += ticks;

	switch (gpuMode) {
		//HBlank after last hblank, push screen to texture
	case GPUMode::HBlank:
		if (modeClock >= GPUTimings::HBlank) {
			modeClock = 0;
			(*lineY)++;
			if (*lineY == 143) {
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
			(*lineY)++;
			if (*lineY > 153) {
				gpuMode = GPUMode::OAM;
				*lineY = 0;
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
	*lcdStatusRegister = (*lcdStatusRegister & 0xFC) | (gpuMode & 0x3);
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
	if (color == Color::White)
		return (Color)((*bgPaletteData) & 0x3);
	if (color == Color::LightGray)
		return (Color)((*bgPaletteData) & 0xC);
	if (color == Color::DarkGray)
		return (Color)((*bgPaletteData) & 0x30);
	if (color == Color::Black)
		return (Color)((*bgPaletteData) & 0xC0);

	return Color::White;
}

void Gpu::renderScanLine() {

	//Check LCDControl Register for which offset the tile map is using
	unsigned char mapOffset = (*lcdControlRegister & (unsigned char)LCDControlFlags::BackgroundTileMapSelect) ? 0x1C00 : 0x1800;

	//Check which line of tiles we're at
	mapOffset += ((*lineY + *bgScrollY) & 0xFF) >> 3;

	//Which tile to start with on the x coordinate
	unsigned char lineOffset = (*bgScrollX >> 3);

	//which line of pixels to use in the tiles
	unsigned char y = (*lineY + *bgScrollY) & 7;

	//Where in the tileline to start
	unsigned char x = *bgScrollX & 7;

	//Where to draw on the texture
	unsigned char pixelOffset = *lineY * 160;

	//Read tile index from background map
	RGB color;

	unsigned short tile = memory->readShort(mapOffset + lineOffset + Address::Vram);

	//If the tile data set in use is 1, the indeces are signed; calculate tile offset
	if (*lcdControlRegister & LCDControlFlags::BackgroundWindowTileDataSelect)
		if (tile < 128)
			tile += 256;


	for (int i = 0; i < 160; i++) {
		Color color = (Color)(tiles[tile][y][x]);

		frameBuffer[pixelOffset] = palette[getBackgroundPaletteShade(color)];
		x++;
		if (x == 8) {
			x = 0;
			lineOffset = (lineOffset + 1) & 31;
			tile = memory->readShort(mapOffset + lineOffset + Address::Vram);
		}
	}
}