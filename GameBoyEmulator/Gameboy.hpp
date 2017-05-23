#pragma once
#include <string>
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window.hpp"
#include <thread>

class Memory;
class Cpu;
class Rom;
class Gpu;
class Timer;

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
	Timer* timer;
	std::thread gameboyThread;

	bool running;
	GameboyModes gameboyMode;
	sf::RenderWindow* window;
	void linkComponents();
public:
	Gameboy(sf::RenderWindow* window, bool runBios);
	bool insertRom(std::string romPath);
	void play();
	void shutDown();
	void runOpcode(std::string com);
	void buttonDown(sf::Keyboard::Key);
	void buttonUp(sf::Keyboard::Key);
};