#pragma once


#ifdef MMemoryDebug_Enabled

#include "MMemoryDebugSharedMemCommon.h"


class MMemCheckSharedMemStackTrace
{
public:

	TCHAR smname[64];
	
	_CRT_ALLOC_HOOK prevhook;

	int responsethreadstate;	// 1: running, 0: quit
	static DWORD WINAPI ResponseWorker(LPVOID lpParameter);

	inline MMemCheckSharedMemStackTrace()
	{
		_tcscpy_s(smname, 64, _T("SMDefault"));
	}
	
	void Start();
	void End(TCHAR * s = _T(""), const bool showinternalframes = false);
};

#else

class MMemCheckSharedMemStackTrace
{
public:

	TCHAR smname[64];
	
	inline void Start()
	{
	}
	inline void End(TCHAR * s = _T(""), const bool showinternalframes = false)
	{
	}
};

#endif




#ifdef MMemoryDebug_Enabled

class MMemCheckSuperSharedMem
{
public:
	inline MMemCheckSuperSharedMem()
	{
		c = new MMemCheck();
		cst = new MMemCheckSharedMemStackTrace();
	}
	~MMemCheckSuperSharedMem()
	{
		delete cst;
		delete c;
	}

	// Note: MMemCheck has a problem with realloc. See MMemCheck for more details.
	MMemCheck * c;
	MMemCheckSharedMemStackTrace * cst;

	inline void Start()
	{
		c->Start();
		cst->Start();
	}
	
	void End(TCHAR * s = _T(""), const bool showinternalframes = true);
};

#else

class MMemCheckSuperSharedMem
{
public:
	inline void Start()
	{
	}
	inline void End(TCHAR * s = _T(""), const bool showinternalframes = false)
	{
	}
};

#endif


#ifdef MMemoryDebug_Enabled

extern int MMemoryDebugStackTraceCLREnabled;

#define MMemoryDebugStackTraceCLROn() ++MMemoryDebugStackTraceCLREnabled
#define MMemoryDebugStackTraceCLROff() --MMemoryDebugStackTraceCLREnabled

#else
#define MMemoryDebugStackTraceCLROn()
#define MMemoryDebugStackTraceCLROff()
#endif
