#pragma once
#include <vector>

struct Instruction;
namespace Opcodes {
	extern std::vector<Instruction>getInstructionVector();
	extern std::vector<Instruction>getExtendedInstructionVector();
}