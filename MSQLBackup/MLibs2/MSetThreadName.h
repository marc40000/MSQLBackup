#pragma once

#ifdef MPLATFORM_WIN32_MSVC

//#include "MMemoryDebug.h"
#include <Windows.h>

#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

inline void MSetThreadName(DWORD dwThreadID, const char* threadName)
{
   Sleep(10);
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = threadName;
   info.dwThreadID = dwThreadID;
   info.dwFlags = 0;

   // This throws an SEH Exception in .NET, but that's ok I guess:
   // "A first chance exception of type 'System.Runtime.InteropServices.SEHException' occurred in Client.exe"
   __try
   {
	   RaiseException(MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info);
   }
   __except(EXCEPTION_EXECUTE_HANDLER)
   {
   }
}

#ifdef wchar_t
inline void MSetThreadName(DWORD dwThreadID, const wchar_t * threadNamet)
{
	char threadName[1024];
	wcstombs(threadName, threadNamet, 1024);
	MSetThreadName(dwThreadID, threadName);
}
#endif

#else

inline void MSetThreadName(DWORD dwThreadID, const char* threadName)
{
}

#endif
