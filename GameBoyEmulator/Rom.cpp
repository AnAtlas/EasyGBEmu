#include "Rom.hpp"
#include <fstream>


Rom::Rom(): romLoaded(false), romName("None") {

}

Rom::Rom(std::string romPath) {
	Rom();
	loadRom(romPath);
}

void Rom::loadRom(std::string romPath) {
	std::basic_fstream<unsigned char> romFile;
	romFile.open(romPath, std::ios::in | std::ios::binary);
	if (romFile.is_open()) {
		romFile.read(romData, 0x8000);
	}
}

unsigned char* Rom::getRomData() {
	return romData;
}