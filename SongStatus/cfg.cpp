#include "cfg.h"
#include <windows.h>
#include <stdio.h>

#include "dll.h"


CFG_ST *g_cfg = NULL;


CFG_ST *cfg_open()
{
	CFG_ST *ret = NULL;
	// already allocated?
	if( g_cfg ) return g_cfg;
	// alloc
	ret = new CFG_ST();
	if( ret ) g_cfg = ret;
	return ret;
}


void cfg_close()
{
	if( g_cfg )
	{
		CFG_ST *sptr = g_cfg;
		g_cfg = NULL;
		delete sptr;
	}
}


bool cfg_load( const wchar_t *cfgFileDir, CFG_ST *cfg )
{
	wchar_t fn[1024];
	swprintf( fn, 1024, L"%s\\SongStatus.ini", cfgFileDir );
	int prev_enabled = cfg->enabled;
	FILE *f = _wfopen( fn, L"rb" );
	if( !f ) return false;
	// check file size
	size_t nRead = 0;
	fseek( f, 0, SEEK_END );
	long int fs = ftell( f );
#ifdef _DEBUG
	if( g_flog )
		fprintf( g_flog, "config file size [%S] = %d (sizeof old = %d, sizeof new = %d)\n",
			fn, fs, sizeof(CFG_ST_old), sizeof(CFG_ST) );
#endif
	fseek( f, 0, SEEK_SET ); // seek file begin
	if( fs == sizeof(CFG_ST_old) )
	{
		// load old config version
		int i = 0;
		CFG_ST_old cfgo; // old cfg struct
		fread( (void *)&cfgo, 1, sizeof(cfgo), f ); // read old config
		// copy old config fields to new config (convert)
		cfg->version = CFG_VERSION;
		cfg->enabled = 0;
		for( i=0; i<4; i++ ) // old config version used exactly 4 players
			cfg->order[i] = cfgo.order[i]; // copy player order
		cfg->action_player_not_found = (int)ACT_SHOW_USER_TEXT;
		wcscpy( cfg->text_PlayerNotFound, L"Player not found!" );
		//
		MessageBoxW( NULL, L"Converted old config format to new", L"SongStatus: cfg_load():", MB_ICONINFORMATION );
	}
	else // load normal new config
	{
		nRead = fread( (void *)cfg, 1, sizeof(CFG_ST), f );
		fclose( f );
	}
	cfg->enabled = prev_enabled;
	// validate version
	if( cfg->version == CFG_VERSION )
	{
		 // all OK
	}
	else if( cfg->version == 0x0200 )
	{
		cfg->order[4] = 0;
		cfg->action_player_not_found = ACT_SHOW_USER_TEXT;
		wcscpy( cfg->text_PlayerNotFound, L"Plugin updated!" );
	}
	else
	{
		MessageBoxW( NULL,
			L"Неверный формат файла настроек или неподдерживаемая версия, проверьте настройки!",
			L"SongStatus: Config file warning", MB_ICONWARNING );
		// zero config (for safe)
		cfg->enabled = 1;
		cfg->action_player_not_found = ACT_SHOW_REAL_PROCESS;
		int i;
		for( i=0; i<PLAYERS_COUNT; i++ )
			cfg->order[i] = 0;
		cfg->version = CFG_VERSION;
		memset( cfg->text_PlayerNotFound, 0, sizeof(cfg->text_PlayerNotFound) );
	}
	return nRead == sizeof(CFG_ST);
}


bool cfg_save( const wchar_t *cfgFileDir, CFG_ST *cfg )
{
	wchar_t fn[1024];
	swprintf( fn, 1024, L"%s\\SongStatus.ini", cfgFileDir );
	// "disable" plugin in saved config
	int prev_enabled = cfg->enabled;
	cfg->enabled = 0;
	cfg->version = CFG_VERSION;
	//
	FILE *f = _wfopen( fn, L"wb" );
	if( !f ) return false;
	size_t nWritten = fwrite( (void *)cfg, 1, sizeof(CFG_ST), f );
	fclose( f );
	//
	cfg->enabled = prev_enabled;
	//
	return nWritten == sizeof(CFG_ST);
}

