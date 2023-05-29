#include "client.h"

Client::Client(QObject *parent)
    : QObject{parent}
{
    m_state_handler.set(ClientState::Disconneted);
    m_socket = new QTcpSocket(this);
    m_update_timer.setInterval(100); // 100 ms

    connect(&m_update_timer, SIGNAL(timeout()), this, SLOT(update()));

    connect(m_socket, SIGNAL(connected()), this, SLOT(connectedToServer()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(disconnectedFromServer()));
    connect(m_socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));

    m_update_timer.start();
}

void Client::tryToConnect(QString server_ip, quint16 server_port)
{
    m_socket->connectToHost(server_ip, server_port);
}

void Client::disconnect()
{
    m_socket->disconnectFromHost();
}

void Client::sendFrame(QByteArray frame)
{
    m_socket->write(frame);
    //    m_socket->waitForBytesWritten()
}

void Client::appendLog(QString txt)
{
    emit appendLogSig("Client: " + txt);
}

void Client::processFrame(ServerFrame<MAX_MSG_SIZE> frame)
{
    /*
    Authentification = 0x01,
    Status = 0x02,
    Debug = 0xFF,
     */
    switch (frame.getId())
    {
    case ServerFrameId::Authentification:
    {

        //        AuthFrameData auth_frame = AuthFrameData(frame.getData());
        break;
    }
    case ServerFrameId::Status:
    {
        // Store Status using a watchdog
        m_status_data.set(StatusFrameData(frame.getData()));
        break;
    }
    case ServerFrameId::Debug:
    {

        break;
    }
    default:
    {
        break;
    }
    }
}

void Client::updateState()
{
    Option<StatusFrameData> opt_status_data = m_status_data.get();

    if (opt_status_data.isSome())
    {
        StatusFrameData status_data = opt_status_data.getData();

        //        uint8_t auth_type=(uint8_t) status_data.m_auth_type;
        //        qDebug()<< auth_type;

        switch (status_data.m_auth_type)
        {
        case AuthentificationType::AsClient:
        {
            m_state_handler.set(ClientState::AuthAsSuperCLient);
            break;
        }
        case AuthentificationType::AsSuperClient:
        {
            m_state_handler.set(ClientState::AuthAsCLient);
            break;
        }
        default:
        {
            m_state_handler.set(ClientState::Connected);
            break;
        }
        }
    }
    else
    {
        m_state_handler.set(ClientState::Connected);
    }
}

void Client::dataReceived()
{
    appendLog("Received data"); // TMP

    QByteArray bytes = m_socket->readAll();

    processFrame(ServerFrame<MAX_MSG_SIZE>(bytes));

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

void Client::socketStateChanged(QAbstractSocket::SocketState state)
{
    emit socketState(state);

    // Note: All state are listed just to keep track (Some will maybe be removed later)
    switch (state)
    {
    case QAbstractSocket::UnconnectedState:
    {
        appendLog("The socket is not connected.");
        m_state_handler.set(ClientState::Disconneted);
        break;
    }
    case QAbstractSocket::HostLookupState:
    {
        appendLog("The socket is performing a host name lookup.");
        break;
    }
    case QAbstractSocket::ConnectingState:
    {
        appendLog("The socket has started establishing a connection.");
        break;
    }
    case QAbstractSocket::ConnectedState:
    {
        appendLog("A connection is established.");
        m_state_handler.set(ClientState::Connected);
        break;
    }
    case QAbstractSocket::BoundState:
    {
        appendLog("The socket is bound to an address and port.");
        break;
    }
    case QAbstractSocket::ClosingState:
    {
        appendLog("The socket is about to close (data may still be waiting to be written).");
        m_state_handler.set(ClientState::Disconneted);
        break;
    }
    case QAbstractSocket::ListeningState:
    {
        appendLog("Socket in ListeningState");
        break;
    }
    default:
    {
        break;
    }
    }
}

void Client::socketError(QAbstractSocket::SocketError error)
{
    emit hasError();
    appendLog("Socket ERREUR : " + m_socket->errorString());
}

void Client::update()
{
    //    qDebug()<<"update";

    updateState();

    emit updateStatus(Status(m_state_handler.get()));
}

void Client::tryLogIn(QString login, QString password)
{
    QString login_data = "{" + login + ":" + password + "}\0";

    QByteArray auth_frame = AuthFrameData(AuthentificationRequest::LogIn, login_data.toUtf8()).toBytes();

    QByteArray frame = ServerFrame<MAX_MSG_SIZE>(ServerFrameId::Authentification, auth_frame.size(), auth_frame).toBytes();

    //    // TMP
    //    qDebug() << frame;

    sendFrame(frame);
}

void Client::logout()
{
    QByteArray auth_frame = AuthFrameData(AuthentificationRequest::LogOut, QByteArray()).toBytes();

    QByteArray frame = ServerFrame<MAX_MSG_SIZE>(ServerFrameId::Authentification, auth_frame.size(), auth_frame).toBytes();

    //    // TMP
    //    qDebug() << frame;

    sendFrame(frame);
}
