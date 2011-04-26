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
int fReadInteger( BYTE *bInBuffer, int *iOffset ) //вспомогательная функция для упрощения работы с чтением данных
{
	int iLength = 0;
	memcpy( &iLength, bInBuffer + (*iOffset), 4 );
	(*iOffset) += 4;
	return iLength;
}

//---------------------------------------------------------------------------
std::wstring fReadText( BYTE *bInBuffer, int *iOffset ) //вспомогательная функция для упрощения работы с чтением данных
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
void fWriteInteger( BYTE *bOutBuffer, int *iOffset, int iValue ) //вспомогательная функция для упрощения работы с записью данных
{
	memcpy( bOutBuffer + (*iOffset), &iValue, 4 );
	(*iOffset) += 4;
}
//---------------------------------------------------------------------------
void fWriteText( BYTE *bOutBuffer, int *iOffset, std::wstring& uValue ) //вспомогательная функция для упрощения работы с записью данных
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
	// Рекомендуемый путь для временных файлов плагинов
	// ID: 2010
	// Блок данных (исходящий): [нулевое значение]
	// Блок данных (входящий): текст(путь)
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
	//При инициализации планину присваивается уникальный идентификатор
	//его необходимо обязательно сохранить, и указывать
	//в качестве первого параметра при инициировании событий

	CommFortProcess = func1;
    //указываем функцию обратного вызова,
	//с помощью которой плагин сможет инициировать события

	CommFortGetData = func2;
    //указываем функцию обратного вызова,
	//с помощью которой можно будет запрашивать необходимые данные от программы


	// Пример получения данных от программы (получение типа программы):
	/*std::string aData; // AnsiString aData;
	int iSize = (*CommFortGetData)( g_dwPluginID, 2000, NULL, NULL, NULL, NULL ); //получаем объем буфера
	aData.reserve( iSize + 16 ); // aData.SetLength(iSize);
	(*CommFortGetData)( g_dwPluginID, 2000, (BYTE *)aData.c_str(), iSize, NULL, NULL );//заполняем буфер

	int iProgramType;
	memcpy( &iProgramType, aData.c_str(), 4 );//копируем число

	if( iProgramType == 0 ) fSendToEvents( 1, std::wstring( L"NULL Плагин успешно запущен на сервере CommFort" ) );
	else if( iProgramType == 1 ) fSendToEvents( 1, std::wstring( L"NULL Плагин успешно запущен на клиенте CommFort" ) );*/
	//fPostToChannel( std::wstring(L"Общий"), 0, std::wstring(L"Я тут!!!") );

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
		std::wstring msg( L"Файл настроек не найден, будет создан новый:\n" );
		msg.append( g_cfg_file_dir );
		msg.append( L"SongStatus.ini" );
		MessageBox( NULL, msg.c_str(), TEXT("SongStatus: Загрузка конфига"), MB_ICONWARNING );
	}
	cfg->enabled = 1;

	if( !Hook_SetHook() )
	{
		MessageBox( NULL, TEXT("Hook_SetHook() failed!"), TEXT("SongStatus ERROR:"), MB_ICONSTOP );
		return FALSE;
	}


	//Возвращаемые значения:
	//TRUE - запуск прошел успешно
	//FALSE - запуск невозможен
	return TRUE;
}
//---------------------------------------------------------------------------
void __stdcall PluginStop()
{
	//данная функция вызывается при завершении работы плагина
	//fSendToEvents( 1, std::wstring( L"Test stop" ) ); // is ignored =(
	CFG_ST *cfg = cfg_open();
	if( cfg )
	{
		cfg_save( g_cfg_file_dir, cfg );
		cfg->enabled = 0;
	}
	if( !Hook_RemoveHook() )
		MessageBox( NULL, TEXT("Не удалось снять хук! :/"), TEXT("SongStatus: Stop ERROR"), MB_ICONWARNING );
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
    // функция передачи данных программе

	UNREFERENCED_PARAMETER(dwInBufferSize);
	UNREFERENCED_PARAMETER(bInBuffer);

	//int iReadOffset = 0; //вспомогательные переменные для упрощения работы с блоком данных
	int iWriteOffset = 0;

	// при значении dwOutBufferSize равным нулю функция должна вернуть объем данных, ничего не записывая

	if( dwID == 2800 ) // предназначение плагина
	{
		if(dwOutBufferSize == 0)
			return 4; //объем памяти в байтах, которую необходимо выделить программе
		//fWriteInteger( bOutBuffer, &iWriteOffset, 0 );  // плагин подходит как для клиента, так и для сервера
		fWriteInteger( bOutBuffer, &iWriteOffset, 2 );  // плагин подходит для клиента
		return 4; //объем заполненного буфера в байтах
	}
	else if( dwID == 2810 ) // название плагина (отображается в списке)
	{
		//UnicodeString uName = L"NULL: проверочный плагин";//название плагина
		std::wstring uName( L"Song Status" );
		int iSize = uName.size() * 2 + 4; // uName.Length()*2+4;
		if( dwOutBufferSize == 0 )
			return iSize; // объем памяти в байтах, которую необходимо выделить программе
		fWriteText( bOutBuffer, &iWriteOffset, uName );
		return iSize; // объем заполненного буфера в байтах
	}

	return 0; // возвращаемое значение - объем записанных данных
}

//---------------------------------------------------------------------------
/*BOOL __stdcall PluginPremoderation( DWORD dwID, wchar_t *wText, DWORD *dwTextLength )
{
	// функция пермодерации
    // если Ваш плагин не использует премодерацию,
	// рекомендуем исключить функцию из исходного кода, это сэкономит вычислительные ресурсы

	// Важно! Буфер выделяется на 40000 символов. Нельзя вносить в него данные бОльшего объема.

	if( dwID == 1010 ) // объявление
	{
		//UnicodeString uCheck = L"проверка премодерации в объявлении";
		std::wstring uCheck( L"проверка премодерации в объявлении" );

		if( wcscmp( wText, uCheck.c_str() ) == 0 )
		{
			//UnicodeString uRet = L"проверка премодерации в объявлении: успешно";
			std::wstring uRet( L"проверка премодерации в объявлении: успешно" );
			memcpy( wText, uRet.c_str(), uRet.size() * 2 ); // uRet.Length()*2r

			(*dwTextLength) = uRet.size(); // uRet.Length(); // корректируем количество символов

			return TRUE; // TRUE означает что буфер был модифицирован
		}
	}

	return FALSE; // важно вернуть FALSE в случае если буфер не был модифицирован
}*/

//---------------------------------------------------------------------------
void __stdcall PluginShowOptions()
{
	// данная функция вызывается при нажатии кнопки "Настроить" в списке плагинов
	// если Вы не желаете чтобы активировалась кнопка "Настроить", просто сотрите данную функцию
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
		MessageBox( NULL, TEXT("Настройка невозможна, до тех пор, пока плагин ни разу не был запущен!\n(Нажмите Старт/Стоп, хотя бы разок)"), TEXT("SongStatus ERROR"), MB_ICONWARNING );
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
	// данная функция вызывается при нажатии кнопки "О плагине" в списке плагинов
	// если Вы не желаете чтобы активировалась кнопка "Настроить", просто сотрите данную функцию
	DialogBoxParamW( g_hInstDLL, MAKEINTRESOURCE(IDD_ABOUT), NULL, DlgProc_About, 0 );
}
