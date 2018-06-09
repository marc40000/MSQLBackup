#pragma once

#if (defined(MPLATFORM_EMSCRIPTEN))
#define MLogEnabled
#define MLogShortenFilenames
#else
#define MLogEnabled
#define MLogThreadSave
#define MLogShortenFilenames
#endif

#ifdef MPLATFORM_WIN32_MSVC
#if (!defined(_DEBUG))
#pragma managed(push, off)
#endif
#endif

const unsigned int MLogType_Unspecified = 0;
const unsigned int MLogType_MNetSocketUDPBasic		= 1;
const unsigned int MLogType_MNetDev					= 2;
const unsigned int MLogType_MNetServerSync			= 3;
const unsigned int MLogType_MNetClientSync			= 4;
const unsigned int MLogType_MSerialize				= 5;
const unsigned int MLogType_MAssert					= 10;
const unsigned int MLogType_MResManager				= 100;
const unsigned int MLogType_MVis					= 200;
const unsigned int MLogType_MRPG					= 201;
const unsigned int MLogType_Debug					= 999;
const unsigned int MLogType_App						= 1000;
const unsigned int MLogType_User					= 1010;

// shortcut
const unsigned int MLDebug							= MLogType_Debug;
const unsigned int MLApp							= MLogType_App;
const unsigned int MLUser							= MLogType_User;


#ifdef MLogEnabled

#include <time.h>
#ifdef MPLATFORM_WIN32
#include <wchar.h>
#include <tchar.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef MLogThreadSave
//#include "MSemaphore.h"
#include "MCriticalSection.h"
#endif
#include "MLogPlugin.h"
#if defined(MPLATFORM_LINUX) || defined(MPLATFORM_EMSCRIPTEN) || defined(MPLATFORM_ANDROID)
#include <string.h>
#include <unistd.h>
#endif

#ifdef MPLATFORM_ANDROID
#include <android/log.h>
#endif


extern bool mlogoutputstdout;
extern bool mlogsavetofile;
extern bool mlogrotate;
extern bool mlogtailaware;
const unsigned int mlogfilenamen = 256;
extern char mlogfilenamea[mlogfilenamen * 2];
extern char mlogpathnamea[mlogfilenamen * 2];
extern int mlogpathnamean;
#ifdef MPLATFORM_WIN32_MSVC
extern TCHAR * mlogfilename;
extern TCHAR * mlogpathname;
#endif
extern FILE * mlogfile;
extern time_t mlogtimelast;
const unsigned int mlogmsgn = 1024 * 1024;
extern char mlogmsga[mlogmsgn * 2];
#ifdef MPLATFORM_WIN32_MSVC
extern TCHAR * mlogmsg;
#endif

extern int mlogcurrentdayofmonth;				// for log rotation
extern int mlogrotatefilesn;

#ifdef MLogThreadSave
//extern MSemaphore * mlogsemaphore;
extern MCriticalSection * mlogsemaphore;
#endif

const unsigned int MLogPluginsNMax = 16;
extern unsigned int mlogpluginsn;
extern LPMLogPlugin mlogplugins[MLogPluginsNMax]; 

inline void MLogPluginAdd(MLogPlugin * plugin)
{
	mlogplugins[mlogpluginsn] = plugin;
	++mlogpluginsn;
}



#if (!((defined(MPLATFORM_EMSCRIPTEN) || defined(MPLATFORM_ANDROID))))
extern void MLogDeleteOldFilesA();

inline void MLogCreateFileNameA(tm * ltm)
{
	sprintf(mlogfilenamea + mlogpathnamean, "%04u%02u%02u.txt", 1900 + ltm->tm_year, ltm->tm_mon + 1, ltm->tm_mday);
}

inline void MLogSwitchFileA(tm * ltm)
{
	if (mlogsavetofile && mlogrotate)
	{
		if (ltm->tm_mday == mlogcurrentdayofmonth)
		{
			// nothing to do, use the same log file we are already using
		}
		else
		{
			mlogcurrentdayofmonth = ltm->tm_mday;

			if (!mlogtailaware)
			{
				fclose(mlogfile);
			}

			MLogCreateFileNameA(ltm);

			if (!mlogtailaware)
			{
				mlogfile = fopen(mlogfilenamea, "at");
			}

			MLogDeleteOldFilesA();
		}
	}
}
#endif

// the same as MStringReplace from MStringUtil.h but I didn't want to add that reference as a requirement to MLog
inline int MLogStringReplace(char * s, char replace, char replacement)
{
	unsigned int i = 0;
	while (s[i] != 0)
	{
		if (s[i] == replace)
		{
			s[i] = replacement;
		}
		++i;
	}
	return 0;
}


/*
// allows for printf style usage
#ifdef MPLATFORM_WIN32_MSVC
inline void MLogDo(const char * filename, const unsigned int line, const char * functionname, const unsigned int msgtype, const bool forcedisplay, const TCHAR * format, ...)
{
	// from HGE: hgehelp: hgeFont::printf() hgefont.cpp(181)
	char *parg = (char *) &format + sizeof(format);

	//buffer[sizeof(buffer) - 1]=0;
	//vsprintf(buffer, format, pArg);
	

#ifdef MLogShortenFilenames
	TCHAR tfilenamet[1024];
	mbstowcs(tfilenamet, filename, 1024);

	// shorten the filename if it is too long
	TCHAR * tfilename;
	int tfilenametlen = wcslen(tfilenamet);
	if (tfilenametlen > 32)
	{
		tfilename = tfilenamet + tfilenametlen - 32;
		//tfilename[0] = _T('.');
		//tfilename[1] = _T('.');
	}
#else
	TCHAR tfilename[256];
	mbstowcs(tfilename, filename, 256);
#endif

	TCHAR tfunctionname[256];
	mbstowcs(tfunctionname, functionname, 256);


#ifdef MLogThreadSave
	mlogsemaphore->P();
#endif

	time_t timet;
	timet = time(0);
	tm ltm;
	localtime_s(&ltm, &timet);

	_stprintf_s(mlogmsg, mlogmsgn, _T("%04u-%02u-%02u %02u:%02u:%02u.0000000\t%s(%u): %s\ttype=%u\t"), 
		1900 + ltm.tm_year, ltm.tm_mon + 1, ltm.tm_mday, ltm.tm_hour, ltm.tm_min, ltm.tm_sec,
		tfilename, line, tfunctionname,
		msgtype);

	unsigned int n = _tcslen(mlogmsg);

	_vsntprintf_s(mlogmsg + n, mlogmsgn - n, mlogmsgn - n, format, parg);
	TCHAR * msg = mlogmsg + n;
	
	n += _tcslen(mlogmsg + n);
	mlogmsg[n] = _T('\n');
	mlogmsg[n + 1] = 0;


	if (mlogfilename[0] != 0)
	{
		FILE * mlogfilelocal;

		if (mlogtailaware)
		{
			_tfopen_s(&mlogfilelocal, mlogfilename, _T("at"));
		}
		else
		{
			mlogfilelocal = mlogfile;
		}

		if (mlogfilelocal != 0)
		{
			_ftprintf(mlogfilelocal, _T("%s"), mlogmsg);

			if (mlogtailaware)
			{
				fclose(mlogfilelocal);
			}
		}
	}

	if (mlogoutputstdout && (forcedisplay || (timet - mlogtimelast >= 5)))
	{
		_tprintf(_T("%s"), mlogmsg);
		mlogtimelast = timet;
	}

	mlogmsg[n] = 0;		// remove the trailing \n for the plugins
	unsigned int i;
	for (i = 0; i < mlogpluginsn; ++i)
	{
		mlogplugins[i]->Log(timet, tfilename, line, tfunctionname, msgtype, forcedisplay, msg);
	}

#ifdef MLogThreadSave
	mlogsemaphore->V();
#endif
}
#endif
*/

/*inline void MLogDo(const char * filename, const unsigned int line, const char * functionname, const unsigned int msgtype, const bool forcedisplay, const char * format, ...)
{
	static char ta[mlogmsgn];
	_vsnprintf_s(ta, mlogmsgn, mlogmsgn, format, args);

	static TCHAR t[mlogmsgn];
	mbstowcs(t, ta, mlogmsgn);

	MLogDo(filename, line, functionname, msgtype, forcedisplay, t);
}*/

/*
inline void MLogDo(const char * filename, const unsigned int line, const char * functionname, const unsigned int msgtype, const bool forcedisplay, const char * format, ...)
{
	// from HGE: hgehelp: hgeFont::printf() hgefont.cpp(181)
	char *parg = (char *) &format + sizeof(format);

	static char ta[mlogmsgn];
	_vsnprintf_s(ta, mlogmsgn, mlogmsgn, format, parg);

	static TCHAR t[mlogmsgn];
	mbstowcs(t, ta, mlogmsgn);

	MLogDo(filename, line, functionname, msgtype, forcedisplay, t);
}
*/

inline void MLogUniversalDo(const bool usetchar, const char * filenamet, const unsigned int line, const char * functionname, const unsigned int msgtype, const bool forcedisplay, const char * format, va_list & parg)
{
	// from HGE: hgehelp: hgeFont::printf() hgefont.cpp(181)
	//char * parg = (char *)&format + sizeof(format);
	//va_list parg;
	//va_start(parg, format);


	//buffer[sizeof(buffer) - 1]=0;
	//vsprintf(buffer, format, pArg);


#ifdef MLogShortenFilenames
	// shorten the filename if it is too long
	/*	// I have no idea why this version results in garbage and a crash when compiled in MSVC in release
		const char * filename;
		int filenametlen = strlen(filenamet);
		printf("%i\n", filenametlen);
		if (filenametlen > 32)
		{
		filename = filenamet + filenametlen - 32;
		//filename[0] = _T('.');
		//filename[1] = _T('.');
		}*/
	char filenamett[1024];
	strcpy(filenamett, filenamet);
	char * filename = filenamett;
	int filenametlen = strlen(filenamet);
	if (filenametlen > 32)
	{
		filename = filename + filenametlen - 32;
		//filename[0] = _T('.');
		//filename[1] = _T('.');
	}
#else
	const char * filename = filenamet;
#endif


#ifdef MLogThreadSave
	mlogsemaphore->P();
#endif

	time_t timet = time(0);
	tm * ltm = localtime(&timet);

#if (!((defined(MPLATFORM_EMSCRIPTEN) || defined(MPLATFORM_ANDROID))))
	MLogSwitchFileA(ltm);
#endif

#if defined(MPLATFORM_ANDROID)
	// Android doesn't show \t in LogCat
	snprintf(mlogmsga, mlogmsgn, "%04u-%02u-%02u %02u:%02u:%02u.0000000    %s(%u): %s    type=%u    ",
		1900 + ltm->tm_year, ltm->tm_mon + 1, ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec,
		filename, line, functionname,
		msgtype);
#else
	snprintf(mlogmsga, mlogmsgn, "%04u-%02u-%02u %02u:%02u:%02u.0000000\t%s(%u): %s\ttype=%u\t",
		1900 + ltm->tm_year, ltm->tm_mon + 1, ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec,
		filename, line, functionname,
		msgtype);
#endif


	unsigned int n = strlen(mlogmsga);
#ifdef MPLATFORM_WIN32
	if (usetchar)
	{
		static TCHAR msgt[mlogmsgn];
		_vsntprintf_s(msgt, mlogmsgn, mlogmsgn, (const TCHAR*)format, parg);
#if defined TCHARISCHAR
		strcpy_s(mlogmsga + n, mlogmsgn - n, msgt);
#else
		wcstombs(mlogmsga + n, msgt, mlogmsgn - n);
#endif
	}
	else
#endif
	{
		vsnprintf(mlogmsga + n, mlogmsgn - n, format, parg);
	}


	n += strlen(mlogmsga + n);
	mlogmsga[n] = '\n';
	mlogmsga[n + 1] = 0;


#if (!((defined(MPLATFORM_EMSCRIPTEN) || defined(MPLATFORM_ANDROID))))
	if (mlogsavetofile)
	{
		FILE * mlogfilelocal;

		if (mlogtailaware)
		{
			mlogfilelocal = fopen(mlogfilenamea, "at");
		}
		else
		{
			mlogfilelocal = mlogfile;
		}

		if (mlogfilelocal != 0)
		{
			fprintf(mlogfilelocal, "%s", mlogmsga);

			if (mlogtailaware)
			{
				fclose(mlogfilelocal);
			}
		}
	}
#endif

	if (mlogoutputstdout && (forcedisplay || (timet - mlogtimelast >= 5)))
	{
#if defined(MPLATFORM_ANDROID)
		char tag[32];
		sprintf(tag, "MLog:%u", msgtype);
		__android_log_print(ANDROID_LOG_DEBUG, tag, "%s", mlogmsga);
#else
		printf("%s", mlogmsga);
#endif
		mlogtimelast = timet;
	}

	mlogmsga[n] = 0;		// remove the trailing \n for the plugins

	// no plugin support for the ascii variant right now
	/*unsigned int i;
	for (i = 0; i < mlogpluginsn; ++i)
	{
		mlogplugins[i]->Log(timet, filename, line, functionname, msgtype, forcedisplay, msg);
	}*/

	va_end(parg);

#ifdef MLogThreadSave
	mlogsemaphore->V();
#endif
}

#if defined(MPLATFORM_WIN32) && (!(defined(TCHARISCHAR)))
inline void MLogDo(const char * filenamet, const unsigned int line, const char * functionname, const unsigned int msgtype, const bool forcedisplay, const TCHAR * format, ...)
{
	va_list parg;
	va_start(parg, format);
	MLogUniversalDo(true, filenamet, line, functionname, msgtype, forcedisplay, (const char *)format, parg);
}
#endif

inline void MLogDo(const char * filenamet, const unsigned int line, const char * functionname, const unsigned int msgtype, const bool forcedisplay, const char * format, ...)
{
	va_list parg;
	va_start(parg, format);
	MLogUniversalDo(false, filenamet, line, functionname, msgtype, forcedisplay, format, parg);
}


/*
#ifdef MPLATFORM_WIN32_MSVC
inline void MLogInit(const bool stdoutput = false, const bool tailaware = false, const TCHAR * logfilename = _T("mlog/"), const bool append = false, const int filesn = 365)
{
	// let the tchar and char version of the buffer use the same memory
	mlogmsg = (TCHAR*)mlogmsg;
	mlogfilename = (TCHAR*)mlogfilenamea;
	mlogpathname = (TCHAR*)mlogpathnamea;


	mlogfile = 0;
	mlogoutputstdout = stdoutput;
	mlogtailaware = tailaware;
	_tcscpy_s(mlogfilename, mlogfilenamen, logfilename);
	MLogStringReplace(mlogfilename, '/', '\\');
	mlogrotatefilesn = filesn;

	// if the filename has a trailing / or \, filename is a path. New logs files are created in that path every day and old ones get deleted
	mlogpathnamean = _tcslen(mlogfilename);
	if (mlogpathnamean == 0)
	{
		// no saving to file
		mlogsavetofile = false;
		mlogrotate = false;
	}
	else
	{
		mlogsavetofile = true;
		TCHAR lastc = mlogfilename[mlogpathnamean - 1];
		if (lastc == '\\')
		{
			mlogrotate = true;
			_tcscpy(mlogpathname, mlogfilename);

			MLogDeleteOldFiles();

			time_t timet = time(0);
			tm * ltm = localtime(&timet);
			MLogCreateFileName(ltm);
			mlogcurrentdayofmonth = ltm->tm_mday;

			if (!append)
			{
				// empty directory ?
			}
		}
		else
		{
			mlogrotate = false;
			// mlogfilenamea is already correct; mlogpathnamea is not used
			if (!append)
			{
				_tunlink(mlogfilename);
			}
		}
	}
	
	if (!mlogsavetofile)
	{
		mlogfile = 0;
	}
	else
	if (!mlogtailaware)
	{
		_tfopen_s(&mlogfile, mlogfilename, _T("at"));
	}

#ifdef MLogThreadSave
	//mlogsemaphore = new MSemaphore(1, 1);
	mlogsemaphore = new MCriticalSection();
#endif

	//MLogS(MLApp, _T("MLogInit"));
	MLogDo(__FILE__, __LINE__, __FUNCTION__, MLApp, true, _T("MLogInit"));
}
#endif
*/

inline void MLogInit(const bool stdoutput = false, const bool tailaware = false, const char * logfilename = "mlog/", const bool append = false, const int filesn = 365)
{
#ifdef MPLATFORM_WIN32_MSVC
	// let the tchar and char version of the buffer use the same memory
	mlogmsg = (TCHAR*)mlogmsg;
	mlogfilename = (TCHAR*)mlogfilenamea;
#endif

	mlogfile = 0;
	mlogoutputstdout = stdoutput;
	mlogtailaware = tailaware;
#if defined(MPLATFORM_EMSCRIPTEN) || defined(MPLATFORM_ANDROID)
	// force no logfile in emscripten and android
	mlogfilenamea[0] = 0;
#else
	strcpy(mlogfilenamea, logfilename);
#if defined(MPLATFORM_WIN32)
	MLogStringReplace(mlogfilenamea, '/', '\\');
#else
	MLogStringReplace(mlogfilenamea, '\\', '/');
#endif
#endif
	mlogrotatefilesn = filesn;

	// if the filename has a trailing / or \, filename is a path. New logs files are created in that path every day and old ones get deleted
	mlogpathnamean = strlen(mlogfilenamea);
	if (mlogpathnamean == 0)
	{
		// no saving to file
		mlogsavetofile = false;
		mlogrotate = false;
	}
	else
	{
#if (!((defined(MPLATFORM_EMSCRIPTEN) || defined(MPLATFORM_ANDROID))))
		mlogsavetofile = true;
		char lastc = mlogfilenamea[mlogpathnamean - 1];
#if defined(MPLATFORM_WIN32)
		if (lastc == '\\')
#else
		if (lastc == '/')
#endif
		{
			mlogrotate = true;
			strcpy(mlogpathnamea, mlogfilenamea);

			MLogDeleteOldFilesA();

			time_t timet = time(0);
			tm * ltm = localtime(&timet);
			MLogCreateFileNameA(ltm);
			mlogcurrentdayofmonth = ltm->tm_mday;

			if (!append)
			{
				// empty directory ?
			}
		}
		else
		{
			mlogrotate = false;
			// mlogfilenamea is already correct; mlogpathnamea is not used
			if (!append)
			{
				_unlink(mlogfilenamea);
			}
		}
#endif
	}

	if (!mlogsavetofile)
	{
		mlogfile = 0;
	}
	else
	if (!mlogtailaware)
	{
		mlogfile = fopen(mlogfilenamea, "at");
	}

#ifdef MLogThreadSave
	//mlogsemaphore = new MSemaphore(1, 1);
	mlogsemaphore = new MCriticalSection();
#endif

	//MLogS(MLApp, _T("MLogInit"));
	MLogDo(__FILE__, __LINE__, __FUNCTION__, MLApp, true, "MLogInit");
}

inline void MLogDestroy()
{
	//MLogS(MLApp, _T("MLogDestroy"));
	MLogDo(__FILE__, __LINE__, __FUNCTION__, MLApp, true, "MLogDestroy");

#ifdef MLogThreadSave
	delete mlogsemaphore;
#endif

#if (!((defined(MPLATFORM_EMSCRIPTEN) || defined(MPLATFORM_ANDROID))))
	if (!mlogtailaware)
	{
		if (mlogfile != 0)
		{
			fclose(mlogfile);
		}
	}
#endif

	mlogpluginsn = 0;
}

#if defined(MPLATFORM_WIN32) && (!(defined(TCHARISCHAR)))
inline void MLogInit(const bool stdoutput = false, const bool tailaware = false, const TCHAR * logfilename = _T("mlog/"), const bool append = false, const int filesn = 365)
{
	char mlogfilenamea[mlogfilenamen];
	wcstombs(mlogfilenamea, logfilename, mlogfilenamen);
	MLogInit(stdoutput, tailaware, mlogfilenamea, append, filesn);
}
#endif


inline void MLogFlush()
{
	if ((mlogfile != 0) && (!mlogtailaware))
	{
		fflush(mlogfile);
	}
}


// New Interface
// according to http://msdn.microsoft.com/en-us/library/ms177415(VS.80).aspx this is how you use variable number of parameters in macros
//#define MLog(msgtype, forcedisplay, format, ...) MLogDo(__FILE__, __LINE__, __FUNCTION__, msgtype, forcedisplay, format, ##__VA_ARGS__)
//#define MLogS(msgtype, format, ...) MLogDo(__FILE__, __LINE__, __FUNCTION__, msgtype, true, format, ##__VA_ARGS__)
// Note: The previous lines work for MSVC and are easier to understand for the user. However, gcc doesn't like having 0 variadic parameters, so I shifted it by one, removing the explicit "format" parameter.
#define MLog(msgtype, forcedisplay, ...) MLogDo(__FILE__, __LINE__, __FUNCTION__, msgtype, forcedisplay, ##__VA_ARGS__)
#define MLogS(msgtype, ...) MLogDo(__FILE__, __LINE__, __FUNCTION__, msgtype, true, ##__VA_ARGS__)

#if (!defined(MPLATFORM_EMSCRIPTEN)) && defined(_DEBUG)
#include <intrin.h>
#define MLogAssert(v, msgtype, forcedisplay, format, ...) { if (!(v)) { MLogDo(__FILE__, __LINE__, __FUNCTION__, msgtype, forcedisplay, format, ##__VA_ARGS__); __debugbreak(); } }
#define MLogSAssert(v, msgtype, format, ...) { if (!(v)) { MLogDo(__FILE__, __LINE__, __FUNCTION__, msgtype, true, format, ##__VA_ARGS__); __debugbreak(); } }
#else
#define MLogAssert(v, msgtype, forcedisplay, format, ...) { if (!(v)) MLogDo(__FILE__, __LINE__, __FUNCTION__, msgtype, forcedisplay, format, ##__VA_ARGS__); }
// ## because of https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html : it removes the , in front of __VA_ARGS__ in case of __VA_ARGS__ being emtpy. MSVC seems to do this automatically
#define MLogSAssert(v, msgtype, format, ...) { if (!(v)) MLogDo(__FILE__, __LINE__, __FUNCTION__, msgtype, true, format, ##__VA_ARGS__); }
#endif

#else

#ifdef MPLATFORM_WIN32_MSVC
inline void MLogInit(const bool stdoutput = false, const bool tailaware = false, const TCHAR * logfilename = _T("mlog/"), const bool append = false, const int filesn = 365)
{
}
#endif

inline void MLogInit(const bool stdoutput = false, const bool tailaware = false, const char * logfilename = "mlog/", const bool append = false, const int filesn = 365)
{
}

inline void MLogDestroy()
{
}

inline void MLogFlush()
{
}

#define MLog(msgtype, forcedisplay, ...)
#define MLogS(msgtype, ...)

#define MLogAssert(v, msgtype, forcedisplay, format, ...)
#define MLogSAssert(v, msgtype, format, ...)

#endif

#ifdef MPLATFORM_WIN32_MSVC
#if (!defined(_DEBUG))
#pragma managed(pop)
#endif
#endif

