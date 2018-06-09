#pragma once

#include "emscripten.h"

#define TCHAR char
#define _T
typedef void * LPVOID;

typedef unsigned long DWORD;

#define WINAPI

#define SOCKET int
//#define closesocket close
// closesocket/close doesn't seem to be available in emscripten, so I put a fake implementation. 
inline const int closesocket(const int sock)
{
	return 0;
}
#define INVALID_SOCKET -1



typedef short __attribute__((aligned(1))) memscripten_align1_short;
typedef unsigned short __attribute__((aligned(1))) memscripten_align1_ushort;

typedef int __attribute__((aligned(2))) memscripten_align2_int;
typedef int __attribute__((aligned(1))) memscripten_align1_int;
typedef unsigned int __attribute__((aligned(2))) memscripten_align2_uint;
typedef unsigned int __attribute__((aligned(1))) memscripten_align1_uint;

typedef long long __attribute__((aligned(4))) memscripten_align4_int64;
typedef long long __attribute__((aligned(2))) memscripten_align2_int64;
typedef long long __attribute__((aligned(1))) memscripten_align1_int64;
typedef unsigned long long __attribute__((aligned(4))) memscripten_align4_uint64;
typedef unsigned long long __attribute__((aligned(2))) memscripten_align2_uint64;
typedef unsigned long long __attribute__((aligned(1))) memscripten_align1_uint64;

typedef float __attribute__((aligned(2))) memscripten_align2_float;
typedef float __attribute__((aligned(1))) memscripten_align1_float;

typedef double __attribute__((aligned(4))) memscripten_align4_double;
typedef double __attribute__((aligned(2))) memscripten_align2_double;
typedef double __attribute__((aligned(1))) memscripten_align1_double;

typedef bool __attribute__((aligned(2))) memscripten_align2_bool;
typedef bool __attribute__((aligned(1))) memscripten_align1_bool;


// for uint8_t and such
#include <stdint.h>
