#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include "dll.h"
#include "dlg.h"
#include "rsrc.h"
#include "cfg.h"

extern CFG_ST *g_cfg;

INT_PTR CALLBACK DlgProc_About( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		lParam = 0;
		break;
	case WM_COMMAND:
		{
			int ctrl_id = LOWORD(wParam);
			switch( ctrl_id )
			{
			case IDOK:
			case IDCANCEL:
				EndDialog( hDlg, ctrl_id );
				break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}

static const TCHAR *szPlayers[PLAYERS_COUNT + 2] = {
	TEXT("NULL"),
	TEXT("Winamp"),
	TEXT("AIMP"),
	TEXT("The KMPlayer"),
	TEXT("Media Player Classic"),
	TEXT("Foobar2000"),
	//
	TEXT("Array overflow!") // for safety!
};

static const TCHAR *szActions[ACT_COUNT] = {
	TEXT("Показывать реальный процесс"),
	TEXT("Показывать заголовок активного окна"),
	TEXT("Показывать текст :")
};


int GetPlayerID( const wchar_t *playerName )
{
	int i;
	for( i=1; i<=PLAYERS_COUNT; i++ )
	{
		if( wcscmp( playerName, szPlayers[i] ) == 0 )
			return i;
	}
	return 0;
}

int IsPlayerAdded( HWND hDlg, int playerID )
{
	HWND hWndLB1 = GetDlgItem( hDlg, IDC_LIST1 );
	int cnt = ListBox_GetCount( hWndLB1 );
	int i;
	for( i=0; i<cnt; i++ )
	{
		TCHAR buffer[1024];
		ListBox_GetText( hWndLB1, i, buffer );
		int added_playerID = GetPlayerID( buffer );
		if( added_playerID == playerID ) return 1;
	}
	return 0;
}

void updateLB1( HWND hDlg )
{
	HWND hWndLB1 = GetDlgItem( hDlg, IDC_LIST1 );
	if( g_cfg )
	{
		int i = 0;
		for( i=0; i<PLAYERS_COUNT; i++ )
		{
			if( g_cfg->order[i] != 0 )
				ListBox_AddString( hWndLB1, szPlayers[g_cfg->order[i]] );
		}
	}
	else
	{
		ListBox_AddString( hWndLB1, TEXT("g_cfg is NULL! Config not loaded") );
	}
}

void updateLB2( HWND hDlg )
{
	HWND hWndLB2 = GetDlgItem( hDlg, IDC_LIST2 );
	int i = 0;
	for( i=1; i<=PLAYERS_COUNT; i++ )
	{
		if( !IsPlayerAdded( hDlg, i ) )
			ListBox_AddString( hWndLB2, szPlayers[i] );
	}
}

void initCBAction( HWND hDlg )
{
	HWND hWndCB = GetDlgItem( hDlg, IDC_CBACTION );
	HWND hWndEdit = GetDlgItem( hDlg, IDC_ETEXT );
	int i;
	for( i=0; i<ACT_COUNT; i++ )
		ComboBox_AddString( hWndCB, szActions[i] );
	ComboBox_SetCurSel( hWndCB, 0 ); // default?
	if( g_cfg )
	{
		ComboBox_SetCurSel( hWndCB, g_cfg->action_player_not_found );
		SetWindowText( hWndEdit, g_cfg->text_PlayerNotFound );
	}
}

void updateCBAction( HWND hDlg )
{
	HWND hWndCB = GetDlgItem( hDlg, IDC_CBACTION );
	HWND hWndEdit = GetDlgItem( hDlg, IDC_ETEXT );
	int cur_sel = ComboBox_GetCurSel( hWndCB );
	if( cur_sel == ACT_SHOW_USER_TEXT )
	{
		EnableWindow( hWndEdit, TRUE );
		SetWindowText( hWndEdit, g_cfg->text_PlayerNotFound );
	}
	else
	{
		EnableWindow( hWndEdit, FALSE );
	}
}


INT_PTR CALLBACK DlgProc_Options( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_INITDIALOG:
		{
			updateLB1( hDlg );
			updateLB2( hDlg );
			initCBAction( hDlg );
			updateCBAction( hDlg );
			lParam = 0;
		} break;
	case WM_COMMAND:
		{
			int ctrl_id = LOWORD(wParam);
			HWND hWndLB1 = GetDlgItem( hDlg, IDC_LIST1 );
			HWND hWndLB2 = GetDlgItem( hDlg, IDC_LIST2 );
			switch( ctrl_id )
			{
			case IDC_ADD:
				{
					int nSel = ListBox_GetCurSel( hWndLB2 );
					if( nSel >= 0 )
					{
						TCHAR buffer[1024];
						ListBox_GetText( hWndLB2, nSel, buffer );
						ListBox_AddString( hWndLB1, buffer );
						ListBox_DeleteString( hWndLB2, nSel );
					}
				} break;
			case IDC_DEL:
				{
					int nSel = ListBox_GetCurSel( hWndLB1 );
					if( nSel >= 0 )
					{
						TCHAR buffer[1024];
						ListBox_GetText( hWndLB1, nSel, buffer );
						ListBox_AddString( hWndLB2, buffer );
						ListBox_DeleteString( hWndLB1, nSel );
					}
				} break;
			case IDC_MOVEUP:
				{
					int nSel = ListBox_GetCurSel( hWndLB1 );
					if( nSel >= 0 )
					{
						if( nSel >= 1 )
						{
							TCHAR buffer[1024];
							ListBox_GetText( hWndLB1, nSel, buffer );
							ListBox_DeleteString( hWndLB1, nSel );
							ListBox_InsertString( hWndLB1, nSel-1, buffer );
							ListBox_SetCurSel( hWndLB1, nSel-1 );
						}
					}
				} break;
			case IDC_MOVEDOWN:
				{
					int nSel = ListBox_GetCurSel( hWndLB1 );
					if( nSel >= 0 )
					{
						if( nSel < PLAYERS_COUNT-1 )
						{
							TCHAR buffer[1024];
							ListBox_GetText( hWndLB1, nSel, buffer );
							ListBox_DeleteString( hWndLB1, nSel );
							ListBox_InsertString( hWndLB1, nSel+1, buffer );
							ListBox_SetCurSel( hWndLB1, nSel+1 );
						}
					}
				} break;
			case IDC_CBACTION:
				{
					updateCBAction( hDlg );
				} break;
			case IDOK:
				{
					int i;
					int cnt = ListBox_GetCount( hWndLB1 );
					// zero orders in config
					if( g_cfg )
						for( i=0; i<PLAYERS_COUNT; i++ )
							g_cfg->order[i] = 0;
					// save players order
					for( i=0; i<cnt; i++ )
					{
						TCHAR buffer[1024];
						ListBox_GetText( hWndLB1, i, buffer );
						if( g_cfg )
							g_cfg->order[i] = GetPlayerID( buffer );
					}
					// save action & text
					int cur_sel = ComboBox_GetCurSel( GetDlgItem( hDlg, IDC_CBACTION ) );
					if( g_cfg ) g_cfg->action_player_not_found = cur_sel;
					if( g_cfg )
					{
						if( g_cfg->action_player_not_found == ACT_SHOW_USER_TEXT )
							GetDlgItemText( hDlg, IDC_ETEXT, g_cfg->text_PlayerNotFound, sizeof(g_cfg->text_PlayerNotFound)/sizeof(g_cfg->text_PlayerNotFound[0]) );
					}
					EndDialog( hDlg, IDOK );
				} break;
			case IDCANCEL:
				EndDialog( hDlg, IDCANCEL );
				break;
			}
		} break;
	default: return FALSE; break;
	}
	return TRUE;
}
