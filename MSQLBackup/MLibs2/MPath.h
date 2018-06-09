#pragma once

#include "stdafx.h"

#ifdef MPLATFORM_WIN32
#include <direct.h>
#endif


// equivalent to the coe in MConfigFile
inline void MPathMakeAbsolute(char * pathin, char * pathout)
{
#ifdef MPLATFORM_WIN32
	if ((pathin[0] == '\\') && (pathin[1] == '\\'))
	{
		// unc path
		strcpy(pathout, pathin);
	}
	else
	if (pathin[1] != ':')
	{
		// relative path
		_getcwd(pathout, 1024);
		int pathoutlen = strlen(pathout);
		pathout[pathoutlen] = '\\';
		pathoutlen++;
		strcpy(pathout + pathoutlen, pathin);
	}
	else
	{
		// absolute path
		strcpy(pathout, pathin);
	}
#else
	if (pathin[0] != '/')
	{
		// relative path
		_getcwd(pathout, 1024);
		int pathoutlen = strlen(pathout);
		pathout[pathoutlen] = '/';
		pathoutlen++;
		strcpy(pathout + pathoutlen, pathin);
	}
	else
	{
		// absolute path
		strcpy(pathout, pathin);
	}
#endif
}
