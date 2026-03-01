#include "VideoWidget.h"
#include <QPainter>

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(320, 240);
}

void VideoWidget::setFrame(const QImage &img)
{
    m_frame = img;
    update();
}

void VideoWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::black);

    if (m_frame.isNull())
        return;

    QImage scaled = m_frame.scaled(size(), Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation);
    QPoint pos((width() - scaled.width()) / 2,
               (height() - scaled.height()) / 2);
    p.drawImage(pos, scaled);
}
