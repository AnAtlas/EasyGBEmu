#pragma once
#include <string>

class Memory;
class Cpu;
class Rom;
class Gpu;

enum GameboyModes {
	DMG,
	MGB,
	SGB,
	CGB
};
class Gameboy {
private:
	Cpu* cpu;
	Rom* rom;
	Memory* memory;
	Gpu* gpu;

	bool running;
	GameboyModes gameboyMode;

	void linkComponents();
public:
	Gameboy();
	bool insertRom(std::string romPath);
	void play();

	void runOpcode(std::string com);
};