#ifndef INPUT_H
#define INPUT_H


#include <libavformat/avformat.h>

#include "types.h"


typedef struct {
    AVIOContext * io_context;
    AVFormatContext * format_context;
    AVStream * stream;
    AVCodec * codec;
    AVCodecContext * codec_context;
    int stream_index;
    AVRational * time_base;
    int64_t duration;
} InputContext;


InputContext * input_context_new (void * opaque, ReadFunction reader,
                                  SeekFunction seeker);
void input_context_delete (InputContext ** input_context);
int input_context_seek(InputContext * input_context, int64_t timestamp);


#endif
