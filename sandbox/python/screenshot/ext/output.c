#include "output.h"


OutputContext * output_context_new (const char * filename,
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
