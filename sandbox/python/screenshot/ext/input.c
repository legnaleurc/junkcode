#include "input.h"
#include "frame.h"


InputContext * input_context_new (void * opaque, ReadFunction reader,
                                  SeekFunction seeker) {
    int ok = 0;

    AVIOContext * pio = NULL;
    uint8_t * io_buffer = NULL;
    io_buffer = av_malloc(4096);
    pio = avio_alloc_context(io_buffer, 4096, 0, opaque, reader, NULL, seeker);

    // open file
    AVFormatContext * pfc = avformat_alloc_context();
    pfc->pb = pio;
    ok = avformat_open_input(&pfc, NULL, NULL, NULL);
    if (ok != 0) {
        goto failed;
    }
    if (pfc->duration == AV_NOPTS_VALUE) {
        goto close_demuxer;
    }
    int64_t duration = av_rescale(pfc->duration, 1, AV_TIME_BASE);

    // find stream
    int stnb = av_find_best_stream(pfc, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream * pst = pfc->streams[stnb];
    AVRational * ptb = &pst->time_base;

    // find codec
    AVCodec * pc = avcodec_find_decoder(pst->codecpar->codec_id);
    if (!pc) {
        goto close_demuxer;
    }
    AVCodecContext * pcc = avcodec_alloc_context3(pc);
    if (!pcc) {
        goto close_demuxer;
    }
    ok = avcodec_parameters_to_context(pcc, pst->codecpar);
    if (ok < 0) {
        goto close_decoder;
    }
    ok = avcodec_open2(pcc, pc, NULL);
    if (ok != 0) {
        goto close_decoder;
    }

    InputContext * context = malloc(sizeof(InputContext));
    context->io_context = pio;
    context->format_context = pfc;
    context->stream = pst;
    context->codec = pc;
    context->codec_context = pcc;
    context->stream_index = stnb;
    context->time_base = ptb;
    context->duration = duration;

    return context;

close_decoder:
    avcodec_free_context(&pcc);
close_demuxer:
    avformat_close_input(&pfc);
failed:
    av_freep(&pio->buffer);
    avio_context_free(&pio);
    return NULL;
}


void input_context_delete (InputContext ** input_context) {
    if (!input_context || !*input_context) {
        return;
    }
    InputContext * context = *input_context;
    avcodec_close(context->codec_context);
    avcodec_free_context(&context->codec_context);
    avformat_close_input(&context->format_context);
    av_freep(&context->io_context->buffer);
    avio_context_free(&context->io_context);
    free(context);
    *input_context = NULL;
}


int input_context_seek(InputContext * input_context, int64_t timestamp) {
    int64_t native_timestamp = av_rescale(timestamp,
                                          input_context->time_base->den,
                                          input_context->time_base->num);
    int rv = av_seek_frame(input_context->format_context,
                           input_context->stream_index, native_timestamp,
                           AVSEEK_FLAG_BACKWARD);
    if (rv < 0) {
        printf("seek failed\n");
        return -1;
    }
    avcodec_flush_buffers(input_context->codec_context);
    return 0;
}
