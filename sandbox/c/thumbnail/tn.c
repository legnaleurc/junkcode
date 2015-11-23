// ex: et ts=4 sts=4 sw=4

#include <libavformat/avformat.h>
#include <libswscale/swscale.h>


static const int START_SECOND = 300;
static const int INTERVAL_SECOND = 600;


int seek_snapshot (int64_t sts, AVFormatContext * pifc, AVCodecContext * picc, AVFrame * pf);
int save_snapshot (const char * filename, const AVCodecContext * picc, const AVFrame * pif);


int main (int argc, char ** argv) {
    if (argc != 3) {
        return 1;
    }

    // initialize
    const char * const INPUT_FILENAME = argv[1];
    const char * const OUTPUT_FILENAME = argv[2];
    av_register_all();
    int ok = 0;

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
    AVCodecContext * pcc = pst->codec;
    AVCodec * pc = avcodec_find_decoder(pcc->codec_id);
    if (!pc) {
        ok = 1;
        goto close_demuxer;
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
        if (ok != 0) {
            printf("seek failed\n");
            goto next_time;
        }

        AVFrame * pf = av_frame_alloc();
        ok = seek_snapshot(sts, pfc, pcc, pf);
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
    if (pc) {
        avcodec_close(pcc);
    }
close_demuxer:
    if (pfc) {
        avformat_close_input(&pfc);
    }
failed:
    printf("result %d\n", ok);
    return ok;
}


int seek_snapshot (int64_t sts, AVFormatContext * pifc, AVCodecContext * picc, AVFrame * pf) {
    int ok = 0;
    int got_frame = 0;

    while (!got_frame) {
        AVPacket pkt;
        av_init_packet(&pkt);
        pkt.data = NULL;
        pkt.size = 0;
        ok = av_read_frame(pifc, &pkt);
        if (ok != 0) {
            ok = 1;
            goto skip_frame;
        }
        if (pifc->streams[pkt.stream_index]->codec != picc) {
            // skip other streams
            ok = 0;
            goto skip_frame;
        }

        do {
            ok = avcodec_decode_video2(picc, pf, &got_frame, &pkt);
            if (ok < 0) {
                ok = 1;
                goto skip_frame;
            }
            pkt.data += ok;
            pkt.size -= ok;
        } while (ok > 0 && pkt.size > 0 && !got_frame);

        ok = 0;
skip_frame:
        av_free_packet(&pkt);
        if (ok != 0) {
            break;
        }
    }

    return ok;
}


int save_snapshot (const char * filename, const AVCodecContext * picc, const AVFrame * pif) {
    int ok = 0;

    AVFormatContext * pfc = NULL;
    avformat_alloc_output_context2(&pfc, NULL, NULL, filename);
    if (!pfc) {
        ok = 1;
        goto save_output_failed;
    }

    AVCodec * pc = avcodec_find_encoder(AV_CODEC_ID_PNG);
    if (!pc) {
        ok = 1;
        goto close_muxer;
    }

    AVStream * pst = avformat_new_stream(pfc, pc);
    if (!pst) {
        ok = 1;
        goto close_muxer;
    }

    AVCodecContext * pcc = pst->codec;
    pcc->width = picc->width;
    pcc->height = picc->height;
    pcc->pix_fmt = pc->pix_fmts[0];
    pcc->bit_rate = picc->bit_rate;
    pcc->codec_id = pc->id;
    pcc->codec_type = pc->type;
    ok = avcodec_open2(pcc, pc, NULL);
    if (ok != 0) {
        ok = 1;
        goto close_muxer;
    }

    ok = avformat_write_header(pfc, NULL);
    if (ok != 0) {
        ok = 1;
        goto close_encoder;
    }

    struct SwsContext * psc = sws_getCachedContext(NULL, picc->width, picc->height, picc->pix_fmt, pcc->width, pcc->height, pcc->pix_fmt, SWS_BILINEAR, NULL, NULL, NULL);
    if (!psc) {
        ok = 1;
        goto close_encoder;
    }

    AVFrame * pf = av_frame_alloc();
    pf->width = pcc->width;
    pf->height = pcc->height;
    pf->format = pcc->pix_fmt;
    avpicture_alloc((AVPicture *)pf, pcc->pix_fmt, pcc->width, pcc->height);
    sws_scale(psc, (const uint8_t * const *)pif->data, pif->linesize, 0, picc->height, pf->data, pf->linesize);

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    int got_frame = 0;
    ok = avcodec_encode_video2(pcc, &pkt, pf, &got_frame);
    if (!got_frame) {
        goto close_scale;
    }
    // NOTE does not matter in image
    pkt.pts = 0;
    pkt.dts = 0;

    avpicture_free((AVPicture *)pf);
    av_frame_free(&pf);

    ok = av_interleaved_write_frame(pfc, &pkt);
    av_free_packet(&pkt);
    av_write_trailer(pfc);

    ok = 0;
close_scale:
    if (psc) {
        sws_freeContext(psc);
    }
close_encoder:
    if (pcc) {
        avcodec_close(pcc);
    }
close_muxer:
    if (pfc) {
        avformat_free_context(pfc);
    }
save_output_failed:
    return ok;
}
