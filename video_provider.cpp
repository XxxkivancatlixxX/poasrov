#include "video_provider.h"
#include <QDebug>
#include <QThread>

extern "C" {
#include <libavutil/imgutils.h>
}

VideoProvider::VideoProvider(QObject *parent)
    : QObject(parent)
{
    avformat_network_init();
}

VideoProvider::~VideoProvider()
{
    disconnect();
    avformat_network_deinit();
}

QImage VideoProvider::currentFrame() const
{
    QMutexLocker locker(&m_frameMutex);
    return m_currentFrame;
}

bool VideoProvider::isConnected() const
{
    return m_connected.load();
}

void VideoProvider::connectToCamera(const QString &rtspUrl)
{
    qDebug() << "VideoProvider::connectToCamera called with URL:" << rtspUrl;
    
    // Stop any existing thread first
    stopThread();

    m_running = true;
    m_thread = std::make_unique<std::thread>(&VideoProvider::videoThread, this, rtspUrl.toStdString());
}

void VideoProvider::stopThread()
{
    qDebug() << "VideoProvider::stopThread called";
    m_running = false;
    
    if (m_thread && m_thread->joinable()) {
        qDebug() << "Waiting for video thread to stop...";
        m_thread->join();
        qDebug() << "Video thread stopped";
    }
    
    m_thread.reset();
}

void VideoProvider::disconnect()
{
    qDebug() << "VideoProvider::disconnect called";
    
    stopThread();
    cleanup();
    
    if (m_connected.load()) {
        m_connected = false;
        QMetaObject::invokeMethod(this, "connectionChanged", Qt::QueuedConnection);
    }
}

void VideoProvider::cleanup()
{
    if (m_swsCtx) {
        sws_freeContext(m_swsCtx);
        m_swsCtx = nullptr;
    }
    if (m_codecCtx) {
        avcodec_free_context(&m_codecCtx);
        m_codecCtx = nullptr;
    }
    if (m_formatCtx) {
        avformat_close_input(&m_formatCtx);
        m_formatCtx = nullptr;
    }
}

void VideoProvider::videoThread(const std::string &url)
{
    qDebug() << "VideoProvider::videoThread started with URL:" << QString::fromStdString(url);
    
    AVDictionary *options = nullptr;
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    av_dict_set(&options, "stimeout", "5000000", 0);
    av_dict_set(&options, "max_delay", "500000", 0);

    qDebug() << "Attempting to open RTSP stream...";
    int ret = avformat_open_input(&m_formatCtx, url.c_str(), nullptr, &options);
    if (ret < 0) {
        av_dict_free(&options);
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errbuf, sizeof(errbuf));
        QString error = QString("Failed to open RTSP stream: %1\nError: %2\n\nTroubleshooting:\n- Check if camera is powered on\n- Verify IP address is correct\n- Try different stream paths: /stream, /video, /live, /h264")
                        .arg(QString::fromStdString(url))
                        .arg(QString::fromUtf8(errbuf));
        qDebug() << error;
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection, 
                                  Q_ARG(QString, error));
        m_running = false;
        return;
    }
    av_dict_free(&options);
    qDebug() << "RTSP stream opened successfully";

    if (avformat_find_stream_info(m_formatCtx, nullptr) < 0) {
        qDebug() << "Failed to find stream info";
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "Failed to find stream info"));
        cleanup();
        m_running = false;
        return;
    }
    qDebug() << "Stream info found, looking for video stream...";

    // Find video stream
    for (unsigned i = 0; i < m_formatCtx->nb_streams; i++) {
        if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_videoStreamIndex = static_cast<int>(i);
            break;
        }
    }

    if (m_videoStreamIndex < 0) {
        qDebug() << "No video stream found in RTSP feed";
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "No video stream found"));
        cleanup();
        m_running = false;
        return;
    }
    qDebug() << "Video stream found at index:" << m_videoStreamIndex;

    AVCodecParameters *codecpar = m_formatCtx->streams[m_videoStreamIndex]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        qDebug() << "Codec not found for codec_id:" << codecpar->codec_id;
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "Codec not found"));
        cleanup();
        m_running = false;
        return;
    }
    qDebug() << "Codec found:" << codec->name;

    m_codecCtx = avcodec_alloc_context3(codec);
    if (!m_codecCtx) {
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "Failed to allocate codec context"));
        cleanup();
        m_running = false;
        return;
    }

    if (avcodec_parameters_to_context(m_codecCtx, codecpar) < 0) {
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "Failed to copy codec parameters"));
        cleanup();
        m_running = false;
        return;
    }

    if (avcodec_open2(m_codecCtx, codec, nullptr) < 0) {
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "Failed to open codec"));
        cleanup();
        m_running = false;
        return;
    }

    m_connected = true;
    qDebug() << "Video decoder initialized successfully. Starting frame loop...";
    qDebug() << "Video size:" << m_codecCtx->width << "x" << m_codecCtx->height;
    QMetaObject::invokeMethod(this, "connectionChanged", Qt::QueuedConnection);

    AVFrame *frame = av_frame_alloc();
    AVFrame *frameRGB = av_frame_alloc();
    AVPacket *packet = av_packet_alloc();

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_codecCtx->width, 
                                             m_codecCtx->height, 1);
    uint8_t *buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(frameRGB->data, frameRGB->linesize, buffer, AV_PIX_FMT_RGB24,
                         m_codecCtx->width, m_codecCtx->height, 1);

    m_swsCtx = sws_getContext(m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,
                               m_codecCtx->width, m_codecCtx->height, AV_PIX_FMT_RGB24,
                               SWS_BILINEAR, nullptr, nullptr, nullptr);

    while (m_running.load()) {
        if (av_read_frame(m_formatCtx, packet) >= 0) {
            if (packet->stream_index == m_videoStreamIndex) {
                if (avcodec_send_packet(m_codecCtx, packet) == 0) {
                    while (avcodec_receive_frame(m_codecCtx, frame) == 0) {
                        sws_scale(m_swsCtx, frame->data, frame->linesize, 0, m_codecCtx->height,
                                  frameRGB->data, frameRGB->linesize);

                        QImage img(frameRGB->data[0], m_codecCtx->width, m_codecCtx->height,
                                   frameRGB->linesize[0], QImage::Format_RGB888);

                        {
                            QMutexLocker locker(&m_frameMutex);
                            m_currentFrame = img.copy();
                        }
                        
                        QMetaObject::invokeMethod(this, "frameChanged", Qt::QueuedConnection);
                    }
                }
            }
            av_packet_unref(packet);
        } else {
            // Read failed, might be end of stream or connection issue
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    qDebug() << "Video thread stopping...";
    av_free(buffer);
    av_frame_free(&frameRGB);
    av_frame_free(&frame);
    av_packet_free(&packet);

    cleanup();
    m_connected = false;
    QMetaObject::invokeMethod(this, "connectionChanged", Qt::QueuedConnection);
}
