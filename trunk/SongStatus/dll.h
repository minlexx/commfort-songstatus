#pragma once

#ifdef _DEBUG
#define HOOK_LOG
#endif

//////////////////////
struct JMP_ADDR
{
	unsigned char jmp;
	unsigned int  addr;
};


// function type declaration
typedef DWORD (WINAPI * t_GetProcessImageFileNameW)( HANDLE, LPWSTR, DWORD );
typedef DWORD (WINAPI * t_GetProcessImageFileNameA)( HANDLE, LPWSTR, DWORD );

// externs
extern HINSTANCE   g_hInstDLL;
extern HINSTANCE   g_hInstPSAPI;
#ifdef _DEBUG
extern FILE       *g_flog;
#endif
extern void       *g_GetProcessImageFileNameW_addr;

// my
DWORD WINAPI my_GetProcessImageFileNameW( __in HANDLE hProcess, __out LPWSTR lpszImageFileName, __in DWORD dwSize );
DWORD WINAPI my_GetProcessImageFileNameA( __in HANDLE hProcess, __out LPWSTR lpszImageFileName, __in DWORD dwSize );

// songs titles
DWORD WINAPI GetWinampSongTitleW( __out LPWSTR lpszSongTitle, __in DWORD nSize );
DWORD WINAPI GetAimpSongTitleW( __out LPWSTR lpszSongTitle,	__in DWORD nSize );
DWORD WINAPI GetKMPlayerSongTitleW( __out LPWSTR lpszSongTitle, __in DWORD nSize );
DWORD WINAPI GetMPCSongTitleW( __out LPWSTR lpszSongTitle, __in DWORD nSize );
DWORD WINAPI GetPotPlayerSongTitleW( __out LPWSTR lpszSongTitle, __in DWORD nSize );

// hooks
bool SetHook();
bool RemoveHook();
