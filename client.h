#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    void tryToConnect(QString server_ip, quint16 server_port);
    void disconnect();

private:
    QTcpSocket *m_socket;
    void appendLog(QString);

private slots:
    void dataReceived();
    void connectedToServer();
    void disconnectedFromServer();
    void socketError(QAbstractSocket::SocketError error);

signals:
    void appendLogSig(QString);
    void connected();
    void disconnected();
};

#endif // CLIENT_H
