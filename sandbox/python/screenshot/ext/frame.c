#include "frame.h"

#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>


FrameContext * frame_context_new (InputContext * input_context) {
    AVPacket * packet = read_raw_video_frame(input_context->format_context,
                                             input_context->stream_index);
    if (!packet) {
        return NULL;
    }

    FrameContext * frame_context = malloc(sizeof(FrameContext));
    if (!frame_context) {
        goto free_packet;
    }
    memset(frame_context, 0, sizeof(*frame_context));

    frame_context->input_context = input_context;
    frame_context->input_frame = av_frame_alloc();
    if (!frame_context->input_frame) {
        goto free_frame_context;
    }

    int rv = 0;
    rv = decode_video_frame(input_context->codec_context, packet,
                            frame_context->input_frame);
    if (rv != 0) {
        goto free_frame_context;
    }
    delete_raw_video_frame(&packet);
    goto succeed;

free_frame_context:
    free(frame_context);
    frame_context = NULL;
free_packet:
    delete_raw_video_frame(&packet);
    return NULL;
succeed:
    return frame_context;
}


void frame_context_delete (FrameContext ** frame_context) {
    if (!frame_context || !*frame_context) {
        return;
    }
    FrameContext * context = *frame_context;
    if (context->output_frame) {
        if (context->output_frame->data[0]) {
            av_freep(&context->output_frame->data[0]);
        }
        av_frame_free(&context->output_frame);
    }
    if (context->input_frame) {
        av_frame_free(&context->input_frame);
    }
    free(context);
    *frame_context = NULL;
}


int frame_context_convert (FrameContext * frame_context) {
    const InputContext * input_context = frame_context->input_context;
    const OutputContext * output_context = frame_context->output_context;
    const AVFrame * input_frame = frame_context->input_frame;
    AVFrame * output_frame = frame_context->output_frame;
    if (!input_context || !output_context || !input_frame) {
        return -1;
    }
    if (output_frame) {
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
    output_frame = av_frame_alloc();
    if (!output_frame) {
        rv = 1;
        goto close_sws;
    }
    output_frame->width = output_codec_context->width;
    output_frame->height = output_codec_context->height;
    output_frame->format = output_codec_context->pix_fmt;
    rv = av_image_alloc(output_frame->data, output_frame->linesize,
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
                   output_frame->data, output_frame->linesize);
    if (rv <= 0) {
        rv = 1;
        goto close_sws;
    }

    rv = 0;
close_sws:
    sws_freeContext(scale_context);
failed:
    frame_context->output_frame = output_frame;
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
