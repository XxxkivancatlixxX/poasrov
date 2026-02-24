#include "video.h"
#include <cstdio>
#include <thread>
#include <atomic>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

static AVFormatContext *fmt_ctx   = nullptr;
static AVCodecContext  *codec_ctx = nullptr;
static int              video_stream_index = -1;
static AVFrame         *frame     = nullptr;
static AVPacket        *pkt       = nullptr;

static SDL_Renderer *g_renderer = nullptr;
static SDL_Texture  *g_texture  = nullptr;
static int g_tex_w = 0, g_tex_h = 0;
static std::atomic<bool> g_initialized{false};

bool video_init(const char *rtsp_url, SDL_Renderer *renderer)
{
    g_renderer = renderer;

    avformat_network_init();
    
    AVDictionary *options = nullptr;
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    av_dict_set(&options, "stimeout", "5000000", 0);
    
    if (avformat_open_input(&fmt_ctx, rtsp_url, nullptr, &options) < 0) {
        std::fprintf(stderr, "Failed to open RTSP: %s\n", rtsp_url);
        av_dict_free(&options);
        return false;
    }
    av_dict_free(&options);
    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        std::fprintf(stderr, "Failed to get stream info\n");
        return false;
    }

    // Find video stream
    for (unsigned i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = static_cast<int>(i);
            break;
        }
    }
    if (video_stream_index < 0) {
        std::fprintf(stderr, "No video stream found\n");
        return false;
    }

    AVCodecParameters *codecpar = fmt_ctx->streams[video_stream_index]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        std::fprintf(stderr, "No suitable decoder\n");
        return false;
    }

    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        std::fprintf(stderr, "Failed to alloc codec context\n");
        return false;
    }

    if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0) {
        std::fprintf(stderr, "Failed to copy codec params\n");
        return false;
    }

    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        std::fprintf(stderr, "Failed to open codec\n");
        return false;
    }

    frame = av_frame_alloc();
    pkt   = av_packet_alloc();
    if (!frame || !pkt) {
        std::fprintf(stderr, "Failed to alloc frame/packet\n");
        return false;
    }

    g_tex_w = codec_ctx->width;
    g_tex_h = codec_ctx->height;
    if (g_tex_w <= 0 || g_tex_h <= 0) {
        std::fprintf(stderr, "Invalid video size\n");
        return false;
    }

    // RGB24 texture. If source isn’t RGB24, we’ll need sws_scale later.
    g_texture = SDL_CreateTexture(
        g_renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING,
        g_tex_w, g_tex_h
    );
    if (!g_texture) {
        std::fprintf(stderr, "Failed to create SDL texture\n");
        return false;
    }

    g_initialized = true;
    return true;
}

void video_init_async(const char *rtsp_url, SDL_Renderer *renderer)
{
    std::thread t([rtsp_url, renderer]() {
        video_init(rtsp_url, renderer);
    });
    t.detach();
}

bool video_is_initialized()
{
    return g_initialized.load();
}

void video_update()
{
    if (!fmt_ctx || !codec_ctx) return;

    if (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == video_stream_index) {
            if (avcodec_send_packet(codec_ctx, pkt) == 0) {
                while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    if (frame->format == AV_PIX_FMT_RGB24) {
                        void *pixels = nullptr;
                        int pitch = 0;
                        if (SDL_LockTexture(g_texture, nullptr, &pixels, &pitch) == 0) {
                            uint8_t *dst = static_cast<uint8_t*>(pixels);
                            for (int y = 0; y < g_tex_h; ++y) {
                                memcpy(dst + y * pitch,
                                       frame->data[0] + y * frame->linesize[0],
                                       g_tex_w * 3);
                            }
                            SDL_UnlockTexture(g_texture);
                        }
                    } else {

                    }
                }
            }
        }
        av_packet_unref(pkt);
    }
}

SDL_Texture *video_get_texture()
{
    return g_texture;
}

void video_shutdown()
{
    if (g_texture) { SDL_DestroyTexture(g_texture); g_texture = nullptr; }
    if (frame)     { av_frame_free(&frame); frame = nullptr; }
    if (pkt)       { av_packet_free(&pkt);  pkt = nullptr; }
    if (codec_ctx) { avcodec_free_context(&codec_ctx); codec_ctx = nullptr; }
    if (fmt_ctx)   { avformat_close_input(&fmt_ctx); fmt_ctx = nullptr; }
    avformat_network_deinit();
}
