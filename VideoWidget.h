// VideoWidget.h
#pragma once

#include <QWidget>
#include <QImage>

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);

public slots:
    void setFrame(const QImage &img);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage m_frame;
};
