#include "VideoDecoder.h"
#include <QDebug>

VideoDecoder::VideoDecoder(QObject *parent)
    : QObject(parent),
      m_running(false),
      m_fmtCtx(nullptr),
      m_codecCtx(nullptr),
      m_streamIdx(-1),
      m_swsCtx(nullptr)
{
    avformat_network_init();
}

VideoDecoder::~VideoDecoder()
{
    stop();
    avformat_network_deinit();
}

bool VideoDecoder::start(const QString &url)
{
    if (m_running.load()) {
        return false; // already running
    }

    m_running.store(true);
    m_thread = std::thread(&VideoDecoder::workerLoop, this, url);
    return true;
}

void VideoDecoder::stop()
{
    if (!m_running.load())
        return;

    m_running.store(false);
    if (m_thread.joinable()) {
        m_thread.join();
    }

    // Worker cleans up FFmpeg resources; nothing else here
}

void VideoDecoder::workerLoop(QString url)
{
    AVFormatContext *fmtCtx = nullptr;
    AVCodecContext  *codecCtx = nullptr;
    SwsContext      *swsCtx = nullptr;
    int streamIdx = -1;

    AVDictionary *options = nullptr;
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    av_dict_set(&options, "stimeout", "5000000", 0); // 5s timeout

    QByteArray urlBytes = url.toUtf8();
    if (avformat_open_input(&fmtCtx, urlBytes.constData(), nullptr, &options) < 0) {
        emit errorOccurred(QStringLiteral("Failed to open video URL: ") + url);
        av_dict_free(&options);
        m_running.store(false);
        return;
    }
    av_dict_free(&options);

    if (avformat_find_stream_info(fmtCtx, nullptr) < 0) {
        emit errorOccurred(QStringLiteral("Failed to get stream info for: ") + url);
        avformat_close_input(&fmtCtx);
        m_running.store(false);
        return;
    }

    // Find first video stream
    for (unsigned i = 0; i < fmtCtx->nb_streams; ++i) {
        if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            streamIdx = static_cast<int>(i);
            break;
        }
    }

    if (streamIdx < 0) {
        emit errorOccurred(QStringLiteral("No video stream found in: ") + url);
        avformat_close_input(&fmtCtx);
        m_running.store(false);
        return;
    }

    AVCodecParameters *codecPar = fmtCtx->streams[streamIdx]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codecPar->codec_id);
    if (!codec) {
        emit errorOccurred(QStringLiteral("Unsupported codec in stream"));
        avformat_close_input(&fmtCtx);
        m_running.store(false);
        return;
    }

    codecCtx = avcodec_alloc_context3(codec);
    if (!codecCtx) {
        emit errorOccurred(QStringLiteral("Failed to allocate codec context"));
        avformat_close_input(&fmtCtx);
        m_running.store(false);
        return;
    }

    if (avcodec_parameters_to_context(codecCtx, codecPar) < 0) {
        emit errorOccurred(QStringLiteral("Failed to copy codec parameters"));
        avcodec_free_context(&codecCtx);
        avformat_close_input(&fmtCtx);
        m_running.store(false);
        return;
    }

    if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
        emit errorOccurred(QStringLiteral("Failed to open codec"));
        avcodec_free_context(&codecCtx);
        avformat_close_input(&fmtCtx);
        m_running.store(false);
        return;
    }

    AVPacket *pkt = av_packet_alloc();
    AVFrame  *frame = av_frame_alloc();
    if (!pkt || !frame) {
        emit errorOccurred(QStringLiteral("Failed to allocate packet/frame"));
        if (pkt) av_packet_free(&pkt);
        if (frame) av_frame_free(&frame);
        avcodec_free_context(&codecCtx);
        avformat_close_input(&fmtCtx);
        m_running.store(false);
        return;
    }

    // Main decode loop
    while (m_running.load()) {
        if (av_read_frame(fmtCtx, pkt) < 0) {
            // End of stream or error; you can decide whether to sleep & retry
            av_packet_unref(pkt);
            continue;
        }

        if (pkt->stream_index != streamIdx) {
            av_packet_unref(pkt);
            continue;
        }

        if (avcodec_send_packet(codecCtx, pkt) == 0) {
            while (avcodec_receive_frame(codecCtx, frame) == 0) {
                // Convert to RGB24
                int w = frame->width;
                int h = frame->height;

                AVPixelFormat srcFmt = static_cast<AVPixelFormat>(frame->format);
                AVPixelFormat dstFmt = AV_PIX_FMT_RGB24;

                if (!swsCtx) {
                    swsCtx = sws_getContext(
                        w, h, srcFmt,
                        w, h, dstFmt,
                        SWS_BILINEAR, nullptr, nullptr, nullptr
                    );
                    if (!swsCtx) {
                        emit errorOccurred(QStringLiteral("Failed to create SWS context"));
                        m_running.store(false);
                        break;
                    }
                }
		// eger yanlış giderse burda gider
                QImage img(w, h, QImage::Format_RGB888);
		uint8_t *dstData[4] = { img.bits(), nullptr, nullptr, nullptr };
		int dstLinesize[4] = { static_cast<int>(img.bytesPerLine()), 0, 0, 0 };

                sws_scale(
                    swsCtx,
                    frame->data,
                    frame->linesize,
                    0,
                    h,
                    dstData,
                    dstLinesize
                );

                // Emit copy (so original buffer can be reused)
                emit frameReady(img.copy());
            }
        }

        av_packet_unref(pkt);
    }

    // Cleanup
    if (swsCtx) sws_freeContext(swsCtx);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&codecCtx);
    avformat_close_input(&fmtCtx);

    m_running.store(false);
}
