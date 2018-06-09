#pragma once

class MLogPlugin
{
public:
	
	MLogPlugin()
	{
	}
	virtual ~MLogPlugin()
	{
	}

#if defined(MPLATFORM_WIN32_MSVC) && (!defined(TCHARISCHAR))
	virtual int Log(const time_t timet, const TCHAR * filename, const unsigned int line, const TCHAR * functionname, const unsigned int msgtype, const bool forcedisplay, const TCHAR * msg) = 0;
#endif
	virtual int Log(const time_t timet, const char * filename, const unsigned int line, const char * functionname, const unsigned int msgtype, const bool forcedisplay, const char * msg) = 0;
};

typedef MLogPlugin * LPMLogPlugin;
