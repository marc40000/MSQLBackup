#pragma once

#ifdef MPLATFORM_WIN32
#include <windows.h>
#include <io.h>
#include <direct.h>
#endif
#include "MAssert.h"
#ifndef MPLATFORM_EMSCRIPTEN
//#include <boost/filesystem.hpp>
//using namespace boost::filesystem;
#include <filesystem>
#endif

#ifdef MPLATFORM_WIN32_MSVC
inline void Mfreadline(FILE *f, TCHAR *s)
{
	unsigned int i=0;
	while (true)
	{
		if (feof(f))
			break;
		s[i] = 0;
		fread(s+i,1,1,f);
		if (s[i] == 13)
			break;
		i++;
	}
	s[i] = 0;
	fread(s+i,1,1,f);
	s[i] = 0;
}

inline void Mfreadline(FILE *f, char *s)
{
	unsigned int i = 0;
	while (true)
	{
		if (feof(f))
			break;
		s[i] = 0;
		fread(s + i, 1, 1, f);
		if (s[i] == 13)
			break;
		i++;
	}
	s[i] = 0;
	fread(s + i, 1, 1, f);
	s[i] = 0;
}
#endif

#ifdef MPLATFORM_WIN32_MSVC
inline void Mfreadline_linux(FILE *f, TCHAR *s)
{
	unsigned int i=0;
	while (true)
	{
		if (feof(f))
			break;
		s[i] = 0;
		fread(s+i,1,1,f);
		if (s[i] == 10)
			break;
		i++;
	}
	s[i] = 0;
}
#endif

// returns false if the file is opened for writing
#ifdef MPLATFORM_WIN32_MSVC
inline bool MFileExistsFOpen(const TCHAR * filename)
{
	FILE * f;
	_tfopen_s(&f, filename, _T("rb"));
	if (f != 0)
	{
		fclose(f);
		return true;
	}
	else
	{
		return false;
	}
}
#endif

// returns true if the file is opened for writing
#ifdef MPLATFORM_WIN32_MSVC
inline bool MFileExistsCreateFile(const TCHAR * filename)
{
	HANDLE f = CreateFile(filename, FILE_READ_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (f != INVALID_HANDLE_VALUE)
	{
		CloseHandle(f);
		return true;
	}
	else
	{
		return false;
	}
}
#endif

// returns true if the file is opened for writing
// ... and hopefully in all other strange situations
#ifdef MPLATFORM_WIN32_MSVC
inline bool MFileExistsFindFirst(const TCHAR * filename)
{
	_tfinddata64_t fd;
	intptr_t fh = _tfindfirst64(filename, &fd);

	bool exists = (fh != -1);

	_findclose(fh);

	return exists;
}
#endif

#ifdef MPLATFORM_WIN32
inline bool MFileExistsFindFirst(const char * filename)
{
	__finddata64_t fd;
	intptr_t fh = _findfirst64(filename, &fd);

	bool exists = (fh != -1);

	_findclose(fh);

	return exists;
}
#endif

#ifdef MPLATFORM_WIN32_MSVC
inline bool MFileExists(const TCHAR * filename)
{
	return MFileExistsFindFirst(filename);
}
#endif

#ifndef MPLATFORM_EMSCRIPTEN
inline bool MFileExistsBoost(const char * filename)
{
	//return std::tr2::sys::exists(filename);
	return std::experimental::filesystem::exists(filename);
}

inline bool MFileExists(const char * filename)
{
	return MFileExistsBoost(filename);
}
#endif

#ifdef MPLATFORM_WIN32_MSVC
inline int MGetFileSize(const wchar_t * filename, unsigned long long * filesize)
{
	HANDLE f = CreateFile(filename, FILE_READ_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (f != INVALID_HANDLE_VALUE)
	{
		GetFileSizeEx(f, (PLARGE_INTEGER)filesize);
		CloseHandle(f);
		return 0;
	}
	else
	{
		return 1;
	}
}
#endif

/*
#ifdef MPLATFORM_WIN32
inline int MGetFileSizeA(const char * filename, unsigned long long * filesize)
{
	HANDLE f = CreateFileA(filename, FILE_READ_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (f != INVALID_HANDLE_VALUE)
	{
		GetFileSizeEx(f, (PLARGE_INTEGER)filesize);
		CloseHandle(f);
		return 0;
	}
	else
	{
		return 1;
	}
}
#endif
*/

#ifdef MPLATFORM_EMSCRIPTEN
inline int MGetFileSizeA(const char * filename, unsigned long long * filesize)
{
	FILE * f = fopen(filename, "rb");
	if (f == 0)
	{
		return 1;
	}
	else
	{
		fseek(f, 0, SEEK_END);
		*filesize = (unsigned long long)ftell(f);
		fclose(f);
		return 0;
	}
}
#else
inline int MGetFileSizeA(const char * filename, unsigned long long * filesize)
{
/*	FILE * f = fopen(filename, "rb");
	if (f == 0)
	{
		return 1;
	}
	else
	{
		fseek(f, 0, SEEK_END);
		*filesize = (unsigned long long)ftelli64(f);
		fclose(f);
		return 0;
	}
	return 0;*/
	if (MFileExists(filename))
	{
		//*filesize = std::tr2::sys::file_size(filename);
		*filesize = std::experimental::filesystem::file_size(filename);
		return 0;
	}
	else
	{
		return 1;
	}
}
#endif

#ifdef MPLATFORM_WIN32
inline void MFileTimeToULL(::FILETIME * ft, unsigned long long * ull)
{
	LARGE_INTEGER i;
	i.LowPart = ft->dwLowDateTime;
	i.HighPart = ft->dwHighDateTime;
	*ull = i.QuadPart;
}

inline void MULLToFileTime(unsigned long long ull, ::FILETIME * ft)
{
	LARGE_INTEGER i;
	i.QuadPart = ull;
	ft->dwLowDateTime = i.LowPart;
	ft->dwHighDateTime = i.HighPart;
}
#endif

#ifdef MPLATFORM_WIN32_MSVC
inline int MGetFileTime(TCHAR * filename, unsigned long long * timecreated, unsigned long long * timeaccessed, unsigned long long * timemodified)
{
	HANDLE f = CreateFile(filename, FILE_READ_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (f != INVALID_HANDLE_VALUE)
	{
		//GetFileTime(f, (LPFILETIME)timecreated, (LPFILETIME)timeaccessed, (LPFILETIME)timemodified);

		::FILETIME tc, ta, tm;
		GetFileTime(f, &tc, &ta, &tm);
		MFileTimeToULL(&tc, timecreated);
		MFileTimeToULL(&ta, timeaccessed);
		MFileTimeToULL(&tm, timemodified);

		CloseHandle(f);
		return 0;
	}
	else
	{
		return 1;
	}
}
#endif

#ifdef MPLATFORM_WIN32
inline int MGetFileTime(char * filename, unsigned long long * timecreated, unsigned long long * timeaccessed, unsigned long long * timemodified)
{
	HANDLE f = CreateFileA(filename, FILE_READ_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (f != INVALID_HANDLE_VALUE)
	{
		//GetFileTime(f, (LPFILETIME)timecreated, (LPFILETIME)timeaccessed, (LPFILETIME)timemodified);

		::FILETIME tc, ta, tm;
		GetFileTime(f, &tc, &ta, &tm);
		MFileTimeToULL(&tc, timecreated);
		MFileTimeToULL(&ta, timeaccessed);
		MFileTimeToULL(&tm, timemodified);

		CloseHandle(f);
		return 0;
	}
	else
	{
		return 1;
	}
}


// MTime returns ys since 1. January 00:00:00 1970 UTC
// FILETIME is 100ns since 1. January, 1601 UTC
// difference is 134774 days
// Note: I used some website to calculate this. It only returned the number of days. 
//       Maybe there are some seconds wrong because of time adjustments over the years
//       So if precise conversion is needed, this difference should be calculated/verified exactly.
inline int MTimeFILETIMEToMTime(::FILETIME * filetime, unsigned long long * mtime)
{
	LARGE_INTEGER li;
	li.LowPart = filetime->dwLowDateTime;
	li.HighPart = filetime->dwHighDateTime;
	*mtime = li.QuadPart / 10 - 134774ULL * 24 * 60 * 60 * 1000000;
	return 0;
}

inline int MTimeMTimeToFILETIME(unsigned long long mtime, ::FILETIME * filetime)
{
	LARGE_INTEGER li;
	li.QuadPart = (mtime + 134774ULL * 24 * 60 * 60 * 1000000) * 10;
	filetime->dwLowDateTime = li.LowPart;
	filetime->dwHighDateTime = li.HighPart;
	return 0;
}

inline int MGetFileMTime(char * filename, unsigned long long * timecreated, unsigned long long * timeaccessed, unsigned long long * timemodified)
{
	HANDLE f = CreateFileA(filename, FILE_READ_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (f != INVALID_HANDLE_VALUE)
	{
		//GetFileTime(f, (LPFILETIME)timecreated, (LPFILETIME)timeaccessed, (LPFILETIME)timemodified);

		::FILETIME tc, ta, tm;
		GetFileTime(f, &tc, &ta, &tm);
		MTimeFILETIMEToMTime(&tc, timecreated);
		MTimeFILETIMEToMTime(&ta, timeaccessed);
		MTimeFILETIMEToMTime(&tm, timemodified);

		CloseHandle(f);
		return 0;
	}
	else
	{
		return 1;
	}
}
#endif

#ifdef MPLATFORM_WIN32_MSVC
inline int MSetFileTime(TCHAR * filename, unsigned long long timemodified)
{
	HANDLE f = CreateFile(filename, FILE_WRITE_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f != INVALID_HANDLE_VALUE)
	{
		//SetFileTime(f, NULL, NULL, (LPFILETIME)&timemodified);

		::FILETIME tm;
		MULLToFileTime(timemodified, &tm);
		SetFileTime(f, NULL, NULL, &tm);

		CloseHandle(f);
		return 0;
	}
	else
	{
		return 1;
	}
}
#endif

#ifdef MPLATFORM_WIN32_MSVC
inline int MSetFileTime(TCHAR * filename, unsigned long long timecreated, unsigned long long timeaccessed, unsigned long long timemodified)
{
	HANDLE f = CreateFile(filename, FILE_WRITE_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f != INVALID_HANDLE_VALUE)
	{
		//SetFileTime(f, NULL, NULL, (LPFILETIME)&timemodified);

		::FILETIME tc, ta, tm;
		MULLToFileTime(timecreated, &tc);
		MULLToFileTime(timeaccessed, &ta);
		MULLToFileTime(timemodified, &tm);
		SetFileTime(f, &tc, &ta, &tm);

		CloseHandle(f);
		return 0;
	}
	else
	{
		return 1;
	}
}
#endif

#ifdef MPLATFORM_WIN32_MSVC
inline int MGetDirTime(TCHAR * filename, unsigned long long * timecreated, unsigned long long * timeaccessed, unsigned long long * timemodified)
{
	HANDLE f = CreateFile(filename, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, 0);
	if (f != INVALID_HANDLE_VALUE)
	{
		GetFileTime(f, (LPFILETIME)timecreated, (LPFILETIME)timeaccessed, (LPFILETIME)timemodified);
		CloseHandle(f);
		return 0;
	}
	else
	{
		return 1;
	}
}
#endif

#ifdef MPLATFORM_WIN32_MSVC
inline int MCreateEmptyFile(TCHAR * filename)
{
	FILE * f;
	_tfopen_s(&f, filename, _T("wb"));
	if (f == 0)
	{
		return 1;
	}
	fclose(f);
	return 0;
}
#endif


// dir has to have a trailing backslash '\\'
#ifdef MPLATFORM_WIN32_MSVC
inline void MDeleteDir(TCHAR * dir)
{
	// this is a delete function so let's add a precaution not to call it wrongly
	unsigned int n = _tcslen(dir);
	if (dir[n - 1] != _T('\\'))
	{
		MAssert(false);
		return;
	}

	TCHAR t[1024];
	TCHAR filemask[1024];
	_stprintf_s(filemask, 1024, _T("%s*"), dir);
	_tfinddata64_t fd;
	intptr_t fh = _tfindfirst64(filemask, &fd);
	t[0] = 0;
	while (fh != -1)
	{
		_stprintf_s(t, 1024, _T("%s%s"), dir, fd.name);
		_tunlink(t);

		if (_tfindnext64(fh, &fd) != 0)
		{
			break;
		}
	}
	_findclose(fh);

	
	_trmdir(dir);
	//_tcscpy_s(t, 1024, dir);
	//t[n - 1] = 0;
	//_trmdir(dir);
};
#endif


#ifdef MPLATFORM_WIN32

// sometimes this is not defined for some reason even though windows.h is included and that's what should be included according to msdn.
#ifdef FSCTL_SET_COMPRESSION
// Note: the _s versions of these functions don't work. For some reason, CreateFile returns an invalid handle. I assume it has to do
// with the security features of the _s versions.
// Opens the file and sets the compressed flag. Best used when the file gets created. Applying the compressed flag afterwards takes some time.
inline FILE * fopencompressed(const char * filename, const char * attributes)
{
	FILE * f = fopen(filename, attributes);
	if (f != 0)
	{
		HANDLE handle = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle != INVALID_HANDLE_VALUE)
		{
			unsigned short int value = COMPRESSION_FORMAT_DEFAULT;
			DWORD bytesreturned = 0;
			BOOL ret = DeviceIoControl(handle, FSCTL_SET_COMPRESSION, &value, sizeof(unsigned short int), NULL, 0, &bytesreturned, 0);
			//if (ret == 0)
			//{
			//	DWORD err = GetLastError();
			//	int a = 3;
			//}
			CloseHandle(handle);
		}

	}
	return f;
}
inline errno_t fopen_scompressed(FILE ** f, const char * filename, const char * attributes)
{
	errno_t errnot = fopen_s(f, filename, attributes);
	if ((*f) != 0)
	{
		HANDLE handle = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle != INVALID_HANDLE_VALUE)
		{
			unsigned short int value = COMPRESSION_FORMAT_DEFAULT;
			DWORD bytesreturned = 0;
			BOOL ret = DeviceIoControl(handle, FSCTL_SET_COMPRESSION, &value, sizeof(unsigned short int), NULL, 0, &bytesreturned, 0);
			//if (ret == 0)
			//{
			//	DWORD err = GetLastError();
			//	int a = 3;
			//}
			CloseHandle(handle);
		}

	}
	return errnot;
}

// This is not really maintained in windows. There are some strange limits of about 300-500MB after which strange things occur.
// Better use fopencompressed
inline FILE * fopensparse(const char * filename, const char * attributes)
{
	FILE * f = fopen(filename, attributes);
	if (f != 0)
	{
		HANDLE handle = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle != INVALID_HANDLE_VALUE)
		{
			unsigned short int value = COMPRESSION_FORMAT_DEFAULT;
			DWORD bytesreturned = 0;
			BOOL ret = DeviceIoControl(handle, FSCTL_SET_SPARSE, &value, sizeof(unsigned short int), NULL, 0, &bytesreturned, 0);
			//if (ret == 0)
			//{
			//	DWORD err = GetLastError();
			//	int a = 3;
			//}
			CloseHandle(handle);
		}

	}
	return f;
}
inline errno_t fopen_ssparse(FILE ** f, const char * filename, const char * attributes)
{
	errno_t errnot = fopen_s(f, filename, attributes);
	if ((*f) != 0)
	{
		HANDLE handle = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle != INVALID_HANDLE_VALUE)
		{
			unsigned short int value = COMPRESSION_FORMAT_DEFAULT;
			DWORD bytesreturned = 0;
			BOOL ret = DeviceIoControl(handle, FSCTL_SET_SPARSE, &value, sizeof(unsigned short int), NULL, 0, &bytesreturned, 0);
			//if (ret == 0)
			//{
			//	DWORD err = GetLastError();
			//	int a = 3;
			//}
			CloseHandle(handle);
		}

	}
	return errnot;
}
#else

#define fopencompressed fopen
#define fopen_scompressed fopen_s
#define fopensparse fopen
#define fopen_ssparse fopen_s

#endif

#else

#define fopencompressed fopen
#define fopen_scompressed fopen_s
#define fopensparse fopen
#define fopen_ssparse fopen_s

#endif
