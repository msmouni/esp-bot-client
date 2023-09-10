#ifndef PICVIEWER_H
#define PICVIEWER_H

#include <QGraphicsView>

/*Class used for displaying images and videos
 *Extends QGraphicsView >> Provides a widget to display QGraphics scene
 */
class PicViewer : public QGraphicsView
{

public:
    PicViewer(QWidget *centralWidget) : QGraphicsView(centralWidget) {}
    PicViewer(QGraphicsScene *scene, QWidget *parent = 0) : QGraphicsView(scene, parent) {}
    ~PicViewer() {}
    void setImage(QImage img);

protected:
    void paintEvent(QPaintEvent *paintEventInfo);

private:
    QImage m_image;
};

#endif // PICVIEWER_H
