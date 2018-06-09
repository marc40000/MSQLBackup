#pragma once

// to make fseeko available as a 64 bit fseek
#define _FILE_OFFSET_BITS 64
#define fseek64 fseeko


#define SOCKET int
#define closesocket close
#define INVALID_SOCKET -1


#define TCHAR char
#define _T
typedef void * LPVOID;

#define WINAPI


#include <unistd.h>
inline void Sleep(int ms)
{
	usleep(ms * 1000);
}


typedef uint32_t DWORD;

#ifdef USEJNIONLOAD
// from here
// https://groups.google.com/forum/#!topic/android-ndk/cKZRjSMUemw
// http://stackoverflow.com/questions/7268450/calling-java-class-member-from-native-c-c-code
#include <android/log.h>
#include <jni.h>
//#include <android/asset_manager.h>
extern JavaVM * androidjavavm;
extern JNIEnv * androidjnienv;
#ifdef __cplusplus
extern "C" {
#endif
	JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* aReserved);
#ifdef __cplusplus
}
#endif
#endif


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
