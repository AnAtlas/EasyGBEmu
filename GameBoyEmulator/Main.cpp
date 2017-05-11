#include <SFML/Graphics.hpp>
#include <iostream>
#include <Windows.h>
#include <thread>

#include "Gameboy.hpp"


void consoleInput(sf::RenderWindow* window, Gameboy* gameboy) {
	while (window->isOpen()) {
		char line[100];
		std::cin.getline(line, 100);
		std::string com(line);
		gameboy->runOpcode(com);
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(160,144), "EasyGBEmu!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	Gameboy* gameboy = new Gameboy();

	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);

	gameboy->insertRom("Tetris.gb");

	std::thread console(consoleInput, &window, gameboy);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear();
		window.draw(shape);
		window.display();
		gameboy->play();
	}

	return 0;
}