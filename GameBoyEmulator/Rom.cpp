#include "Rom.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>


const unsigned char scrollingNintendoGraphic[48] = {
	0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
	0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
	0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E 
};

Rom::Rom() 
	:romLoaded(false)
{

}

Rom::Rom(std::string romPath){
	Rom();
	loadRom(romPath);
}

bool Rom::loadRom(std::string romPath) {
	std::basic_fstream<unsigned char> romFile;
	romFile.open(romPath, std::ios::in | std::ios::binary);
	if (romFile.is_open()) {
		unsigned char romBank0[0x4000];
		romFile.read(romBank0, 0x4000);
		romFile.close();
		CartridgeType cartType = (CartridgeType)romBank0[CartAddress::CartridgeType];
		cartridge = generateCartridge(cartType, romPath);
		if (cartridge == nullptr)
			return false;
		cartridge->loadRom(romPath);
	}
	romLoaded = true;
	return true;
}

bool Rom::unloadRom() {
	if (!romLoaded)
		return false;
	delete cartridge;
	cartridge = nullptr;
	romLoaded = false;
	return true;
}

unsigned char Rom::readByte(unsigned short address) {
	return cartridge->readByte(address);
}

void Rom::writeByte(unsigned short address, unsigned char value) {
	cartridge->writeByte(address, value);
}

Cartridge* Rom::generateCartridge(CartridgeType cartType, std::string romPath) {
	switch (cartType) {
	case CartridgeType::RomOnly:		return new RomOnly(false, false); break; //0x00
	case CartridgeType::MBC1:			return new MBC1(false, false); break; //0x01
	case CartridgeType::MBC1Ram:		return new MBC1(true, false); break; //0x02
	case CartridgeType::MBC1RamBattery: return new MBC1(true, true); break; //0x03
	case CartridgeType::MBC2:			return new MBC2(false, false); break; //0x05
	case CartridgeType::MBC2RamBattery: return new MBC2(true, true); break; //0x06
	case CartridgeType::RomRam:			return new RomOnly(true, false); break; //0x08
	case CartridgeType::RomRamBattery:  return new RomOnly(true, true); break; //0x09
		//Skipping MMM01's for now
	case CartridgeType::MBC3TimerBattery: return new MBC3(false, true, true); break; //0x0F
	case CartridgeType::MBC3RamTimerBattery: return new MBC3(true, true, true); break; //0x10
	case CartridgeType::MBC3:			return new MBC3(false, false, false); break; //0x11
	case CartridgeType::MBC3Ram:		return new MBC3(true, false, false); break; //0x12
	case CartridgeType::MBC3RamBattery: return new MBC3(true, true, false); break; //0x13
	default:
		std::cout << "Cartridge Type Not Supported! " << (unsigned char)cartType << std::endl;
		return nullptr;
	}
}

//CARTRIDGE CLASS


Cartridge::Cartridge() : selectedRomRamBank(1)
{
	romRamMode = RomRamMode::RomBankingMode;
	hasTimer = false;
	hasBattery = false;
	hasRam = false;
}

void Cartridge::loadRom(std::string romPath) {
	this->romPath = romPath;
	loadBank0();
	loadBankX();
}

bool Cartridge::checkScrollingGraphic() {
	for (int i = 0; i < scrollingNintendoSize; i++) {
		if (romBank0[scrollingNintendoAddress + i] != scrollingNintendoGraphic[i])
			return false;
	}
	return true;
}

bool Cartridge::loadBank0() {
	std::basic_fstream<unsigned char> romFile;
	romFile.open(romPath, std::ios::in | std::ios::binary);
	if (romFile.is_open()) {
		unsigned char temp[0x4000];
		romFile.read(temp, 0x4000);
		romBank0 = std::vector<unsigned char>(std::begin(temp), std::end(temp));
		char tempName[16];
		for (int i = 0; i < 16; i++) {
			tempName[i] = (char)(romBank0[0x134 + i]);
		}
		romName = std::string(tempName);
	}
	else {
		std::cout << "Error Opening File: " << romPath << std::endl;
		return false;
	}
	romFile.close();

	if (!checkScrollingGraphic()) {
		std::cout << "ERROR: Rom failed to pass scrolling nintendo graphic check" << std::endl;
		return false;
	}
	romSize = (RomSize)romBank0[CartAddress::RomSize];
	ramSize = (RamSize)romBank0[CartAddress::RamSize];
	return true;
}

//load ROM bank dependant on the selectedRomBank
void Cartridge::loadBankX() {
	unsigned char bankIndex = 0;
	std::basic_fstream<unsigned char> romFile;
	romFile.open(romPath, std::ios::in | std::ios::binary);
	if (romRamMode == RomRamMode::RomBankingMode)
		bankIndex = selectedRomRamBank & 0b1111111;
	else
		bankIndex = selectedRomRamBank & 0b11111;
	if (romFile.is_open()) {
		unsigned char temp[0x4000];
		romFile.seekp(bankIndex * 0x4000);
		romFile.read(temp, 0x4000);
		romBankX = std::vector<unsigned char>(std::begin(temp), std::end(temp));
	}
}

void Cartridge::saveRamBank() {
	unsigned char bankIndex = 0;
	if (romRamMode == RomRamMode::RamBankingMode)
		bankIndex = selectedRomRamBank & 0b1100000;
	//std::copy(sRamBankX.begin(), sRamBankX.end(), sRamTotal.begin() + bankIndex * 0x2000);
}
//load RAM bank dependant on the selectedRomRamBank
void Cartridge::loadRamBankX() {
	unsigned char bankIndex = 0;
	if (romRamMode == RomRamMode::RamBankingMode)
		bankIndex = selectedRomRamBank & 0b1100000;
	//std::copy(sRamTotal.begin() + bankIndex * 0x2000, sRamTotal.begin() + 0x2000 + bankIndex * 0x2000, sRamBankX);
}

unsigned char Cartridge::readByte(unsigned short address) {
	if (address >= CartAddress::RomBank0 && address <= CartAddress::RomBank0End)
		return romBank0[address];
	if (address >= CartAddress::RomBankX && address <= CartAddress::RomBankXEnd)
		return romBankX[address];
	if (address >= CartAddress::SRamBankX && address <= CartAddress::SRamBankXEnd) {
		if (hasRam && ramEnabled) {
			unsigned short ramSizeBytes = 8192;
			if (ramSize == RamSize::kB_0)
				ramSizeBytes = 0;
			else if (ramSize == RamSize::kB_2)
				ramSizeBytes = 2048;
			if (address - CartAddress::SRamBankX <= ramSizeBytes)
				return sRamBankX[address - CartAddress::SRamBankX];
		}
	}
	return 0xFF;
}


//Handles ramEnabling, and writes to sRam, returns true if handled
bool Cartridge::writeByte(unsigned short address, unsigned char value) {
	if (address >= CartAddress::RamEnable && address <= CartAddress::RamEnableEnd) {
		if ((value & 0xF) == 0xA)
			ramEnabled = true;
		else
			ramEnabled = false;
		return true;
	}
	if (address >= CartAddress::SRamBankX && address <= CartAddress::SRamBankXEnd) {
		if (hasRam && ramEnabled) {
			unsigned short ramSizeBytes = 8192;
			if (ramSize == RamSize::kB_0)
				ramSizeBytes = 0;
			else if (ramSize == RamSize::kB_2)
				ramSizeBytes = 2048;
			if (address - CartAddress::SRamBankX <= ramSizeBytes) {
				sRamBankX[address - CartAddress::SRamBankX] = value;
				return true;
			}
		}
	}
	return false;
}

RomOnly::RomOnly(bool hasRam, bool hasBattery){
	this->hasRam = hasRam;
	this->hasBattery = hasBattery;
}

MBC1::MBC1(bool hasRam, bool hasBattery) {
	this->hasRam = hasRam;
	this->hasBattery = hasBattery;
}


bool MBC1::writeByte(unsigned short address, unsigned char value) {
	if (Cartridge::writeByte(address, value))
		return true;
	if (address >= CartAddress::RomBankSelect && address <= CartAddress::RomBankSelectEnd) {
		//only look at bottom 5 bits of value
		unsigned char temp = value & 0b11111;
		if (temp == 0)
			temp = 1;
		selectedRomRamBank = (selectedRomRamBank & 0b1100000) | temp;
		loadBankX();
		return true;
	}
	if (address >= CartAddress::RamRomBankSelect && address <= CartAddress::RamRomBankSelectEnd) {
		unsigned char temp = value & 0b11;
		saveRamBank();
		selectedRomRamBank = (selectedRomRamBank & 0b11111) | (temp << 5);
		if (romRamMode == RomRamMode::RomBankingMode)
			loadBankX();
		else
			loadRamBankX();
	}
	if (address >= CartAddress::RomRamModeSelect && address <= CartAddress::RomRamModeSelectEnd) {
		unsigned char temp = value & 0b1;
		if (temp == (unsigned char)romRamMode)
			return true;
		saveRamBank();
		if (temp)
			romRamMode = RomRamMode::RomBankingMode;
		else
			romRamMode = RomRamMode::RamBankingMode;
		loadBankX();
		loadRamBankX();
	}
	return false;
}

MBC2::MBC2(bool hasRam, bool hasBattery) {
	this->hasRam = hasRam;
	this->hasBattery = hasBattery;
}

//MBC2 Ram only has 4 lines for address lines, so only 16 roms are used and Ram Area ends at 0xA1FF
unsigned char MBC2::readByte(unsigned short address) {
	if (address >= CartAddress::RomBank0 && address <= CartAddress::RomBankXEnd) {
		return Cartridge::readByte(address);
	}
	if (address >= CartAddress::SRamBankX && address <= CartAddress::SRamBankXEnd) {
		if (address <= 0xA1FF)//Valid
			return sRamBankX[address - CartAddress::SRamBankX];
	}
	return 0xFF;
}

//MBC2 Ram only has 4 lines for address lines, so only 16 roms are used and Ram Area ends at 0xA1FF
bool MBC2::writeByte(unsigned short address, unsigned char value) {
	if (address >= CartAddress::RomBank0 && address <= CartAddress::RomBankXEnd) {
		if (Cartridge::writeByte(address, value))
			return true;
	}
	if (address >= CartAddress::SRamBankX && address <= CartAddress::SRamBankXEnd) {
		if (address <= 0xA1FF) {//Valid
			if (ramEnabled) {
				sRamBankX[address - CartAddress::SRamBankX] = value;
				return true;
			}
		}
	}
	if (address >= CartAddress::RomBankSelect && address <= CartAddress::RomBankSelectEnd) {
		//only look at bottom 5 bits of value
		unsigned char temp = value & 0b1111;
		if (temp == 0)
			temp = 1;
		selectedRomRamBank = (selectedRomRamBank & 0b1100000) | temp;
		loadBankX();
		return true;
	}
	return false;
}





MBC3::MBC3(bool hasRam, bool hasBattery, bool hasTimer) {
	this->hasRam = hasRam;
	this->hasBattery = hasBattery;
	this->hasTimer = hasTimer;
	this->selectedRamBank = 0;
	this->selectedRomBank = 1;
}

void MBC3::latchTime() {
	//GET CURRENT TIME AND INSERT INTO REGISTERS
}

//load ROM bank dependant on the selectedRomBank
void MBC3::loadBankX() {
	std::basic_fstream<unsigned char> romFile;
	romFile.open(romPath, std::ios::in | std::ios::binary);
	if (romFile.is_open()) {
		unsigned char temp[0x4000];
		romFile.seekp(selectedRomBank * 0x4000);
		romFile.read(temp, 0x4000);
		romBankX = std::vector<unsigned char>(std::begin(temp), std::end(temp));
	}
}

//load RAM bank dependant on the selectedRomRamBank
void MBC3::loadRamBankX() {
	//if (selectedRamBank <= 0x07)
		//std::copy(sRamTotal.begin() + selectedRamBank * 0x2000, sRamTotal.begin() + 0x2000 + selectedRamBank * 0x2000, sRamBankX);
}

unsigned char MBC3::readByte(unsigned short address) {
	if (selectedRamBank <= 0x07 || (address < CartAddress::SRamBankX || address > CartAddress::SRamBankXEnd))
		return Cartridge::readByte(address);
	if (selectedRamBank == 0x08)
		return rtc.seconds;
	if (selectedRamBank == 0x09)
		return rtc.minutes;
	if (selectedRamBank == 0x0A)
		return rtc.hours;
	if (selectedRamBank == 0x0B)
		return rtc.lowerDayCounter;
	if (selectedRamBank == 0x0C)
		return rtc.upperDayCounterAndFlags;
	return 0xFF;
}
//Has real time clock, ALSO can access rom banks 20h, 40h, and 60h.
bool MBC3::writeByte(unsigned short address, unsigned char value) {
	if ((address >= CartAddress::RomBank0 && address <= CartAddress::RomBankXEnd) ||
		(address >= CartAddress::RamEnable && address <= CartAddress::RamEnableEnd)) {
		if (Cartridge::writeByte(address, value))
			return true;
	}
	if (address >= CartAddress::SRamBankX && address <= CartAddress::SRamBankXEnd) {
		if (ramEnabled) {
			if (selectedRamBank <= 0x07) {
				sRamBankX[address - CartAddress::SRamBankX] = value;
				return true;
			}
			else {
				if (selectedRamBank > 0x0C)
					return false;
				if (selectedRamBank == 0x08)
					rtc.seconds = value;
				else if (selectedRamBank == 0x09)
					rtc.minutes = value;
				else if (selectedRamBank == 0x0A)
					rtc.hours = value;
				else if (selectedRamBank == 0x0D)
					rtc.lowerDayCounter = value;
				else if (selectedRamBank == 0x0C)
					rtc.upperDayCounterAndFlags = value;
			}
		}
	}
	if (address >= CartAddress::RomBankSelect && address <= CartAddress::RomBankSelectEnd) {
		selectedRomBank = value;
		if (selectedRomBank == 0)
			selectedRomBank = 1;
		loadBankX();
		return true;
	}
	//In this cart, this only selects the RAM/RTC 
	if (address >= CartAddress::RamRomBankSelect && address <= CartAddress::RamRomBankSelectEnd) {
		saveRamBank();
		selectedRamBank = value;
		loadRamBankX();
	}
	if (address >= CartAddress::RomRamModeSelect && address <= CartAddress::RomRamModeSelectEnd) {
		if (value == 0x01) {
			if (!latchClockEnabled) {
				latchClockEnabled = true;
				return true;
			}
			latchTime();
		}
		else
			latchClockEnabled = false;
	}
	return false;
}