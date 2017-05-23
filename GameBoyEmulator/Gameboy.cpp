#include "Gameboy.hpp"
#include "Cpu.hpp"
#include "Rom.hpp"
#include "Gpu.hpp"
#include "Timer.hpp"

#include <Windows.h>

Gameboy::Gameboy(sf::RenderWindow* window, bool runBios) : window(window),running(false), gameboyMode(GameboyModes::DMG) {
	memory = new Memory(runBios);
	cpu = new Cpu(gameboyMode, memory, runBios);
	rom = new Rom();
	gpu = new Gpu(window);
	timer = new Timer();
	linkComponents();
}

void Gameboy::linkComponents() {
	memory->linkRom(rom);
	gpu->linkMemory(memory);
	timer->linkMemory(memory);
	memory->linkGpu(gpu);
}

bool Gameboy::insertRom(std::string romPath) {
	if (!running) {
		if (!rom->loadRom(romPath))
			return false;
	}
	return false;
}

void Gameboy::play() {
	running = true;
	unsigned char ticks;
	while (running) {
		ticks = cpu->step();
		timer->step(ticks);
		gpu->step(ticks);
	}
}

void Gameboy::shutDown() {
	running = false;
	Sleep(100);
	delete gpu;
	gpu = nullptr;
	delete cpu;
	cpu = nullptr;
	delete memory;
	memory = nullptr;
	delete rom;
	rom = nullptr;
}
void Gameboy::runOpcode(std::string com) {
	cpu->runCommand(com);
}

void Gameboy::buttonDown(sf::Keyboard::Key key) {
	unsigned char keyReq = memory->readByte(Address::P1);
	bool button = false;
	bool requestInterrupt = false;
	bool previouslyUnpressed = false;

	if (key == sf::Keyboard::Left) {
		if (memory->inputRow2 & 0b00000010)
			previouslyUnpressed = true;
		memory->inputRow2 &= 0b11111101;
	}
	if (key == sf::Keyboard::Right) {
		if (memory->inputRow2 & 0b00000001)
			previouslyUnpressed = true;
		memory->inputRow2 &= 0b11111110;
	}
	if (key == sf::Keyboard::Up) {
		if (memory->inputRow2 & 0b00000100)
			previouslyUnpressed = true;
		memory->inputRow2 &= 0b11111011;
	}
	if (key == sf::Keyboard::Down) {
		if (memory->inputRow2 & 0b00001000)
			previouslyUnpressed = true;
		memory->inputRow2 &= 0b11110111;
	}
	if (key == sf::Keyboard::Z) { // B
		if (memory->inputRow1 & 0b00000010)
			previouslyUnpressed = true;
		memory->inputRow1 &= 0b11111101;
		button = true;
	}
	if (key == sf::Keyboard::X) { // A
		if (memory->inputRow1 & 0b00000001)
			previouslyUnpressed = true;
		memory->inputRow1 &= 0b11111110;
		button = true;
	}
	if (key == sf::Keyboard::Return) { // Start
		if (memory->inputRow1 & 0b00001000)
			previouslyUnpressed = true;
		memory->inputRow1 &= 0b11110111;
		button = true;
	}
	if (key == sf::Keyboard::BackSlash) { // Select
		if (memory->inputRow1 & 0b00000100)
			previouslyUnpressed = true;
		memory->inputRow1 &= 0b11111011;
		button = true;
	}

	if (button && !(keyReq & (1 << 5)))
		requestInterrupt = true;
	else if (!button && !(keyReq & (1 << 4)))
		requestInterrupt = true;

	if (requestInterrupt && previouslyUnpressed)
		memory->requestInterrupt(4);
}

void Gameboy::buttonUp(sf::Keyboard::Key key) {
	if (key == sf::Keyboard::Left) {
		memory->inputRow2 |= 0b00000010;
	}
	if (key == sf::Keyboard::Right) {
		memory->inputRow2 |= 0b00000001;
	}
	if (key == sf::Keyboard::Up) {
		memory->inputRow2 |= 0b00000100;
	}
	if (key == sf::Keyboard::Down) {
		memory->inputRow2 |= 0b00001000;
	}
	if (key == sf::Keyboard::Z) { // B
		memory->inputRow1 |= 0b00000010;
	}
	if (key == sf::Keyboard::X) { // A
		memory->inputRow1 |= 0b00000001;
	}
	if (key == sf::Keyboard::Return) { // Start
		memory->inputRow1 |= 0b00001000;
	}
	if (key == sf::Keyboard::BackSlash) { // Select
		memory->inputRow1 |= 0b00000100;
	}
}
