#ifndef LEGNALEURC_OS_H
#define LEGNALEURC_OS_H

#include <tchar.h>

void cd( const TCHAR * );

void hide( const TCHAR * path );

void show( const TCHAR * path );

int findFile( TCHAR * result, size_t len, const TCHAR * target, const TCHAR * from, const TCHAR * exclude );

intptr_t execute( const TCHAR * path, const TCHAR * const * args );

void getPaths( TCHAR * pwd, size_t pwd_size, TCHAR * target, size_t tgt_size, TCHAR * config, size_t conf_size, const TCHAR * arg0 );

#endif
