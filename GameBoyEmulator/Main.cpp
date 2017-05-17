#include <SFML/Graphics.hpp>
#include <iostream>
#include <Windows.h>
#include <thread>

#include "Gameboy.hpp"

bool running = true;

bool runBios = true;

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
		}
	}
	Sleep(200);
	gameboy->shutDown();
	return 0;
}