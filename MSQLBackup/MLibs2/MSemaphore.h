/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// MSemaphore
//
// In _DEBUG mode, it counts the value along. So you can set breakpoints and look at the current value of the Semaphore.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>

class MSemaphore
{
public:
	// Note:
	//	for process local semaphores, set name to NULL
	//	for terminal session local semaphores, prefix name with "Local\"
	//	for global semaphores, prefix name with "Global\"
	inline MSemaphore(const unsigned int valuestart = 1, const unsigned int valuemax = 1, TCHAR * name = NULL)
	{
		semaphore = CreateSemaphore(NULL, valuestart, valuemax, name);
#ifdef _DEBUG
		value = valuestart;
		this->valuemax = valuemax;
#endif
	}
	virtual ~MSemaphore()
	{
		CloseHandle(semaphore);
	}

	HANDLE semaphore;

#ifdef _DEBUG
	int value;
	int valuemax;
#endif

	// P, wait, acquire, down
	// returns 
	//		WAIT_OBJECT_0	on continuation
	//		WAIT_TIMEOUT	timeout
	//		WAIT_ABANDONED	error
	inline DWORD P(const DWORD waitmilliseconds = INFINITE)
	{
		DWORD result = WaitForSingleObject(semaphore, waitmilliseconds);
#ifdef _DEBUG
		if (result == WAIT_OBJECT_0)
		{
			--value;
		}
#endif
		return result;
	}

	// V, signal, release, up
	// returns 
	//		!= 0	success
	//		== 0	failure, call GetLastError() for details
	inline DWORD V()
	{
		DWORD result = ReleaseSemaphore(semaphore, 1, NULL);
#ifdef _DEBUG
		if (result != 0)
		{
			++value;
			if (value > valuemax)
			{
				value = valuemax;
			}
		}
#endif
		return result;
	}
};
