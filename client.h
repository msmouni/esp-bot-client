#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork>
#include "frame.h"
#include "watchdog.h"
#include "client_state.h"
#include "status.h"
#include <QImage>

const uint32_t SERVER_STATUS_TIMEOUT_MS = 250; // 2.5 * STATUS_FRAME_PERIOD

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);

    static const uint8_t MAX_MSG_SIZE = 128; // 1024; //  4096; // TMP // 128; // 8192;//128; // To adjust later reg Msgs to send

    void tryToConnect(QString server_ip, quint16 server_port);
    void disconnect();
    void sendFrame(QByteArray);
    void tryLogIn(QString, QString);
    void logout();

private:
    ClientStateHandler m_state_handler;
    QTcpSocket *m_tcp_socket;
    QUdpSocket *m_udp_socket;
    WatchDog<StatusFrameData> m_status_data = WatchDog<StatusFrameData>(SERVER_STATUS_TIMEOUT_MS);
    QTimer m_update_timer;
    QByteArray m_cam_pic_buff;
    //    uint8_t m_cam_pic_nb_tracking;
    QImage m_image; // TMP
    void appendLog(QString);
    void updateState();
    bool isConnected();
    bool isAuthentificated();
    void processFrame(ServerFrame<MAX_MSG_SIZE>);

private slots:
    void streamDataReceived();
    void socketStateChanged(QAbstractSocket::SocketState);
    void socketError(QAbstractSocket::SocketError);
    void readPendingDatagrams();
    void update();

signals:
    void appendLogSig(QString);
    void socketState(QAbstractSocket::SocketState);
    void hasError();
    void updateStatus(Status);
    void setImage(QImage);
};

#endif // CLIENT_H
