#include "Gpu.hpp"
#include "Memory.hpp"
#include <iostream>
#include <stdio.h>

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
	renderTexture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	renderTexture.setSmooth(false);
	renderTexture.update(tFrameBuffer, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
	draw.setTexture(renderTexture);
}

void Gpu::linkMemory(Memory* memory) {
	this->memory = memory;
}

void Gpu::setLCDMode(unsigned char mode) {
	unsigned char stat = memory->readByte(Address::LcdStatus);
	stat &= 0xFC;
	memory->writeByte(Address::LcdStatus, stat | (mode & 0b11));
}
GPUMode Gpu::getLCDMode() {
	return (GPUMode)(memory->readByte(Address::LcdStatus) & 0b11);
}

void Gpu::requestInterrupt(int bit) {
	unsigned char reqFlags = memory->readByte(Address::IntFlags);
	reqFlags |= (1 << bit);
	memory->writeByte(Address::IntFlags, reqFlags);
}

void Gpu::step(unsigned char ticks) {
	unsigned char lcdControlRegister = memory->readByte(Address::LcdControl);
	unsigned char lcdStatus = memory->readByte(Address::LcdStatus);
	unsigned char lineY = memory->readByte(Address::LineY);

	if (!(lcdControlRegister & LCDControlFlags::DisplayEnable)) {
		memory->writeByteGpu(Address::LineY, 0x00);
		setLCDMode(GPUMode::VBlank);
		return;
	}
		
	modeClock += ticks;
	unsigned char mode = memory->readByte(Address::LcdStatus) & 0b11;
	bool reqInt = false;

	switch (mode) {
		//HBlank after last hblank, push screen to texture
	case GPUMode::HBlank:
		if (modeClock >= GPUTimings::HBlank) {
			modeClock = 0;
			memory->writeByteGpu(Address::LineY, memory->readByte(Address::LineY) + 1);
			if (memory->readByte(Address::LineY) == 143) {
				//Enter VBlank
				setLCDMode(GPUMode::VBlank);

				//Vblank interrupt
				requestInterrupt(0);
				reqInt = lcdStatus & (1 << 4);

				//Push framebuffer to screen
				memcpy(tFrameBuffer, frameBuffer, SCREEN_HEIGHT * SCREEN_WIDTH * 4);
				renderTexture.update(tFrameBuffer, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
				window->clear();
				window->draw(draw);
				window->display();
			}
			else {
				setLCDMode(GPUMode::OAM);
				reqInt = lcdStatus & (1 << 5);
			}
				
		}
		break;

	case GPUMode::VBlank:
		if (modeClock >= GPUTimings::VBlank) {
			modeClock = 0;
			memory->writeByteGpu(Address::LineY, memory->readByte(Address::LineY) + 1);
			if (memory->readByte(Address::LineY) > 153) {
				setLCDMode(GPUMode::OAM);
				reqInt = lcdStatus & (1 << 5);
				memory->writeByte(Address::LineY, 0);
			}
		}
		break;

	case GPUMode::OAM:
		if (modeClock >= GPUTimings::AccessOAM) {
			modeClock = 0;
			setLCDMode(GPUMode::VRAM);
		}
		break;

	case GPUMode::VRAM:
		if (modeClock >= GPUTimings::AccessVRAM) {
			modeClock = 0;
			setLCDMode(GPUMode::HBlank);
			reqInt = lcdStatus & (1 << 3);

			renderScanLine();
		}
	}

	if (reqInt)
		requestInterrupt(1);

	//Check for coincidence flag
	if (lineY == memory->readByte(Address::LYC)) {
		lcdStatus = memory->readByte(Address::LcdStatus) | (1 << 2);

		if (lcdStatus & (1 << 6))
			requestInterrupt(1);
	}
	else
		lcdStatus = memory->readByte(Address::LcdStatus) & 0b11111011;

	memory->writeByte(Address::LcdStatus, lcdStatus);
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

Color Gpu::getObjectPaletteShade(Color color, bool paletteIndex) {
	unsigned char objPaletteData;
	if (paletteIndex)
		objPaletteData = memory->readByte(Address::ObjectPalette1);
	else
		objPaletteData = memory->readByte(Address::ObjectPalette0);
	if (color == Color::White)
		return (Color)(objPaletteData & 0x3);
	if (color == Color::LightGray)
		return (Color)((objPaletteData & 0xC) >> 2);
	if (color == Color::DarkGray)
		return (Color)((objPaletteData & 0x30) >> 4);
	if (color == Color::Black)
		return (Color)((objPaletteData & 0xC0) >> 6);

	return Color::White;
}

void Gpu::renderScanLine() {
	renderBackground();
	renderSprites();
}

void Gpu::renderSprites() {
	
	unsigned char lcdControl = memory->readByte(Address::LcdControl);
	unsigned char lineY = memory->readByte(Address::LineY);

	//Check if sprites are enabled
	if (!(lcdControl & LCDControlFlags::ObjectDisplayEnable))
		return;

	bool use8x16 = false;
	if (lcdControl & LCDControlFlags::ObjectSize)
		use8x16 = true;

	//Only allow 40 sprites per x line
	for (int sprite = 0; sprite < 40; sprite++) {
		unsigned char index = sprite * 4;
		unsigned char yPos = memory->readByte(Address::Oam + index) - 16;
		unsigned char xPos = memory->readByte(Address::Oam + index + 1) - 8;
		unsigned char tileLocation = memory->readByte(Address::Oam + index + 2);
		unsigned char attributes = memory->readByte(Address::Oam + index + 3);

		bool yFlip = attributes & Sprite::AttributeFlags::Yflip;
		bool xFlip = attributes & Sprite::AttributeFlags::Xflip;

		int scanline = memory->readByte(Address::LineY);

		int ysize = 8;

		if (use8x16)
			ysize = 16;

		if ((scanline >= yPos) && (scanline < (yPos + ysize))) {
			int line = scanline - yPos;

			if (yFlip){
				line -= ysize;
				line *= -1;
			}
			 
			line *= 2;
			unsigned char data1 = memory->readByte((Address::Vram + (tileLocation * 16)) + line);
			unsigned char data2 = memory->readByte((Address::Vram + (tileLocation * 16)) + line + 1);

			for (int tilePixel = 7; tilePixel >= 0; tilePixel--) {
				int colorbit = tilePixel;
				if (xFlip) {
					colorbit -= 7;
					colorbit *= -1;
				}
				int colorNum = (data2 & (1 << colorbit)) >> colorbit;
				colorNum <<= 1;
				colorNum |= (data1 & (1 << colorbit)) >> colorbit;

				Color color = (Color)(colorNum);

				color = getObjectPaletteShade(color, attributes & Sprite::AttributeFlags::PaletteNumber);

				int xPix = 0 - tilePixel;
				xPix += 7;

				int pixel = xPos + xPix;

				if (attributes & Sprite::AttributeFlags::Priority) {
					if (frameBuffer[lineY * SCREEN_WIDTH + pixel].r != 255)
						continue;
				}

				frameBuffer[lineY * SCREEN_WIDTH + pixel] = palette[color];
			}
		}
	}
}

void Gpu::renderBackground() {

	//check if Background is enabled
	if (!(memory->readByte(Address::LcdControl) & LCDControlFlags::BackgroundDisplay))
		return;
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
		colorNum >>= colorBit;
		colorNum <<= 1;
		colorNum |= ((data1 & (1 << colorBit)) >> colorBit);

		Color color = (Color)(colorNum);

		frameBuffer[lineY * SCREEN_WIDTH + pixel] = palette[getBackgroundPaletteShade(color)];
	}
}