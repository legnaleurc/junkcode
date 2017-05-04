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
int input_context_seek_frame (InputContext * input_context, int64_t timestamp,
                              AVFrame ** target_frame);

OutputContext * output_context_new (const char * filename,
                                    int width, int height);
void output_context_delete (OutputContext ** output_context);
int output_context_write_frame (OutputContext * output_context,
                               const AVFrame * frame);

int convert_frame (const InputContext * input_context,
                   const OutputContext * output_context,
                   const AVFrame * input_frame, AVFrame ** output_frame);

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
        AVFrame * input_frame = NULL;
        ok = input_context_seek_frame(input_context, sts, &input_frame);
        if (ok != 0) {
            ok = 1;
            goto close_input_frame;
        }

        char filename[4096] = "";
        snprintf(filename, sizeof(filename), OUTPUT_FILENAME, counter);
        OutputContext * output_context =
            output_context_new(filename,
                               input_frame->width, input_frame->height);
        if (!output_context) {
            ok = 1;
            goto close_input_frame;
        }

        AVFrame * output_frame = NULL;
        ok = convert_frame(input_context, output_context,
                           input_frame, &output_frame);
        if (ok != 0) {
            ok = 1;
            goto close_output_frame;
        }

        ok = output_context_write_frame(output_context, output_frame);
        if (ok != 0) {
            ok = 1;
            goto close_output_context;
        }

        printf("take %d\n", counter);

close_output_context:
        output_context_delete(&output_context);
close_output_frame:
        if (output_frame) {
            if (output_frame->data[0]) {
                av_freep(&output_frame->data[0]);
            }
            av_frame_free(&output_frame);
        }
close_input_frame:
        if (input_frame) {
            av_frame_free(&input_frame);
        }
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


int input_context_seek_frame(InputContext * input_context, int64_t timestamp,
                             AVFrame ** target_frame) {
    if (!target_frame || *target_frame) {
        return -1;
    }

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

    AVPacket * packet = read_raw_video_frame(input_context->format_context,
                                             input_context->stream_index);
    if (!packet) {
        return -1;
    }

    *target_frame = av_frame_alloc();
    if (!*target_frame) {
        return -1;
    }

    rv = decode_video_frame(input_context->codec_context, packet, *target_frame);
    delete_raw_video_frame(&packet);

    if (rv != 0) {
        return -1;
    }

    return 0;
}


OutputContext* output_context_new (const char * filename,
                                   int width, int height) {
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
    pcc->width = width;
    pcc->height = height;
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


int convert_frame (const InputContext * input_context,
                   const OutputContext * output_context,
                   const AVFrame * input_frame, AVFrame ** output_frame) {
    if (!input_context || !output_context || !input_frame) {
        return -1;
    }
    if (!output_frame || *output_frame) {
        return -1;
    }

    int rv = 0;

    // prepare pixel convertion
    AVCodecContext * input_codec_context = input_context->codec_context;
    AVCodecContext * output_codec_context = output_context->codec_context;
    struct SwsContext * scale_context =
        sws_getCachedContext(NULL,
                             input_codec_context->width,
                             input_codec_context->height,
                             input_codec_context->pix_fmt,
                             output_codec_context->width,
                             output_codec_context->height,
                             output_codec_context->pix_fmt,
                             SWS_BILINEAR, NULL, NULL, NULL);
    if (!scale_context) {
        rv = 1;
        goto failed;
    }

    // prepare converted picture
    AVFrame * frame = av_frame_alloc();
    if (!frame) {
        rv = 1;
        goto close_sws;
    }
    frame->width = output_codec_context->width;
    frame->height = output_codec_context->height;
    frame->format = output_codec_context->pix_fmt;
    rv = av_image_alloc(frame->data, frame->linesize,
                        output_codec_context->width,
                        output_codec_context->height,
                        output_codec_context->pix_fmt, 1);
    if (rv < 0) {
        rv = AVUNERROR(rv);
        goto close_sws;
    }
    // convert pixel format
    rv = sws_scale(scale_context, (const uint8_t * const *)input_frame->data,
                   input_frame->linesize, 0, input_codec_context->height,
                   frame->data, frame->linesize);
    if (rv <= 0) {
        rv = 1;
        goto close_sws;
    }

    rv = 0;
close_sws:
    sws_freeContext(scale_context);
failed:
    return rv;
}


int output_context_write_frame (OutputContext * output_context,
                                const AVFrame * frame) {
    int rv = 0;
    // write file header
    rv = avformat_write_header(output_context->format_context, NULL);
    if (rv != 0) {
        rv = 1;
        goto failed;
    }

    // encode frame
    AVPacket packet;
    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;
    rv = avcodec_send_frame(output_context->codec_context, frame);
    if (rv != 0) {
        rv = AVUNERROR(rv);
        goto close_packet;
    }
    rv = avcodec_receive_packet(output_context->codec_context, &packet);
    if (rv != 0) {
        rv = AVUNERROR(rv);
        goto close_packet;
    }
    // NOTE does not matter in image
    packet.pts = 0;
    packet.dts = 0;

    // write encoded frame
    rv = av_interleaved_write_frame(output_context->format_context, &packet);
    if (rv != 0) {
        rv = 1;
        goto close_file;
    }

    rv = 0;
close_file:
    av_write_trailer(output_context->format_context);
close_packet:
    av_packet_unref(&packet);
failed:
    return rv;
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
