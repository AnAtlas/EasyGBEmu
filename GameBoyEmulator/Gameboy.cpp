#include "Gameboy.hpp"
#include "Cpu.hpp"
#include "Rom.hpp"
#include "Gpu.hpp"

Gameboy::Gameboy() : running(false), gameboyMode(GameboyModes::DMG) {
	cpu = new Cpu(gameboyMode);
	rom = new Rom();
	memory = new Memory();
	gpu = new Gpu();
	linkComponents();
}

void Gameboy::linkComponents() {
	memory->linkRom(rom);
	cpu->linkMemory(memory);
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
	while (running) {
		cpu->step();
	}
}

void Gameboy::runOpcode(std::string com) {
	cpu->runCommand(com);
}
