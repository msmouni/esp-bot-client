#include "picviewer.h"


void PicViewer::setImage(QImage img)
{
    image = img;
    //When scene()->update is called >>> the paintEvent is called
    this->scene()->update();

}

void PicViewer::paintEvent(QPaintEvent* evnt)
{
    QPainter painter(viewport());
    //Draw the image provided to setImage
    painter.drawImage(QRect(0,0,viewport()->width(),viewport()->height()), image, QRect(0, 0, image.width(), image.height()));
    QGraphicsView::paintEvent(evnt);
}
