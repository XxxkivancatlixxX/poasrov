#include "video_provider.h"
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QDir>

extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}

VideoProvider::VideoProvider(QObject *parent)
    : QObject(parent)
{
    avformat_network_init();
}

VideoProvider::~VideoProvider()
{
    stopRecording();
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

bool VideoProvider::isRecording() const
{
    return m_recording.load();
}

void VideoProvider::takePicture(const QString &savePath)
{
    QMutexLocker locker(&m_frameMutex);
    
    if (m_currentFrame.isNull()) {
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "No frame available to save"));
        return;
    }
    
    // Generate filename with timestamp
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString filename = QString("%1/image_%2.png").arg(savePath).arg(timestamp);
    
    // Ensure directory exists
    QDir dir;
    dir.mkpath(savePath);
    
    if (m_currentFrame.save(filename, "PNG")) {
        qDebug() << "Picture saved:" << filename;
        QMetaObject::invokeMethod(this, "pictureSaved", Qt::QueuedConnection,
                                  Q_ARG(QString, filename));
    } else {
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "Failed to save picture"));
    }
}

void VideoProvider::startRecording(const QString &savePath)
{
    if (m_recording.load()) {
        qDebug() << "Already recording";
        return;
    }
    
    if (!m_connected.load()) {
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "Camera not connected"));
        return;
    }
    
    QMutexLocker locker(&m_recordingMutex);
    
    // Generate filename with timestamp
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    m_recordingPath = QString("%1/video_%2.mp4").arg(savePath).arg(timestamp);
    
    // Ensure directory exists
    QDir dir;
    dir.mkpath(savePath);
    
    // Allocate output format context
    avformat_alloc_output_context2(&m_outputFormatCtx, nullptr, nullptr, m_recordingPath.toUtf8().constData());
    if (!m_outputFormatCtx) {
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "Failed to create output context"));
        return;
    }
    
    // Find H.264 encoder
    const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        avformat_free_context(m_outputFormatCtx);
        m_outputFormatCtx = nullptr;
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "H.264 encoder not found"));
        return;
    }
    
    // Create output stream
    m_outputStream = avformat_new_stream(m_outputFormatCtx, nullptr);
    if (!m_outputStream) {
        avformat_free_context(m_outputFormatCtx);
        m_outputFormatCtx = nullptr;
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "Failed to create output stream"));
        return;
    }
    
    // Allocate codec context
    m_outputCodecCtx = avcodec_alloc_context3(codec);
    if (!m_outputCodecCtx) {
        avformat_free_context(m_outputFormatCtx);
        m_outputFormatCtx = nullptr;
        m_outputStream = nullptr;
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "Failed to allocate output codec context"));
        return;
    }
    
    // Set codec parameters
    m_outputCodecCtx->codec_id = AV_CODEC_ID_H264;
    m_outputCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    m_outputCodecCtx->width = m_codecCtx->width;
    m_outputCodecCtx->height = m_codecCtx->height;
    m_outputCodecCtx->time_base = (AVRational){1, 30};
    m_outputCodecCtx->framerate = (AVRational){30, 1};
    m_outputCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    m_outputCodecCtx->bit_rate = 2000000;
    
    // Set H.264 options for better compatibility
    av_opt_set(m_outputCodecCtx->priv_data, "preset", "ultrafast", 0);
    av_opt_set(m_outputCodecCtx->priv_data, "tune", "zerolatency", 0);
    
    if (m_outputFormatCtx->oformat->flags & AVFMT_GLOBALHEADER) {
        m_outputCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    
    // Open codec
    if (avcodec_open2(m_outputCodecCtx, codec, nullptr) < 0) {
        avcodec_free_context(&m_outputCodecCtx);
        avformat_free_context(m_outputFormatCtx);
        m_outputFormatCtx = nullptr;
        m_outputStream = nullptr;
        m_outputCodecCtx = nullptr;
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "Failed to open output codec"));
        return;
    }
    
    // Copy codec parameters to stream
    avcodec_parameters_from_context(m_outputStream->codecpar, m_outputCodecCtx);
    m_outputStream->time_base = m_outputCodecCtx->time_base;
    
    // Open output file
    if (!(m_outputFormatCtx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&m_outputFormatCtx->pb, m_recordingPath.toUtf8().constData(), AVIO_FLAG_WRITE) < 0) {
            avcodec_free_context(&m_outputCodecCtx);
            avformat_free_context(m_outputFormatCtx);
            m_outputFormatCtx = nullptr;
            m_outputStream = nullptr;
            m_outputCodecCtx = nullptr;
            QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                      Q_ARG(QString, "Failed to open output file"));
            return;
        }
    }
    
    // Write header
    if (avformat_write_header(m_outputFormatCtx, nullptr) < 0) {
        if (!(m_outputFormatCtx->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&m_outputFormatCtx->pb);
        }
        avcodec_free_context(&m_outputCodecCtx);
        avformat_free_context(m_outputFormatCtx);
        m_outputFormatCtx = nullptr;
        m_outputStream = nullptr;
        m_outputCodecCtx = nullptr;
        QMetaObject::invokeMethod(this, "errorOccurred", Qt::QueuedConnection,
                                  Q_ARG(QString, "Failed to write output header"));
        return;
    }
    
    m_recording = true;
    qDebug() << "Recording started:" << m_recordingPath;
    QMetaObject::invokeMethod(this, "recordingChanged", Qt::QueuedConnection);
}

void VideoProvider::stopRecording()
{
    if (!m_recording.load()) {
        return;
    }
    
    QMutexLocker locker(&m_recordingMutex);
    
    m_recording = false;
    
    if (m_outputFormatCtx) {
        av_write_trailer(m_outputFormatCtx);
        
        if (!(m_outputFormatCtx->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&m_outputFormatCtx->pb);
        }
    }
    
    if (m_outputCodecCtx) {
        avcodec_free_context(&m_outputCodecCtx);
        m_outputCodecCtx = nullptr;
    }
    
    if (m_outputFormatCtx) {
        avformat_free_context(m_outputFormatCtx);
        m_outputFormatCtx = nullptr;
    }
    
    m_outputStream = nullptr;
    
    qDebug() << "Recording stopped:" << m_recordingPath;
    QString savedPath = m_recordingPath;
    m_recordingPath.clear();
    
    QMetaObject::invokeMethod(this, "recordingChanged", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "recordingSaved", Qt::QueuedConnection,
                              Q_ARG(QString, savedPath));
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
    
    stopRecording();
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
    AVFrame *frameYUV = av_frame_alloc();
    AVPacket *packet = av_packet_alloc();

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_codecCtx->width, 
                                             m_codecCtx->height, 1);
    uint8_t *buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(frameRGB->data, frameRGB->linesize, buffer, AV_PIX_FMT_RGB24,
                         m_codecCtx->width, m_codecCtx->height, 1);

    int numBytesYUV = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, m_codecCtx->width,
                                                m_codecCtx->height, 1);
    uint8_t *bufferYUV = (uint8_t *)av_malloc(numBytesYUV * sizeof(uint8_t));
    av_image_fill_arrays(frameYUV->data, frameYUV->linesize, bufferYUV, AV_PIX_FMT_YUV420P,
                         m_codecCtx->width, m_codecCtx->height, 1);

    m_swsCtx = sws_getContext(m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,
                               m_codecCtx->width, m_codecCtx->height, AV_PIX_FMT_RGB24,
                               SWS_BILINEAR, nullptr, nullptr, nullptr);

    SwsContext *swsCtxYUV = sws_getContext(m_codecCtx->width, m_codecCtx->height, m_codecCtx->pix_fmt,
                                            m_codecCtx->width, m_codecCtx->height, AV_PIX_FMT_YUV420P,
                                            SWS_BILINEAR, nullptr, nullptr, nullptr);

    int64_t frameCount = 0;

    while (m_running.load()) {
        if (av_read_frame(m_formatCtx, packet) >= 0) {
            if (packet->stream_index == m_videoStreamIndex) {
                if (avcodec_send_packet(m_codecCtx, packet) == 0) {
                    while (avcodec_receive_frame(m_codecCtx, frame) == 0) {
                        // Convert to RGB for display
                        sws_scale(m_swsCtx, frame->data, frame->linesize, 0, m_codecCtx->height,
                                  frameRGB->data, frameRGB->linesize);

                        QImage img(frameRGB->data[0], m_codecCtx->width, m_codecCtx->height,
                                   frameRGB->linesize[0], QImage::Format_RGB888);

                        {
                            QMutexLocker locker(&m_frameMutex);
                            m_currentFrame = img.copy();
                        }
                        
                        QMetaObject::invokeMethod(this, "frameChanged", Qt::QueuedConnection);

                        // If recording, encode and write frame
                        if (m_recording.load()) {
                            QMutexLocker recLocker(&m_recordingMutex);
                            if (m_outputCodecCtx && m_outputFormatCtx) {
                                // Convert to YUV420P for encoding
                                sws_scale(swsCtxYUV, frame->data, frame->linesize, 0, m_codecCtx->height,
                                          frameYUV->data, frameYUV->linesize);

                                frameYUV->pts = frameCount++;
                                frameYUV->width = m_codecCtx->width;
                                frameYUV->height = m_codecCtx->height;
                                frameYUV->format = AV_PIX_FMT_YUV420P;

                                if (avcodec_send_frame(m_outputCodecCtx, frameYUV) == 0) {
                                    AVPacket *outPacket = av_packet_alloc();
                                    while (avcodec_receive_packet(m_outputCodecCtx, outPacket) == 0) {
                                        av_packet_rescale_ts(outPacket, m_outputCodecCtx->time_base, m_outputStream->time_base);
                                        outPacket->stream_index = m_outputStream->index;
                                        av_interleaved_write_frame(m_outputFormatCtx, outPacket);
                                        av_packet_unref(outPacket);
                                    }
                                    av_packet_free(&outPacket);
                                }
                            }
                        }
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
    sws_freeContext(swsCtxYUV);
    av_free(bufferYUV);
    av_free(buffer);
    av_frame_free(&frameYUV);
    av_frame_free(&frameRGB);
    av_frame_free(&frame);
    av_packet_free(&packet);

    cleanup();
    m_connected = false;
    QMetaObject::invokeMethod(this, "connectionChanged", Qt::QueuedConnection);
}
