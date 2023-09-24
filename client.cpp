#include "client.h"

Client::Client(QObject *parent)
    : QObject{parent}
{
    m_state_handler.set(ClientState::Disconneted);
    m_tcp_socket = new QTcpSocket(this);
    m_udp_socket = new QUdpSocket(this);
    m_update_timer.setInterval(50); // 50 ms

    connect(&m_update_timer, SIGNAL(timeout()), this, SLOT(update()));

    connect(m_tcp_socket, &QTcpSocket::readyRead, this, &Client::streamDataReceived);
    connect(m_tcp_socket, &QTcpSocket::errorOccurred, this, &Client::socketError);
    connect(m_tcp_socket, &QTcpSocket::stateChanged, this, &Client::socketStateChanged);

    connect(m_udp_socket, &QUdpSocket::readyRead, this, &Client::readPendingDatagrams);

    m_update_timer.start();
}

void Client::tryToConnect(QString server_ip, quint16 server_port)
{
    m_tcp_socket->connectToHost(server_ip, server_port);
}

void Client::disconnect()
{
    m_tcp_socket->disconnectFromHost();
}

void Client::sendTcpFrame(QByteArray frame)
{
    m_tcp_socket->write(frame);
    //    m_tcp_socket->waitForBytesWritten()
}

void Client::sendUdpFrame(QByteArray frame)
{
    if (m_state_handler.isConnected())
    {
        m_udp_socket->write(frame);
    }
}

void Client::appendLog(QString txt)
{
    emit appendLogSig("Client: " + txt);
}

template <uint16_t MaxFrameLen>
void Client::processFrame(ServerFrame<MaxFrameLen> frame)
{
    switch (frame.getId())
    {
    case ServerFrameId::Authentification:
    {
        break;
    }
    case ServerFrameId::Status:
    {
        // Store Status using a watchdog
        m_status_data.set(StatusFrameData(frame.getData()));
        break;
    }
    case ServerFrameId::CamPic:
    {
        m_cam_pic_buff.append(frame.getData());

        if (frame.getNum() == 0)
        {
            QImage img = QImage::fromData(m_cam_pic_buff);
            emit setImage(img);

            m_cam_pic_buff.clear();
        }

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

bool Client::isConnected()
{
    return m_state_handler.isConnected();
}

bool Client::isAuthentificated()
{
    return m_state_handler.isAuthentificated();
}

void Client::updateState()
{
    Option<StatusFrameData> opt_status_data = m_status_data.get();

    if (opt_status_data.isSome())
    {
        StatusFrameData status_data = opt_status_data.getData();

        switch (status_data.m_auth_type)
        {
        case AuthentificationType::AsClient:
        {
            m_state_handler.set(ClientState::AuthAsCLient);
            break;
        }
        case AuthentificationType::AsSuperClient:
        {
            m_state_handler.set(ClientState::AuthAsSuperCLient);
            break;
        }
        default:
        {
            m_state_handler.set(ClientState::Connected);
            break;
        }
        }
    }
    else if (isAuthentificated())
    {
        m_state_handler.set(ClientState::Connected);
    }
}

void Client::streamDataReceived()
{

    QByteArray bytes = m_tcp_socket->readAll();

    processFrame(ServerFrame<MAX_TCP_MSG_SIZE>(bytes));
}

void Client::socketStateChanged(QAbstractSocket::SocketState state)
{
    emit socketState(state);

    // Note: All state are listed just to keep track (Some will maybe be removed later)
    switch (state)
    {
    case QAbstractSocket::UnconnectedState:
    {
        m_udp_socket->close();
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
        appendLog("A connection is established: (IP:" + m_tcp_socket->localAddress().toString() + ", Port:" + QString::number(m_tcp_socket->localPort()) + ")");

        m_udp_socket->bind(m_tcp_socket->localAddress(), m_tcp_socket->localPort());

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
    appendLog("Socket ERROR : " + m_tcp_socket->errorString());
}

void Client::readPendingDatagrams()
{
    while (m_udp_socket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = m_udp_socket->receiveDatagram();

        QByteArray bytes = datagram.data();

        ServerFrame<MAX_UDP_MSG_SIZE> frame = ServerFrame<MAX_UDP_MSG_SIZE>(bytes);

        processFrame(frame);
    }
}

void Client::update()
{
    updateState();

    emit updateStatus(Status(m_state_handler.get()));
}

void Client::tryLogIn(QString login, QString password)
{
    QString login_data = "{" + login + ":" + password + "}\0";

    QByteArray auth_frame = AuthFrameData(AuthentificationRequest::LogIn, login_data.toUtf8()).toBytes();

    QByteArray frame = ServerFrame<MAX_TCP_MSG_SIZE>(ServerFrameId::Authentification, auth_frame.size(), auth_frame).toBytes();

    sendTcpFrame(frame);
}

void Client::logout()
{
    QByteArray auth_frame = AuthFrameData(AuthentificationRequest::LogOut, QByteArray()).toBytes();

    QByteArray frame = ServerFrame<MAX_TCP_MSG_SIZE>(ServerFrameId::Authentification, auth_frame.size(), auth_frame).toBytes();

    sendTcpFrame(frame);
}
