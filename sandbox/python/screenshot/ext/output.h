#ifndef OUTPUT_H
#define OUTPUT_H


#include <libavformat/avformat.h>


typedef struct {
    AVFormatContext * format_context;
    AVStream * stream;
    AVCodec * codec;
    AVCodecContext * codec_context;
} OutputContext;


OutputContext * output_context_new (const char * filename,
                                   int width, int height);
void output_context_delete (OutputContext ** self);
int output_context_write_frame (OutputContext * output_context,
                                const AVFrame * frame);


#endif
