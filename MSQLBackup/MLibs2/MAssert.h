#pragma once

// __WFILE__ definition from
// ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.VisualStudio.v80.en/dv_vclang/html/1cc5f70a-a225-469c-aed0-fe766238e23f.htm
#include <stdio.h>
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#define __WTIMESTAMP__ WIDEN(__TIMESTAMP__)
#define __WFUNCDNAME__ WIDEN(__FUNCDNAME__)
#define __WFUNCSIG__ WIDEN(__FUNCSIG__)
#define __WFUNCTION__ WIDEN(__FUNCTION__)


//#define MAssertThroughMLog


#ifdef _DEBUG

#ifdef MPLATFORM_WIN32
#include <intrin.h>
#endif

#define MAssert(v) { if (!(v)) __debugbreak(); }
#define MVerify(v) { bool mverifytemp = (v); if (!mverifytemp) __debugbreak(); }


#else

//#define MAssertBlank

#ifdef MAssertBlank

//#ifdef MB_OK
//#define MAssert(v) { if (!(v)) { TCHAR s[1024]; swprintf(s, 1023, _T("Assertion occured in %s on line %u.\n\nFunction:\n%s\n%s\n%s\n\nBinary Date: %s"), __WFILE__, __LINE__, __WFUNCDNAME__, __WFUNCSIG__, __WFUNCTION__, __WTIMESTAMP__); MessageBox(0, s, _T("MAssert"), MB_OK); exit(12345678); } }
//#else
// *** assert to cmdline or somewhere else
#define MAssert(v)
#define MVerify(v) (v)
//#endif

#else

inline void MAssertHelper(const char * filename, const unsigned int line, const char * functionname)
{
	printf("MAssert: %s(%u): %s\n", filename, line, functionname);
}

#ifdef MAssertThroughMLog

#include "MLog.h"

#define MAssert(v) MLogSAssert((v), MLogType_MAssert, "MAssert")
// the following line does not yet go through MLog, but I don't use MVerify anyways.
#define MVerify(v) { bool mverifytemp = (v); if (!mverifytemp) MAssertHelper(__FILE__, __LINE__, __FUNCTION__); }
#else
#define MAssert(v) { if (!(v)) MAssertHelper(__FILE__, __LINE__, __FUNCTION__); }
#define MVerify(v) { bool mverifytemp = (v); if (!mverifytemp) MAssertHelper(__FILE__, __LINE__, __FUNCTION__); }
#endif

#endif

#endif
