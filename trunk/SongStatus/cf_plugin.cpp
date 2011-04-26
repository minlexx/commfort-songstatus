#include <windows.h>
#include <string>
#include "dll.h"
#include "cfg.h"
#include "rsrc.h"
#include "dlg.h"
#include "cf_plugin.h"
#include "hooks.h"


/////////////////////////////////////////
fnCommFortProcess CommFortProcess = NULL;
fnCommFortGetData CommFortGetData = NULL;
DWORD    g_dwPluginID = 0;
wchar_t  g_cfg_file_dir[1024] = {0};


///////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------//
int fReadInteger( BYTE *bInBuffer, int *iOffset ) //��������������� ������� ��� ��������� ������ � ������� ������
{
	int iLength = 0;
	memcpy( &iLength, bInBuffer + (*iOffset), 4 );
	(*iOffset) += 4;
	return iLength;
}

//---------------------------------------------------------------------------
std::wstring fReadText( BYTE *bInBuffer, int *iOffset ) //��������������� ������� ��� ��������� ������ � ������� ������
{
	int iLength = 0;
	memcpy( &iLength, bInBuffer + (*iOffset), 4 );
	(*iOffset) += 4;
	std::wstring uRet;
	uRet.reserve( iLength + 16 );
	uRet.resize( iLength, 0 );
	memcpy( (void *)uRet.c_str(), bInBuffer + (*iOffset), iLength*2 );
	(*iOffset) += iLength*2;
	return uRet;
}

//---------------------------------------------------------------------------
void fWriteInteger( BYTE *bOutBuffer, int *iOffset, int iValue ) //��������������� ������� ��� ��������� ������ � ������� ������
{
	memcpy( bOutBuffer + (*iOffset), &iValue, 4 );
	(*iOffset) += 4;
}
//---------------------------------------------------------------------------
void fWriteText( BYTE *bOutBuffer, int *iOffset, std::wstring& uValue ) //��������������� ������� ��� ��������� ������ � ������� ������
{
	int iLength = uValue.size(); // uValue.Length();
	// write text length
	memcpy( bOutBuffer + (*iOffset), &iLength, 4 );
	(*iOffset) += 4;
	// write text
	memcpy( bOutBuffer + (*iOffset), uValue.c_str(), iLength*2 );
	(*iOffset) += iLength*2;
}

void fSendToEvents( int styleId, std::wstring& text )
{
	// styleId: 0-black, 1-gray, 2-red
	std::wstring s;
	s.reserve( 4+4+4+text.size()*2 + 16 );
	int iOffset = 0;
	fWriteInteger( (BYTE *)s.c_str(), &iOffset, styleId );
	fWriteInteger( (BYTE *)s.c_str(), &iOffset, 0 ); // log time
	fWriteInteger( (BYTE *)s.c_str(), &iOffset, 0 ); // log to log
	fWriteText( (BYTE *)s.c_str(), &iOffset, text );
	if( CommFortProcess )
		(*CommFortProcess)( g_dwPluginID, 100, (BYTE *)s.c_str(), iOffset );
}

void fSendToChannel( std::wstring& channelName, int messageType, std::wstring& text )
{
	// messageType: 0-normal, 1-state
	std::wstring s;
	s.reserve( channelName.size()*2 + 4 + text.size()*2 );
	int iOffset = 0;
	fWriteText( (BYTE *)s.c_str(),    &iOffset, channelName );
	fWriteInteger( (BYTE *)s.c_str(), &iOffset, messageType );
	fWriteText( (BYTE *)s.c_str(),    &iOffset, text );
	if( CommFortProcess )
		(*CommFortProcess)( g_dwPluginID, 50, (BYTE *)s.c_str(), iOffset );
}

bool fGetPluginsTempPath( wchar_t *outResult, size_t resultSizeWChars )
{
	// ������������� ���� ��� ��������� ������ ��������
	// ID: 2010
	// ���� ������ (���������): [������� ��������]
	// ���� ������ (��������): �����(����)
	if( (CommFortProcess == NULL) || (CommFortGetData == NULL) || (g_dwPluginID == 0) )
	{
		outResult[0] = 0;
		return false;
	}
	//
	std::string buffer;
	int iOffset = 0;
	int iSize = CommFortGetData( g_dwPluginID, 2010, NULL, 0, NULL, 0 );
	buffer.reserve( iSize + 16 );
	iSize = CommFortGetData( g_dwPluginID, 2010, (BYTE *)buffer.c_str(), iSize, NULL, 0 );
	std::wstring pluginPath = fReadText( (BYTE *)buffer.c_str(), &iOffset );
	wcsncpy( outResult, pluginPath.c_str(), resultSizeWChars - 1 );
	outResult[resultSizeWChars - 1] = 0;
	return true;
}


///////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------//
BOOL __stdcall PluginStart( DWORD dwThisPluginID, fnCommFortProcess func1, fnCommFortGetData func2 )
{
	g_dwPluginID = dwThisPluginID;
	//��� ������������� ������� ������������� ���������� �������������
	//��� ���������� ����������� ���������, � ���������
	//� �������� ������� ��������� ��� ������������� �������

	CommFortProcess = func1;
    //��������� ������� ��������� ������,
	//� ������� ������� ������ ������ ������������ �������

	CommFortGetData = func2;
    //��������� ������� ��������� ������,
	//� ������� ������� ����� ����� ����������� ����������� ������ �� ���������


	// ������ ��������� ������ �� ��������� (��������� ���� ���������):
	/*std::string aData; // AnsiString aData;
	int iSize = (*CommFortGetData)( g_dwPluginID, 2000, NULL, NULL, NULL, NULL ); //�������� ����� ������
	aData.reserve( iSize + 16 ); // aData.SetLength(iSize);
	(*CommFortGetData)( g_dwPluginID, 2000, (BYTE *)aData.c_str(), iSize, NULL, NULL );//��������� �����

	int iProgramType;
	memcpy( &iProgramType, aData.c_str(), 4 );//�������� �����

	if( iProgramType == 0 ) fSendToEvents( 1, std::wstring( L"NULL ������ ������� ������� �� ������� CommFort" ) );
	else if( iProgramType == 1 ) fSendToEvents( 1, std::wstring( L"NULL ������ ������� ������� �� ������� CommFort" ) );*/
	//fPostToChannel( std::wstring(L"�����"), 0, std::wstring(L"� ���!!!") );

	CFG_ST *cfg = cfg_open();
	if( !cfg )
	{
		MessageBox( NULL, TEXT("cfg_open() failed!"), TEXT("ERROR"), MB_ICONWARNING );
		return FALSE;
	}

	// get directory to store plugins temp files
	fGetPluginsTempPath( g_cfg_file_dir, sizeof(g_cfg_file_dir)/2 );

	if( !cfg_load( g_cfg_file_dir, cfg ) )	
	{
		std::wstring msg( L"���� �������� �� ������, ����� ������ �����:\n" );
		msg.append( g_cfg_file_dir );
		msg.append( L"SongStatus.ini" );
		MessageBox( NULL, msg.c_str(), TEXT("SongStatus: �������� �������"), MB_ICONWARNING );
	}
	cfg->enabled = 1;

	if( !Hook_SetHook() )
	{
		MessageBox( NULL, TEXT("Hook_SetHook() failed!"), TEXT("SongStatus ERROR:"), MB_ICONSTOP );
		return FALSE;
	}


	//������������ ��������:
	//TRUE - ������ ������ �������
	//FALSE - ������ ����������
	return TRUE;
}
//---------------------------------------------------------------------------
void __stdcall PluginStop()
{
	//������ ������� ���������� ��� ���������� ������ �������
	//fSendToEvents( 1, std::wstring( L"Test stop" ) ); // is ignored =(
	CFG_ST *cfg = cfg_open();
	if( cfg )
	{
		cfg_save( g_cfg_file_dir, cfg );
		cfg->enabled = 0;
	}
	if( !Hook_RemoveHook() )
		MessageBox( NULL, TEXT("�� ������� ����� ���! :/"), TEXT("SongStatus: Stop ERROR"), MB_ICONWARNING );
}

//---------------------------------------------------------------------------
void __stdcall PluginProcess( DWORD dwID, BYTE *bInBuffer, DWORD dwInBufferSize )
{
	UNREFERENCED_PARAMETER(dwID);
	UNREFERENCED_PARAMETER(bInBuffer);
	UNREFERENCED_PARAMETER(dwInBufferSize);
}

//---------------------------------------------------------------------------
DWORD __stdcall PluginGetData( DWORD dwID, BYTE *bInBuffer, DWORD dwInBufferSize, BYTE *bOutBuffer, DWORD dwOutBufferSize )
{
    // ������� �������� ������ ���������

	UNREFERENCED_PARAMETER(dwInBufferSize);
	UNREFERENCED_PARAMETER(bInBuffer);

	//int iReadOffset = 0; //��������������� ���������� ��� ��������� ������ � ������ ������
	int iWriteOffset = 0;

	// ��� �������� dwOutBufferSize ������ ���� ������� ������ ������� ����� ������, ������ �� ���������

	if( dwID == 2800 ) // �������������� �������
	{
		if(dwOutBufferSize == 0)
			return 4; //����� ������ � ������, ������� ���������� �������� ���������
		//fWriteInteger( bOutBuffer, &iWriteOffset, 0 );  // ������ �������� ��� ��� �������, ��� � ��� �������
		fWriteInteger( bOutBuffer, &iWriteOffset, 2 );  // ������ �������� ��� �������
		return 4; //����� ������������ ������ � ������
	}
	else if( dwID == 2810 ) // �������� ������� (������������ � ������)
	{
		//UnicodeString uName = L"NULL: ����������� ������";//�������� �������
		std::wstring uName( L"Song Status" );
		int iSize = uName.size() * 2 + 4; // uName.Length()*2+4;
		if( dwOutBufferSize == 0 )
			return iSize; // ����� ������ � ������, ������� ���������� �������� ���������
		fWriteText( bOutBuffer, &iWriteOffset, uName );
		return iSize; // ����� ������������ ������ � ������
	}

	return 0; // ������������ �������� - ����� ���������� ������
}

//---------------------------------------------------------------------------
/*BOOL __stdcall PluginPremoderation( DWORD dwID, wchar_t *wText, DWORD *dwTextLength )
{
	// ������� ������������
    // ���� ��� ������ �� ���������� ������������,
	// ����������� ��������� ������� �� ��������� ����, ��� ��������� �������������� �������

	// �����! ����� ���������� �� 40000 ��������. ������ ������� � ���� ������ �������� ������.

	if( dwID == 1010 ) // ����������
	{
		//UnicodeString uCheck = L"�������� ������������ � ����������";
		std::wstring uCheck( L"�������� ������������ � ����������" );

		if( wcscmp( wText, uCheck.c_str() ) == 0 )
		{
			//UnicodeString uRet = L"�������� ������������ � ����������: �������";
			std::wstring uRet( L"�������� ������������ � ����������: �������" );
			memcpy( wText, uRet.c_str(), uRet.size() * 2 ); // uRet.Length()*2r

			(*dwTextLength) = uRet.size(); // uRet.Length(); // ������������ ���������� ��������

			return TRUE; // TRUE �������� ��� ����� ��� �������������
		}
	}

	return FALSE; // ����� ������� FALSE � ������ ���� ����� �� ��� �������������
}*/

//---------------------------------------------------------------------------
void __stdcall PluginShowOptions()
{
	// ������ ������� ���������� ��� ������� ������ "���������" � ������ ��������
	// ���� �� �� ������� ����� �������������� ������ "���������", ������ ������� ������ �������
	//
	// load config if not loaded now
	CFG_ST *cfg = cfg_open();
	if( !cfg )
	{
		MessageBox( NULL, TEXT("cfg_open() failed!"), TEXT("SongStatus ERROR"), MB_ICONSTOP );
		return;
	}
	// get directory to store plugins temp files
	if( !fGetPluginsTempPath( g_cfg_file_dir, sizeof(g_cfg_file_dir)/2 ) )
	{
		MessageBox( NULL, TEXT("��������� ����������, �� ��� ���, ���� ������ �� ���� �� ��� �������!\n(������� �����/����, ���� �� �����)"), TEXT("SongStatus ERROR"), MB_ICONWARNING );
		return;
	}
	cfg_load( g_cfg_file_dir, cfg );
	// run dialog
	INT_PTR ret = DialogBoxParamW( g_hInstDLL, MAKEINTRESOURCE(IDD_OPTIONS), NULL, DlgProc_Options, 0 );
	if( ret == IDOK )
	{
		if( cfg )
			cfg_save( g_cfg_file_dir, cfg );
	}
}
//---------------------------------------------------------------------------
void __stdcall PluginShowAbout()
{
	// ������ ������� ���������� ��� ������� ������ "� �������" � ������ ��������
	// ���� �� �� ������� ����� �������������� ������ "���������", ������ ������� ������ �������
	DialogBoxParamW( g_hInstDLL, MAKEINTRESOURCE(IDD_ABOUT), NULL, DlgProc_About, 0 );
}
