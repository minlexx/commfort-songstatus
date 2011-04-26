#pragma once

#define WINAMP_ID    1
#define AIMP_ID      2
#define KMP_ID       3
#define MPC_ID       4

struct CFG_ST
{
	int enabled;
	int order[4];
	wchar_t text_PlayerNotFound[256];
	wchar_t text_PluginStopped[256];
};

CFG_ST   *cfg_open();
void      cfg_close();
bool      cfg_load( const wchar_t *cfgFileDir, CFG_ST *cfg );
bool      cfg_save( const wchar_t *cfgFileDir, CFG_ST *cfg );

