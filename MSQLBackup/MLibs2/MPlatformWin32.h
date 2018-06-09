#pragma once

#include <Winsock2.h>
#include <Windows.h>

#if (_MSC_VER < 1900)
// according to http://stackoverflow.com/questions/11579095/why-in-msvc-we-have-snprintf-while-other-compilers-allows-snprintf
// this is correct except for the return value
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#endif

#define unlink _unlink

#define fseek64 _fseeki64

#define getcwd _getcwd

#define exit _exit

#include <tchar.h>
#ifdef _MBCS
	// _TCHAR is defined as char
	// This is ugly and usually not supported throughout MLibs. I use this define to make it work in some places anyways
	#define TCHARISCHAR
#else
	// _TCHAR is defined as wchar_t
#endif


typedef short memscripten_align1_short;
typedef unsigned memscripten_align1_ushort;

typedef int memscripten_align2_int;
typedef int memscripten_align1_int;
typedef unsigned int memscripten_align2_uint;
typedef unsigned int memscripten_align1_uint;

typedef long long memscripten_align4_int64;
typedef long long memscripten_align2_int64;
typedef long long memscripten_align1_int64;
typedef unsigned long long memscripten_align4_uint64;
typedef unsigned long long memscripten_align2_uint64;
typedef unsigned long long memscripten_align1_uint64;

typedef float memscripten_align2_float;
typedef float memscripten_align1_float;

typedef double memscripten_align4_double;
typedef double memscripten_align2_double;
typedef double memscripten_align1_double;

typedef bool memscripten_align2_bool;
typedef bool memscripten_align1_bool;

