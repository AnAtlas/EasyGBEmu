#pragma once

class Memory;

class Timer {
private:
	Memory* memory;
	unsigned short divRegister;
	unsigned short timerTicks;

	void requestInterrupt(int bit);
public:
	Timer();
	void linkMemory(Memory* memory);
	void step(unsigned char ticks);
};