#pragma once
#include <vector>

#include "Registers.hpp"
#include "Memory.hpp"

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

	struct Clock {
		unsigned int m = 0;
		unsigned int t = 0;
	};

public:
	Cpu();
	void printRegisters();
	void step();

	//Debug
	void runCommand(std::string command);

private:
	Registers registers;
	Clock clock;
	Memory memory;
	bool stopped;

	void clearRegisters();
	void generateInstructions();

	std::vector<Instruction> instructions;

	void reset();
	void setFlag(unsigned char flag);
	void clearFlag(unsigned char flag);
	bool checkFlag(unsigned char flag);
	void clearAllFlags();
	//General Functions
	unsigned char inc(unsigned char value);
	unsigned char dec(unsigned char value);
	unsigned short add2(unsigned short v1, unsigned short v2);

	//Instruction functions
	void nop(std::vector<unsigned char> parms);			//0x00
	void ld_bc_nn(std::vector<unsigned char> parms);	//0x01
	void ld_bcp_a(std::vector<unsigned char> parms);	//0x02
	void inc_bc(std::vector<unsigned char> parms);
	void inc_b(std::vector<unsigned char> parms);
	void dec_b(std::vector<unsigned char> parms);
	void ld_b_n(std::vector<unsigned char> parms);
	void rlca(std::vector<unsigned char> parms);
	void ld_nnp_sp(std::vector<unsigned char> parms);
	void add_hl_bc(std::vector<unsigned char> parms);
	void ld_a_bcp(std::vector<unsigned char> parms);	//0x0A
	void dec_bc(std::vector<unsigned char> parms);
	void inc_c(std::vector<unsigned char> parms);
	void dec_c(std::vector<unsigned char> parms);
	void ld_c_n(std::vector<unsigned char> parms);
	void rrca(std::vector<unsigned char> parms);
	void stop(std::vector<unsigned char> parms);		//0x10
	void ld_de_nn(std::vector<unsigned char> parms);
	void ld_dep_a(std::vector<unsigned char> parms);
	void inc_de(std::vector<unsigned char> parms);
	void inc_d(std::vector<unsigned char> parms);
	void dec_d(std::vector<unsigned char> parms);
	void ld_d_n(std::vector<unsigned char> parms);		
	void rla(std::vector<unsigned char> parms);
	void jr_n(std::vector<unsigned char> parms);
	void add_hl_de(std::vector<unsigned char> parms);
	void ld_a_dep(std::vector<unsigned char> parms);	//0x1A
	void dec_de(std::vector<unsigned char> parms);
	void inc_e(std::vector<unsigned char> parms);		
	void dec_e(std::vector<unsigned char> parms);
	void ld_e_n(std::vector<unsigned char> parms);
	void rra(std::vector<unsigned char> parms);
	void jr_nz_n(std::vector<unsigned char> parms);		//0x20
	void ld_hl_nn(std::vector<unsigned char> parms);
	void ldi_hl_a(std::vector<unsigned char> parms);
	void inc_hl(std::vector<unsigned char> parms);
	void inc_h(std::vector<unsigned char> parms);
	void dec_h(std::vector<unsigned char> parms);
	void ld_h_n(std::vector<unsigned char> parms);
	void daa(std::vector<unsigned char> parms);
	void jr_z_n(std::vector<unsigned char> parms);
	void add_hl_hl(std::vector<unsigned char> parms);
	void ld_a_hlpi(std::vector<unsigned char> parms);	//0x2A
};