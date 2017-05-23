#include "Timer.hpp"
#include "Memory.hpp"

enum TimerControlFlags {
	TimerEnable = 0b100,
	MainFrequencyDivider = 0b11
};

unsigned int timerFrequencies[4] = { 1024, 16, 64, 256 };

Timer::Timer() {
	timerTicks = 0;
	divRegister = 0;
}

void Timer::linkMemory(Memory* memory) {
	this->memory = memory;
}

void Timer::step(unsigned char ticks) {
	divRegister += ticks;
	memory->writeByteTimer(Address::DivReg, (divRegister & 0xFF00) >> 8);

	if (memory->readByte(Address::TimerControl) & TimerControlFlags::TimerEnable) {
		timerTicks += ticks;
		if (timerTicks >= timerFrequencies[memory->readByte(Address::TimerControl) & TimerControlFlags::MainFrequencyDivider]) {
			timerTicks = 0;
			unsigned char timerCounter = memory->readByte(Address::TimerCounter);
			if (timerCounter == 0xFF) {
				timerCounter = memory->readByte(Address::TimerModulo);
				requestInterrupt(2);
			}
			else {
				timerCounter++;
			}
			memory->writeByteTimer(Address::TimerCounter, timerCounter);
		}
	}
}

void Timer::requestInterrupt(int bit) {
	unsigned char reqFlags = memory->readByte(Address::IntFlags);
	reqFlags |= (1 << bit);
	memory->writeByte(Address::IntFlags, reqFlags);
}