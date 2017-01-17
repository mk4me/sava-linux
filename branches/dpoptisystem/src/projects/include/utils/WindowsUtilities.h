#ifndef WindowsUtilities_h__
#define WindowsUtilities_h__

#ifdef WIN32

#include <Windows.h>

#define TA_FAILED 0
#define TA_SUCCESS_CLEAN 1
#define TA_SUCCESS_KILL 2
#define TA_SUCCESS_16 3

DWORD WINAPI TerminateApp(DWORD dwPID, DWORD dwTimeout);
BOOL CALLBACK TerminateAppEnum(HWND hwnd, LPARAM lParam);

DWORD WINAPI TerminateApp(DWORD dwPID, DWORD dwTimeout)
{
	HANDLE   hProc;
	DWORD   dwRet;
	
	hProc = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, dwPID);
	if (hProc == nullptr)
		return TA_FAILED;

	EnumWindows((WNDENUMPROC)TerminateAppEnum, (LPARAM)dwPID);
	if (WaitForSingleObject(hProc, dwTimeout) != WAIT_OBJECT_0)
		dwRet = (TerminateProcess(hProc, 0) ? TA_SUCCESS_KILL : TA_FAILED);
	else
		dwRet = TA_SUCCESS_CLEAN;

	CloseHandle(hProc);

	return dwRet;
}

BOOL CALLBACK TerminateAppEnum(HWND hwnd, LPARAM lParam)
{
	DWORD dwID;
	GetWindowThreadProcessId(hwnd, &dwID);
	if (dwID == (DWORD)lParam)
		PostMessage(hwnd, WM_CLOSE, 0, 0);

	return TRUE;
}

#endif	// WIN32
#endif // WindowsUtilities_h__
