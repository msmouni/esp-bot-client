#include "picviewer.h"

void PicViewer::setImage(QImage img)
{
    m_image = img;
    // When scene()->update is called >>> the paintEvent is called
    this->scene()->update();
}

void PicViewer::paintEvent(QPaintEvent *evnt)
{
    QPainter painter(viewport());
    // Draw the image provided to setImage
    painter.drawImage(QRect(0, 0, viewport()->width(), viewport()->height()), m_image, QRect(0, 0, m_image.width(), m_image.height()));
    QGraphicsView::paintEvent(evnt);
}
