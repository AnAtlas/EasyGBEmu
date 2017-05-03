#pragma once

struct Registers{
	struct{
		unsigned char ZeroFlag = (1 << 7);
		unsigned char SubtractFlag = (1 << 6);
		unsigned char HalfCarryFlag = (1 << 5);
		unsigned char CarryFlag = (1 << 4);
	}Flags;


	//Registers a and f
	struct {
		union {
			struct {
				unsigned char f;
				unsigned char a;
			};
			unsigned short af;
		};
	};

	//Registers b and c
	struct {
		union {
			struct {
				unsigned char c;
				unsigned char b;
			};
			unsigned short bc;
		};
	};

	//Registers d and e
	struct {
		union {
			struct {
				unsigned char e;
				unsigned char d;
			};
			unsigned short de;
		};
	};

	//Registers h and l
	struct {
		union {
			struct {
				unsigned char l;
				unsigned char h;
			};
			unsigned short hl;
		};
	};
	unsigned short sp;
	unsigned short pc;
};