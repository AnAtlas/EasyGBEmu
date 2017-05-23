#include "Config.h"
#include "Emulator.h"

#define VERTICAL_BLANK_SCAN_LINE 0x90
#define VERTICAL_BLANK_SCAN_LINE_MAX 0x99
#define RETRACE_START 456

//////////////////////////////////////////////////////////////////
unsigned char bios[] = {
	0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
	0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
	0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
	0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
	0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
	0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
	0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
	0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
	0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xF2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
	0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
	0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
	0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
	0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
	0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3c, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x4C,
	0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20,
	0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50
};

Emulator::Emulator(void) :
	m_GameLoaded(false)
	,m_CyclesThisUpdate(0)
	,m_UsingMBC1(false)
	,m_EnableRamBank(false)
	,m_UsingMemoryModel16_8(true)
	,m_EnableInterupts(false)
	,m_PendingInteruptDisabled(false)
	,m_PendingInteruptEnabled(false)
	,m_RetraceLY(RETRACE_START)
	,m_JoypadState(0)
	,m_Halted(false)
	,m_TimerVariable(0)
	,m_CurrentClockSpeed(1024)
	,m_DividerVariable(0)
	,m_CurrentRamBank(0)
	,m_DebugPause(false)
	,m_DebugPausePending(false)
	,m_TimeToPause(NULL)
	,m_TotalOpcodes(0)
	,m_DoLogging(false)
{
	ResetScreen( );
	cpuLog = std::fopen("cpuLogOther.log", "w");
	opsRan = 0;
}

//////////////////////////////////////////////////////////////////

Emulator::~Emulator(void)
{
	for (std::vector<BYTE*>::iterator it = m_RamBank.begin(); it != m_RamBank.end(); it++)
		delete[] (*it) ;
}

//////////////////////////////////////////////////////////////////

bool Emulator::LoadRom(const std::string& romName)
{
	if (m_GameLoaded)
		StopGame( );

	m_GameLoaded = true ;

	memset(m_Rom,0,sizeof(m_Rom)) ;
	memset(m_GameBank,0,sizeof(m_GameBank)) ;

	FILE *in;
	in = fopen( romName.c_str(), "rb" );
	fread(m_GameBank, 1, 0x200000, in);
	fclose(in);

	memcpy(&m_Rom[0x0], &m_GameBank[0], 0x8000) ; // this is read only and never changes

	//DEREK
	//memcpy(&m_Rom[0x0], bios, sizeof(bios));
	m_CurrentRomBank = 1;

	return true ;
}

//////////////////////////////////////////////////////////////////

bool Emulator::InitGame( RenderFunc func )
{
	m_RenderFunc = func ;
	return ResetCPU( ) ;
}

//////////////////////////////////////////////////////////////////

void Emulator::ResetScreen( )
{
 	for (int x = 0 ; x < 144; x++)
 	{
 		for (int y = 0; y < 160; y++)
 		{
 			m_ScreenData[x][y][0] = 255 ;
 			m_ScreenData[x][y][1] = 255 ;
 			m_ScreenData[x][y][2] = 255 ;
 		}
 	}
}

//////////////////////////////////////////////////////////////////

bool Emulator::ResetCPU( )
{
	ResetScreen( ) ;
	m_DoLogging = false ;
	m_CurrentRamBank = 0 ;
	m_TimerVariable = 0 ;
	m_CurrentClockSpeed = 1024 ;
	m_DividerVariable = 0 ;
	m_Halted = false ;
	m_TotalOpcodes = 0 ;
	m_JoypadState = 0xFF ;
	m_CyclesThisUpdate = 0 ;
	//Derek
	m_ProgramCounter = 0x100 ;
	m_RegisterAF.hi = 0x1;
	m_RegisterAF.lo = 0xB0 ;
	m_RegisterBC.reg = 0x0013 ;
	m_RegisterDE.reg = 0x00D8 ;
	m_RegisterHL.reg = 0x014D ;
	m_StackPointer.reg = 0xFFFE ;

	m_Rom[0xFF00] = 0xFF ;
	m_Rom[0xFF05] = 0x00   ;
	m_Rom[0xFF06] = 0x00   ;
	m_Rom[0xFF07] = 0x00   ;
	m_Rom[0xFF10] = 0x80   ;
	m_Rom[0xFF11] = 0xBF   ;
	m_Rom[0xFF12] = 0xF3   ;
	m_Rom[0xFF14] = 0xBF   ;
	m_Rom[0xFF16] = 0x3F   ;
	m_Rom[0xFF17] = 0x00   ;
	m_Rom[0xFF19] = 0xBF   ;
	m_Rom[0xFF1A] = 0x7F   ;
	m_Rom[0xFF1B] = 0xFF   ;
	m_Rom[0xFF1C] = 0x9F   ;
	m_Rom[0xFF1E] = 0xBF   ;
	m_Rom[0xFF20] = 0xFF   ;
	m_Rom[0xFF21] = 0x00   ;
	m_Rom[0xFF22] = 0x00   ;
	m_Rom[0xFF23] = 0xBF   ;
	m_Rom[0xFF24] = 0x77   ;
	m_Rom[0xFF25] = 0xF3   ;
	m_Rom[0xFF26] = 0xF1   ;
	m_Rom[0xFF40] = 0x91   ;
	m_Rom[0xFF42] = 0x00   ;
	m_Rom[0xFF43] = 0x00   ;
	m_Rom[0xFF45] = 0x00   ;
	m_Rom[0xFF47] = 0xFC   ;
	m_Rom[0xFF48] = 0xFF   ;
	m_Rom[0xFF49] = 0xFF   ;
	m_Rom[0xFF4A] = 0x00   ;
	m_Rom[0xFF4B] = 0x00   ;
	m_Rom[0xFFFF] = 0x00   ;
	m_RetraceLY = RETRACE_START ;

	m_DebugValue = m_Rom[0x40] ;

	m_EnableRamBank = false ;

	m_UsingMBC2 = false;

	// what kinda rom switching are we using, if any?
	switch(ReadMemory(0x147))
	{
		case 0: m_UsingMBC1 = false ; break ; // not using any memory swapping
		case 1:
		case 2:
		case 3 : m_UsingMBC1 = true ; break ;
		case 5 : m_UsingMBC2 = true ; break ;
		case 6 : m_UsingMBC2 = true ; break ;
		default: return false ; // unhandled memory swappping, probably MBC2
	}

	// how many ram banks do we neeed, if any?
	int numRamBanks = 0 ;
	switch (ReadMemory(0x149))
	{
		case 0: numRamBanks = 0 ;break ;
		case 1: numRamBanks = 1 ;break ;
		case 2: numRamBanks = 1 ;break ;
		case 3: numRamBanks = 4 ;break ;
		case 4: numRamBanks = 16 ;break ;
	}

	CreateRamBanks(numRamBanks) ;

	return true ;

}

//////////////////////////////////////////////////////////////////

static int hack = 0 ;
static long long counter9 = 0 ;

// remember this update function is not the same as the virtual update function. This gets specifically
// called by the Game::Update function. This way I have control over when to execute the next opcode. Mainly for the debug window
void Emulator::Update( )
{
	hack++ ;

	m_CyclesThisUpdate = 0 ;
	const int m_TargetCycles = 70221 ;

	while ((m_CyclesThisUpdate < m_TargetCycles))//||(ReadMemory(0xFF44) < 144))
	{
		if (m_DebugPause)
 			return ;
 		if (m_DebugPausePending)
 		{
 			if ( m_TimeToPause && (m_TimeToPause() == true))
 			{
 				m_DebugPausePending = false ;
 				m_DebugPause = true ;
 				return ;
 			}
 		}

 		int currentCycle = m_CyclesThisUpdate ;
 		BYTE opcode = ExecuteNextOpcode( ) ;
 		int cycles = m_CyclesThisUpdate - currentCycle ;

		DoTimers( cycles ) ;
		DoGraphics( cycles ) ;
		DoInput( ) ;
		DoInterupts( ) ;


	}

	counter9 += m_CyclesThisUpdate ;
	m_RenderFunc() ;
}

//////////////////////////////////////////////////////////////////

void Emulator::DoInput( )
{

}

//////////////////////////////////////////////////////////////////

void Emulator::DoGraphics( int cycles )
{
	SetLCDStatus( ) ;

	// count down the LY register which is the current line being drawn. When reaches 144 (0x90) its vertical blank time
	if (TestBit(ReadMemory(0xFF40), 7))
		m_RetraceLY -= cycles ;

	if (m_Rom[0xFF44] > VERTICAL_BLANK_SCAN_LINE_MAX)
		m_Rom[0xFF44] = 0 ;
	//else if (m_Rom[0xFF44] == 0)
	//	ResetScreen( ) ;

	if (m_RetraceLY <= 0)
		DrawCurrentLine( ) ;
}

//////////////////////////////////////////////////////////////////


BYTE Emulator::ExecuteNextOpcode( )
{
	opsRan++;
	BYTE opcode = m_Rom[m_ProgramCounter] ;

	if (opsRan == 12331) {
		std::fclose(cpuLog);
	}
	int b = 0;
	if (b != 0) {
		FILE* memFile;
		memFile = std::fopen("otherMemory.log", "w");
		m_Rom[0x8010] = 0xFF;
		std::fwrite(&(m_Rom[0]), sizeof(unsigned char), sizeof(m_Rom), memFile);
		std::fclose(memFile);
	}
	if ((m_ProgramCounter >= 0x4000 && m_ProgramCounter <= 0x7FFF) || (m_ProgramCounter >= 0xA000 && m_ProgramCounter <= 0xBFFF))
		opcode = ReadMemory(m_ProgramCounter) ;

	if (!m_Halted)
	{
		if (true)
		{
			char buffer[200] ;
			sprintf(buffer, "%x OP = %x AF = %x BC = %x DE = %x HL = %x\n", m_ProgramCounter, opcode,  m_RegisterAF.reg, m_RegisterBC.reg, m_RegisterDE.reg, m_RegisterHL.reg) ;
			LogMessage::GetSingleton()->DoLogMessage(buffer,false) ;
		}

		m_ProgramCounter++ ;
		m_TotalOpcodes++ ;

		ExecuteOpcode( opcode ) ;


	}
	else
	{
		m_CyclesThisUpdate += 4;
	}

	// we are trying to disable interupts, however interupts get disabled after the next instruction
	// 0xF3 is the opcode for disabling interupt
	if (m_PendingInteruptDisabled)
	{
		if (ReadMemory(m_ProgramCounter-1) != 0xF3)
		{
			m_PendingInteruptDisabled = false ;
			m_EnableInterupts = false ;
		}
	}

	if (m_PendingInteruptEnabled)
	{
		if (ReadMemory(m_ProgramCounter-1) != 0xFB)
		{
			m_PendingInteruptEnabled = false ;
			m_EnableInterupts = true ;
		}
	}

	return opcode ;

}

//////////////////////////////////////////////////////////////////

void Emulator::StopGame( )
{
	m_GameLoaded = false ;
}

//////////////////////////////////////////////////////////////////

std::string Emulator::GetCurrentOpcode( ) const
{
	return std::string("%x", m_Rom[m_ProgramCounter]) ;
}

//////////////////////////////////////////////////////////////////


std::string Emulator::GetImmediateData1( ) const
{
	return std::string("%x", m_Rom[m_ProgramCounter+1]) ;
}

//////////////////////////////////////////////////////////////////

std::string Emulator::GetImmediateData2( ) const
{
	return std::string("%x", m_Rom[m_ProgramCounter+2]) ;
}

//////////////////////////////////////////////////////////////////

// all reading of rom should go through here so I can trap it.
BYTE Emulator::ReadMemory(WORD memory)const
{
	// reading from rom bank
	if (memory >= 0x4000 && memory <= 0x7FFF)
	{
		unsigned int newAddress = memory ;
		newAddress += ((m_CurrentRomBank-1)*0x4000) ;
		return m_GameBank[newAddress] ;
	}

	// reading from RAM Bank
	else if (memory >= 0xA000 && memory <= 0xBFFF)
	{
		WORD newAddress = memory - 0xA000 ;
		return m_RamBank.at(m_CurrentRamBank)[newAddress] ;
	}
	// trying to read joypad state
	else if (memory == 0xFF00)
		return GetJoypadState( );

	return m_Rom[memory] ;
}

//////////////////////////////////////////////////////////////////

WORD Emulator::ReadWord( ) const
{
	WORD res = ReadMemory(m_ProgramCounter+1) ;
	res = res << 8 ;
	res |= ReadMemory(m_ProgramCounter) ;
	return res ;
}

//////////////////////////////////////////////////////////////////

WORD Emulator::ReadLSWord( ) const
{
	WORD res = ReadMemory(m_ProgramCounter+1) ;
	res = res << 8 ;
	res |= ReadMemory(m_ProgramCounter) ;
	return res ;
}

//////////////////////////////////////////////////////////////////

// writes a byte to memory. Remember that address 0 - 07FFF is rom so we cant write to this address
void Emulator::WriteByte(WORD address, BYTE data)
{
	// writing to memory address 0x0 to 0x1FFF this disables writing to the ram bank. 0 disables, 0xA enables
	if (address <= 0x1FFF)
	{
	    if (m_UsingMBC1)
	    {
            if ((data & 0xF) == 0xA)
                m_EnableRamBank = true ;
            else if (data == 0x0)
                m_EnableRamBank = false ;
	    }
	    else if (m_UsingMBC2)
	    {
        //bit 0 of upper byte must be 0
	     if (false == TestBit(address,8))
	     {
	         if ((data & 0xF) == 0xA)
                m_EnableRamBank = true ;
            else if (data == 0x0)
                m_EnableRamBank = false ;
	     }
	    }

	}

	// if writing to a memory address between 2000 and 3FFF then we need to change rom bank
	else if ( (address >= 0x2000) && (address <= 0x3FFF) )
	{
		if (m_UsingMBC1)
		{
			if (data == 0x00)
				data++;

			data &= 31;

			// Turn off the lower 5-bits.
			m_CurrentRomBank &= 224;

			// Combine the written data with the register.
			m_CurrentRomBank |= data;

			char buffer[256] ;
			sprintf(buffer, "Chaning Rom Bank to %d", m_CurrentRomBank) ;
			LogMessage::GetSingleton()->DoLogMessage(buffer, false) ;

		}
		else if (m_UsingMBC2)
		{
            data &= 0xF ;
            m_CurrentRomBank = data ;
		}
	}

	// writing to address 0x4000 to 0x5FFF switches ram banks (if enabled of course)
	else if ( (address >= 0x4000) && (address <= 0x5FFF))
	{
		if (m_UsingMBC1)
		{
			// are we using memory model 16/8
			if (m_UsingMemoryModel16_8)
			{
				// in this mode we can only use Ram Bank 0
				m_CurrentRamBank = 0 ;

				data &= 3;
				data <<= 5;

				if ((m_CurrentRomBank & 31) == 0)
				{
					data++;
				}

				// Turn off bits 5 and 6, and 7 if it somehow got turned on.
				m_CurrentRomBank &= 31;

				// Combine the written data with the register.
				m_CurrentRomBank |= data;

				char buffer[256] ;
				sprintf(buffer, "Chaning Rom Bank to %d", m_CurrentRomBank) ;
				LogMessage::GetSingleton()->DoLogMessage(buffer, false) ;

			}
			else
			{
				m_CurrentRamBank = data & 0x3 ;
				char buffer[256] ;
				sprintf(buffer, "=====Chaning Ram Bank to %d=====", m_CurrentRamBank) ;
				LogMessage::GetSingleton()->DoLogMessage(buffer, false) ;

			}
		}
	}

	// writing to address 0x6000 to 0x7FFF switches memory model
	else if ( (address >= 0x6000) && (address <= 0x7FFF))
	{
		if (m_UsingMBC1)
		{
			// we're only interested in the first bit
			data &= 1 ;
			if (data == 1)
			{
				m_CurrentRamBank = 0 ;
				m_UsingMemoryModel16_8 = false ;
			}
			else
				m_UsingMemoryModel16_8 = true ;
		}
	}

	// from now on we're writing to RAM

 	else if ((address >= 0xA000) && (address <= 0xBFFF))
 	{
 		if (m_EnableRamBank)
 		{
 		    if (m_UsingMBC1)
 		    {
                WORD newAddress = address - 0xA000 ;
                m_RamBank.at(m_CurrentRamBank)[newAddress] = data;
 		    }
 		}
 		else if (m_UsingMBC2 && (address < 0xA200))
 		{
 		    WORD newAddress = address - 0xA000 ;
            m_RamBank.at(m_CurrentRamBank)[newAddress] = data;
 		}

 	}


	// we're right to internal RAM, remember that it needs to echo it
	else if ( (address >= 0xC000) && (address <= 0xDFFF) )
	{
		m_Rom[address] = data ;
	}

	// echo memory. Writes here and into the internal ram. Same as above
	else if ( (address >= 0xE000) && (address <= 0xFDFF) )
	{
		m_Rom[address] = data ;
		m_Rom[address -0x2000] = data ; // echo data into ram address
	}

	// This area is restricted.
 	else if ((address >= 0xFEA0) && (address <= 0xFEFF))
 	{
 	}

	// reset the divider register
	else if (address == 0xFF04)
	{
		m_Rom[0xFF04] = 0 ;
		m_DividerVariable = 0 ;
	}

	// not sure if this is correct
	else if (address == 0xFF07)
	{
		m_Rom[address] = data ;

		int timerVal = data & 0x03 ;

		int clockSpeed = 0 ;

		switch(timerVal)
		{
			case 0: clockSpeed = 1024 ; break ;
			case 1: clockSpeed = 16; break ;
			case 2: clockSpeed = 64 ;break ;
			case 3: clockSpeed = 256 ;break ; // 256
			default: assert(false); break ; // weird timer val
		}

		if (clockSpeed != m_CurrentClockSpeed)
		{
			m_TimerVariable = 0 ;
			m_CurrentClockSpeed = clockSpeed ;
		}
	}


	// FF44 shows which horizontal scanline is currently being draw. Writing here resets it
	else if (address == 0xFF44)
	{
		m_Rom[0xFF44] = 0 ;
	}

	else if (address == 0xFF45)
	{
		m_Rom[address] = data ;
	}
	// DMA transfer
	else if (address == 0xFF46)
	{
	    WORD newAddress = (data << 8) ;
		for (int i = 0; i < 0xA0; i++)
		{
			m_Rom[0xFE00 + i] = ReadMemory(newAddress + i);
		}
	}

	// This area is restricted.
 	else if ((address >= 0xFF4C) && (address <= 0xFF7F))
 	{
 	}


	// I guess we're ok to write to memory... gulp
	else
	{
		m_Rom[address] = data ;
	}
}

//////////////////////////////////////////////////////////////////

int Emulator::GetCarryFlag( ) const
{
	if (TestBit(m_RegisterAF.lo, FLAG_C))
		return 1 ;

	return 0 ;
}

//////////////////////////////////////////////////////////////////

int Emulator::GetZeroFlag( ) const
{
	if (TestBit(m_RegisterAF.lo, FLAG_Z))
		return 1 ;

	return 0 ;
}

//////////////////////////////////////////////////////////////////

int Emulator::GetHalfCarryFlag( ) const
{
	if (TestBit(m_RegisterAF.lo, FLAG_H))
		return 1 ;

	return 0 ;
}

//////////////////////////////////////////////////////////////////

int Emulator::GetSubtractFlag( ) const
{
	if (TestBit(m_RegisterAF.lo, FLAG_N))
		return 1 ;

	return 0 ;
}

//////////////////////////////////////////////////////////////////

static int vblankcount = 0 ;

void Emulator::IssueVerticalBlank( )
{
	vblankcount++ ;
	RequestInterupt(0) ;
	if (hack == 60)
	{
		//OutputDebugStr(STR::Format("Total VBlanks was: %d\n", vblankcount)) ;
		vblankcount = 0 ;
	}

}

//////////////////////////////////////////////////////////////////

static int counter = 0 ;
static int count2 = 0 ;

void Emulator::DrawCurrentLine( )
{
	if (TestBit(ReadMemory(0xFF40), 7)== false)
		return ;

	m_Rom[0xFF44]++ ;
	m_RetraceLY = RETRACE_START ;

	BYTE scanLine = ReadMemory(0xFF44) ;

	if ( scanLine == VERTICAL_BLANK_SCAN_LINE)
		IssueVerticalBlank( ) ;

	if (scanLine > VERTICAL_BLANK_SCAN_LINE_MAX)
		m_Rom[0xFF44] = 0 ;

	if (scanLine < VERTICAL_BLANK_SCAN_LINE)
	{
		DrawScanLine( ) ;
	}

}

//////////////////////////////////////////////////////////////////

void Emulator::PushWordOntoStack(WORD word)
{
	BYTE hi = word >> 8 ;
	BYTE lo = word & 0xFF;
	m_StackPointer.reg-- ;
	WriteByte(m_StackPointer.reg, hi) ;
	m_StackPointer.reg-- ;
	WriteByte(m_StackPointer.reg, lo) ;
}

//////////////////////////////////////////////////////////////////

WORD Emulator::PopWordOffStack( )
{
	WORD word = ReadMemory(m_StackPointer.reg+1) << 8 ;
	word |= ReadMemory(m_StackPointer.reg) ;
	m_StackPointer.reg+=2 ;

	return word ;
}

//////////////////////////////////////////////////////////////////

void Emulator::DoInterupts( )
{
	// are interrupts enabled
	if (m_EnableInterupts)
	{
		// has anything requested an interrupt?
		BYTE requestFlag = ReadMemory(0xFF0F);
		if (requestFlag > 0)
		{
			// which requested interrupt has the lowest priority?
			for (int bit = 0; bit < 8; bit++)
			{
				if (TestBit(requestFlag, bit))
				{
					// this interupt has been requested. But is it enabled?
					BYTE enabledReg = ReadMemory(0xFFFF);
					if (TestBit(enabledReg,bit))
					{
						// yup it is enabled, so lets DOOOOO ITTTTT
						ServiceInterrupt(bit) ;
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////

void Emulator::ServiceInterrupt( int num )
{
	// save current program counter
	PushWordOntoStack(m_ProgramCounter) ;
	m_Halted = false ;

	char buffer[200] ;
	sprintf(buffer, "servicing interupt %d", num) ;
	LogMessage::GetSingleton()->DoLogMessage(buffer, false) ;

//	unsigned long long limit =(8000000);
//	if (m_TotalOpcodes > limit)
//		LOGMESSAGE(Logging::MSG_INFO, STR::Format("Servicing interrupt %d", num).ConstCharPtr() ) ;

	switch(num)
	{
		case 0: m_ProgramCounter = 0x40 ; break ;// V-Blank
		case 1: m_ProgramCounter = 0x48 ; break ;// LCD-STATE
		case 2: m_ProgramCounter = 0x50 ; break ;// Timer
		case 4: m_ProgramCounter = 0x60 ; break ;// JoyPad
		default: assert(false) ; break ;
	}

	m_EnableInterupts = false ;
	m_Rom[0xFF0F] = BitReset(m_Rom[0xFF0F], num) ;
}

//////////////////////////////////////////////////////////////////

void Emulator::DrawScanLine( )
{
	BYTE lcdControl = ReadMemory(0xFF40) ;

	// we can only draw of the LCD is enabled
	if (TestBit(lcdControl, 7))
	{
		RenderBackground( lcdControl ) ;
		RenderSprites( lcdControl ) ;
		//m_RenderFunc() ;
	}

}

//////////////////////////////////////////////////////////////////

void Emulator::RenderBackground(BYTE lcdControl)
{
	// lets draw the background (however it does need to be enabled)
	if (TestBit(lcdControl, 0))
	{
		WORD tileData = 0 ;
		WORD backgroundMemory =0 ;
		bool unsig = true ;

		BYTE scrollY = ReadMemory(0xFF42) ;
		BYTE scrollX = ReadMemory(0xFF43) ;
		BYTE windowY = ReadMemory(0xFF4A) ;
		BYTE windowX = ReadMemory(0xFF4B) - 7;

		bool usingWindow = false ;

		if (TestBit(lcdControl,5))
		{
			if (windowY <= ReadMemory(0xFF44))
				usingWindow = true ;
		}
		else
		{
			usingWindow = false ;
		}

		// which tile data are we using?
		if (TestBit(lcdControl,4))
		{
			tileData = 0x8000 ;
		}
		else
		{
			tileData = 0x8800 ;
			unsig= false ;
		}

		// which background mem?
		if (false == usingWindow)
		{
			if (TestBit(lcdControl,3))
				backgroundMemory = 0x9C00 ;
			else
				backgroundMemory = 0x9800 ;
		}
		else
		{
			if (TestBit(lcdControl,6))
				backgroundMemory = 0x9C00 ;
			else
				backgroundMemory = 0x9800 ;
		}


		BYTE yPos = 0 ;

		if (!usingWindow)
			yPos = scrollY + ReadMemory(0xFF44) ;
		else
			yPos = ReadMemory(0xFF44) - windowY;

		WORD tileRow = (((BYTE)(yPos/8))*32) ;

		for (int pixel = 0 ; pixel < 160; pixel++)
		{
			BYTE xPos = pixel+scrollX ;

			if (usingWindow)
			{
				if (pixel >= windowX)
				{
					xPos = pixel - windowX ;
				}
			}

			WORD tileCol = (xPos/8) ;
			SIGNED_WORD tileNum ;

			if(unsig)
				tileNum = (BYTE)ReadMemory(backgroundMemory+tileRow + tileCol) ;
			else
				tileNum = (SIGNED_BYTE)ReadMemory(backgroundMemory+tileRow + tileCol) ;

			WORD tileLocation = tileData ;

			if (unsig)
				tileLocation += (tileNum * 16) ;
			else
				tileLocation += ((tileNum+128) *16) ;

			BYTE line = yPos % 8 ;
			line *= 2;
			BYTE data1 = ReadMemory(tileLocation + line) ;
			BYTE data2 = ReadMemory(tileLocation + line + 1) ;

			int colourBit = xPos % 8 ;
			colourBit -= 7 ;
			colourBit *= -1 ;

			int colourNum = BitGetVal(data2,colourBit) ;
			colourNum <<= 1;
			colourNum |= BitGetVal(data1,colourBit) ;

			COLOUR col = GetColour(colourNum, 0xFF47) ;

			int red = 0;
			int green = 0;
			int blue = 0;

			switch(col)
			{
			case WHITE:	red = 255; green = 255 ; blue = 255; break ;
			case LIGHT_GRAY:red = 0xCC; green = 0xCC ; blue = 0xCC; break ;
			case DARK_GRAY:	red = 0x77; green = 0x77 ; blue = 0x77; break ;
			}
			int finaly = ReadMemory(0xFF44) ;

			if ((finaly < 0) || (finaly > 143) || (pixel < 0) || (pixel > 159))
			{
				assert(false);
				continue ;
			}
			m_ScreenData[finaly][pixel][0] = red ;
			m_ScreenData[finaly][pixel][1] = green ;
			m_ScreenData[finaly][pixel][2] = blue ;
		}
	}
}

//////////////////////////////////////////////////////////////////

void Emulator::RenderSprites(BYTE lcdControl)
{
	// lets draw the sprites (however it does need to be enabled)

	if (TestBit(lcdControl, 1))
	{
		bool use8x16 = false ;
		if (TestBit(lcdControl,2))
			use8x16 = true ;

		for (int sprite = 0 ; sprite < 40; sprite++)
		{
 			BYTE index = sprite*4 ;
 			BYTE yPos = ReadMemory(0xFE00+index) - 16;
 			BYTE xPos = ReadMemory(0xFE00+index+1)-8;
 			BYTE tileLocation = ReadMemory(0xFE00+index+2) ;
 			BYTE attributes = ReadMemory(0xFE00+index+3) ;

			bool yFlip = TestBit(attributes,6) ;
			bool xFlip = TestBit(attributes,5) ;

			int scanline = ReadMemory(0xFF44);

			int ysize = 8;

			if (use8x16)
				ysize = 16;

 			if ((scanline >= yPos) && (scanline < (yPos+ysize)))
 			{
 				int line = scanline - yPos ;

 				if (yFlip)
 				{
 					line -= ysize ;
 					line *= -1 ;
 				}

 				line *= 2;
 				BYTE data1 = ReadMemory( (0x8000 + (tileLocation * 16)) + line ) ;
 				BYTE data2 = ReadMemory( (0x8000 + (tileLocation * 16)) + line+1 ) ;



 				for (int tilePixel = 7; tilePixel >= 0; tilePixel--)
 				{
					int colourbit = tilePixel ;
 					if (xFlip)
 					{
 						colourbit -= 7 ;
 						colourbit *= -1 ;
 					}
 					int colourNum = BitGetVal(data2,colourbit) ;
 					colourNum <<= 1;
 					colourNum |= BitGetVal(data1,colourbit) ;

					COLOUR col = GetColour(colourNum, TestBit(attributes,4)?0xFF49:0xFF48) ;

 					// white is transparent for sprites.
 					if (col == WHITE)
 						continue ;

 					int red = 0;
 					int green = 0;
 					int blue = 0;

					switch(col)
					{
					case WHITE:	red = 255; green = 255 ; blue = 255; break ;
					case LIGHT_GRAY:red = 0xCC; green = 0xCC ; blue = 0xCC; break ;
					case DARK_GRAY:	red = 0x77; green = 0x77 ; blue = 0x77; break ;
					}

 					int xPix = 0 - tilePixel ;
 					xPix += 7 ;

					int pixel = xPos+xPix ;

					if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159))
					{
					//	assert(false) ;
						continue ;
					}

					// check if pixel is hidden behind background
					if (TestBit(attributes, 7) == 1)
					{
						if ( (m_ScreenData[scanline][pixel][0] != 255) || (m_ScreenData[scanline][pixel][1] != 255) || (m_ScreenData[scanline][pixel][2] != 255) )
							continue ;
					}

 					m_ScreenData[scanline][pixel][0] = red ;
 					m_ScreenData[scanline][pixel][1] = green ;
 					m_ScreenData[scanline][pixel][2] = blue ;

 				}
 			}
		}
	}
}

//////////////////////////////////////////////////////////////////

Emulator::COLOUR Emulator::GetColour(BYTE colourNum, WORD address) const
{
	COLOUR res = WHITE ;
	BYTE palette = ReadMemory(address) ;
	int hi = 0 ;;
	int lo = 0 ;

	switch (colourNum)
	{
	case 0: hi = 1 ; lo = 0 ;break ;
	case 1: hi = 3 ; lo = 2 ;break ;
	case 2: hi = 5 ; lo = 4 ;break ;
	case 3: hi = 7 ; lo = 6 ;break ;
	default: assert(false) ; break ;
	}

	int colour = 0;
	colour = BitGetVal(palette, hi) << 1;
	colour |= BitGetVal(palette, lo) ;

	switch (colour)
	{
	case 0: res = WHITE ;break ;
	case 1: res = LIGHT_GRAY ;break ;
	case 2: res = DARK_GRAY ;break ;
	case 3: res = BLACK ;break ;
	default: assert(false) ; break ;
	}

	return res ;
}

//////////////////////////////////////////////////////////////////
void Emulator::SetLCDStatus( )
{
	BYTE lcdStatus = m_Rom[0xFF41] ;

	if (TestBit(ReadMemory(0xFF40), 7)== false)
	{
		m_RetraceLY = RETRACE_START ;
		m_Rom[0xFF44] = 0 ;

		// mode gets set to 1 when disabled screen.
		lcdStatus &= 252 ;
		lcdStatus = BitSet(lcdStatus,0) ;
		WriteByte(0xFF41,lcdStatus) ;
		return ;
	}


	BYTE lY = ReadMemory(0xFF44) ;

	BYTE currentMode = GetLCDMode( ) ;

	int mode = 0 ;
	bool reqInt = false ;

	// set mode as vertical blank
	if (lY >= VERTICAL_BLANK_SCAN_LINE)
	{
		// mode 1
		mode = 1 ;
		lcdStatus = BitSet(lcdStatus,0) ;
		lcdStatus = BitReset(lcdStatus,1) ;
		reqInt = TestBit(lcdStatus, 4) ;
	}
	else
	{
		int mode2Bounds = (RETRACE_START - 80) ;
		int mode3Bounds = (mode2Bounds - 172) ;


		// mode 2
		if (m_RetraceLY >= mode2Bounds)
		{
			mode = 2 ;
			lcdStatus = BitSet(lcdStatus,1) ;
			lcdStatus = BitReset(lcdStatus,0) ;
			reqInt = TestBit(lcdStatus,5) ;
		}
		// mode 3
		else if (m_RetraceLY >= mode3Bounds)
		{
			mode = 3 ;
			lcdStatus = BitSet(lcdStatus,1) ;
			lcdStatus = BitSet(lcdStatus,0) ;
		}
		// mode 3
		else
		{
			mode = 0 ;
			lcdStatus = BitReset(lcdStatus,1) ;
			lcdStatus = BitReset(lcdStatus,0) ;
			reqInt = TestBit(lcdStatus,3) ;
		}

	}

	// just entered a new mode. Request interupt
	if (reqInt && (currentMode != mode))
		RequestInterupt(1) ;

	// check for coincidence flag
	if ( lY == ReadMemory(0xFF45))
	{
		lcdStatus = BitSet(lcdStatus,2) ;

		if (TestBit(lcdStatus,6))
		{
			RequestInterupt(1) ;
		}
	}
	else
	{
		lcdStatus = BitReset(lcdStatus,2) ;
	}

	WriteByte(0xFF41, lcdStatus) ;
}

//////////////////////////////////////////////////////////////////

BYTE Emulator::GetJoypadState( ) const
{
	// this function CANNOT call ReadMemory(0xFF00) it must access it directly from m_Rom[0xFF00]
	// because ReadMemory traps this address

	BYTE res = m_Rom[0xFF00] ;
	res ^= 0xFF ;

	if (!TestBit(res, 4))
	{
		BYTE topJoypad = m_JoypadState >> 4 ;
		topJoypad |= 0xF0 ;
		res &= topJoypad ;
	}
	else if (!TestBit(res,5))
	{
		BYTE bottomJoypad = m_JoypadState & 0xF ;
		bottomJoypad |= 0xF0 ;
		res &= bottomJoypad ;
	}
	return res ;
}

//////////////////////////////////////////////////////////////////

void Emulator::KeyPressed(int key)
{
	// this function CANNOT call ReadMemory(0xFF00) it must access it directly from m_Rom[0xFF00]
	// because ReadMemory traps this address

	bool previouslyUnset = false ;

	if (TestBit(m_JoypadState, key) == false)
		previouslyUnset = true ;

	m_JoypadState = BitReset(m_JoypadState, key) ;

	// button pressed
	bool button = true ;

	if (key > 3)
		button = true ;
	else // directional button pressed
		button = false ;

	BYTE keyReq = m_Rom[0xFF00] ;
	bool requestInterupt = false ;

	// player pressed button and programmer intersted in button
	if (button && !TestBit(keyReq,5))
	{
		requestInterupt = true ;
	}
	// player pressed directional and programmer interested
	else if (!button && !TestBit(keyReq,4))
	{
		requestInterupt = true ;
	}

	if (requestInterupt && !previouslyUnset)
	{
		RequestInterupt(4) ;
	}
}

//////////////////////////////////////////////////////////////////

void Emulator::KeyReleased(int key)
{
	// this function CANNOT call ReadMemory(0xFF00) it must access it directly from m_Rom[0xFF00]
	// because ReadMemory traps this address

	m_JoypadState = BitSet(m_JoypadState, key) ;
}

//////////////////////////////////////////////////////////////////

static int timerhack = 0 ;

void Emulator::DoTimers( int cycles )
{
	BYTE timerAtts = m_Rom[0xFF07];

	m_DividerVariable += cycles ;

	if (TestBit(timerAtts, 2))
	{
		m_TimerVariable += cycles ;

		// time to increment the timer
		if (m_TimerVariable >= m_CurrentClockSpeed)
		{
			m_TimerVariable = 0 ;
			bool overflow = false ;
			if (m_Rom[0xFF05] == 0xFF)
			{
				overflow = true ;
			}
			m_Rom[0xFF05]++ ;

			if (overflow)
			{
				timerhack++ ;

				m_Rom[0xFF05] = m_Rom[0xFF06] ;

				// request the interupt
				RequestInterupt(2) ;
			}
		}
	}

	// do divider register
	if (m_DividerVariable >= 256)
	{
		m_DividerVariable = 0;
		m_Rom[0xFF04]++ ;
	}
}

//////////////////////////////////////////////////////////////////

void Emulator::CreateRamBanks(int numBanks)
{
	// DOES THE FIRST RAM BANK NEED TO BE SET TO THE CONTENTS of m_Rom[0xA000] - m_Rom[0xC000]?
	for (int i = 0; i < 17; i++)
	{
		BYTE* ram = new BYTE[0x2000] ;
		memset(ram, 0, sizeof(ram)) ;
		m_RamBank.push_back(ram) ;
	}

	for (int i = 0 ; i < 0x2000; i++)
		m_RamBank[0][i] = m_Rom[0xA000+i] ;
}

//////////////////////////////////////////////////////////////////

void Emulator::RequestInterupt(int bit)
{
	BYTE requestFlag = ReadMemory(0xFF0F) ;
	requestFlag = BitSet(requestFlag,bit) ;
	WriteByte(0xFF0F, requestFlag) ;
}

//////////////////////////////////////////////////////////////////

BYTE Emulator::GetLCDMode() const
{
	BYTE lcdStatus = m_Rom[0xFF41] ;
	return lcdStatus & 0x3 ;
}

//////////////////////////////////////////////////////////////////

