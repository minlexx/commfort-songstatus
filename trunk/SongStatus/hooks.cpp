#include <windows.h>
#include <stdio.h>
#include "dll.h"


unsigned char g_prev_bytes[5] = {0,0,0,0,0};


bool Hook_SetHook()
{
	if( !g_GetProcessImageFileNameW_addr )
	{
#ifdef HOOK_LOG
		if( g_flog ) fprintf( g_flog, "SetHook(): g_GetProcessImageFileNameW_addr == NULL, failing\n" );
#endif
		return false;
	}
	DWORD oldProtect = 0;
	BOOL bRet = VirtualProtectEx( INVALID_HANDLE_VALUE, g_GetProcessImageFileNameW_addr,
		sizeof(JMP_ADDR), PAGE_EXECUTE_READWRITE, &oldProtect );
	if( !bRet )
	{
#ifdef HOOK_LOG
		if( g_flog ) fprintf( g_flog, "SetHook(): Failed to VirtualProtectEx()!\n" );
#endif
		return false;
	}
	JMP_ADDR jmp;
	jmp.jmp  = 0xE9;
	jmp.addr = (unsigned int)my_GetProcessImageFileNameW - (unsigned int)g_GetProcessImageFileNameW_addr - 5;
	unsigned char *src = NULL;
	unsigned char *dst = NULL;
	// save old
	src = (unsigned char *)&g_GetProcessImageFileNameW_addr;
	dst = g_prev_bytes;
	g_prev_bytes[0] = 0; // flag that prev bytes were not saved :/
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
	dst[3] = src[3];
	dst[4] = src[4];
	// set new
	src = (unsigned char *)&jmp;
	dst = (unsigned char *)g_GetProcessImageFileNameW_addr;
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
	dst[3] = src[3];
	dst[4] = src[4];
#ifdef HOOK_LOG
	fprintf( g_flog, "SetHook(): OK\nPrev bytes: " );
	int i;
	for( i=0; i<5; i++ )
	{
		fprintf( g_flog, "%02X ", (unsigned)( g_prev_bytes[i] & 0x000000FF ) );
	}
	fprintf( g_flog, "\n" );
	fflush( g_flog );
#endif
	return true;
}


bool Hook_RemoveHook()
{
	if( !g_GetProcessImageFileNameW_addr )
	{
#ifdef HOOK_LOG
		if( g_flog ) fprintf( g_flog, "Hook_RemoveHook(): g_GetProcessImageFileNameW_addr == NULL, failing\n" );
#endif
		return false;
	}
	DWORD oldProtect = 0;
	BOOL bRet = VirtualProtectEx( INVALID_HANDLE_VALUE, g_GetProcessImageFileNameW_addr,
		sizeof(JMP_ADDR), PAGE_EXECUTE_READWRITE, &oldProtect );
	if( !bRet )
	{
#ifdef HOOK_LOG
		if( g_flog ) fprintf( g_flog, "Hook_RemoveHook(): Failed to VirtualProtectEx()!\n" );
#endif
		return false;
	}
	// check if old was saved
	if( g_prev_bytes[0] == 0 )
	{
#ifdef HOOK_LOG
		if( g_flog ) fprintf( g_flog, "Hook_RemoveHook(): old prolog was not saved = hook is not installed, return true\n" );
#endif
		return true;
	}
	unsigned char *src = NULL;
	unsigned char *dst = NULL;
	// set olg
	src = (unsigned char *)g_prev_bytes;
	dst = (unsigned char *)g_GetProcessImageFileNameW_addr;
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
	dst[3] = src[3];
	dst[4] = src[4];
#ifdef HOOK_LOG
	fprintf( g_flog, "Hook_RemoveHook(): OK\n" );
#endif
	return true;
}

__declspec(naked)
DWORD WINAPI orig_GetProcessImageFileNameW(
	__in   HANDLE  hProcess,
	__out  LPWSTR  lpszImageFileName,
	__in   DWORD   dwSize )
{
	UNREFERENCED_PARAMETER( hProcess );
	UNREFERENCED_PARAMETER( lpszImageFileName );
	UNREFERENCED_PARAMETER( dwSize );
	__asm mov     edi, edi
	__asm push    ebp
	__asm mov     ebp, esp
	__asm mov     eax, g_GetProcessImageFileNameW_addr
	__asm add     eax, 5
	__asm jmp     eax
}
