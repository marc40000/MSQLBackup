#pragma once

// to make fseeko available as a 64 bit fseek
#define _FILE_OFFSET_BITS 64
#define fseek64 fseeko

#define SOCKET int
#define closesocket close


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
