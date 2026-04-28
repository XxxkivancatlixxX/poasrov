#pragma once

#include <QQuickImageProvider>
#include <QImage>

class Backend;

class CameraImageProvider : public QQuickImageProvider {
public:
    explicit CameraImageProvider(Backend *backend);
    
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    Backend *m_backend;
};
