#define _POSIX_C_SOURCE 200809L
#include "cover_handler.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

struct CoverLoader {
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t wake;
    atomic_ulong generation;
    atomic_int stopping;
    char *pending;
    unsigned char *pixels;
    size_t width, height;
};

typedef struct {
    CoverLoader *loader;
    unsigned long generation;
    int64_t deadline;
} CoverRequest;

static int interrupted(void *opaque)
{
    CoverRequest *request = opaque;
    return(atomic_load(&request->loader->stopping) ||
           atomic_load(&request->loader->generation) != request->generation ||
           av_gettime_relative() >= request->deadline);
}

static unsigned char *decode_cover(CoverRequest *request, const char *uri, size_t *width, size_t *height)
{
    AVFormatContext *format = avformat_alloc_context();
    AVCodecContext *codec = NULL;
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    struct SwsContext *scale = NULL;
    AVDictionary *options = NULL;
    unsigned char *pixels = NULL;
    if (!format || !packet || !frame) { goto done; }
    format->interrupt_callback = (AVIOInterruptCB){interrupted, request};
    format->probesize = 1024 * 1024;
    format->max_analyze_duration = AV_TIME_BASE;
    av_dict_set(&options, "protocol_whitelist", "file,http,https,tcp,tls,crypto", 0);
    av_dict_set(&options, "rw_timeout", "3000000", 0);
    if (avformat_open_input(&format, uri, NULL, &options) < 0) { goto done; }
    if (avformat_find_stream_info(format, NULL) < 0 || interrupted(request)) { goto done; }
    const AVCodec *decoder = NULL;
    int stream = av_find_best_stream(format, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
    if (stream < 0) { goto done; }
    codec = avcodec_alloc_context3(decoder);
    if (!codec || avcodec_parameters_to_context(codec, format->streams[stream]->codecpar) < 0) { goto done; }
    codec->max_pixels = 16 * 1024 * 1024;
    codec->thread_count = 1;
    if (avcodec_open2(codec, decoder, NULL) < 0) { goto done; }
    int decoded = 0;
    for (size_t count = 0; count < 128 && !interrupted(request); ++count) {
        int result = av_read_frame(format, packet);
        if (result < 0) {
            avcodec_send_packet(codec, NULL);
            decoded = avcodec_receive_frame(codec, frame) == 0;
            break;
        }
        if (packet->stream_index == stream) {
            result = avcodec_send_packet(codec, packet);
            if (result >= 0) { decoded = avcodec_receive_frame(codec, frame) == 0; }
        }
        av_packet_unref(packet);
        if (decoded) { break; }
    }
    if (!decoded || interrupted(request) || frame->width <= 0 || frame->height <= 0 ||
        (int64_t)frame->width * frame->height > codec->max_pixels) { goto done; }
    int w = frame->width, h = frame->height;
    if (w > 512 || h > 512) {
        if (w >= h) { h = (int)((int64_t)h * 512 / w); w = 512; }

        else { w = (int)((int64_t)w * 512 / h); h = 512; }
    }
    if (w < 1) { w = 1; }
    if (h < 1) { h = 1; }
    scale = sws_getContext(frame->width, frame->height, frame->format, w, h,
                          AV_PIX_FMT_RGBA, SWS_BILINEAR, NULL, NULL, NULL);
    if (!scale) { goto done; }
    pixels = malloc((size_t)w * h * 4);
    if (!pixels) { goto done; }
    unsigned char *destination[] = {pixels, NULL, NULL, NULL};
    int strides[] = {w * 4, 0, 0, 0};
    if (sws_scale(scale, (const unsigned char *const *)frame->data, frame->linesize,
                  0, frame->height, destination, strides) != h || interrupted(request)) {
        free(pixels);
        pixels = NULL;
        goto done;
    }
    *width = (size_t)w;
    *height = (size_t)h;
done:
    sws_freeContext(scale);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codec);
    avformat_close_input(&format);
    av_dict_free(&options);
    return(pixels);
}

static void *cover_worker(void *opaque)
{
    CoverLoader *loader = opaque;
    pthread_mutex_lock(&loader->mutex);
    while (!atomic_load(&loader->stopping)) {
        while (!loader->pending && !atomic_load(&loader->stopping)) {
            pthread_cond_wait(&loader->wake, &loader->mutex);
        }
        if (atomic_load(&loader->stopping)) { break; }
        char *uri = loader->pending;
        loader->pending = NULL;
        CoverRequest request = {loader, atomic_load(&loader->generation), av_gettime_relative() + 5000000};
        pthread_mutex_unlock(&loader->mutex);
        size_t width = 0, height = 0;
        unsigned char *pixels = decode_cover(&request, uri, &width, &height);
        free(uri);
        pthread_mutex_lock(&loader->mutex);
        if (!interrupted(&request)) {
            loader->pixels = pixels;
            loader->width = width;
            loader->height = height;
        }

        else { free(pixels); }
    }
    pthread_mutex_unlock(&loader->mutex);
    return(NULL);
}

CoverLoader *cover_create(void)
{
    CoverLoader *loader = calloc(1, sizeof *loader);
    if (!loader) { return(NULL); }
    atomic_init(&loader->generation, 0);
    atomic_init(&loader->stopping, 0);
    if (pthread_mutex_init(&loader->mutex, NULL)) { free(loader); return(NULL); }
    if (pthread_cond_init(&loader->wake, NULL)) {
        pthread_mutex_destroy(&loader->mutex);
        free(loader);
        return(NULL);
    }
    if (pthread_create(&loader->thread, NULL, cover_worker, loader)) {
        pthread_cond_destroy(&loader->wake);
        pthread_mutex_destroy(&loader->mutex);
        free(loader);
        return(NULL);
    }
    return(loader);
}

void cover_request(CoverLoader *loader, const char *uri)
{
    if (!loader) { return; }
    char *pending = uri && *uri ? strdup(uri) : NULL;
    pthread_mutex_lock(&loader->mutex);
    atomic_fetch_add(&loader->generation, 1);
    free(loader->pending);
    free(loader->pixels);
    loader->pixels = NULL;
    loader->pending = pending;
    pthread_cond_signal(&loader->wake);
    pthread_mutex_unlock(&loader->mutex);
}

unsigned char *cover_take(CoverLoader *loader, size_t *width, size_t *height)
{
    if (!loader) { return(NULL); }
    pthread_mutex_lock(&loader->mutex);
    unsigned char *pixels = loader->pixels;
    loader->pixels = NULL;
    *width = loader->width;
    *height = loader->height;
    pthread_mutex_unlock(&loader->mutex);
    return(pixels);
}

void cover_destroy(CoverLoader *loader)
{
    if (!loader) { return; }
    atomic_store(&loader->stopping, 1);
    pthread_mutex_lock(&loader->mutex);
    pthread_cond_signal(&loader->wake);
    pthread_mutex_unlock(&loader->mutex);
    pthread_join(loader->thread, NULL);
    free(loader->pending);
    free(loader->pixels);
    pthread_cond_destroy(&loader->wake);
    pthread_mutex_destroy(&loader->mutex);
    free(loader);
}
