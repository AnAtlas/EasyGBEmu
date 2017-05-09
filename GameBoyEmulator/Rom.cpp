#include "Rom.hpp"
#include <fstream>


Rom::Rom(): romLoaded(false), romName("None") {

}

Rom::Rom(std::string romPath) {
	Rom();
	loadRom(romPath);
}

//TODO: Handle different sized carts
bool Rom::loadRom(std::string romPath) {
	std::ifstream in(romPath, std::ifstream::ate | std::ifstream::binary);
	std::ifstream::pos_type size = in.tellg();
	in.close();
	std::basic_fstream<unsigned char> romFile;
	romFile.open(romPath, std::ios::in | std::ios::binary);
	if (romFile.is_open()) {
		romFile.read(romData, size);
		romLoaded = true;
		return true;
	}
	return false;
}

unsigned char* Rom::getRomData() {
	return romData;
}