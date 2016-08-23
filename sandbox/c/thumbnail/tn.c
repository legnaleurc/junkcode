// ex: et ts=4 sts=4 sw=4

#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>


static const int START_SECOND = 300;
static const int INTERVAL_SECOND = 300;


int seek_snapshot (AVFormatContext * pifc, AVCodecContext * picc, AVFrame * pf, int vi);
int save_snapshot (const char * filename, const AVCodecContext * picc, const AVFrame * pif);
AVPacket * read_raw_video_frame(AVFormatContext * pifc, int vi);
void delete_raw_video_frame(AVPacket ** pkt);
int decode_video_frame(AVCodecContext * picc, const AVPacket * pkt, AVFrame * pf);


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

    // open file
    AVFormatContext * pfc = NULL;
    ok = avformat_open_input(&pfc, INPUT_FILENAME, NULL, NULL);
    if (ok != 0) {
        ok = 1;
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
        ok = 1;
        goto close_demuxer;
    }
    AVCodecContext * pcc = avcodec_alloc_context3(pc);
    if (!pcc) {
        ok = 1;
        goto close_demuxer;
    }
    ok = avcodec_parameters_to_context(pcc, pst->codecpar);
    if (ok < 0) {
        ok = 1;
        goto close_decoder;
    }
    ok = avcodec_open2(pcc, pc, NULL);
    if (ok != 0) {
        ok = 1;
        goto close_demuxer;
    }

    int64_t sts = START_SECOND;
    int counter = 0;
    while (sts < duration) {
        // seek keyframe
        int64_t ffts = av_rescale(sts, ptb->den, ptb->num);
        ok = av_seek_frame(pfc, stnb, ffts, AVSEEK_FLAG_BACKWARD);
        if (ok < 0) {
            printf("seek failed\n");
            goto next_time;
        }
        avcodec_flush_buffers(pcc);

        AVFrame * pf = av_frame_alloc();
        ok = seek_snapshot(pfc, pcc, pf, stnb);
        if (ok != 0) {
            ok = 1;
            goto close_frame;
        }

        char filename[4096] = "";
        snprintf(filename, sizeof(filename), OUTPUT_FILENAME, counter);
        ok = save_snapshot(filename, pcc, pf);
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

    ok = 0;
    avcodec_close(pcc);
close_decoder:
    avcodec_free_context(&pcc);
close_demuxer:
    avformat_close_input(&pfc);
failed:
    printf("result %d\n", ok);
    return ok;
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

    // prepare muxer
    AVFormatContext * pfc = NULL;
    avformat_alloc_output_context2(&pfc, NULL, NULL, filename);
    if (!pfc) {
        ok = 1;
        goto failed;
    }

    // prepare encoding stream
    AVStream * pst = avformat_new_stream(pfc, NULL);
    if (!pst) {
        ok = 1;
        goto close_muxer;
    }

    // find encoder
    enum AVCodecID codec_id = av_guess_codec(pfc->oformat, NULL, filename, NULL, AVMEDIA_TYPE_VIDEO);
    if (codec_id == AV_CODEC_ID_NONE) {
        ok = 1;
        goto close_muxer;
    }
    AVCodec * pc = avcodec_find_encoder(codec_id);
    if (!pc) {
        ok = 1;
        goto close_muxer;
    }

    // prepare encoder
    AVCodecContext * pcc = avcodec_alloc_context3(pc);
    pcc->width = picc->width;
    pcc->height = picc->height;
    pcc->pix_fmt = pc->pix_fmts[0];
    pcc->bit_rate = picc->bit_rate;
    pcc->codec_id = pc->id;
    pcc->codec_type = pc->type;
    pcc->time_base.num = 1;
    pcc->time_base.den = 1;
    ok = avcodec_open2(pcc, pc, NULL);
    if (ok != 0) {
        ok = 1;
        goto free_encoder;
    }

    ok = avcodec_parameters_from_context(pst->codecpar, pcc);
    if (ok < 0) {
        ok = 1;
        goto free_encoder;
    }

    // write file header
    ok = avformat_write_header(pfc, NULL);
    if (ok != 0) {
        ok = 1;
        goto close_encoder;
    }

    // prepare pixel convertion
    struct SwsContext * psc = sws_getCachedContext(NULL, picc->width, picc->height, picc->pix_fmt, pcc->width, pcc->height, pcc->pix_fmt, SWS_BILINEAR, NULL, NULL, NULL);
    if (!psc) {
        ok = 1;
        goto close_encoder;
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
    ok = av_interleaved_write_frame(pfc, &pkt);
    if (ok != 0) {
        ok = 1;
        goto close_file;
    }

    ok = 0;
close_file:
    av_write_trailer(pfc);
close_packet:
    av_packet_unref(&pkt);
close_picture:
    av_freep(&pf->data[0]);
close_frame:
    av_frame_free(&pf);
close_sws:
    sws_freeContext(psc);
close_encoder:
    avcodec_close(pcc);
free_encoder:
    avcodec_free_context(&pcc);
close_muxer:
    avformat_free_context(pfc);
failed:
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
    if (!pkt || !*pkt) {
        return;
    }

    av_packet_unref(*pkt);
    av_freep(pkt);
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
