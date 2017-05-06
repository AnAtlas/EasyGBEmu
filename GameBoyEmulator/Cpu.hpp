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
	unsigned char and(unsigned char v1, unsigned char v2);
	unsigned char xor(unsigned char v1, unsigned char v2);
	unsigned char or(unsigned char v1, unsigned char v2);
	void compare(unsigned char value);
	unsigned char addCarry(unsigned char value);
	unsigned char subCarry(unsigned char value);
	unsigned char addBytes(unsigned char v1, unsigned char v2);
	unsigned char subBytes(unsigned char v1, unsigned char v2);
	unsigned short addShorts(unsigned short v1, unsigned short v2);

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
	void dec_hl(std::vector<unsigned char> parms);
	void inc_l(std::vector<unsigned char> parms);
	void dec_l(std::vector<unsigned char> parms);
	void ld_l_n(std::vector<unsigned char> parms);
	void cpl(std::vector<unsigned char> parms);
	void jr_nc_n(std::vector<unsigned char> parms);		//0x30
	void ld_sp_nn(std::vector<unsigned char> parms);
	void ldd_hlp_a(std::vector<unsigned char> parms);
	void inc_sp(std::vector<unsigned char> parms);
	void inc_hlp(std::vector<unsigned char> parms);
	void dec_hlp(std::vector<unsigned char> parms);
	void ld_hlp_n(std::vector<unsigned char> parms);
	void scf(std::vector<unsigned char> parms);
	void jr_c_n(std::vector<unsigned char> parms);
	void add_hl_sp(std::vector<unsigned char> parms);
	void ld_a_hlpd(std::vector<unsigned char> parms);	//0x3A
	void dec_sp(std::vector<unsigned char> parms);
	void inc_a(std::vector<unsigned char> parms);
	void dec_a(std::vector<unsigned char> parms);
	void ld_a_n(std::vector<unsigned char> parms);
	void ccf(std::vector<unsigned char> parms);
	void ld_b_b(std::vector<unsigned char> parms);		//0x40
	void ld_b_c(std::vector<unsigned char> parms);
	void ld_b_d(std::vector<unsigned char> parms);
	void ld_b_e(std::vector<unsigned char> parms);
	void ld_b_h(std::vector<unsigned char> parms);
	void ld_b_l(std::vector<unsigned char> parms);
	void ld_b_hlp(std::vector<unsigned char> parms);
	void ld_b_a(std::vector<unsigned char> parms);
	void ld_c_b(std::vector<unsigned char> parms);
	void ld_c_c(std::vector<unsigned char> parms);
	void ld_c_d(std::vector<unsigned char> parms);		//0x4A
	void ld_c_e(std::vector<unsigned char> parms);
	void ld_c_h(std::vector<unsigned char> parms);
	void ld_c_l(std::vector<unsigned char> parms);
	void ld_c_hlp(std::vector<unsigned char> parms);
	void ld_c_a(std::vector<unsigned char> parms);
	void ld_d_b(std::vector<unsigned char> parms);		//0x50
	void ld_d_c(std::vector<unsigned char> parms);
	void ld_d_d(std::vector<unsigned char> parms);
	void ld_d_e(std::vector<unsigned char> parms);
	void ld_d_h(std::vector<unsigned char> parms);
	void ld_d_l(std::vector<unsigned char> parms);
	void ld_d_hlp(std::vector<unsigned char> parms);
	void ld_d_a(std::vector<unsigned char> parms);
	void ld_e_b(std::vector<unsigned char> parms);
	void ld_e_c(std::vector<unsigned char> parms);
	void ld_e_d(std::vector<unsigned char> parms);		//0x5A
	void ld_e_e(std::vector<unsigned char> parms);
	void ld_e_h(std::vector<unsigned char> parms);
	void ld_e_l(std::vector<unsigned char> parms);
	void ld_e_hlp(std::vector<unsigned char> parms);
	void ld_e_a(std::vector<unsigned char> parms);
	void ld_h_b(std::vector<unsigned char> parms);		//0x60
	void ld_h_c(std::vector<unsigned char> parms);
	void ld_h_d(std::vector<unsigned char> parms);
	void ld_h_e(std::vector<unsigned char> parms);
	void ld_h_h(std::vector<unsigned char> parms);
	void ld_h_l(std::vector<unsigned char> parms);
	void ld_h_hlp(std::vector<unsigned char> parms);
	void ld_h_a(std::vector<unsigned char> parms);
	void ld_l_b(std::vector<unsigned char> parms);
	void ld_l_c(std::vector<unsigned char> parms);
	void ld_l_d(std::vector<unsigned char> parms);
	void ld_l_e(std::vector<unsigned char> parms);
	void ld_l_h(std::vector<unsigned char> parms);
	void ld_l_l(std::vector<unsigned char> parms);
	void ld_l_hlp(std::vector<unsigned char> parms);
	void ld_l_a(std::vector<unsigned char> parms);		//0x6F
	void ld_hlp_b(std::vector<unsigned char> parms);
	void ld_hlp_c(std::vector<unsigned char> parms);
	void ld_hlp_d(std::vector<unsigned char> parms);
	void ld_hlp_e(std::vector<unsigned char> parms);
	void ld_hlp_h(std::vector<unsigned char> parms);
	void ld_hlp_l(std::vector<unsigned char> parms);
	void halt(std::vector<unsigned char> parms);
	void ld_hlp_a(std::vector<unsigned char> parms);
	void ld_a_b(std::vector<unsigned char> parms);
	void ld_a_c(std::vector<unsigned char> parms);
	void ld_a_d(std::vector<unsigned char> parms);
	void ld_a_e(std::vector<unsigned char> parms);
	void ld_a_h(std::vector<unsigned char> parms);
	void ld_a_l(std::vector<unsigned char> parms);
	void ld_a_hlp(std::vector<unsigned char> parms);
	void ld_a_a(std::vector<unsigned char> parms);		//0x7F
	void add_a_b(std::vector<unsigned char> parms);
	void add_a_c(std::vector<unsigned char> parms);
	void add_a_d(std::vector<unsigned char> parms);
	void add_a_e(std::vector<unsigned char> parms);
	void add_a_h(std::vector<unsigned char> parms);
	void add_a_l(std::vector<unsigned char> parms);
	void add_a_hlp(std::vector<unsigned char> parms);
	void add_a_a(std::vector<unsigned char> parms);
	void adc_a_b(std::vector<unsigned char> parms);
	void adc_a_c(std::vector<unsigned char> parms);
	void adc_a_d(std::vector<unsigned char> parms);
	void adc_a_e(std::vector<unsigned char> parms);
	void adc_a_h(std::vector<unsigned char> parms);
	void adc_a_l(std::vector<unsigned char> parms);
	void adc_a_hlp(std::vector<unsigned char> parms);
	void adc_a_a(std::vector<unsigned char> parms);		//0x8F
	void sub_b(std::vector<unsigned char> parms);
	void sub_c(std::vector<unsigned char> parms);
	void sub_d(std::vector<unsigned char> parms);
	void sub_e(std::vector<unsigned char> parms);
	void sub_h(std::vector<unsigned char> parms);
	void sub_l(std::vector<unsigned char> parms);
	void sub_hlp(std::vector<unsigned char> parms);
	void sub_a(std::vector<unsigned char> parms);
	void sbc_a_b(std::vector<unsigned char> parms);
	void sbc_a_c(std::vector<unsigned char> parms);
	void sbc_a_d(std::vector<unsigned char> parms);
	void sbc_a_e(std::vector<unsigned char> parms);
	void sbc_a_h(std::vector<unsigned char> parms);
	void sbc_a_l(std::vector<unsigned char> parms);
	void sbc_a_hlp(std::vector<unsigned char> parms);
	void sbc_a_a(std::vector<unsigned char> parms);		//0x9F
	void and_b(std::vector<unsigned char> parms);
	void and_c(std::vector<unsigned char> parms);
	void and_d(std::vector<unsigned char> parms);
	void and_e(std::vector<unsigned char> parms);
	void and_h(std::vector<unsigned char> parms);
	void and_l(std::vector<unsigned char> parms);
	void and_hlp(std::vector<unsigned char> parms);
	void and_a(std::vector<unsigned char> parms);
	void xor_b(std::vector<unsigned char> parms);
	void xor_c(std::vector<unsigned char> parms);
	void xor_d(std::vector<unsigned char> parms);
	void xor_e(std::vector<unsigned char> parms);
	void xor_h(std::vector<unsigned char> parms);
	void xor_l(std::vector<unsigned char> parms);
	void xor_hlp(std::vector<unsigned char> parms);
	void xor_a(std::vector<unsigned char> parms);		//0xAF
	void or_b(std::vector<unsigned char> parms);
	void or_c(std::vector<unsigned char> parms);
	void or_d(std::vector<unsigned char> parms);
	void or_e(std::vector<unsigned char> parms);
	void or_h(std::vector<unsigned char> parms);
	void or_l(std::vector<unsigned char> parms);
	void or_hlp(std::vector<unsigned char> parms);
	void or_a(std::vector<unsigned char> parms);
	void cp_b(std::vector<unsigned char> parms);
	void cp_c(std::vector<unsigned char> parms);
	void cp_d(std::vector<unsigned char> parms);
	void cp_e(std::vector<unsigned char> parms);
	void cp_h(std::vector<unsigned char> parms);
	void cp_l(std::vector<unsigned char> parms);
	void cp_hlp(std::vector<unsigned char> parms);
	void cp_a(std::vector<unsigned char> parms);		//0xBF
	void ret_nz(std::vector<unsigned char> parms);
	void pop_bc(std::vector<unsigned char> parms);
	void jp_nz_nn(std::vector<unsigned char> parms);
	void jp_nn(std::vector<unsigned char> parms);
	void call_nz_nn(std::vector<unsigned char> parms);
	void push_bc(std::vector<unsigned char> parms);
	void add_a_n(std::vector<unsigned char> parms);
	void rst_0(std::vector<unsigned char> parms);
	void ret_z(std::vector<unsigned char> parms);
	void ret(std::vector<unsigned char> parms);
	void jp_z_nn(std::vector<unsigned char> parms);
	void cb(std::vector<unsigned char> parms);
	void call_z_nn(std::vector<unsigned char> parms);
	void call_nn(std::vector<unsigned char> parms);
	void adc_a_n(std::vector<unsigned char> parms);
	void rst_08(std::vector<unsigned char> parms);		//0xCF
	void ret_nc(std::vector<unsigned char> parms);
	void pop_de(std::vector<unsigned char> parms);
	void jp_nc_nn(std::vector<unsigned char> parms);
	void call_nc_nn(std::vector<unsigned char> parms);
	void push_de(std::vector<unsigned char> parms);
	void sub_n(std::vector<unsigned char> parms);
	void rst_10(std::vector<unsigned char> parms);
	void ret_c(std::vector<unsigned char> parms);
	void reti(std::vector<unsigned char> parms);
	void jp_c_nn(std::vector<unsigned char> parms);
	void call_c_nn(std::vector<unsigned char> parms);
	void sbc_a_n(std::vector<unsigned char> parms);
	void rst_18(std::vector<unsigned char> parms);		//0xDF
	void ld_ffnp_a(std::vector<unsigned char> parms);
	void pop_hl(std::vector<unsigned char> parms);
	void ld_cp_a(std::vector<unsigned char> parms);
	void push_hl(std::vector<unsigned char> parms);
	void and_n(std::vector<unsigned char> parms);
	void rst_20(std::vector<unsigned char> parms);
	void add_sp_n(std::vector<unsigned char> parms);
	void jp_hlp(std::vector<unsigned char> parms);
	void ld_nnp_a(std::vector<unsigned char> parms);
	void xor_n(std::vector<unsigned char> parms);
	void rst_28(std::vector<unsigned char> parms);		//0xEF
	void ld_a_ffnp(std::vector<unsigned char> parms);
	void pop_af(std::vector<unsigned char> parms);
	void ld_a_cp(std::vector<unsigned char> parms);
	void di(std::vector<unsigned char> parms);
	void push_af(std::vector<unsigned char> parms);
	void or_n(std::vector<unsigned char> parms);
	void rst_30(std::vector<unsigned char> parms);
	void ld_hl_sp_n(std::vector<unsigned char> parms);
	void ld_sp_hl(std::vector<unsigned char> parms);
	void ld_a_nnp(std::vector<unsigned char> parms);
	void ei(std::vector<unsigned char> parms);
	void cp_n(std::vector<unsigned char> parms);
	void rst_38(std::vector<unsigned char> parms);

	void null(std::vector<unsigned char> parms);
};