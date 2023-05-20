#include "client.h"

Client::Client(QObject *parent)
    : QObject{parent}
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, SIGNAL(connected()), this, SLOT(connectedToServer()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnectedFromServer()));
    connect(m_socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}

void Client::tryToConnect(QString server_ip, quint16 server_port)
{
    m_socket->connectToHost(server_ip, server_port);
}

void Client::appendLog(QString txt)
{
    emit(appendLogSig("\nClient: " + txt));
}

void Client::dataReceived()
{
    appendLog("Received data"); // TMP
    /*
    QDataStream in(socket);

    // For more informations refer to the sending method in the server : "send_to_all"
    // We first collect the packet size
    if (Data_size == 0)
    {
        if (socket->bytesAvailable() < (int)sizeof(quint32))
             return;

        in >> Data_size;
    }

    if (socket->bytesAvailable() < Data_size)
        return;


    // Then we retrieve the sent packet, in this case the image
    QImage imgRecu;
    in >> imgRecu;


    // Set image on the GUI
    ui->camViewer->setImage(imgRecu);


    // The message size is reset to 0 in order to be able to receive incoming packets
    Data_size = 0;
     */
}

void Client::connectedToServer()
{
    appendLog("Connected to server");
}

void Client::disconnectedFromServer()
{
    appendLog("Disconnected from server");
}

void Client::socketError(QAbstractSocket::SocketError error)
{
    appendLog("Socket ERREUR : " + m_socket->errorString());
}
