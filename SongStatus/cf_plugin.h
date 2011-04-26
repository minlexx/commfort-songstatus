#pragma once

extern DWORD    g_dwPluginID;// = 0;
extern wchar_t  g_cfg_file_dir[1024];// = {0};


/////////////////////////////////////////
typedef void  (__stdcall *fnCommFortProcess)( DWORD dwPluginID, DWORD dwID, BYTE *bOutBuffer, DWORD dwOutBufferSize );
typedef DWORD (__stdcall *fnCommFortGetData)( DWORD dwPluginID, DWORD dwID, BYTE *bInBuffer,  DWORD dwInBufferSize, BYTE *bOutBuffer, DWORD dwOutBufferSize );


/////////////////////////////////////////
BOOL __stdcall PluginStart( DWORD dwThisPluginID, fnCommFortProcess, fnCommFortGetData );
void __stdcall PluginStop();

void  __stdcall PluginProcess( DWORD dwID, BYTE *bInBuffer, DWORD dwInBufferSize );
DWORD __stdcall PluginGetData( DWORD dwID, BYTE *bInBuffer, DWORD dwInBufferSize, BYTE *bOutBuffer, DWORD dwOutBufferSize );
//BOOL  __stdcall PluginPremoderation( DWORD dwID, wchar_t *wText, DWORD *dwTextLength );

void __stdcall PluginShowOptions();
void __stdcall PluginShowAbout();

