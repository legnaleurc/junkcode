#ifndef FRAME_H
#define FRAME_H


#include <libavformat/avformat.h>


#include "input.h"
#include "output.h"


typedef struct {
    InputContext * input_context;
    OutputContext * output_context;
    AVFrame * input_frame;
    AVFrame * output_frame;
} FrameContext;


FrameContext * frame_context_new (InputContext * input_context);
void frame_context_delete (FrameContext ** frame_context);
int frame_context_convert (FrameContext * frame_context);

AVPacket * read_raw_video_frame (AVFormatContext * pifc, int vi);
void delete_raw_video_frame (AVPacket ** pkt);
int decode_video_frame (AVCodecContext * picc, const AVPacket * pkt, AVFrame * pf);


#endif
