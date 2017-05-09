#pragma once
#include <string>

class Rom {
private:
	bool romLoaded;
	std::string romName;
	unsigned char romData[0x8000];
public:
	Rom();
	Rom(std::string romPath);

	bool loadRom(std::string romPath);
	unsigned char * getRomData();
};