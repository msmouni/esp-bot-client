#include "appwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppWindow w;
    /*
    w.setWindowState(Qt::WindowMaximized);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();

    int height = screenGeometry.height();
    int width = screenGeometry.width();

    w.resize(width,height);
    */
    w.show();
    return a.exec();
}
