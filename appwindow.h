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

private slots:
    void tryToConnect();
    void wifiConfigChanged(int);
    void appendLog(QString);
};
#endif // appwindow_H
