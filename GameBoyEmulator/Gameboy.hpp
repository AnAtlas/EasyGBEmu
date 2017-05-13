#pragma once
#include <string>
#include "SFML/Graphics/RenderWindow.hpp"
#include <thread>

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
	std::thread gameboyThread;

	bool running;
	GameboyModes gameboyMode;
	sf::RenderWindow* window;
	void linkComponents();
public:
	Gameboy(sf::RenderWindow* window);
	bool insertRom(std::string romPath);
	void play();
	void shutDown();
	void runOpcode(std::string com);
};