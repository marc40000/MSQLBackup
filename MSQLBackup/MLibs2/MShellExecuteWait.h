#pragma once

#include "stdafx.h"

inline void MShellExecuteWait(const HWND hwnd, const char * operation, const char * file, const char * params, const char * dir = NULL, const int showcmd = SW_SHOW)
{
	// according to https://stackoverflow.com/questions/17638674/how-to-wait-for-shellexecute-to-run
	SHELLEXECUTEINFOA ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = hwnd;
	ShExecInfo.lpVerb = operation;
	ShExecInfo.lpFile = file;
	ShExecInfo.lpParameters = params;
	ShExecInfo.lpDirectory = dir;
	ShExecInfo.nShow = showcmd;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteExA(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	CloseHandle(ShExecInfo.hProcess);
}
