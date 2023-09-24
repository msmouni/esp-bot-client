#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

/*class Joystick : public QWidget
{
    Q_OBJECT

public:
    Joystick(QWidget *parent = nullptr);

private:
    QLabel *background;
    QLabel *handle;
//    QImage handle;
    QLabel *txt;

//protected:
//    void paintEvent(QPaintEvent *event) override;

signals:

};*/

//class QPropertyAnimation;
//class QParallelAnimationGroup;

class Joystick : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(float y READ y WRITE setY NOTIFY yChanged)
public:
    explicit Joystick(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

    float x() const;
    float y() const;

signals:
    void xChanged(float value);
    void yChanged(float value);

public slots:
    void setX(float value);
    void setY(float value);

    // Add or remove the knob return animations in x or y- direction.
    // void removeXAnimation();
    // void addXAnimation();

    // void removeYAnimation();
    // void addYAnimation();

    /*  Set the alignment of the quadratic content if the widgets geometry isn quadratic.
     *  Flags can be combined eg. setAlignment(Qt::AlignLeft | Qt::AlignBottom);
     */
    void setAlignment(Qt::Alignment f);

private:
    void resizeEvent(QResizeEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    float m_x;
    float m_y;

    QParallelAnimationGroup *m_returnAnimation;
    QPropertyAnimation *m_xAnimation;
    QPropertyAnimation *m_yAnimation;

    QRectF m_bounds;
    QRectF m_knopBounds;

    QPoint m_lastPos;
    bool knopPressed;

    Qt::Alignment m_alignment;
};

#endif // JOYSTICK_H
