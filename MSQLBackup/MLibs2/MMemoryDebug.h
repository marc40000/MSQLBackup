////////////////////////////////////////////////////////////////////////////////////////////
// 
// MMemoryDebug  -  Marc's Memory Debugger
//
// Only works on Windows Vista and VS >= 2008 !!!
//
// native c++ as well as c++/clr is supported.
// For c++/clr change the building property of MMemoryDebug.cpp to native, no clr support
// For both: disable precompiled headers.
// alternatively, link to the lib and don't include MMemoryDebug.cpp
// The reason is: MMemoryDebug needs some code to be executed at really critical situations
//	sometimes because it hooks itself to the memory allocator. During .net's initializations 
//	however, the so called OS Loader Lock get's enabled. While the OS Loader Lock is enabled
//	no managed code is allowed to be executed. But still, allocations are done. So if
//	you compile MMemoryDebug.cpp as managed and an allocation gets done during the initialisation,
//  of .net, you get an OS Loader Lock error telling you not to execute managed code during
//	the Initialization. Compiling it as native as described above solves the problem.
//
//	2008-06-29 (the day of Germany vs. Spain in European Soccer Champignonship)	Marc
//
//
// Use it like this:
//
//	   MMemoryDebugInit();
//	   MMemCheckSuper mc;
//	   mc.Start();
//
//	   {some code that has to be tested for memory leaks}
//
//	   mc.End();
//	   MMemoryDebugDestroy();
//
// MMemCheckSuper can be used multiple times during the code to check multiple sections.
// But MMemoryDebugInit()/Destroy() have to be called only once at the beginning and the
// end of the code.
//
// Turn off C/C++ / Precompiled Headers / Use Precompiled Headers for MMemoryDebug.cpp
//
// Note: MMemCheck has a problem with realloc. See MMemCheck for more details.
//       Later: I think this is not true. It works correctly.
//
// If you are using c++/clr, you can define MCLR and MDEBUGCLR to enable CLR Stacktrace support.
// This also gets you stacktraces of managed c++ code that allocates unmanaged memory and
// leaks.
//
//
//
//
// To have filenames and line numbers in the _CRTDebug stuff, add
//
//		_CRTDBG_MAP_ALLOC
//
// to the preprocessor defines. 
// see http://stackoverflow.com/questions/4177269/crtsetallochook-never-shows-filename-line-number
//
// Also for 32 bit stacktraces to work set 
//
//		Linker->Debugging->Generate Debug Info to Full
//
// at least that made it work for me in VS2017
// I wasn't able to make it work in x64
//
////////////////////////////////////////////////////////////////////////////////////////////



#pragma once

#include "MPlatform.h"

#ifdef _DEBUG
//#define MMemoryDebug_Enabled
#endif

#ifdef MCLR
#ifdef MDEBUGCLR
//#define MMemoryDebug_CLRSupport
#endif
#endif

#ifdef MMemoryDebug_Enabled
#include <malloc.h>
// defining this before including crtdbg logs file and line numbers in malloc (without symbollookup) - but not in new for some reason :/
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include "MHashE.h"
#include "MAssert.h"
#include "MSemaphore.h"
#endif

#ifdef MPLATFORM_WIN32
#include <conio.h>
#include <stdio.h>
#endif

#ifdef MMemoryDebug_Enabled

extern bool mmemorydebuginitialized;
extern MSemaphore * mmemorydebugsymbollock;
extern MSemaphore * mmemorydebugstackwalklock;
extern MSemaphore * allochooklock;


// Note: I extracted the definition included from dbgint.h afterwards, the _CrtMemBlockHeader definition. dbgint.h is not supposed to be included
// by non microsoft users. Only internal system libraries should use it. So ...

#define _CRTBLD
#include <dbgint.h>
#undef _CRTBLD


#ifdef fgregfdf
/*
 * For diagnostic purpose, blocks are allocated with extra information and
 * stored in a doubly-linked list.  This makes all blocks registered with
 * how big they are, when they were allocated, and what they are used for.
 */

#define nNoMansLandSize 4

typedef struct _CrtMemBlockHeader
{
		struct _CrtMemBlockHeader * pBlockHeaderNext;
		struct _CrtMemBlockHeader * pBlockHeaderPrev;
		char *                      szFileName;
		int                         nLine;
#ifdef _WIN64
		/* These items are reversed on Win64 to eliminate gaps in the struct
		 * and ensure that sizeof(struct)%16 == 0, so 16-byte alignment is
		 * maintained in the debug heap.
		 */
		int                         nBlockUse;
		size_t                      nDataSize;
#else  /* _WIN64 */
		size_t                      nDataSize;
		int                         nBlockUse;
#endif  /* _WIN64 */
		long                        lRequest;
		unsigned char               gap[nNoMansLandSize];
		/* followed by:
		 *  unsigned char           data[nDataSize];
		 *  unsigned char           anotherGap[nNoMansLandSize];
		 */
} _CrtMemBlockHeader;
#endif

#include <Dbghelp.h>

typedef _CrtMemBlockHeader * LP_CrtMemBlockHeader;
typedef MHashE < unsigned int , LP_CrtMemBlockHeader > _CrtMemBlockHeaderHashMap;

#endif


//////////////////////////////////////////////////////////////////////////////////////////
// Some functions

#ifdef _DEBUG
#include <Windows.h>
inline void MDebugPrint(TCHAR * s)
{
	OutputDebugString(s);
}
inline void MDebugPrint(char * s)
{
	OutputDebugStringA(s);
}
#else
inline void MDebugPrint(char * s)
{
}
#endif

#ifdef MMemoryDebug_Enabled
inline void MHeapDump(void)
{
	_HEAPINFO hinfo;
	int heapstatus;
	int numLoops;
	hinfo._pentry = NULL;
	numLoops = 0;
	while((heapstatus = _heapwalk(&hinfo)) == _HEAPOK && numLoops < 100)
	{
		printf("%6s block at %Fp of size %4.4X\n",
			   (hinfo._useflag == _USEDENTRY ? "USED" : "FREE"),
			   hinfo._pentry, hinfo._size);
		numLoops++;
	}

	switch(heapstatus)
	{
	case _HEAPEMPTY:
		printf("OK - empty heap\n");
		break;
	case _HEAPEND:
		printf("OK - end of heap\n");
		break;
	case _HEAPBADPTR:
		printf("ERROR - bad pointer to heap\n");
		break;
	case _HEAPBADBEGIN:
		printf("ERROR - bad start of heap\n");
		break;
	case _HEAPBADNODE:
		printf("ERROR - bad node in heap\n");
		break;
	}
}
#endif

#ifdef MMemoryDebug_Enabled

inline void MHeapVerify()
{
	/*_HEAPINFO hinfo;
	int heapstatus;
	int numLoops;
	hinfo._pentry = NULL;
	numLoops = 0;
	while((heapstatus = _heapwalk(&hinfo)) == _HEAPOK)
	{
	}*/

	int heapstatus = _heapchk();

	switch(heapstatus)
	{
	case _HEAPEMPTY:
		//MDebugPrint("OK - empty heap\n");
		break;
	case _HEAPEND:
		//MDebugPrint("OK - end of heap\n");
		break;
	case _HEAPOK:
		//MDebugPrint("OK - OK\n");
		break;
	case _HEAPBADPTR:
		MDebugPrint("ERROR - bad pointer to heap\n");
		MAssert(false);
		break;
	case _HEAPBADBEGIN:
		MDebugPrint("ERROR - bad start of heap\n");
		MAssert(false);
		break;
	case _HEAPBADNODE:
		MDebugPrint("ERROR - bad node in heap\n");
		MAssert(false);
		break;
	default:
		MAssert(false);
	}
}

// Intentionally corrupts the heap. Directly after executing this, a MVerifyHeap() or _heapchk() will fail.
// Of course, this should never be called in a production release. Only use this for debugging special cases etc.
inline void MHeapMakeCorrupt()
{
	int * a = new int[16];
	a[15] = 16;
	a[16] = 16;
	a[17] = 16;
	a[18] = 16;
	a[19] = 16;
	a[20] = 16;
}

#ifdef MMemoryDebug_CLRSupport
inline void MGCVerify()
{
	// This also does some checking on the validy of the managed heap. It might get screwed in mixed apps.
	System::GC::Collect();
	System::GC::WaitForPendingFinalizers();
}
#else
inline void MGCVerify()
{
}
#endif


#else

inline void MHeapVerify()
{
}

inline void MHeapMakeCorrupt()
{
}

inline void MGCVerify()
{
}

#endif

#ifdef MMemoryDebug_Enabled
inline void MEndCode()
{
	int heapstatus = _heapchk();
	if (heapstatus != _HEAPOK)
	{
		printf("heapstatus is not ok: %i\n", heapstatus);
	}

	// setting this bit makes it call _CrtDumpMemoryLeaks() at the very end of the program,
	// even after things like OpenTNL's global destruction took place, which happens somehow after
	// main() gets left.
	_CrtSetDbgFlag(_CrtSetDbgFlag(0) | _CRTDBG_LEAK_CHECK_DF);
	//_CrtDumpMemoryLeaks();
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// MMemCheckDummy

class MMemCheckDummy
{
public:
	inline void Start()
	{
	}
	inline void End(char * s = "")
	{
	}
};


#ifdef MMemoryDebug_Enabled


//////////////////////////////////////////////////////////////////////////////////////////
// Symbols

extern void MOutputSymbols();
extern void MGenerateSymbolsFile();


//////////////////////////////////////////////////////////////////////////////////////////
// MMemCheck

// Note: MMemCheck has a problem with realloc: Assuming the following:
//
//    int * a = malloc(10000);
//    {MMemCheck Start}
//    a = realloc(a, 20000);
//    {MMemCheck Stop}
//    free(a);
//
// MMemCheck reports a leak in this case. The CRT debugging functions put the responsibility for that memory block a the code
// block where realloc lives - MMem* usually leaves it where it was created initially because there lies the responsibility
// to free it as well.
//
// Later: I think this is not true anymore.
//
inline char * MGetBlockTypeString(const int blocktype)
{
	static char t[32];
	switch (blocktype)
	{
	case _FREE_BLOCK:
		strcpy(t, "0_FREE_BLOCK");
		break;
	case _NORMAL_BLOCK:
		strcpy(t, "1_NORMAL_BLOCK");
		break;
	case _CRT_BLOCK:
		strcpy(t, "2_CRT_BLOCK");
		break;
	case _IGNORE_BLOCK:
		strcpy(t, "3_IGNORE_BLOCK");
		break;
	case _CLIENT_BLOCK:
		strcpy(t, "4_CLIENT_BLOCK");
		break;
	default:
		strcpy(t, "UNDEF_BLOCK");
		break;
	}
	return t;
}

class MMemCheck
{
public:
	_CrtMemState mmemstate;

	inline void Start()
	{
		_CrtMemCheckpoint(&mmemstate);
	}
	inline void EndSimple(char * s = "")
	{
		char t[1024];
		sprintf_s(t, 1024, "*** MMemCheck 0 : %s **************************************************************************************************************\n", s);
		MDebugPrint(t);
		//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF);
		_CrtMemDumpAllObjectsSince(&mmemstate);
		//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);	// default according to _CrtSetDbgFlag docu in msdn
		sprintf_s(t, 1024, "*** MMemCheck 1 : %s **************************************************************************************************************\n", s);
		MDebugPrint(t);
	}
	
	// bugged because of _CrtMemDifference() calculating the wrong difference somehow
	inline void EndInclSummary(char * s = "")
	{
		char t[1024];
		sprintf_s(t, 1024, "*** MMemCheck 0 : %s **************************************************************************************************************\n", s);
		MDebugPrint(t);
		
		//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF);
		_CrtMemDumpAllObjectsSince(&mmemstate);
		//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);	// default according to _CrtSetDbgFlag docu in msdn

		_CrtMemState mmemstatenow, diff;
		_CrtMemCheckpoint(&mmemstatenow);

		sprintf_s(t, 1024, ">>>Leak Summary:\n");
		MDebugPrint(t);
		_CrtMemDifference(&diff, &mmemstate, &mmemstatenow);
		int i;
		unsigned int blocksum = 0, sizesum = 0;

		for (i = 0; i < _MAX_BLOCKS; ++i)
		{
			sprintf_s(t, 1024, "    blocktype %i %s:\t    blocks=%u\t    size=%u\n",
				i, MGetBlockTypeString(i), diff.lCounts[i], diff.lSizes[i]);
			MDebugPrint(t);

			blocksum += diff.lCounts[i];
			sizesum += diff.lSizes[i];
		}
		sprintf_s(t, 1024, "    sum:       \t\t\t\t \t    blocks=%u\t    size=%u\n",
			blocksum, sizesum);
		MDebugPrint(t);

		sprintf_s(t, 1024, "    most bytes allocated at once=%u    total bytes allocated=%u\n",
			diff.lHighWaterCount, diff.lTotalCount);
		MDebugPrint(t);
		
		sprintf_s(t, 1024, "*** MMemCheck 1 : %s **************************************************************************************************************\n", s);
		MDebugPrint(t);
	}

	inline void End(char * s = "")
	{
		char t[1024];
		sprintf_s(t, 1024, "*** MMemCheck 0 : %s **************************************************************************************************************\n", s);
		MDebugPrint(t);
		
		//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF);
		_CrtMemDumpAllObjectsSince(&mmemstate);
		//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);	// default according to _CrtSetDbgFlag docu in msdn

		_CrtMemState mmemstatenow, diff;
		_CrtMemCheckpoint(&mmemstatenow);


		_CrtMemBlockHeaderHashMap oldblocks;
		_CrtMemBlockHeader * currentmemblock = mmemstate.pBlockHeader;
		while (currentmemblock != 0)
		{
			oldblocks[currentmemblock->lRequest] = currentmemblock;
			oldblocks.Optimize();

			currentmemblock = currentmemblock->pBlockHeaderNext;			// Prev crashes for some reason, maybe because Optimze adds new request to the list and invalidates currentmemblock?
		}

		int i;
		for (i = 0; i < _MAX_BLOCKS; ++i)
		{
			diff.lCounts[i] = 0;
			diff.lSizes[i] = 0;
		}
		diff.lHighWaterCount = mmemstatenow.lHighWaterCount;
		diff.lTotalCount = mmemstatenow.lTotalCount;
		long requestnumbermax = -1;

		currentmemblock = mmemstatenow.pBlockHeader;
		_CrtMemBlockHeader * lookedupmemblock;
		while (currentmemblock != 0)
		{
			requestnumbermax = ((requestnumbermax > currentmemblock->lRequest) ? requestnumbermax : currentmemblock->lRequest);


			if (oldblocks.Lookup(currentmemblock->lRequest, lookedupmemblock))
			{
				// this existed before => no leak
			}
			else
			{
				// this did not exist before => leak
				if (_BLOCK_TYPE(currentmemblock->nBlockUse) < _MAX_BLOCKS)
				{
					++diff.lCounts[_BLOCK_TYPE(currentmemblock->nBlockUse)];
					diff.lSizes[_BLOCK_TYPE(currentmemblock->nBlockUse)] += currentmemblock->nDataSize;
				}
			}

			currentmemblock = currentmemblock->pBlockHeaderNext;
		}

		sprintf_s(t, 1024, ">>>Leak Summary:\n");
		MDebugPrint(t);
		//_CrtMemDifference(&diff, &mmemstate, &mmemstatenow);
		unsigned int blocksum = 0, sizesum = 0;

		for (i = 0; i < _MAX_BLOCKS; ++i)
		{
			sprintf_s(t, 1024, "    blocktype %i %s:\t    blocks=%u\t    size=%u\n",
				i, MGetBlockTypeString(i), diff.lCounts[i], diff.lSizes[i]);
			MDebugPrint(t);

			blocksum += diff.lCounts[i];
			sizesum += diff.lSizes[i];
		}
		sprintf_s(t, 1024, "    sum:       \t\t\t\t \t    blocks=%u\t    size=%u\n",
			blocksum, sizesum);
		MDebugPrint(t);

		sprintf_s(t, 1024, "    most bytes allocated at once=%u    total bytes allocated=%u    total allocation requests=%i\n",
			diff.lHighWaterCount, diff.lTotalCount, requestnumbermax + 1);
		MDebugPrint(t);
		
		sprintf_s(t, 1024, "*** MMemCheck 1 : %s **************************************************************************************************************\n", s);
		MDebugPrint(t);
	}

	//inline void DumpDifference()
	//{
	//	_CrtMemState mmemstate1, mmemstatediff;
	//	_CrtMemCheckpoint(&mmemstate1);
	//	_CrtMemDifference(&mmemstate1, &mmemstate, &mmemstatediff);
	//	_CrtMemDumpStatistics(&mmemstatediff);
	//}
};

#else

class MMemCheck
{
public:
	inline void Start()
	{
	}
	inline void End(char * s = "")
	{
	}
};

#endif


//////////////////////////////////////////////////////////////////////////////////////////
// MMemCheckStackTrace

#ifdef MMemoryDebug_Enabled

const unsigned int MMemCheckStackTraceMaxStackRecur = 1024;

class MMemCheckStackTraceEntryPosition
{
public:
	DWORD64 address;
};

const unsigned int MMemCheckStackTraceEntryCLR_filenamen = 128;
const unsigned int MMemCheckStackTraceEntryCLR_functionnamen = 32;
class MMemCheckStackTraceEntryCLR
{
public:
	unsigned int line;
	char filename[MMemCheckStackTraceEntryCLR_filenamen];
	char functionname[MMemCheckStackTraceEntryCLR_functionnamen];
};

//#define MnNoMansLandSize 4
//typedef struct MCrtMemBlockHeader		//from dbgint.h slightly renamed
//{
//        struct _CrtMemBlockHeader * pBlockHeaderNext;
//        struct _CrtMemBlockHeader * pBlockHeaderPrev;
//        char *                      szFileName;
//        int                         nLine;
//#ifdef _WIN64
//        /* These items are reversed on Win64 to eliminate gaps in the struct
//         * and ensure that sizeof(struct)%16 == 0, so 16-byte alignment is
//         * maintained in the debug heap.
//         */
//        int                         nBlockUse;
//        size_t                      nDataSize;
//#else  /* _WIN64 */
//        size_t                      nDataSize;
//        int                         nBlockUse;
//#endif  /* _WIN64 */
//        long                        lRequest;
//        unsigned char               gap[MnNoMansLandSize];
//        /* followed by:
//         *  unsigned char           data[nDataSize];
//         *  unsigned char           anotherGap[nNoMansLandSize];
//         */
//} MCrtMemBlockHeader;

class MMemCheckStackTraceEntry
{
public:
	int allocType;
	size_t size;
	int blockType;
	long requestNumber;
	char filename[1024];
	int lineNumber;
	MArrayE < MMemCheckStackTraceEntryPosition > stacktrace;
	unsigned int stacktracemethod;
	//MMemCheckStackTraceEntryPosition stacktrace[1024];
	//unsigned int stacktracen;
	//MCrtMemBlockHeader pheader;
#ifdef MMemoryDebug_CLRSupport
	MArrayE < MMemCheckStackTraceEntryCLR > stacktraceclr;
#endif
};
typedef MMemCheckStackTraceEntry * LPMMemCheckStackTraceEntry;
typedef MHashE < long, LPMMemCheckStackTraceEntry > MMemCheckStackTraceEntryMap;

extern void MMemoryDebugGetStackTrace (unsigned int maxdepth, SIZE_T *framepointer, MMemCheckStackTraceEntry * entry);
extern int MMemoryDebugGetStackTraceFast(unsigned int maxdepth, SIZE_T *framepointer, MMemCheckStackTraceEntry * entry);
extern VOID strapp (LPWSTR *dest, LPCWSTR source);
extern LPWSTR buildsymbolsearchpath ();
extern MMemCheckStackTraceEntryMap * mmemcheckstacktraceentries;

extern int AllocHook(int allocType, void *userData, size_t size, int blockType, long requestNumber, const unsigned char *filename, int lineNumber);

class MMemCheckStackTrace
{
public:

	_CRT_ALLOC_HOOK prevhook;

	unsigned int resultn;

	void Start();
	void End(char * s = "", const bool showinternalframes = false);
	void EndNoOutput();
};


#else

class MMemCheckStackTrace
{
public:
	inline void Start()
	{
	}
	inline void End(char * s = "", const bool showinternalframes = false)
	{
	}
};

#endif


#ifdef MMemoryDebug_Enabled

extern class MMemCheckSuper * mmemchecksuper_dumpclientfunction_owner;
extern void MMemCheckSuper_DumpClientFunction( void *userPortion, size_t blockSize );

class MMemCheckSuper
{
public:
	inline MMemCheckSuper()
	{
	}
	~MMemCheckSuper()
	{
	}

	// Note: MMemCheck has a problem with realloc. See MMemCheck for more details.
	MMemCheck * c;
	MMemCheckStackTrace * cst;

	unsigned int mixn;

	inline void Start()
	{
		c = new MMemCheck();
		c->Start();
		cst = new MMemCheckStackTrace();
		cst->Start();
	}
	
	inline void EndMix(char * s = "", const bool showinternalframes = true)
	{
		char t[1024];
		sprintf(t, "*** MMemCheckMix 0 : %s   **************************************************************************************************************\n", s);
		MDebugPrint(t);
		
		mixn = 0;
		mmemchecksuper_dumpclientfunction_owner = this;
		_CrtSetDumpClient(MMemCheckSuper_DumpClientFunction);
		_CrtMemDumpAllObjectsSince(&c->mmemstate);
		_CrtSetDumpClient(0);
		mmemchecksuper_dumpclientfunction_owner = 0;

		sprintf(t, "*** MMemCheckMix 1 : %s   mixn = %u **************************************************************************************************************\n", s, mixn);
		MDebugPrint(t);
	}
	
	void EndMix1(char * s = "", const bool showinternalframes = true);

	inline void End(char * s = "", const bool showinternalframes = true)
	{
		//new int[1024];
		//_malloc_dbg(10, _CLIENT_BLOCK, __FILE__, __LINE__);

		unsigned int nunfiltered = mmemcheckstacktraceentries->GetN();

		//cst->End(s, showinternalframes);
		//unsigned int n = cst->resultn;
		//delete cst;

		//EndMix(s, showinternalframes);
		//unsigned n = mixn;
		//cst->EndNoOutput();
		//delete cst;

		_CrtSetAllocHook(cst->prevhook);
		EndMix1(s, showinternalframes);
		unsigned n = mixn;
		cst->EndNoOutput();
		delete cst;

		c->End(s);
		delete c;

		char t[1024];
		sprintf_s(t, 1024, "*** MMemCheckSuper :     n = %u    n unfiltered = %u **************************************************************************************************************\n",
			n,
			nunfiltered);
		MDebugPrint(t);
	}
};

#else

class MMemCheckSuper
{
public:
	inline void Start()
	{
	}
	inline void End(char * s = "", const bool showinternalframes = false)
	{
	}
};

#endif


//////////////////////////////////////////////////////////////////////////////////////////
// MMemoryDebug Init and Destroy

#ifdef MMemoryDebug_Enabled

extern void * MMemoryDebugGetStackTraceFast_stacktop;
extern void * MMemoryDebugGetStackTraceFast_stackbottom;

inline void MMemoryDebugInit()
{
	if (mmemorydebuginitialized)
	{
		MAssert(false);
		return;
	}
	mmemorydebugsymbollock = new MSemaphore(1, 1);
	mmemorydebugstackwalklock = new MSemaphore(1, 1);
	mmemorydebuginitialized = true;
	allochooklock = new MSemaphore();


	// do a save stacktrace to find the stack top
	MMemCheckStackTraceEntry stacktrace;
	MMemoryDebugGetStackTrace(1024, 0, &stacktrace);
	MMemoryDebugGetStackTraceFast_stacktop = 0;
	unsigned int i;
	for (i = stacktrace.stacktrace.GetN() - 1; i < stacktrace.stacktrace.GetN(); ++i)
	{
		MMemoryDebugGetStackTraceFast_stacktop = 
			(MMemoryDebugGetStackTraceFast_stacktop >= (void*)(stacktrace.stacktrace.GetBack().address)
			? MMemoryDebugGetStackTraceFast_stacktop : (void*)(stacktrace.stacktrace.GetBack().address));
	}
}

inline void MMemoryDebugDestroy()
{
	if (!mmemorydebuginitialized)
	{
		MAssert(false);
		return;
	}
	delete allochooklock;
	delete mmemorydebugstackwalklock;
	delete mmemorydebugsymbollock;
	mmemorydebuginitialized = true;
}

#else

inline void MOutputSymbols()
{
}

inline void MGenerateSymbolsFile()
{
}

inline void MMemoryDebugInit()
{
}

inline void MMemoryDebugDestroy()
{
}

#endif


#ifdef MPLATFORM_WIN32
#include <windows.h>
#include <process.h>


// this requires admin privileges :/
inline void MMemoryDebugUMHDSetImageFlags(char * imagename)
{
	_spawnlp(_P_WAIT, "gflags.exe", "gflags.exe", "-i", imagename, "+ust", 0);
}

inline void MMemoryDebugUMHDDump(char * outputfilename)
{
	DWORD pid = GetCurrentProcessId();
	char pidparam[64];
	sprintf_s(pidparam, 64, "-p:%u", pid);
	char outputfilenameparam[256];
	sprintf_s(outputfilenameparam, 256, "-f:%s", outputfilename);
	_spawnlp(_P_WAIT, "umdh.exe", "umdh.exe", pidparam, outputfilenameparam, 0);
}

inline void MMemoryDebugUMHDCompare(char * filename0, char * filename1)
{
	_spawnlp(_P_WAIT, "umdh.exe", "umdh.exe", filename0, filename1, "-f:MMemoryDebugUMHDdiff.txt", 0);
}

inline void MMemoryDebugUMHDStart()
{
	printf("MMemoryDebugUMHDStart\n");
	MMemoryDebugUMHDDump("MMemoryDebugUMHDStart.txt");
}

inline void MMemoryDebugUMHDEnd()
{
	printf("MMemoryDebugUMHDEnd\n");
	MMemoryDebugUMHDDump("MMemoryDebugUMHDEnd.txt");
	MMemoryDebugUMHDCompare("MMemoryDebugUMHDStart.txt", "MMemoryDebugUMHDEnd.txt");

	printf("Press 'S' to show the diff or any other key to continue.\n");
	char k = _getch();
	if ((k == 's') || (k == 'S'))
	{
		_spawnlp(_P_NOWAIT, "notepad.exe", "notepad.exe", "MMemoryDebugUMHDdiff.txt", 0);
	}
}

#endif


//#include "MMemoryDebugSharedMem.h"
