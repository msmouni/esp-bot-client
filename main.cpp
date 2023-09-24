#include "appwindow.h"

#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AppWindow w;
    w.setWindowTitle("Esp-Bot Client");
    w.show();

    return a.exec();
}

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);
//     QMainWindow mainWindow;

//    Joystick *joystick = new Joystick(&mainWindow);
//    mainWindow.setCentralWidget(joystick);

//    mainWindow.show();
//    return app.exec();
//}
