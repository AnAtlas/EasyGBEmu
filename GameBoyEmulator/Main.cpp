#include <SFML/Graphics.hpp>
#include <iostream>
#include <Windows.h>
#include <thread>

#include "Gameboy.hpp"
#include "Debug.hpp"

bool running = true;

bool runBios = false;

void consoleInput(sf::RenderWindow* window, Gameboy* gameboy) {
	while (window->isOpen()) {
		char line[100];
		std::cin.getline(line, 100);
		std::string com(line);
		gameboy->runOpcode(com);
	}
	int a = 0;
}


int main()
{
	sf::RenderWindow window(sf::VideoMode(160, 144), "EasyGBEmu!");
	window.setActive(false);

	Gameboy* gameboy = new Gameboy(&window, runBios);

	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);

	gameboy->insertRom("TestRoms/Tetris.gb");

	std::thread console(consoleInput, &window, gameboy);
	std::thread gb(&Gameboy::play, gameboy);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
				gameboy->buttonDown(event.key.code);
			if (event.type == sf::Event::KeyReleased)
				gameboy->buttonUp(event.key.code);
		}
	}
	Sleep(200);
	DebugLogMessageClose();
	gameboy->shutDown();
	return 0;
}