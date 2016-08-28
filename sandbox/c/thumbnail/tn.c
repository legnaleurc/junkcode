// ex: et ts=4 sts=4 sw=4

#include <stdlib.h>

#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>


static const int START_SECOND = 300;
static const int INTERVAL_SECOND = 300;


typedef struct {
    AVFormatContext * format_context;
    AVStream * stream;
    AVCodec * codec;
    AVCodecContext * codec_context;
    int stream_index;
    AVRational * time_base;
    int64_t duration;
} InputContext;


typedef struct {
    AVFormatContext * format_context;
    AVStream * stream;
    AVCodec * codec;
    AVCodecContext * codec_context;
} OutputContext;


InputContext * input_context_new (const char * filename);
void input_context_delete (InputContext ** input_context);

OutputContext * output_context_new (const char * filename);
void output_context_delete (OutputContext ** output_context);
int output_context_setup(OutputContext * self, const AVFrame * input_frame);

int seek_snapshot (AVFormatContext * pifc, AVCodecContext * picc, AVFrame * pf, int vi);
int save_snapshot (const char * filename, const AVCodecContext * picc, const AVFrame * pif);
AVPacket * read_raw_video_frame (AVFormatContext * pifc, int vi);
void delete_raw_video_frame (AVPacket ** pkt);
int decode_video_frame (AVCodecContext * picc, const AVPacket * pkt, AVFrame * pf);


int main (int argc, char ** argv) {
    if (argc != 3) {
        return 1;
    }

    // initialize
    const char * const INPUT_FILENAME = argv[1];
    const char * const OUTPUT_FILENAME = argv[2];
    int ok = 0;
    av_register_all();

    printf("%s\n", INPUT_FILENAME);

    InputContext * input_context = input_context_new(INPUT_FILENAME);
    if (!input_context) {
        ok = EXIT_FAILURE;
        goto failed;
    }

    int64_t sts = START_SECOND;
    int counter = 0;
    while (sts < input_context->duration) {
        // seek keyframe
        int64_t ffts = av_rescale(sts, input_context->time_base->den, input_context->time_base->num);
        ok = av_seek_frame(input_context->format_context, input_context->stream_index, ffts, AVSEEK_FLAG_BACKWARD);
        if (ok < 0) {
            printf("seek failed\n");
            goto next_time;
        }
        avcodec_flush_buffers(input_context->codec_context);

        AVFrame * pf = av_frame_alloc();
        ok = seek_snapshot(input_context->format_context, input_context->codec_context, pf, input_context->stream_index);
        if (ok != 0) {
            ok = 1;
            goto close_frame;
        }

        char filename[4096] = "";
        snprintf(filename, sizeof(filename), OUTPUT_FILENAME, counter);
        ok = save_snapshot(filename, input_context->codec_context, pf);
        if (ok != 0) {
            ok = 1;
            goto close_frame;
        }

        printf("take %d\n", counter);

close_frame:
        av_frame_free(&pf);
next_time:
        sts += INTERVAL_SECOND;
        counter += 1;
    }

    ok = EXIT_SUCCESS;
    input_context_delete(&input_context);
failed:
    printf("result %d\n", ok);
    return ok;
}


InputContext * input_context_new (const char * filename) {
    int ok = 0;

    // open file
    AVFormatContext * pfc = NULL;
    ok = avformat_open_input(&pfc, filename, NULL, NULL);
    if (ok != 0) {
        goto failed;
    }

    // find stream
    int stnb = av_find_best_stream(pfc, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream * pst = pfc->streams[stnb];
    AVRational * ptb = &pst->time_base;
    int64_t duration = av_rescale(pst->duration, ptb->num, ptb->den);

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
    free(context);
    *input_context = NULL;
}


OutputContext* output_context_new (const char * filename) {
    int ok = 0;

    // prepare muxer
    AVFormatContext * pfc = NULL;
    avformat_alloc_output_context2(&pfc, NULL, NULL, filename);
    if (!pfc) {
        goto failed;
    }

    // prepare encoding stream
    AVStream * pst = avformat_new_stream(pfc, NULL);
    if (!pst) {
        goto close_muxer;
    }

    // find encoder
    enum AVCodecID codec_id = av_guess_codec(pfc->oformat, NULL, filename, NULL, AVMEDIA_TYPE_VIDEO);
    if (codec_id == AV_CODEC_ID_NONE) {
        goto close_muxer;
    }
    AVCodec * pc = avcodec_find_encoder(codec_id);
    if (!pc) {
        goto close_muxer;
    }

    // prepare encoder
    AVCodecContext * pcc = avcodec_alloc_context3(pc);
    pcc->pix_fmt = pc->pix_fmts[0];
    pcc->codec_id = pc->id;
    pcc->codec_type = pc->type;
    pcc->time_base.num = 1;
    pcc->time_base.den = 1;
    ok = avcodec_open2(pcc, pc, NULL);
    if (ok != 0) {
        goto free_encoder;
    }

    ok = avcodec_parameters_from_context(pst->codecpar, pcc);
    if (ok < 0) {
        goto free_encoder;
    }

    OutputContext * context = malloc(sizeof(OutputContext));
    context->format_context = pfc;
    context->stream = pst;
    context->codec = pc;
    context->codec_context = pcc;

    return context;

free_encoder:
    avcodec_free_context(&pcc);
close_muxer:
    avformat_free_context(pfc);
failed:
    return NULL;
}


void output_context_delete (OutputContext ** self) {
    if (!self || !*self) {
        return;
    }
    OutputContext * context = *self;
    avcodec_close(context->codec_context);
    avcodec_free_context(&context->codec_context);
    avformat_free_context(context->format_context);
    free(context);
    *self = NULL;
}


int output_context_setup (OutputContext * self, const AVFrame * input_frame) {
    AVCodecContext * pcc = self->codec_context;
    pcc->width = input_frame->width;
    pcc->height = input_frame->height;
    // pcc->bit_rate = input->codec_context->bit_rate;
    return 0;
}


int seek_snapshot (AVFormatContext * pifc, AVCodecContext * picc, AVFrame * pf, int vi) {
    int ok = 0;

    AVPacket * pkt = read_raw_video_frame(pifc, vi);
    if (!pkt) {
        return -1;
    }

    ok = decode_video_frame(picc, pkt, pf);

    delete_raw_video_frame(&pkt);

    return ok;
}


int save_snapshot (const char * filename, const AVCodecContext * picc, const AVFrame * pif) {
    int ok = 0;

    OutputContext * output_context = output_context_new(filename);
    if (!output_context) {
        ok = -1;
        goto failed;
    }

    ok = output_context_setup(output_context, pif);

    // write file header
    ok = avformat_write_header(output_context->format_context, NULL);
    if (ok != 0) {
        ok = 1;
        goto failed;
    }

    // prepare pixel convertion
    AVCodecContext * pcc = output_context->codec_context;
    struct SwsContext * psc = sws_getCachedContext(NULL, picc->width, picc->height, picc->pix_fmt, pcc->width, pcc->height, pcc->pix_fmt, SWS_BILINEAR, NULL, NULL, NULL);
    if (!psc) {
        ok = 1;
        goto failed;
    }

    // prepare converted picture
    AVFrame * pf = av_frame_alloc();
    if (!pf) {
        ok = 1;
        goto close_sws;
    }
    pf->width = pcc->width;
    pf->height = pcc->height;
    pf->format = pcc->pix_fmt;
    ok = av_image_alloc(pf->data, pf->linesize, pcc->width, pcc->height, pcc->pix_fmt, 1);
    if (ok < 0) {
        ok = AVUNERROR(ok);
        goto close_frame;
    }
    // convert pixel format
    ok = sws_scale(psc, (const uint8_t * const *)pif->data, pif->linesize, 0, picc->height, pf->data, pf->linesize);
    if (ok <= 0) {
        ok = 1;
        goto close_picture;
    }

    // encode frame
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    ok = avcodec_send_frame(pcc, pf);
    if (ok != 0) {
        ok = AVUNERROR(ok);
        goto close_packet;
    }
    ok = avcodec_receive_packet(pcc, &pkt);
    if (ok != 0) {
        ok = AVUNERROR(ok);
        goto close_packet;
    }
    // NOTE does not matter in image
    pkt.pts = 0;
    pkt.dts = 0;

    // write encoded frame
    ok = av_interleaved_write_frame(output_context->format_context, &pkt);
    if (ok != 0) {
        ok = 1;
        goto close_file;
    }

    ok = 0;
close_file:
    av_write_trailer(output_context->format_context);
close_packet:
    av_packet_unref(&pkt);
close_picture:
    av_freep(&pf->data[0]);
close_frame:
    av_frame_free(&pf);
close_sws:
    sws_freeContext(psc);
failed:
    output_context_delete(&output_context);
    return ok;
}


AVPacket * read_raw_video_frame (AVFormatContext * pifc, int vi) {
    int ok = 0;

    AVPacket * pkt = av_malloc(sizeof(AVPacket));
    av_init_packet(pkt);
    pkt->data = NULL;
    pkt->size = 0;

    while (1) {
        // read a raw packet
        ok = av_read_frame(pifc, pkt);
        if (ok != 0) {
            ok = AVUNERROR(ok);
            goto failed;
        }

        // the raw packet may belong to other streams after seeking
        if (pkt->stream_index == vi) {
            break;
        }
        av_packet_unref(pkt);
    }

    return pkt;

failed:
    delete_raw_video_frame(&pkt);
    return NULL;
}


void delete_raw_video_frame (AVPacket ** pkt) {
    if (pkt) {
        if (*pkt) {
            av_packet_unref(*pkt);
        }
        av_freep(pkt);
    }
}


int decode_video_frame (AVCodecContext * picc, const AVPacket * pkt, AVFrame * pf) {
    int ok = 0;

    // may need to feed multiple times to get one frame
    while (1) {
        ok = avcodec_send_packet(picc, pkt);
        if (ok != 0) {
            ok = AVUNERROR(ok);
            return ok;
        }

        ok = avcodec_receive_frame(picc, pf);
        ok = AVUNERROR(ok);
        if (ok != EAGAIN) {
            // maybe failed, just return the error code
            return ok;
        }
        // need to feed raw packet again
    }

    return ok;
}
