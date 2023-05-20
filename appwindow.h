#ifndef appwindow_H
#define appwindow_H

#include <QWidget>
#include <QMap>
#include "client.h"
#include "server.h"

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
    Client *m_client;
    WifiSettingMap m_wifi_settings;
    bool m_client_connected; // TODO: Add states instead

private slots:
    void connexionButtonPushed();
    void wifiConfigChanged(int);
    void appendLog(QString);
    void clientConnected();
    void clientDisconnected();
    void hidePassword(int);
};
#endif // appwindow_H
