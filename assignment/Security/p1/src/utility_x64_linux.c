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
		'\xE2', '\x2B', '\x46', '\x72', '\xAA', '\xAA', '\xAA',         /* sub    $0xd8,%rsp       */
		'\x2E', '\x6A',                                                 /* test   %al,%al          */
		'\xE3', '\x23', '\x50',                                         /* mov    %rdi,%r10        */
		'\xE2', '\x23', '\xFE', '\x8E', '\x9A',                         /* mov    %rdx,0x30(%rsp)  */
		'\xE2', '\x23', '\xE6', '\x8E', '\x92',                         /* mov    %rcx,0x38(%rsp)  */
		'\xE6', '\x23', '\xEE', '\x8E', '\xEA',                         /* mov    %r8,0x40(%rsp)   */
		'\xE6', '\x23', '\xE6', '\x8E', '\xE2',                         /* mov    %r9,0x48(%rsp)   */
		'\xDE', '\x9D',                                                 /* je     400645 <p+0x59>  */
		'\xA5', '\x83', '\xEE', '\x8E', '\xFA',                         /* movaps %xmm0,0x50(%rsp) */
		'\xA5', '\x83', '\xE6', '\x8E', '\xCA',                         /* movaps %xmm1,0x60(%rsp) */
		'\xA5', '\x83', '\xFE', '\x8E', '\xDA',                         /* movaps %xmm2,0x70(%rsp) */
		'\xA5', '\x83', '\x36', '\x8E', '\x2A', '\xAA', '\xAA', '\xAA', /* movaps %xmm3,0x80(%rsp) */
		'\xA5', '\x83', '\x0E', '\x8E', '\x3A', '\xAA', '\xAA', '\xAA', /* movaps %xmm4,0x90(%rsp) */
		'\xA5', '\x83', '\x06', '\x8E', '\x0A', '\xAA', '\xAA', '\xAA', /* movaps %xmm5,0xa0(%rsp) */
		'\xA5', '\x83', '\x1E', '\x8E', '\x1A', '\xAA', '\xAA', '\xAA', /* movaps %xmm6,0xb0(%rsp) */
		'\xA5', '\x83', '\x16', '\x8E', '\x6A', '\xAA', '\xAA', '\xAA', /* movaps %xmm7,0xc0(%rsp) */
		'\xE2', '\x27', '\x2E', '\x8E', '\x4A', '\xAA', '\xAA', '\xAA', /* lea    0xe0(%rsp),%rax  */
		'\xE2', '\x23', '\x5D',                                         /* mov    %rsi,%rdi        */
		'\x6D', '\xEE', '\x8E', '\xA2', '\xBA', '\xAA', '\xAA', '\xAA', /* movl   $0x10,0x8(%rsp)  */
		'\x6D', '\xEE', '\x8E', '\xA6', '\x9A', '\xAA', '\xAA', '\xAA', /* movl   $0x30,0xc(%rsp)  */
		'\xE2', '\x27', '\xDE', '\x8E', '\xA2',                         /* lea    0x8(%rsp),%rsi   */
		'\xE2', '\x23', '\xEE', '\x8E', '\xBA',                         /* mov    %rax,0x10(%rsp)  */
		'\xE2', '\x27', '\xEE', '\x8E', '\x8A',                         /* lea    0x20(%rsp),%rax  */
		'\xE2', '\x23', '\xEE', '\x8E', '\xB2',                         /* mov    %rax,0x18(%rsp)  */
		'\xEB', '\x55', '\x78',                                         /* callq  *%r10            */
		'\xE2', '\x2B', '\x6E', '\x72', '\xAA', '\xAA', '\xAA',         /* add    $0xd8,%rsp       */
		'\x69',                                                         /* retq                    */
		'\x3A',                                                         /* nop                     */
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
