#include <string.h>

#include "utility.h"

static void decryptCode( unsigned char * code, unsigned long length ) {
	unsigned long i = 0;
	for( ; i < length; ++i ) {
		code[i] ^= '\xAA';
	}
}

unsigned long extractCode( unsigned char * code, unsigned long length ) {
	/* NOTE: encrypted by XOR to 10101010b */
	static const unsigned char CODE[] = {
	/*  void inline_code( int ( * a )( const char *, va_list ), const char * b, ... ) { */
		'\xE2', '\x23', '\xFE', '\x8E', '\xBA', /* mov         qword ptr [rsp+10h],rdx  */
		'\xE6', '\x23', '\xEE', '\x8E', '\xB2', /* mov         qword ptr [rsp+18h],r8   */
		'\xE6', '\x23', '\xE6', '\x8E', '\x8A', /* mov         qword ptr [rsp+20h],r9   */
		'\xE2', '\x29', '\x46', '\x82',         /* sub         rsp,28h                  */
		'\xE6', '\x21', '\x68',                 /* mov         r8,rdx                   */
		'\xE2', '\x21', '\x6B',                 /* mov         rax,rcx                  */
	/*      va_list args;                                                               */
	/*      va_start( args, b );                                                        */
		'\xE2', '\x27', '\xFE', '\x8E', '\xEA', /* lea         rdx,[rsp+40h]            */
	/*      a( b, args );                                                               */
		'\xE3', '\x21', '\x62',                 /* mov         rcx,r8                   */
		'\x55', '\x7A',                         /* call        rax                      */
	/*      va_end( args );                                                             */
	/*  }                                                                               */
		'\xE2', '\x29', '\x6E', '\x82',         /* add         rsp,28h                  */
		'\x69',                                 /* ret                                  */
	};
	static const unsigned long LENGTH = sizeof( CODE );

	if( length < LENGTH ) {
		return 0;
	}

	/* NOP in x86 is 0x90 */
	memset( code, '\x90', length );
	/* the inline asm is in stack, which is not executable, so we need to copy it to the executable page */
	memcpy( code, CODE, LENGTH );
	/* decrypt machine code */
	decryptCode( code, LENGTH );

	return LENGTH;
}
