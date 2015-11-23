// ex: et ts=4 sts=4 sw=4

#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <assert.h>


int seek_snapshot (int64_t sts, AVFormatContext * pifc, AVCodecContext * picc, AVFrame * pf);
int save_snapshot (const char * filename, const AVCodecContext * picc, const AVFrame * pif);


int main (int argc, char ** argv) {
    if (argc != 3) {
        return 1;
    }

    // initialize
    av_register_all();
    const char * const INPUT_FILENAME = argv[1];
    const char * const OUTPUT_FILENAME = argv[2];
    int ok = 0;

    // open file
    AVFormatContext * pfc = NULL;
    ok = avformat_open_input(&pfc, INPUT_FILENAME, NULL, NULL);
    assert(ok == 0 || !"file");

    // find stream
    int stnb = av_find_best_stream(pfc, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    AVStream * pst = pfc->streams[stnb];
    AVRational * ptb = &pst->time_base;
    int64_t duration = av_rescale(pst->duration, ptb->num, ptb->den);

    // find codec
    AVCodecContext * pcc = pst->codec;
    AVCodec * pc = avcodec_find_decoder(pcc->codec_id);
    assert(pc || !"codec");
    ok = avcodec_open2(pcc, pc, NULL);
    assert(ok == 0 || !"codec");

    int64_t sts = 300;
    int counter = 0;
    while (sts < duration) {
        printf("%s %d\n", INPUT_FILENAME, counter);

        // seek keyframe
        int64_t ffts = av_rescale(sts, ptb->den, ptb->num);
        ok = av_seek_frame(pfc, stnb, ffts, AVSEEK_FLAG_BACKWARD);
        if (ok != 0) {
            printf("seek failed\n");
            sts += 600;
            counter += 1;
            continue;
        }

        AVFrame * pf = av_frame_alloc();
        ok = seek_snapshot(sts, pfc, pcc, pf);
        assert(ok == 0 || !"seek");

        char filename[4096] = "";
        snprintf(filename, sizeof(filename), OUTPUT_FILENAME, counter);
        save_snapshot(filename, pcc, pf);

        av_frame_free(&pf);

        // add 10 minute
        sts += 600;
        counter += 1;
    }

    if (pcc) {
        avcodec_close(pcc);
    }
    if (pfc) {
        avformat_close_input(&pfc);
    }

    printf("%s ok\n", INPUT_FILENAME);

    return 0;
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
        assert(ok == 0 || !"frame");
        printf("pkt %d %d\n", pkt.size, pkt.flags);

        do {
            ok = avcodec_decode_video2(picc, pf, &got_frame, &pkt);
            if (ok < 0) {
                break;
            }
            pkt.data += ok;
            pkt.size -= ok;
        } while (ok > 0 && pkt.size > 0 && !got_frame);

        av_free_packet(&pkt);
    }

    return 0;
}


int save_snapshot (const char * filename, const AVCodecContext * picc, const AVFrame * pif) {
    AVFormatContext * pfc = NULL;
    avformat_alloc_output_context2(&pfc, NULL, NULL, filename);
    assert(pfc || !"output");

    AVCodec * pc = avcodec_find_encoder(AV_CODEC_ID_PNG);
    assert(pc || !"codec");

    AVStream * pst = avformat_new_stream(pfc, pc);
    assert(pst || !"stream");

    AVCodecContext * pcc = pst->codec;

    int ok = 0;

    pcc->width = picc->width;
    pcc->height = picc->height;
    pcc->pix_fmt = pc->pix_fmts[0];
    pcc->bit_rate = picc->bit_rate;
    pcc->codec_id = pc->id;
    pcc->codec_type = pc->type;
    ok = avcodec_open2(pcc, pc, NULL);
    assert(ok == 0 || !"codec");

    ok = avformat_write_header(pfc, NULL);
    assert(ok == 0 || !"codec");

    struct SwsContext * psc = sws_getCachedContext(NULL, picc->width, picc->height, picc->pix_fmt, pcc->width, pcc->height, pcc->pix_fmt, SWS_BILINEAR, NULL, NULL, NULL);
    assert(psc || !"sws");
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
    assert(got_frame || !"encode");

    avpicture_free((AVPicture *)pf);
    av_frame_free(&pf);

    ok = av_interleaved_write_frame(pfc, &pkt);
    av_free_packet(&pkt);
    av_write_trailer(pfc);

    if (psc) {
        sws_freeContext(psc);
    }
    if (pcc) {
        avcodec_close(pcc);
    }
    if (pfc) {
        avformat_free_context(pfc);
    }

    return 0;
}
