#ifndef appwindow_H
#define appwindow_H

#include <QWidget>
#include <QMap>
#include <QTime>
#include <QImage>
#include <QGraphicsScene>
#include "client.h"
#include "server.h"
#include "status.h"

// TODO: Move all member which are not related to GUI to another class

QT_BEGIN_NAMESPACE
namespace Ui
{
    class AppWindow;
}
QT_END_NAMESPACE

class AppWindow : public QWidget
{
    Q_OBJECT

public:
    AppWindow(QWidget *parent = nullptr);
    ~AppWindow();

private:
    Ui::AppWindow *ui;
    QGraphicsScene scene;
    Client *m_client;
    WifiSettingMap m_wifi_settings;
    bool m_client_connected;       // TODO: Add states instead
    bool m_client_authentificated; // TODO: Add states instead
    void connexionInit();
    void updateState(Status);

private slots:
    void connexionButtonPushed();
    void wifiConfigChanged(int);
    void appendLog(QString);
    void clientSocketSate(QAbstractSocket::SocketState);
    void clientError();
    void hidePassword(int);
    void authButtonPushed();
    void updateStatus(Status);
    void setImage(QImage);
};
#endif // appwindow_H
