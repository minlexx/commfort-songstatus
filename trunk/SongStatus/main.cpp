#include <windows.h>
#include <stdio.h>
#include "dll.h"
#include "cfg.h"


// global vars
HINSTANCE   g_hInstDLL   = NULL;
HINSTANCE   g_hInstPSAPI = NULL;
#ifdef HOOK_LOG
FILE       *g_flog       = NULL;
#endif
void       *g_GetProcessImageFileNameW_addr = NULL;


// DllMain
DWORD WINAPI DllMain( HINSTANCE hInstDLL, DWORD dwReasonForCall, LPVOID pvReserved )
{
	UNREFERENCED_PARAMETER(pvReserved);
	switch( dwReasonForCall )
	{
	case DLL_PROCESS_ATTACH:
		{
			g_hInstDLL = hInstDLL;
			DisableThreadLibraryCalls( hInstDLL );
#ifdef HOOK_LOG
			g_flog = fopen( "SongStatus.log", "wt" );
			if( g_flog ) fprintf( g_flog, "SongStatus: DLL loaded!\n" );
#endif
			g_hInstPSAPI = GetModuleHandle( TEXT("psapi.dll") );
			if( !g_hInstPSAPI )
			{
#ifdef HOOK_LOG
				if( g_flog ) fprintf( g_flog, "GetModuleHandle() failed to get address of PSAPI.DLL;  trying LoadLibrary!\n" );
#endif
				g_hInstPSAPI = LoadLibrary( TEXT("psapi.dll") );
			}
			if( !g_hInstPSAPI )
			{
#ifdef HOOK_LOG
				if( g_flog )
				{
					fprintf( g_flog, "Failed to load (get address of) psapi.dll! What the fuck?\n" );
					fflush( g_flog );
					fclose( g_flog );
					g_flog = NULL;
				}
#endif
				return FALSE;
			}
			// psapi loaded
			g_GetProcessImageFileNameW_addr = (void *)GetProcAddress( g_hInstPSAPI, "GetProcessImageFileNameW" );
#ifdef HOOK_LOG
			if( g_flog )
			{
				fprintf( g_flog, "Got address of PSAPI = 0x%08X\n", (unsigned int)g_hInstPSAPI );
				wchar_t wszPSAPI[MAX_PATH] = {0};
				GetModuleFileName( g_hInstPSAPI, wszPSAPI, MAX_PATH );
				fprintf( g_flog, "Psapi path: [%S]\n", wszPSAPI );
				fprintf( g_flog, "GetProcessImageFileNameW() addr = 0x%08X\n", (unsigned int)g_GetProcessImageFileNameW_addr );
			}
#endif
		} break;
	case DLL_PROCESS_DETACH:
		{
#ifdef HOOK_LOG
			if( g_flog )
			{
				fflush( g_flog );
				fclose( g_flog );
				g_flog = NULL;
			}
#endif
		} break;
	}
	return TRUE;
}



//__declspec(naked)
DWORD WINAPI my_GetProcessImageFileNameW(
	__in   HANDLE  hProcess,
	__out  LPWSTR  lpszImageFileName,
	__in   DWORD   dwSize )
{
	/*__asm
	{
		mov     edi, edi
		push    ebp
		mov     ebp, esp
	}*/
#ifdef HOOK_LOG
	if( g_flog ) fprintf( g_flog, "my_GetProcessImageFileNameW() called, dwSize = %u\n", dwSize );
#endif
	UNREFERENCED_PARAMETER(hProcess);
	if( dwSize < 1  ||  !lpszImageFileName ) return ERROR_INSUFFICIENT_BUFFER;
	DWORD strLen = 0;

	CFG_ST *cfg = cfg_open();
	if( !cfg )
	{
		// old way, hardcoded order
#ifdef HOOK_LOG
		fprintf( g_flog, "cfg_open() returned NULL, use hardcoded player order!\n" );
#endif
		if( (strLen = GetWinampSongTitleW( lpszImageFileName, dwSize )) == 0 )
		{
			if( (strLen = GetAimpSongTitleW( lpszImageFileName, dwSize )) == 0 )
			{
				if( (strLen = GetKMPlayerSongTitleW( lpszImageFileName, dwSize )) == 0 )
				{
					if( (strLen = GetMPCSongTitleW( lpszImageFileName, dwSize )) == 0 )
					{
						if( (strLen = GetPotPlayerSongTitleW( lpszImageFileName, dwSize )) == 0 )
						{
							wcsncpy( lpszImageFileName, L"SongStatus: No player found!", dwSize-1 );
							strLen = wcslen( lpszImageFileName );
						}
					}
				}
			}
		}
		return strLen;
	}

	if( !cfg->enabled )
	{
		wcsncpy( lpszImageFileName, L"SongStatus plugin stopped!", dwSize-1 );
		strLen = wcslen( lpszImageFileName );
	}
	else
	{
		int i = 0;
		strLen = 0; // player not found / title not found
		for( i=0; i<4; i++ )
		{
			if( (strLen == 0) && (cfg->order[i] != 0) )
			{
				switch( cfg->order[i] )
				{
				case WINAMP_ID:
					strLen = GetWinampSongTitleW( lpszImageFileName, dwSize-1 );
					break;
				case AIMP_ID:
					strLen = GetAimpSongTitleW( lpszImageFileName, dwSize-1 );
					break;
				case KMP_ID:
					strLen = GetKMPlayerSongTitleW( lpszImageFileName, dwSize-1 );
					break;
				case MPC_ID:
					strLen = GetMPCSongTitleW( lpszImageFileName, dwSize-1 );
					break;
				}
			}
		} // end order loop
		// still player not found?
		if( strLen == 0 )
		{
			wcsncpy( lpszImageFileName, L"No player found!", dwSize-1 );
			strLen = wcslen( lpszImageFileName );
		}
	}

	return strLen;
}


DWORD WINAPI my_GetProcessImageFileNameA(
	__in   HANDLE  hProcess,
	__out  LPSTR   lpszImageFileName,
	__in   DWORD   dwSize )
{
	UNREFERENCED_PARAMETER(hProcess);
	if( dwSize < 1  ||  !lpszImageFileName ) return ERROR_INSUFFICIENT_BUFFER;
	memset( lpszImageFileName, 0, (size_t)dwSize );
	strncpy( lpszImageFileName, "GetProcessImageFileNameA", (size_t)dwSize );
	return 24;
}

