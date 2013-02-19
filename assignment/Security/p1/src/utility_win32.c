#include <stdio.h>
#include <string.h>

#include <Windows.h>
#include <tchar.h>

#include "utility.h"

void printMessage( void ) {
	SYSTEM_INFO sysInfo;
	union {
		void * real;
		void ( * fake )( int ( * )( const char *, va_list ), const char *, ... );
	} heap;

	/* we need to get system page size information */
	GetSystemInfo( &sysInfo );
	/* enable execute premission to a page */
	heap.real = VirtualAlloc( NULL, sysInfo.dwPageSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
	/* extract machine code */
	extractCode( heap.real, sysInfo.dwPageSize );
	/* close read/write premission to prevent abuse */
	VirtualProtect( heap.real, sysInfo.dwPageSize, PAGE_EXECUTE, NULL );

	/* call the heap as a function */
	heap.fake( vprintf, "Ethan Hunt, Run!\n" );

	/* free the allocated page */
	VirtualFree( heap.real, sysInfo.dwPageSize, MEM_RELEASE );
}

void deleteSelf( void ) {
	char script[] =
		"@echo off\n"
#ifdef _UNICODE
		"chcp 65001\n"
#endif
		":Repeat\n"
		"del \"%s\"\n"
		"if exist \"%s\" goto Repeat\n"
		"del \"%s\"\n"
	;
	TCHAR filePath[MAX_PATH];
	TCHAR tmpPath[MAX_PATH];
#ifdef _UNICODE
	char u8FilePath[MAX_PATH*4];
	char u8TmpPath[MAX_PATH*4];
#endif
	FILE * batch = NULL;
	errno_t iResult = 0;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	GetModuleFileName( NULL, filePath, MAX_PATH );
	GetTempPath( MAX_PATH, tmpPath );
	_tcscat_s( tmpPath, MAX_PATH, TEXT( "__META_MAGIC__.bat" ) );

#ifdef _UNICODE
	WideCharToMultiByte( CP_UTF8, 0, filePath, MAX_PATH, u8FilePath, MAX_PATH * 4, NULL, NULL );
	WideCharToMultiByte( CP_UTF8, 0, tmpPath, MAX_PATH, u8TmpPath, MAX_PATH * 4, NULL, NULL );
#endif

	iResult = _tfopen_s( &batch, tmpPath, TEXT( "w" ) );
	if( batch == NULL ) {
		return;
	}
#ifdef _UNICODE
	fprintf( batch, script, u8FilePath, u8FilePath, u8TmpPath );
#else
	fprintf( batch, script, filePath, filePath, tmpPath );
#endif
	fclose( batch );

	_tcscpy_s( filePath, MAX_PATH, tmpPath );
	_stprintf_s( tmpPath, MAX_PATH, TEXT( "/c \"%s\"" ), filePath );
	memset( &si, 0, sizeof( si ) );
	si.cb = sizeof( si );
	memset( &pi, 0, sizeof( pi ) );
	CreateProcess( TEXT( "C:\\Windows\\System32\\cmd.exe" ), tmpPath, NULL, NULL, FALSE, BELOW_NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi );
	CloseHandle( si.hStdError );
	CloseHandle( si.hStdInput );
	CloseHandle( si.hStdOutput );
	CloseHandle( pi.hThread );
	CloseHandle( pi.hProcess );
}

void blockByMilliseconds( unsigned long ms ) {
	Sleep( ms );
}
