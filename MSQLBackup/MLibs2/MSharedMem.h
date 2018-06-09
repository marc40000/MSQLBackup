#pragma once

#ifdef MPLATFORM_WIN32

//#define MSHAREDMEMDOINITIALREAD


#ifndef DOTNET2
#include <windows.h>
#else

// Support for .Net 2.0 /clr mixed mode
using namespace System;
using namespace System::Runtime::InteropServices;

#ifndef FILE_MAP_WRITE
#define FILE_MAP_WRITE 2
#define FILE_MAP_READ 4

[DllImport("kernel32.dll",EntryPoint="OpenFileMappingA",SetLastError=true, CharSet=CharSet::Auto) ]
extern "C" unsigned int OpenFileMappingA (int dwDesiredAccess, bool bInheritHandle, const char * lpName );

[DllImport("Kernel32.dll")]
extern "C" unsigned int MapViewOfFile(unsigned int hFileMappingObject,unsigned int dwDesiredAccess, unsigned int dwFileOffsetHigh,unsigned int dwFileOffsetLow, unsigned int dwNumberOfBytesToMap);

[DllImport("Kernel32.dll",EntryPoint="UnmapViewOfFile",SetLastError=true,CharSet=CharSet::Auto)]
extern "C" bool UnmapViewOfFile(unsigned char * lpBaseAddress);

[DllImport("kernel32.dll",EntryPoint="CloseHandle",SetLastError=true,CharSet=CharSet::Auto)]
extern "C" bool CloseHandle(unsigned int hHandle);

typedef unsigned int HANDLE;
#endif

#endif


template <class TYPE> class MSharedMem
{
public:
	TYPE * p;
	unsigned int sharedmemsize;
	HANDLE sharedfile;
	unsigned char *sharedmem;
	unsigned int connectioncount;

	MSharedMem()
	{
		connectioncount = 0;
	}
	~MSharedMem()
	{
	}

	inline unsigned char Hash()
	{
		unsigned char v = 0;
		unsigned int i;
		for (i = 0; i < sharedmemsize; ++i)
		{
			v ^= sharedmem[i];
		}
		unsigned int t = v;
		printf("MSharedMem::Hash: %u/%u\n", sharedmemsize, t);
		return v;
	}

	int InitAsServer(const TCHAR * SHAREDMEMNAME, const unsigned int size = -1)
	{
		if (connectioncount == 0)
		{
			if (size == -1)
			{
				sharedmemsize = sizeof(TYPE);
			}
			else
			{
				sharedmemsize = size;
			}
				
			sharedfile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
				PAGE_READWRITE,
				0,
				sharedmemsize,
				SHAREDMEMNAME);
			sharedmem = (unsigned char *) MapViewOfFile(sharedfile,
				FILE_MAP_READ | FILE_MAP_WRITE, 
				0, 0, 0);
		}

		if (sharedmem == 0)
		{
			return 1;
		}
		else
		{
			ZeroMemory(sharedmem, sharedmemsize);
			p = (TYPE*)sharedmem;
			++connectioncount;

#ifdef MSHAREDMEMDOINITIALREAD
			Hash();
#endif
		}

		return 0;
	}

	int InitAsClient(const TCHAR * SHAREDMEMNAME, const unsigned int size = -1)
	{
		if (connectioncount == 0)
		{
			// sharedmemsize is only set for convenience on the client side.
			// if the rest of the app doesn't use that value, then size = -1 is ok
			if (size == -1)
			{
				sharedmemsize = sizeof(TYPE);
			}
			else
			{
				sharedmemsize = size;
			}

			sharedfile = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, false, SHAREDMEMNAME);
			//sharedfile = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, false, SHAREDMEMNAME);
			sharedmem = (unsigned char *) MapViewOfFile(sharedfile,
				FILE_MAP_READ | FILE_MAP_WRITE, 
				//		FILE_MAP_READ, 
				0, 0, 0);
		}

		if (sharedmem == 0)
		{
			p = 0;
			return 1;
		}
		else
		{
			p = (TYPE*)sharedmem;
			++connectioncount;

#ifdef MSHAREDMEMDOINITIALREAD
			Hash();
#endif
		}

		return 0;
	}

	int Destroy()
	{
		--connectioncount;
		if (connectioncount == 0)
		{
			UnmapViewOfFile(sharedmem);
			CloseHandle(sharedfile);
			p = 0;
		}

		return 0;
	}

	inline bool IsAvailable() const
	{
		return p != 0;
	}

};

// Note: Check for ==0 !!!  It returns 0 if the sm exists, otherwise 1
inline int MSharedMemExists(const TCHAR * SHAREDMEMNAME)
{
	HANDLE sharedfile;
	unsigned char *sharedmem;

	int ret;

	sharedfile = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, false, SHAREDMEMNAME);
	sharedmem = (unsigned char *) MapViewOfFile(sharedfile,
		FILE_MAP_READ | FILE_MAP_WRITE, 
		//		FILE_MAP_READ, 
		0, 0, 0);

	if (sharedmem != 0)
	{
		ret = 0;
	}
	else
	{
		ret = 1;
	}

	UnmapViewOfFile(sharedmem);
	CloseHandle(sharedfile);
	
	return ret;
}

#endif
