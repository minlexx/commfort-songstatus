#pragma once

bool Hook_SetHook();
bool Hook_RemoveHook();

DWORD WINAPI orig_GetProcessImageFileNameW(
	__in   HANDLE  hProcess,
	__out  LPWSTR  lpszImageFileName,
	__in   DWORD   dwSize );
