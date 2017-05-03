#pragma once
#include <vector>

#include "Registers.hpp"



class Cpu {
	struct Instruction {
		Instruction(std::string instruction, unsigned char parameterLength, unsigned char ticks,
			void (Cpu::*fp)(std::vector<unsigned char> parms))
				: instruction(instruction), parameterLength(parameterLength), ticks(ticks), fp(fp){

		}
		std::string instruction;
		unsigned char parameterLength;
		unsigned char ticks;
		void (Cpu::*fp)(std::vector<unsigned char> parms);
	};

public:
	Cpu();
	void printRegisters();
private:
	Registers registers;
	
	void clearRegisters();
	void generateInstructions();

	std::vector<Instruction> instructions;

	//Instruction functions
	void nop(std::vector<unsigned char> parms);
	void ld_bc_nn(std::vector<unsigned char> parms);
};