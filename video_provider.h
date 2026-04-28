#pragma once

#include <QObject>
#include <QImage>
#include <QTimer>
#include <QString>
#include <QMutex>
#include <atomic>
#include <thread>
#include <memory>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

class VideoProvider : public QObject {
    Q_OBJECT
    Q_PROPERTY(QImage currentFrame READ currentFrame NOTIFY frameChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionChanged)

public:
    explicit VideoProvider(QObject *parent = nullptr);
    ~VideoProvider();

    QImage currentFrame() const;
    bool isConnected() const;

public slots:
    void connectToCamera(const QString &rtspUrl);
    void disconnect();

signals:
    void frameChanged();
    void connectionChanged();
    void errorOccurred(const QString &error);

private:
    void videoThread(const std::string &url);
    void cleanup();
    void stopThread();

    QImage m_currentFrame;
    mutable QMutex m_frameMutex;
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_connected{false};
    std::unique_ptr<std::thread> m_thread;
    
    AVFormatContext *m_formatCtx = nullptr;
    AVCodecContext *m_codecCtx = nullptr;
    SwsContext *m_swsCtx = nullptr;
    int m_videoStreamIndex = -1;
};
