#include "camera_image_provider.h"
#include "Backend.h"

CameraImageProvider::CameraImageProvider(Backend *backend)
    : QQuickImageProvider(QQuickImageProvider::Image)
    , m_backend(backend)
{
}

QImage CameraImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);
    Q_UNUSED(requestedSize);
    
    if (!m_backend) {
        return QImage();
    }
    
    QImage frame = m_backend->cameraFrame();
    
    if (size) {
        *size = frame.size();
    }
    
    return frame;
}
