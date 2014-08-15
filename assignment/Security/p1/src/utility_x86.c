#include <string.h>

#include "utility.h"

static void decryptCode (unsigned char * code, unsigned long length) {
    unsigned long i = 0;
    for(; i < length; ++i) {
        code[i] ^= '\xAA';
    }
}

unsigned long extractCode (unsigned char * code, unsigned long length) {
    /* NOTE: encrypted by XOR to 10101010b */
    static const unsigned char CODE[] = {
    /*  void inline_code (int (* a)(const char *, va_list), const char * b, ...) { */
        '\xFF',                 /*push        ebp                                  */
        '\x21', '\x46',         /*mov         ebp,esp                              */
    /*      va_list args;                                                          */
    /*      va_start( args, b );                                                   */
    /*      a( b, args );                                                          */
        '\x21', '\xE7', '\xA6', /*mov         ecx,dword ptr [b]                    */
        '\x27', '\xEF', '\xBA', /*lea         eax,[ebp+10h]                        */
        '\xFA',                 /*push        eax                                  */
        '\xFB',                 /*push        ecx                                  */
        '\x55', '\xFF', '\xA2', /*call        dword ptr [a]                        */
        '\x29', '\x6E', '\xA2', /*add         esp,8                                */
    /*      va_end( args );                                                        */
    /*  }                                                                          */
        '\xF7',                 /*pop         ebp                                  */
        '\x69',                 /*ret                                              */
    };
    static const unsigned long LENGTH = sizeof(CODE);

    if (length < LENGTH) {
        return 0;
    }

    /* NOP in x86 is 0x90 */
    memset(code, '\x90', length);
    /* the inline asm is in stack, which is not executable, so we need to copy it to the executable page */
    memcpy(code, CODE, LENGTH);
    /* decrypt machine code */
    decryptCode(code, LENGTH);

    return LENGTH;
}
