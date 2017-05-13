#include "Gameboy.hpp"
#include "Cpu.hpp"
#include "Rom.hpp"
#include "Gpu.hpp"
#include <Windows.h>

Gameboy::Gameboy(sf::RenderWindow* window) : window(window),running(false), gameboyMode(GameboyModes::DMG) {
	cpu = new Cpu(gameboyMode);
	rom = new Rom();
	memory = new Memory();
	gpu = new Gpu(window);
	linkComponents();
}

void Gameboy::linkComponents() {
	memory->linkRom(rom);
	cpu->linkMemory(memory);
	gpu->linkMemory(memory);
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
