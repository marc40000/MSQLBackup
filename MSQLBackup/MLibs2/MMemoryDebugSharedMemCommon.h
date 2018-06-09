#pragma once

#ifdef MMemoryDebug_Enabled

#ifdef MMemoryDebugSharedMem_CLRSupport
#pragma unmanaged
#endif

#include "MRingBufferStatic.h"
#include "MSharedMem.h"


const unsigned int MMemCheckSharedMemStackTraceMaxStackRecur = 32;

class MMemCheckSharedMemStackTraceEntryPosition
{
public:
	DWORD64 address;
};

const unsigned int MMemCheckSharedMemStackTraceEntryCLR_filenamen = 128;
const unsigned int MMemCheckSharedMemStackTraceEntryCLR_functionnamen = 32;
class MMemCheckSharedMemStackTraceEntryCLR
{
public:
	unsigned int line;
	char filename[MMemCheckSharedMemStackTraceEntryCLR_filenamen];
	char functionname[MMemCheckSharedMemStackTraceEntryCLR_functionnamen];
};

class MMemCheckSharedMemStackTraceEntry
{
public:
	unsigned long long id;		// counts upwards, so the collector can check if it dropped something
	int allocType;
	//size_t size;
	unsigned int size;
	int blockType;
	long requestNumberLast;
	long requestNumber;
	char filename[128];
	int lineNumber;
	unsigned int stacktracen;
	MMemCheckSharedMemStackTraceEntryPosition stacktrace[MMemCheckSharedMemStackTraceMaxStackRecur];
	unsigned int stacktracemethod;
	//MMemCheckStackTraceEntryPosition stacktrace[1024];
	//unsigned int stacktracen;
	//MCrtMemBlockHeader pheader;
//#ifdef MMemoryDebug_CLRSupport	// since this is the shared mem design we put it in here always so there won't be a problem on the reader side when switching this
	unsigned int stacktraceclrn;
	MMemCheckSharedMemStackTraceEntryCLR stacktraceclr[MMemCheckSharedMemStackTraceMaxStackRecur];
//#endif
};
typedef MMemCheckSharedMemStackTraceEntry * LPMMemCheckSharedMemStackTraceEntry;



// copied from DbgHelp
typedef struct M_IMAGEHLP_LINEW64 {
	DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_LINE64)
	//PVOID    Key;                    // internal
	unsigned int    Key;                    // internal
	DWORD    LineNumber;             // line number in file
	//PWSTR    FileName;               // full filename
	unsigned int    FileName;               // full filename
	DWORD64  Address;                // first instruction of line
} MIMAGEHLP_LINEW64, *PMIMAGEHLP_LINEW64;

typedef struct M_SYMBOL_INFOW {
	ULONG       SizeOfStruct;
	ULONG       TypeIndex;        // Type Index of symbol
	ULONG64     Reserved[2];
	ULONG       Index;
	ULONG       Size;
	ULONG64     ModBase;          // Base Address of module comtaining this symbol
	ULONG       Flags;
	ULONG64     Value;            // Value of symbol, ValuePresent should be 1
	ULONG64     Address;          // Address of symbol including base address of module
	ULONG       Register;         // register holding value or pointer to value
	ULONG       Scope;            // scope of the symbol
	ULONG       Tag;              // pdb classification
	ULONG       NameLen;          // Actual length of name
	ULONG       MaxNameLen;
	WCHAR       Name[1];          // Name of symbol
} MSYMBOL_INFOW, *PMSYMBOL_INFOW;



const unsigned int MAXSYMBOLNAMELENGTH = 256;
const unsigned int memblocksnmax = 1024 * 1024 * 32;
class MMemCheckSharedMemStackTraceBuffer
{
public:
	unsigned long long initdone;
	unsigned int version;
	unsigned int state;		// 0: before start, 1: running, 2: ended
	MRingBufferStatic < MMemCheckSharedMemStackTraceEntry, (1024 * 64) > stackentries;
	unsigned int readpointer;		// the readpointer of the collector - the debugged process will wait if it's too far away from the writepointer


	// memblocks
	long memblocksstart[memblocksnmax];
	unsigned int memblocksstartn;
	long memblocksend[memblocksnmax];
	unsigned int memblocksendn;


	// Collectors
	int collectorsn;


	// Response-Thread
	
	// keep alive
	unsigned int keepalive;	// gets increased at least once per second from the process getting debugged

	// Quit-Command: Set by the Collector to indicate that the Response Thread is no longer used
	unsigned int cmdquit;

	// Symbol Get Command
	// in
	//SIZE_T programcounter;
	unsigned int programcounter;
	// execute
	unsigned int cmdsymbolget;
	// out
	BOOL              foundline;
	DWORD             displacement;
	MIMAGEHLP_LINEW64 sourceinfo;
	TCHAR			  sourceinfofilename[1024];
	BOOL			  foundfunctionname;
	DWORD64           displacement64;
	BYTE			  symbolbuffer [sizeof(MSYMBOL_INFOW) + (MAXSYMBOLNAMELENGTH * sizeof(WCHAR)) - 1];
};
typedef MSharedMem < MMemCheckSharedMemStackTraceBuffer > MMemCheckSharedMemStackTraceBufferSM;


#ifdef MMemoryDebugSharedMem_CLRSupport
#pragma managed
#endif

#endif
