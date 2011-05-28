#pragma once

enum PLAYER_IDS
{
	WINAMP_ID = 1,
	AIMP_ID = 2,
	KMP_ID = 3,
	MPC_ID = 4,
	FOOBAR_ID = 5,
	//
	PLAYERS_COUNT = FOOBAR_ID
};


enum ACTIONS
{
	ACT_SHOW_REAL_PROCESS = 0,
	ACT_SHOW_ACTIVE_WINDOW_TITLE,
	ACT_SHOW_USER_TEXT,
	ACT_COUNT
};


struct CFG_ST_old
{
	int enabled;
	int order[4];
	wchar_t text_PlayerNotFound[256];
	wchar_t text_PluginStopped[256];
};

#define CFG_VERSION 0x0201
struct CFG_ST
{
	int      version;
	int      enabled;
	int      order[PLAYERS_COUNT];
	int      action_player_not_found;
	wchar_t  text_PlayerNotFound[256];
};


CFG_ST  *cfg_open();
void     cfg_close();
bool     cfg_load( const wchar_t *cfgFileDir, CFG_ST *cfg );
bool     cfg_save( const wchar_t *cfgFileDir, CFG_ST *cfg );

