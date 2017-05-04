#include <SFML/Graphics.hpp>
#include <iostream>
#include <Windows.h>

#include "Cpu.hpp"


int main()
{
	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);



	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);

	Cpu* cpu = new Cpu();

	cpu->printRegisters();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		char line[100];
		std::cin.getline(line, 100);
		std::string com(line);
		cpu->runCommand(com);
		window.clear();
		window.draw(shape);
		window.display();
	}

	return 0;
}