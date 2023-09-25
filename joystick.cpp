/*#include "joystick.h"
#include <QMouseEvent>
#include <QPainter>

Joystick::Joystick(QWidget *parent)
    : QWidget(parent) {
    // Set up the joystick background
    background = new QLabel(this);
    background->setPixmap(QPixmap("/home/marwan/Projects/ESP32/esp-bot-client/esp-bot-client/images/joystick_background.png"));//"images/joystick_background.png"));

    // Set up the joystick handle
    handle = new QLabel(this);
    handle->setPixmap(QPixmap("/home/marwan/Projects/ESP32/esp-bot-client/esp-bot-client/images/joystick_handle.png"));
    handle->setAlignment(Qt::AlignCenter);

    // Position the handle in the center of the background
    handle->move((background->width() - handle->width()) / 2,
                 (background->height() - handle->height()) / 2);

    // Make the handle movable
    handle->setMouseTracking(true);
    handle->installEventFilter(this);

   txt = new QLabel("Joystick");
}


//void Joystick::paintEvent(QPaintEvent *event) {
//   // Create a QPainter to paint on this widget
//   QPainter painter(this);

//   // Draw the joystick background
//   painter.drawEllipse(rect());

//   // Draw the joystick handle at a specific position (you can calculate this)
//   QPoint handlePosition(50, 50); // Adjust these coordinates
//   painter.drawEllipse(handlePosition, 20, 20); // Adjust the size as needed

//   // You can add more custom drawing as necessary.

////   painter.drawImage(QRect(0, 0, this->width(), this->height()), (QImage *)handle, QRect(0, 0, handle->width(), handle->height()));

//    painter.drawPixmap(handle->pos(), handle->pixmap());
//   // Call the base class implementation to ensure proper behavior
//   QWidget::paintEvent(event);
//}
*/

#include "joystick.h"

#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <math.h>
#include <QDebug>

template <typename T>
T constrain(T Value, T Min, T Max)
{
    return (Value < Min) ? Min : (Value > Max) ? Max
                                               : Value;
}

Joystick::Joystick(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f),
                                                         m_x(0), m_y(0),
                                                         m_returnAnimation(new QParallelAnimationGroup(this)),
                                                         m_xAnimation(new QPropertyAnimation(this, "x")),
                                                         m_yAnimation(new QPropertyAnimation(this, "y")),
                                                         m_alignment(Qt::AlignTop | Qt::AlignLeft)
{
    m_xAnimation->setEndValue(0.f);
    m_xAnimation->setDuration(400);
    m_xAnimation->setEasingCurve(QEasingCurve::OutSine);

    m_yAnimation->setEndValue(0.f);
    m_yAnimation->setDuration(400);
    m_yAnimation->setEasingCurve(QEasingCurve::OutSine);

    m_returnAnimation->addAnimation(m_xAnimation);
    m_returnAnimation->addAnimation(m_yAnimation);
}

/**
 * @brief Joystick::x
 * @return
 */
float Joystick::x() const
{
    return m_x;
}

/**
 * @brief Joystick::y
 * @return
 */
float Joystick::y() const
{
    return m_y;
}

/**
 * @brief Joystick::setX
 * @param value of x axis from -1 to 1
 */
void Joystick::setX(float value)
{
    m_x = constrain(value, -1.f, 1.f);

    qreal radius = (m_bounds.width() - m_knopBounds.width()) / 2;
    m_knopBounds.moveCenter(QPointF(m_bounds.center().x() + m_x * radius, m_knopBounds.center().y()));

    update();
    //    qDebug() << "X:" << m_x;
    emit xChanged(m_x);
}

/**
 * @brief Joystick::setY
 * @param value of y axis from -1 to 1
 */
void Joystick::setY(float value)
{
    m_y = constrain(value, -1.f, 1.f);

    qreal radius = (m_bounds.width() - m_knopBounds.width()) / 2;
    m_knopBounds.moveCenter(QPointF(m_knopBounds.center().x(), m_bounds.center().y() - m_y * radius));

    update();
    emit yChanged(m_y);
}

// void Joystick::removeXAnimation()
// {
//     // return if the animation is already removed
//     if (m_xAnimation->parent() != m_returnAnimation)
//         return;

//     m_returnAnimation->removeAnimation(m_xAnimation);

//     // take ownership of the animation (parent is 0 after removeAnimation())
//     m_xAnimation->setParent(this);
// }

// void Joystick::addXAnimation()
// {
//     // abort if the animation is already added
//     if (m_xAnimation->parent() == m_returnAnimation)
//         return;

//     m_returnAnimation->addAnimation(m_xAnimation);
// }

// void Joystick::removeYAnimation()
// {
//     if (m_yAnimation->parent() != m_returnAnimation)
//         return;

//     m_returnAnimation->removeAnimation(m_yAnimation);
//     m_yAnimation->setParent(this);
// }

// void Joystick::addYAnimation()
// {
//     if (m_yAnimation->parent() == m_returnAnimation)
//         return;

//     m_returnAnimation->addAnimation(m_yAnimation);
// }

void Joystick::setAlignment(Qt::Alignment f)
{
    m_alignment = f;
}

/**
 * @brief Joystick::resizeEvent
 * @param event
 *
 * calculates a square bounding rect for the background and the knob
 */
void Joystick::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    float a = qMin(width(), height());

    QPointF topleft;

    if (m_alignment.testFlag(Qt::AlignTop))
    {
        topleft.setY(0);
    }
    else if (m_alignment.testFlag(Qt::AlignVCenter))
    {
        topleft.setY(((height() - a) / 2));
    }
    else if (m_alignment.testFlag(Qt::AlignBottom))
    {
        topleft.setY(height() - a);
    }

    if (m_alignment.testFlag(Qt::AlignLeft))
    {
        topleft.setX(0);
    }
    else if (m_alignment.testFlag(Qt::AlignHCenter))
    {
        topleft.setX((width() - a) / 2);
    }
    else if (m_alignment.testFlag(Qt::AlignRight))
    {
        topleft.setX(width() - a);
    }

    m_bounds = QRectF(topleft, QSize(a, a));
    qDebug() << "m_bounds:" << m_bounds;

    m_knopBounds.setWidth(a * 0.3);
    m_knopBounds.setHeight(a * 0.3);

    // adjust knob position
    qreal radius = (m_bounds.width() - m_knopBounds.width()) / 2;
    m_knopBounds.moveCenter(QPointF(m_bounds.center().x() + m_x * radius, m_bounds.center().y() - m_y * radius));
}

/**
 * @brief Joystick::paintEvent
 * @param event
 */
void Joystick::paintEvent(QPaintEvent *event)
{
    /*
     * // TO COMPLETE LATER
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    //    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    // Set up the joystick background
    QLabel *background = new QLabel(this);
    QPixmap img_background=QPixmap("/home/marwan/Projects/ESP32/esp-bot-client/esp-bot-client/images/joystick_background.png");
//    img_background.scaled(m_bounds.toRect().width(),m_bounds.toRect().height());
    background->setPixmap(img_background.scaled(m_bounds.toRect().width(),m_bounds.toRect().height()));//QPixmap("/home/marwan/Projects/ESP32/esp-bot-client/esp-bot-client/images/joystick_background.png"));//"images/joystick_background.png"));
    background->setGeometry(m_bounds.toRect());// >setBaseSize()

    qDebug()<<"Size:"<<background->size();


    painter.drawPixmap(background->pos(), background->pixmap());


//    // Set up the joystick handle
    QLabel *handle = new QLabel(this);
    QPixmap img_handle =QPixmap("/home/marwan/Projects/ESP32/esp-bot-client/esp-bot-client/images/joystick_handle.png");
//    img_handle.scaled(m_knopBounds.toRect().width(),m_knopBounds.toRect().height());
    handle->setPixmap(img_handle.scaled(m_knopBounds.toRect().width(),m_knopBounds.toRect().height()));
    handle->setGeometry(m_knopBounds.toRect());
    qDebug()<<"Knob pos:"<<handle->pos()<<"|"<<m_knopBounds<<"|"<<m_knopBounds.toRect();
    painter.drawPixmap(handle->pos(), handle->pixmap());

    // Call the base class implementation to ensure proper behavior
    QWidget::paintEvent(event);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    */
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    //    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    // draw background
    QRadialGradient gradient(m_bounds.center(), m_bounds.width() / 2, m_bounds.center());
    gradient.setFocalRadius(m_bounds.width() * 0.3);
    gradient.setCenterRadius(m_bounds.width() * 0.7);
    gradient.setColorAt(0, QColor(200, 200, 200,150));//Qt::white);
    gradient.setColorAt(1, QColor(127, 127, 127,150));//Qt::lightGray);
    /*QRadialGradient::MidnightBloom*/;

    painter.setPen(QPen(QBrush(QColor(75, 75, 75)), m_bounds.width() * 0.005));
    painter.setBrush(QBrush(gradient));//QGradient(QGradient::SummerGames)));
    painter.drawEllipse(m_bounds);

    // draw crosshair
    painter.setPen(QPen(QBrush(Qt::gray), m_bounds.width() * 0.005));
    painter.drawLine(QPointF(m_bounds.left(), m_bounds.center().y()), QPointF(m_bounds.center().x() - m_bounds.width() * 0.35, m_bounds.center().y()));
    painter.drawLine(QPointF(m_bounds.center().x() + m_bounds.width() * 0.35, m_bounds.center().y()), QPointF(m_bounds.right(), m_bounds.center().y()));
    painter.drawLine(QPointF(m_bounds.center().x(), m_bounds.top()), QPointF(m_bounds.center().x(), m_bounds.center().y() - m_bounds.width() * 0.35));
    painter.drawLine(QPointF(m_bounds.center().x(), m_bounds.center().y() + m_bounds.width() * 0.35), QPointF(m_bounds.center().x(), m_bounds.bottom()));

    // draw knob
    if (!this->isEnabled())
        return;

    gradient = QRadialGradient(m_knopBounds.center(), m_knopBounds.width() / 2, m_knopBounds.center());
    gradient.setColorAt(0, QColor(90, 90, 90,200));//Qt::gray);
    gradient.setColorAt(1, QColor(50, 50, 50,200));//Qt::darkGray);
    gradient.setFocalRadius(m_knopBounds.width() * 0.2);
    gradient.setCenterRadius(m_knopBounds.width() * 0.5);

    painter.setPen(QPen(QBrush(Qt::darkGray), m_bounds.width() * 0.005));
    painter.setBrush(QBrush(gradient));
    painter.drawEllipse(m_knopBounds);
}

/**
 * @brief Joystick::mousePressEvent
 * @param event
 */
void Joystick::mousePressEvent(QMouseEvent *event)
{
    if (m_knopBounds.contains(event->pos()))
    {
        m_returnAnimation->stop();
        m_lastPos = event->pos();
        knopPressed = true;
    }
}

/**
 * @brief Joystick::mouseReleaseEvent
 * @param event
 */
void Joystick::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    knopPressed = false;
    m_returnAnimation->start();
}

/**
 * @brief Joystick::mouseMoveEvent
 * @param event
 */
void Joystick::mouseMoveEvent(QMouseEvent *event)
{
    if (!knopPressed)
        return;

    // moved distance
    QPointF dPos = event->pos() - m_lastPos;

    // change the distance sligthly to guarantee overlaping knop and pointer
    dPos += 0.05 * (event->pos() - m_knopBounds.center());

    QPointF fromCenterToKnop = m_knopBounds.center() + dPos - m_bounds.center();

    qreal radius = (m_bounds.width() - m_knopBounds.width()) / 2;

    fromCenterToKnop.setX(constrain(fromCenterToKnop.x(), -radius, radius));
    fromCenterToKnop.setY(constrain(fromCenterToKnop.y(), -radius, radius));

    m_knopBounds.moveCenter(fromCenterToKnop + m_bounds.center());
    m_lastPos = event->pos();

    update();

    if (radius == 0)
        return;
    float x = (m_knopBounds.center().x() - m_bounds.center().x()) / radius;
    float y = (-m_knopBounds.center().y() + m_bounds.center().y()) / radius;

    if (m_x != x)
    {
        //        qDebug() << "check: " << x;
        m_x = x;
        emit xChanged(m_x);
    }

    if (m_y != y)
    {
        m_y = y;
        emit yChanged(m_y);
    }
}
