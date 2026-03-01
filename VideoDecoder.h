#pragma once

#include <QObject>
#include <QImage>
#include <atomic>
#include <thread>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

// Decode RTSP (or any FFmpeg-supported) stream in a worker thread
// and emit QImage frames for display in VideoWidget.
class VideoDecoder : public QObject
{
    Q_OBJECT
public:
    explicit VideoDecoder(QObject *parent = nullptr);
    ~VideoDecoder() override;

    // Start decoding from URL, e.g. "rtsp://192.168.1.2:8554/cam"
    bool start(const QString &url);

    // Stop decoding and join thread
    void stop();

signals:
    void frameReady(const QImage &frame);
    void errorOccurred(const QString &err);

private:
    void workerLoop(QString url);

    std::atomic<bool> m_running;
    std::thread       m_thread;

    // FFmpeg stuff (only used in worker thread)
    AVFormatContext  *m_fmtCtx   = nullptr;
    AVCodecContext   *m_codecCtx = nullptr;
    int               m_streamIdx = -1;
    SwsContext       *m_swsCtx   = nullptr;
};
