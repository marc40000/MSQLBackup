/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// MCriticalSection
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifdef MPLATFORM_WIN32

class MCriticalSection
{
public:
	inline MCriticalSection()
	{
		InitializeCriticalSection(&cs);
	}
	virtual ~MCriticalSection()
	{
		DeleteCriticalSection(&cs);
	}

	CRITICAL_SECTION cs;

	inline void Enter()
	{
		EnterCriticalSection(&cs);
	}

	inline void Leave()
	{
		LeaveCriticalSection(&cs);
	}

	inline bool TryEnter()
	{
		return 0 != TryEnterCriticalSection(&cs);
	}


	// for drop-in-replacement of MSemaphore(1, 1):
	inline void P()
	{
		Enter();
	}
	inline void V()
	{
		Leave();
	}
	inline bool PTry()
	{
		return TryEnter();
	}
};

#else
#ifdef MPLATFORM_EMSCRIPTEN

// emscripten only has one thread, so we can put a dummy here
class MCriticalSection
{
public:
	inline MCriticalSection()
	{
	}
	virtual ~MCriticalSection()
	{
	}

	inline void Enter()
	{
	}

	inline void Leave()
	{
	}

	inline bool TryEnter()
	{
	}


	// for drop-in-replacement of MSemaphore(1, 1):
	inline void P()
	{
		Enter();
	}
	inline void V()
	{
		Leave();
	}
	inline bool PTry()
	{
		return TryEnter();
	}
};

#else

#include <boost/thread.hpp>

class MCriticalSection
{
public:
	inline MCriticalSection()
	{
	}
	virtual ~MCriticalSection()
	{
	}

	boost::mutex mutex;

	inline void Enter()
	{
		mutex.lock();
	}

	inline void Leave()
	{
		mutex.unlock();
	}

	inline bool TryEnter()
	{
		return mutex.try_lock();
	}


	// for drop-in-replacement of MSemaphore(1, 1):
	inline void P()
	{
		Enter();
	}
	inline void V()
	{
		Leave();
	}
	inline bool PTry()
	{
		return TryEnter();
	}
};

#endif
#endif
