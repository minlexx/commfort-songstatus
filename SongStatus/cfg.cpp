#include "cfg.h"
#include <windows.h>
#include <stdio.h>


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
	size_t nRead = fread( (void *)cfg, 1, sizeof(CFG_ST), f );
	fclose( f );
	cfg->enabled = prev_enabled;
	return nRead == sizeof(CFG_ST);
}


bool cfg_save( const wchar_t *cfgFileDir, CFG_ST *cfg )
{
	wchar_t fn[1024];
	swprintf( fn, 1024, L"%s\\SongStatus.ini", cfgFileDir );
	//int prev_enabled = cfg->enabled;
	FILE *f = _wfopen( fn, L"wb" );
	if( !f ) return false;
	size_t nWritten = fwrite( (void *)cfg, 1, sizeof(CFG_ST), f );
	fclose( f );
	//cfg->enabled = prev_enabled;
	return nWritten == sizeof(CFG_ST);
}

