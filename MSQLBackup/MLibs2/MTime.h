#pragma once

typedef unsigned long long MTimeStamp;

#ifdef MPLATFORM_WIN32
#ifdef MPLATFORM_CLR

#include <TCHAR.h>
//#include "MSetThreadName.h"

int MTimeInit();
int MTimeDestroy();

// returns time in milli seconds
unsigned int MTimeMS();

// returns time in micro seconds
unsigned long long MTimeYS();

#else

#include "windows.h"
#include <TCHAR.h>
//#include "MSetThreadName.h"

extern LARGE_INTEGER freq;
//extern LARGE_INTEGER freq_2;

inline int MTimeInit()							// This is the only function of MTime that is not threadsafe. Make sure to call it only once by one thread!
{
	QueryPerformanceFrequency(&freq);
	//freq_2.QuadPart = freq.QuadPart / 2;
	return 0;
}

inline int MTimeDestroy()
{
	// nothing to do
	return 0;
}

// returns time in seconds
inline double MTimeS()
{
	//return GetTickCount();
	LARGE_INTEGER v;
	QueryPerformanceCounter(&v);
	
	//return (unsigned int)((((unsigned long long)v.QuadPart) * 1000 + freq_2.QuadPart) / freq.QuadPart);

	double t = (double)v.QuadPart;
	t /= (double)freq.QuadPart;
	return t;
}

// returns time in milli seconds
inline unsigned int MTimeMS()
{
	//return GetTickCount();
	LARGE_INTEGER v;
	QueryPerformanceCounter(&v);
	
	//return (unsigned int)((((unsigned long long)v.QuadPart) * 1000 + freq_2.QuadPart) / freq.QuadPart);

	double t = (double)v.QuadPart;
	t *= 1000.0;
	t /= (double)freq.QuadPart;
	return (unsigned int)((unsigned long long)t);
}

// returns time in micro seconds
inline unsigned long long MTimeYS()
{
	//return GetTickCount();
	LARGE_INTEGER v;
	QueryPerformanceCounter(&v);

	//return (((unsigned long long)v.QuadPart) * 1000000 + freq_2.QuadPart) / freq.QuadPart;

	double t = (double)v.QuadPart;
	t *= 1000000.0;
	t /= (double)freq.QuadPart;
	return (unsigned long long)t;
}

inline LARGE_INTEGER GetPerfFreq()
{
	return freq;
}

inline unsigned long long PerfToMS(const unsigned long long v)
{
	double t = (double)v;
	t *= 1000.0;
	t /= (double)freq.QuadPart;
	return (unsigned long long)t;
}

inline unsigned long long PerfToYS(const unsigned long long v)
{
	double t = (double)v;
	t *= 1000000.0;
	t /= (double)freq.QuadPart;
	return (unsigned long long)t;
}

#endif
#endif


#if defined(MPLATFORM_LINUX) || defined(MPLATFORM_ANDROID)

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

inline int MTimeInit()
{
	return 0;
}

inline int MTimeDestroy()
{
	return 0;
}

// returns time in seconds
inline double MTimeS()
{
	struct timeval t;
	gettimeofday(&t, NULL);

	double v = ((double)t.tv_sec) + (((double)t.tv_usec) / 1000000.0);

	return v;
}

// returns time in milli seconds
inline unsigned int MTimeMS()
{
	struct timeval t;
	gettimeofday(&t, NULL);

	unsigned int v = ((unsigned int)t.tv_sec) * 1000 + ((unsigned int)(t.tv_usec / 1000));

	return v;
}

// returns time in micro seconds
inline unsigned long long MTimeYS()
{
	struct timeval t;
	gettimeofday(&t, NULL);

	unsigned long long v = ((unsigned long long)t.tv_sec) * 1000000ULL + ((unsigned long long)t.tv_usec);

	return v;
}

#endif


#ifdef MPLATFORM_EMSCRIPTEN

#include <emscripten.h>

inline int MTimeInit()
{
	return 0;
}

inline int MTimeDestroy()
{
	return 0;
}

inline double MTimeS()
{
	return ((double)emscripten_get_now()) / 1000.0;
}

// returns time in milli seconds
inline unsigned int MTimeMS()
{
	return (unsigned int)emscripten_get_now();		// note: emscripten_get_now() returns the time in ms as a double
}

// returns time in micro seconds
inline unsigned long long MTimeYS()
{
	return (unsigned long long)(emscripten_get_now() * 1000.0);
}

#endif



#include "MMath.h"
#include <time.h>
#ifdef MPLATFORM_WIN32
#include <windows.h>
#endif

class MTimeProvider
{
public:
	inline MTimeProvider()
	{
	}
	virtual ~MTimeProvider()
	{
	}

	// guarantees v(t+1) >= v(t) for all t
	// returns -1 on error, e.g. time not defined
	virtual unsigned long long GetTimeInc() = 0;

	// might be v(t+1) < v(t)
	// returns -1 on error, e.g. time not defined
	virtual unsigned long long GetTimeAccurate() = 0;

	// returns if time is getable
	virtual const bool IsValid() const = 0;
};

class MTimeLocal : public MTimeProvider
{
public:
	inline MTimeLocal()
	{
		lasttime = 0;
	}
	unsigned long long lasttime;
	virtual unsigned long long GetTimeInc()
	{
		// because of strange time shifting bug of Performance Counters on some boards. see msdn.
		lasttime = MMax(lasttime, MTimeYS());
		return lasttime;
	}
	virtual unsigned long long GetTimeAccurate()
	{
		return MTimeYS();
	}
	virtual const bool IsValid() const
	{
		return true;
	}
};

#ifdef MPLATFORM_WIN32
class MTimeLocalUTC : public MTimeProvider
{
public:
	inline MTimeLocalUTC(const bool waitforgettingvalid = false, const unsigned int sleepinterval = 1000, const unsigned int changedivider = 2)
	{
		lasttime = 0;
		torealtimedeltanext = 0;
		torealtimedelta[0] = 0;
		torealtimedelta[1] = 0;
		torealtimedelta[2] = 0;
		torealtimedelta[3] = 0;
		isvalid = false;
		this->sleepinterval = sleepinterval;
		this->changedivider = changedivider;
		
		cont = true;

		DWORD id;
		threadhandle = CreateThread(NULL, 0, MTimeLocalUTCWorker, this, 0, &id);
		//MSetThreadName(id, "MTimeLocalUTC::Worker");

		if (waitforgettingvalid)
		{
			while (!IsValid())
			{
				Sleep(1);
			}
		}
	}
	virtual ~MTimeLocalUTC()
	{
		if (cont)
		{
			cont = false;
			while (!cont)
			{
				Sleep(1);
			}
		}
		//TerminateThread(threadhandle, 0);
		CloseHandle(threadhandle);
	}

	static DWORD WINAPI MTimeLocalUTCWorker(LPVOID lpParameter)
	{
		MTimeLocalUTC * t = (MTimeLocalUTC *)lpParameter;

		while (t->cont)
		{
			Sleep(1);

			unsigned long long t0 = _time64(0);
			while (_time64(0) == t0)
			{
				Sleep(1);
			}
			// the time just changed 1 sec
			unsigned long long tys = MTimeYS();
			
			++t0;
			unsigned long long torealtimedeltanew = t0 * 1000000 - tys;


			// 2 Options:

			// just use the new delta
			//t->torealtimedelta[t->torealtimedeltanext % 4] = torealtimedeltanew;

			// smooth it with the previous deltas
			if (t->isvalid)
			{
				unsigned long long torealtimedeltaprevious = t->torealtimedelta[(t->torealtimedeltanext + 3) % 4];
				//long long change = (((long long)torealtimedeltanew) - ((long long)torealtimedeltaprevious)) / 8;
				//long long change = (((long long)torealtimedeltanew) - ((long long)torealtimedeltaprevious)) / 4;
				//long long change = (((long long)torealtimedeltanew) - ((long long)torealtimedeltaprevious)) / 2;
				//long long change = (((long long)torealtimedeltanew) - ((long long)torealtimedeltaprevious)) / 1;
				long long change = (((long long)torealtimedeltanew) - ((long long)torealtimedeltaprevious)) / t->changedivider;
				t->torealtimedelta[t->torealtimedeltanext % 4] = torealtimedeltaprevious + change;
			}
			else
			{
				t->torealtimedelta[t->torealtimedeltanext % 4] = torealtimedeltanew;
				t->isvalid = true;
			}

			++t->torealtimedeltanext;

			if (t->sleepinterval == -1)
			{
				// No further synchronization between the system time and our time. Only the first time.
				// This is needed by my blobby pc because of the crappy system time going wrong +/- a minute within half an hour.
				break;
			}
			else
			{
				Sleep(t->sleepinterval);
			}
		}

		t->cont = true;

		return 0;
	}

	HANDLE threadhandle;
	bool cont;

	unsigned long long lasttime;

	unsigned int torealtimedeltanext;
	unsigned long long torealtimedelta[4];
	bool isvalid;
	unsigned int sleepinterval;
	unsigned int changedivider;

	/*inline int AdjustToSystemTime()
	{
		unsigned long long nowmtime;
		unsigned long long nowrealtime;
		nowmtime = MTimeYS();
		
		unsigned int i;
		nowrealtime = 0;
		const unsigned int n = 100;
		for (i = 0; i < n; ++i)
		{
			nowrealtime += _time64(0);		// UTC 1970-01-01
			Sleep(1);
		}
		nowrealtime *= 1000000;
		nowrealtime = (nowrealtime + (n / 2)) / n;

		unsigned long long nowmtime2 = MTimeYS();
		nowmtime += nowmtime2;
		nowmtime = (nowmtime + 1) / 2;

		torealtimedelta = nowrealtime - nowmtime;
		
		return 0;
	}*/
	/*inline int AdjustToSystemTimeWait()
	{
		Sleep(1);

		unsigned long long t0 = _time64(0);
		while (_time64(0) == t0)
		{
		}
		// the time just changed 1 sec
		unsigned long long tys = MTimeYS();
		
		torealtimedelta = t0 * 1000000 - tys;

		return 0;
	}*/

	virtual unsigned long long GetTimeInc()
	{
		// because of strange time shifting bug of Performance Counters on some boards. see msdn.
		lasttime = MMax(lasttime, MTimeYS() + torealtimedelta[(torealtimedeltanext + 3) % 4]);
		return lasttime;
	}
	virtual unsigned long long GetTimeAccurate()
	{
		return MTimeYS() + torealtimedelta[(torealtimedeltanext + 3) % 4];
	}
	virtual const bool IsValid() const
	{
		return isvalid;
	}

	inline unsigned long long GetToRealTimeDelta() const
	{
		return torealtimedelta[(torealtimedeltanext + 3) % 4];
	}
	inline void SetSleepInterval(const unsigned int sleepinterval)
	{
		this->sleepinterval = sleepinterval;
	}

	// Note: The difference can be up to 1 sec between the system time and mtime while still being in sync.
	//       That's because the system time's accuracy is only 1 sec. So this function only makes sense when called
	//       with bounds at least equals to 1000000 + some synchronization error, in the size of window's task switching.
	//       So 1032000 should be ok. Let's use 1050000.
	inline const bool IsWithinBounds(const unsigned long long bounds = 1050000)
	{
		if (IsValid())
		{
			unsigned long long tsys = _time64(0);
			unsigned long long tm = GetTimeAccurate();

			tsys *= 1000000;

			long long delta = (long long)(tsys - tm);
			long long epsilon = ((delta < 0) ? -delta : delta);

			if (epsilon > (long long)bounds)
			{
				_tprintf(_T("Critical Error:   MTimeLocalUTC is out of bounds!   delta: %lli\n"), delta);
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
};

#else

class MTimeLocalUTC : public MTimeProvider
{
public:
	inline MTimeLocalUTC(const bool waitforgettingvalid = false, const unsigned int sleepinterval = 1000, const unsigned int changedivider = 2)
	{
	}
	virtual ~MTimeLocalUTC()
	{
	}

	virtual unsigned long long GetTimeInc()
	{
		return MTimeYS();
	}
	virtual unsigned long long GetTimeAccurate()
	{
		return MTimeYS();
	}
	virtual const bool IsValid() const
	{
		return true;
	}

	inline unsigned long long GetToRealTimeDelta() const
	{
		return 0;
	}
	inline void SetSleepInterval(const unsigned int sleepinterval)
	{
	}

	// Note: The difference can be up to 1 sec between the system time and mtime while still being in sync.
	//       That's because the system time's accuracy is only 1 sec. So this function only makes sense when called
	//       with bounds at least equals to 1000000 + some synchronization error, in the size of window's task switching.
	//       So 1032000 should be ok. Let's use 1050000.
	inline const bool IsWithinBounds(const unsigned long long bounds = 1050000)
	{
		return true;
	}
};
#endif
