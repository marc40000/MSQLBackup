//#########################################################################################################################
//#
//# This header file tries to automatically detect the plattform and set the M-preprocessorvaribales equivalently.
//# That way, I can change the detection or set it manually if something changes
//#
//# You can easily include this file in stdafx.h, since it gets included by everything anyways
//#
//# You can do things like 
//#
//# #if defined(LINUX) || defined(ANDROID)
//#
//# to test for multiple plattforms at once.
//#
//# There is a website with definitions of various compilers here http://sourceforge.net/p/predef/wiki/Home/
//# Also this might bei interesting http://blogs.msdn.com/b/vcblog/archive/2015/12/04/introducing-clang-with-microsoft-codegen-in-vs-2015-update-1.aspx
//#
//#########################################################################################################################

#pragma once

#define MPLATFORM

#if defined(WIN32) || defined(_WIN64)

#define MPLATFORM_WIN32

#ifdef __MINGW32__
#define MPLATFORM_WIN32_MINGW32
#else
#define MPLATFORM_WIN32_MSVC
#endif

#endif


//#ifdef __GNUC__
#ifdef __linux
#define MPLATFORM_LINUX
// can I differentiate somehow between LINUX and QNAP?
// Is not needed anymore. I got a new-enough boost version compiled and installed for qnap cross-compile so this is obsolete and could be removed from the code.
//#define MPLATFORM_LINUX_QNAP
#endif

#ifdef EMSCRIPTEN
#define MPLATFORM_EMSCRIPTEN
#endif

// here I don't know if I can automatically detect the plattform. ANDROID might be wrong here
#ifdef __ANDROID__
// just in case it cot defined earlier:
#undef MPLATFORM_LINUX
#define MPLATFORM_ANDROID
#endif


// this one is an addon-plattform for win32
#if defined(DOTNET2) || defined(MCLR)
#define MPLATFORM_CLR
#endif



#ifdef MPLATFORM_WIN32
#include "MPlatformWin32.h"
#endif
#ifdef MPLATFORM_LINUX
#include "MPlatformLinux.h"
#endif
#ifdef MPLATFORM_EMSCRIPTEN
#include "MPlatformEmscripten.h"
#endif
#ifdef MPLATFORM_ANDROID
#include "MPlatformAndroid.h"
#endif


#ifdef MPLATFORM_DETECTION_DEBUG
#ifdef MPLATFORM_WIN32
#pragma message ("----------------------------------------")
#pragma message ("Platforms detected:")
#ifdef MPLATFORM_WIN32
#pragma message ("WIN32")
#endif
#ifdef MPLATFORM_LINUX
#pragma message ("LINUX")
#endif
#ifdef MPLATFORM_EMSCRIPTEN
#pragma message ("EMSCRIPTEN")
#endif
#ifdef MPLATFORM_ANDROID
#pragma message ("ANDROID")
#endif
#pragma message ("----------------------------------------")
#endif
#endif


#include <stdint.h>
