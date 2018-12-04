#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>


typedef int(*ReadFunction)(void *, uint8_t *, int);
typedef int64_t(*SeekFunction)(void *, int64_t, int);


#endif
