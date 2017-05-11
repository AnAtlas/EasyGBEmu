#pragma once
#include <string>
#include <vector>

struct CartAddress{
	enum {
		RomBank0 = 0x0000,
		RomBank0End = 0x3FFF,
		RomBankX = 0x4000,
		RomBankXEnd = 0x7FFF,
		SRamBankX = 0xA000,
		SRamBankXEnd = 0xBFFF,
		RamEnable = 0x0000, //Writing here enables/disables Ram
		RamEnableEnd = 0x1FFF,
		RomBankSelect = 0x2000, //Writing here selects the lower 5 bits of rom bank number
		RomBankSelectEnd = 0x3FFF,
		RamRomBankSelect = 0x4000, // Writing here either selects ram bank, or upper 2 bits of rom bank
		RamRomBankSelectEnd = 0x5FFF,
		RomRamModeSelect = 0x6000, // Writing here determines which mode ramRomBankSelect is in
		RomRamModeSelectEnd = 0x7FFF,
		Name = 0x134,
		CartridgeType = 0x147,
		RomSize = 0x148,
		RamSize = 0x149
	};
};

enum class CartridgeType {
	RomOnly = 0x00,
	MBC1 = 0x01,
	MBC1Ram = 0x02,
	MBC1RamBattery = 0x03,
	MBC2 = 0x05,
	MBC2RamBattery = 0x06,
	RomRam = 0x08,
	RomRamBattery = 0x09,
	MMM01 = 0x0B,
	MMM01Ram = 0x0C,
	MMM01RamBattery = 0x0D,
	MBC3TimerBattery = 0x0F,
	MBC3RamTimerBattery = 0x10,
	MBC3 = 0x11,
	MBC3Ram = 0x12,
	MBC3RamBattery = 0x13
};

enum class RomSize {
	kB_32,
	kB_64,
	kB_128,
	kB_256,
	kB_512,
	kB_1024,
	kB_2048,
	kB_4096,
	kB_8192
};

enum class RamSize {
	kB_0, // None
	kB_2, // 1/4 bank
	kB_8, // 1 bank
	kB_32, // 4 banks
	kB_128, // 16 banks
	kB_64  // 8 banks
};

//This determines which banks are switched when 1-bit register 0x4000-0x5FFF is written too
//Default is RomBankingMode
enum class RomRamMode {
	RomBankingMode = 0,
	RamBankingMode = 1
};

class Cartridge {
protected:
	CartridgeType cartridgeType;
	std::string romName;
	std::string romPath;
	std::vector<unsigned char> romBank0;
	std::vector<unsigned char> romBankX;
	std::vector<unsigned char> sRamBankX;
	std::vector<unsigned char> sRamTotal;
	RomSize romSize;
	RamSize ramSize;
	RomRamMode romRamMode;
	bool hasBattery;
	bool hasTimer;
	bool hasRam;
	bool ramEnabled;
	unsigned char selectedRomRamBank = 1;
	unsigned short scrollingNintendoAddress = 0x104;
	unsigned short scrollingNintendoSize = 48;

	bool checkScrollingGraphic();
	bool loadBank0();
	void loadBankX();
	void loadRamBankX();
	void saveRamBank();
public:
	Cartridge();
	void loadRom(std::string romPath);
	unsigned char readByte(unsigned short address);
	bool writeByte(unsigned short address, unsigned char value);
};

class RomOnly : public Cartridge {

public:
	RomOnly(bool hasRam, bool hasBattery);
};

class MBC1 : public Cartridge {

public:
	MBC1(bool hasRam, bool hasBattery);
	bool writeByte(unsigned short address, unsigned char value);
};

class MBC2 : public Cartridge {

public:
	MBC2(bool hasRam, bool hasBattery);
	unsigned char readByte(unsigned short address);
	bool writeByte(unsigned short address, unsigned char value);
};

struct RTC {
	unsigned char seconds; //Ram Bank 0x08
	unsigned char minutes; //Ram Bank 0x09
	unsigned char hours; //Ram Bank 0x0A
	unsigned char lowerDayCounter;  //Ram Bank 0x0B
	unsigned char upperDayCounterAndFlags; //Ram Bank 0x0C

	enum Flags {
		Halt = (0b1000000),
		DayCarry = (0b10000000)
	};
};

class MBC3 : public Cartridge {
private:
	unsigned char selectedRomBank;
	unsigned char selectedRamBank;
	bool latchClockEnabled;
	RTC rtc;

	void MBC3::loadBankX();
	void MBC3::loadRamBankX();
	void latchTime();
public:
	MBC3(bool hasRam, bool hasBattery, bool hasTimer);
	unsigned char readByte(unsigned short address);
	bool writeByte(unsigned short address, unsigned char value);
};

class Rom {
private:
	bool romLoaded;
	Cartridge* cartridge;
	Cartridge* generateCartridge(CartridgeType cartType, std::string romPath);
public:
	Rom();
	Rom(std::string romPath);

	bool loadRom(std::string romPath);
	//bool saveRam();
	bool unloadRom();

	unsigned char readByte(unsigned short address);
	void writeByte(unsigned short address, unsigned char value);
};