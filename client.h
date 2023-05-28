#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork>
#include "frame.h"
#include "watchdog.h"

enum class ClientState
{
    Init,
    Connected,
    AuthAsSuperCLient,
    AuthAsCLient,
    Disconneted,
};

const uint32_t SERVER_STATUS_TIMEOUT_MS = 250; // 2.5 * STATUS_FRAME_PERIOD

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);

    static const int MAX_MSG_SIZE = 128; // To adjust later reg Msgs to send

    void tryToConnect(QString server_ip, quint16 server_port);
    void disconnect();
    void sendFrame(QByteArray);
    void sendAuthenfification(QString, QString);

private:
    ClientState m_state;
    QTcpSocket *m_socket;
    WatchDog<StatusFrameData> m_status_data = WatchDog<StatusFrameData>(SERVER_STATUS_TIMEOUT_MS);
    QTimer m_update_timer;
    void appendLog(QString);
    void processFrame(ServerFrame<MAX_MSG_SIZE>);

private slots:
    void dataReceived();
    void socketStateChanged(QAbstractSocket::SocketState);
    void socketError(QAbstractSocket::SocketError);
    void update();

signals:
    void appendLogSig(QString);
    void socketState(QAbstractSocket::SocketState);
    void hasError();
};

#endif // CLIENT_H
