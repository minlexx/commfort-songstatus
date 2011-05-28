#include <windows.h>
#include <stdio.h>
#include "dll.h"


DWORD WINAPI GetWinampSongTitleW(
	__out  LPWSTR  lpszSongTitle,
	__in   DWORD   nSize
)
{
	DWORD ret = 0;
	if( !lpszSongTitle || nSize<1 ) return 0;
	memset( lpszSongTitle, 0, nSize*2 );
	HWND hWndWinamp = FindWindowW( L"Winamp v1.x", NULL ); // standard skin
	if( hWndWinamp == NULL ) hWndWinamp = FindWindowW( L"BaseWindow_RootWnd", NULL ); // modern skin
	// still NULL? no winamp
	if( hWndWinamp == NULL ) return 0;
	// get winamp window text
	GetWindowTextW( hWndWinamp, lpszSongTitle, nSize );
	ret = wcslen( lpszSongTitle );
	return ret;
}


DWORD WINAPI GetAimpSongTitleW(
	__out LPWSTR lpszSongTitle,
	__in  DWORD  nSize
)
{
	DWORD ret = 0;
	if( !lpszSongTitle || nSize<1 ) return 0;
	memset( lpszSongTitle, 0, nSize*2 );
	HWND hWndAimp = FindWindowW( L"TAIMPMainFrame", NULL );
	if( hWndAimp == NULL ) hWndAimp = FindWindowW( L"TAIMP3MainWindow", NULL );
	if( hWndAimp == NULL ) return 0;
	GetWindowTextW( hWndAimp, lpszSongTitle, nSize );
	ret = wcslen( lpszSongTitle );
	return ret;
}


DWORD WINAPI GetKMPlayerSongTitleW(
	__out LPWSTR lpszSongTitle,
	__in  DWORD  nSize
)
{
	DWORD ret = 0;
	if( !lpszSongTitle || nSize<1 ) return 0;
	memset( lpszSongTitle, 0, nSize*2 );
	HWND hwnd = FindWindowW( L"TSkinPanel", NULL );
	if( !hwnd ) return 0;
	GetWindowTextW( hwnd, lpszSongTitle, nSize );
	ret = wcslen( lpszSongTitle );
	return ret;
}


DWORD WINAPI GetMPCSongTitleW(
	__out LPWSTR lpszSongTitle,
	__in  DWORD  nSize
)
{
	DWORD ret = 0;
	if( !lpszSongTitle || nSize<1 ) return 0;
	memset( lpszSongTitle, 0, nSize*2 );
	HWND hwnd = FindWindowW( L"MediaPlayerClassicW", NULL );
	if( !hwnd ) return 0;
	GetWindowTextW( hwnd, lpszSongTitle, nSize );
	ret = wcslen( lpszSongTitle );
	return ret;
}


DWORD WINAPI GetPotPlayerSongTitleW(
	__out LPWSTR lpszSongTitle,
	__in  DWORD  nSize
)
{
	DWORD ret = 0;
	if( !lpszSongTitle || nSize<1 ) return 0;
	memset( lpszSongTitle, 0, nSize*2 );
	HWND hwnd = FindWindowW( L"AfxWnd90su", NULL );
	if( !hwnd ) return 0;
	GetWindowTextW( hwnd, lpszSongTitle, nSize );
	ret = wcslen( lpszSongTitle );
	return ret;
}

DWORD WINAPI GetFoobarSongTitleW(
	__out  LPWSTR  lpszSongTitle,
	__in   DWORD   nSize
)
{
	DWORD ret = 0;
	if( !lpszSongTitle || nSize<1 ) return 0;
	memset( lpszSongTitle, 0, nSize*2 );
	HWND hWnd = FindWindowW( L"{97E27FAA-C0B3-4b8e-A693-ED7881E99FC1}", NULL ); // standard skin
	if( hWnd == NULL ) hWnd = FindWindowW( L"{E7076D1C-A7BF-4f39-B771-BCBE88F2A2A8}", NULL ); // ColumnsUI skin
	// still NULL? no foobar
	if( hWnd == NULL ) return 0;
	// get winamp window text
	GetWindowTextW( hWnd, lpszSongTitle, nSize );
	ret = wcslen( lpszSongTitle );
	return ret;
}
