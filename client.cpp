#include "client.h"

Client::Client(QObject *parent)
    : QObject{parent}
{
    m_state_handler.set(ClientState::Disconneted);
    m_tcp_socket = new QTcpSocket(this);
    m_udp_socket = new QUdpSocket(this);
    m_update_timer.setInterval(50); // 50 ms

    qDebug() << "1"
             << "Addr:" << m_tcp_socket->localAddress() << "|Port:" << m_tcp_socket->localPort();

    /*
    try this on m_tcp_socket(at creation/ after connecting...) -> store @ and port to use the same for UDP
    QHostAddress 	localAddress() const
    quint16 	localPort() const
    */

    /*
    // https://doc.qt.io/qt-6/qudpsocket.html
    void Server::initSocket()
    {
        udpSocket = new QUdpSocket(this);
        udpSocket->bind(QHostAddress::LocalHost, 7755);

        connect(udpSocket, &QUdpSocket::readyRead,
                this, &Server::readPendingDatagrams);
    }

    void Server::readPendingDatagrams()
    {
        while (udpSocket->hasPendingDatagrams()) {
            QNetworkDatagram datagram = udpSocket->receiveDatagram();
            processTheDatagram(datagram);
        }
    }
    */

    // m_cam_pic_nb_tracking = 0;

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

void Client::sendFrame(QByteArray frame)
{
    m_tcp_socket->write(frame);
    //    m_tcp_socket->waitForBytesWritten()
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
    case ServerFrameId::CamPic:
    {
        emit setImage(QImage::fromData(frame.getData()));

        /*qDebug()<<"Nb:"<<frame.getNumber();
        if (m_cam_pic_nb_tracking==0){
            m_cam_pic_nb_tracking=frame.getNumber();
        }else {
            m_cam_pic_nb_tracking-=1;
        }

        if (m_cam_pic_nb_tracking ==frame.getNumber()){
            m_cam_pic_buff.append(frame.getData());
            if (frame.getNumber()==0){
                // QImage::loadFromData()
//                m_image= QImage::fromData(m_cam_pic_buff);
                emit setImage(QImage::fromData(m_cam_pic_buff));


                m_cam_pic_buff.clear();
                m_cam_pic_nb_tracking=0;
                qDebug()<<"Image recved";
            }
        }else {
            m_cam_pic_buff.clear();
            m_cam_pic_nb_tracking=0;
        }*/

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
    else if (isAuthentificated())
    {
        m_state_handler.set(ClientState::Connected);
    }
}

void Client::streamDataReceived()
{
    appendLog("Received data"); // TMP

    QByteArray bytes = m_tcp_socket->readAll();

    processFrame(ServerFrame<MAX_MSG_SIZE>(bytes));

    // https://stackoverflow.com/questions/35419786/stop-tcp-packets-from-concatenating

    /*while (!bytes.isEmpty())
    {
        if (bytes.length() < MAX_MSG_SIZE)
        {
            bytes.resize(MAX_MSG_SIZE);
        }
        //        qDebug()<<"4"<<"size"<<bytes.size()<<"length"<< bytes.length()<<"| frame_pos"<<frame_pos <<"slice_size"<<MAX_MSG_SIZE;
        ServerFrame<MAX_MSG_SIZE> frame = ServerFrame<MAX_MSG_SIZE>(bytes.sliced(0, MAX_MSG_SIZE));
        //        frame.debug();
        processFrame(frame);

        bytes.remove(0, MAX_MSG_SIZE);

        //        frame_pos+=MAX_MSG_SIZE;
    }*/

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
        qDebug() << "2"
                 << "Addr:" << m_tcp_socket->localAddress() << "|Port:" << m_tcp_socket->localPort();

        m_udp_socket->bind(m_tcp_socket->localAddress() ,m_tcp_socket->localPort());
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
    appendLog("Socket ERREUR : " + m_tcp_socket->errorString());
}

void Client::readPendingDatagrams()
{
    while (m_udp_socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_udp_socket->receiveDatagram();
//        processTheDatagram(datagram);
        qDebug()<< datagram.data();
        ServerFrame<MAX_MSG_SIZE> frame=ServerFrame<MAX_MSG_SIZE>(datagram.data());
        frame.debug();
        processFrame(frame);
    }


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

    //    qDebug() << login_data;

    //    QByteArray login_data_bytes = login_data.toUtf8();//.constData(); // TO test: The QByteArray returned by the toUtf8() function is a temporary object
    ////    qDebug() << login_data_bytes;
    //    QByteArray auth_frame = AuthFrameData(AuthentificationRequest::LogIn, login_data_bytes).toBytes();

    QByteArray auth_frame = AuthFrameData(AuthentificationRequest::LogIn, login_data.toUtf8()).toBytes();

    //    qDebug() << auth_frame;

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
