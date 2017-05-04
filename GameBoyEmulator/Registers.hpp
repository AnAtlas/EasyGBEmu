#pragma once

typedef enum Flags{
	Zero = (1 << 7),
	Subtract = (1 << 6),
	HalfCarry = (1 << 5),
	Carry = (1 << 4)
};

struct Registers{
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