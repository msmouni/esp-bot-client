#include "appwindow.h"
#include "./ui_appwindow.h"

AppWindow::AppWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::AppWindow)
{
    m_client = new Client(this);
    m_send_timer= new QTimer(this);

    // Adjust Gui ///////////////////////////////////////////
    ui->setupUi(this);
    ui->picView->setScene(&scene);

    for (auto iter = m_wifi_settings.m_map.begin(); iter != m_wifi_settings.m_map.end(); ++iter)
    {
        ui->wifiConfigComboBox->addItem(iter.value().m_config_name);
    }

    ui->connexionButton->setEnabled(false);
    ui->ipValIn->setEnabled(false);
    ui->portValIn->setEnabled(false);

    QHBoxLayout *picViewLayout = new QHBoxLayout(ui->picView);
    ui->picView->setLayout(picViewLayout);

    // Joystick
    m_joystick = new Joystick(ui->picView);
    m_joystick->setMinimumSize(100, 100);

    QVBoxLayout *joystickLayout = new QVBoxLayout(nullptr);

    joystickLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
    joystickLayout->addWidget(m_joystick);

    picViewLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    picViewLayout->addLayout(joystickLayout);
    picViewLayout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));

    m_joystick->setEnabled(false);

    ui->stackedWidget->setCurrentWidget(ui->connexionPage);

    // Connect signals ///////////////////////////////////////////
    connect(ui->connexionButton, SIGNAL(clicked()), this, SLOT(connexionButtonPushed()));
    connect(ui->wifiConfigComboBox, SIGNAL(activated(int)), this, SLOT(wifiConfigChanged(int)));
    connect(ui->hidePasswordCheckBox, SIGNAL(stateChanged(int)), this, SLOT(hidePassword(int)));
    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(authButtonPushed()));

    connect(m_client, SIGNAL(appendLogSig(QString)), this, SLOT(appendLog(QString)));
    connect(m_client, SIGNAL(socketState(QAbstractSocket::SocketState)), this, SLOT(clientSocketSate(QAbstractSocket::SocketState)));
    connect(m_client, SIGNAL(hasError()), this, SLOT(clientError()));
    connect(m_client, SIGNAL(updateStatus(Status)), this, SLOT(updateStatus(Status)));
    connect(m_client, SIGNAL(setImage(QImage)), this, SLOT(setImage(QImage)));

    connect(m_joystick, SIGNAL(xChanged(float)), this, SLOT(joystickXchanged(float)));
    connect(m_joystick, SIGNAL(yChanged(float)), this, SLOT(joystickYchanged(float)));

    connect(m_send_timer, SIGNAL(timeout()), this, SLOT(sendControlFrame()));

    appendLog("Client Created");
}

AppWindow::~AppWindow()
{
    delete ui;
}

void AppWindow::connexionInit()
{
    ui->connexionButton->setText("Connect");
    ui->connexionButton->setEnabled(true);

    ui->stackedWidget->setCurrentWidget(ui->connexionPage);
}

void AppWindow::updateState(Status status)
{
    if (status.m_client_state == ClientState::AuthAsCLient || status.m_client_state == ClientState::AuthAsSuperCLient)
    {
        ui->loginValIn->setEnabled(false);
        ui->passwordValIn->setEnabled(false);
        ui->loginButton->setText("Log Out");

        // TODO: factorize
        if (status.m_client_state == ClientState::AuthAsSuperCLient)
        {
//            appendLog("Client is taking control");
            m_joystick->setEnabled(true);
            if (!m_send_timer->isActive()){
                        m_send_timer->start(M_SEND_TIMER_PERIOD_MS);
            }
        }else {
            m_joystick->setEnabled(false);
            if (m_send_timer->isActive()){
                m_send_timer->stop();
            }

        }
    }
    else
    {

        ui->loginValIn->setEnabled(true);
        ui->passwordValIn->setEnabled(true);
        ui->hidePasswordCheckBox->setEnabled(true);
        ui->loginButton->setText("Log In");

        m_joystick->setEnabled(false);
        if (m_send_timer->isActive()){
            m_send_timer->stop();
        }
    }
}

void AppWindow::appendLog(QString txt)
{
    ui->logText->append("-[" + QTime::currentTime().toString() + "] " + txt + "\n");
}

void AppWindow::clientSocketSate(QAbstractSocket::SocketState socket_state)
{
    switch (socket_state)
    {
    case QAbstractSocket::UnconnectedState:
    {
        connexionInit();
        break;
    }
    case QAbstractSocket::ConnectingState:
    {
        ui->connexionButton->setEnabled(false);
        break;
    }
    case QAbstractSocket::ConnectedState:
    {
        ui->connexionButton->setText("Disconnect");
        ui->connexionButton->setEnabled(true);

        // TODO: Move to a function
        ui->loginValIn->clear();
        ui->passwordValIn->clear();

        ui->stackedWidget->setCurrentWidget(ui->authPage);
        break;
    }
    default:
    {
        break;
    }
    }
}

void AppWindow::clientError()
{
    connexionInit();
}

void AppWindow::hidePassword(int state)
{
    Qt::CheckState check_state = (Qt::CheckState)state;
    switch (check_state)
    {
    case Qt::CheckState::Checked:
    {
        ui->passwordValIn->setEchoMode(QLineEdit::EchoMode::Password);
        break;
    }
    case Qt::CheckState::Unchecked:
    {
        ui->passwordValIn->setEchoMode(QLineEdit::EchoMode::Normal);
        break;
    }
    default:
    {
        break;
    }
    }
}

void AppWindow::authButtonPushed()
{
    if (m_client != nullptr)
    {
        if (m_client->isAuthentificated())
        {
            m_client->logout();
        }
        else
        {
            m_client->tryLogIn(ui->loginValIn->text(), ui->passwordValIn->text());
        }
    }
}

void AppWindow::updateStatus(Status status)
{
    updateState(status);
}

void AppWindow::setImage(QImage image)
{
    //    qDebug() << "setImage:"<<image.size();

    ui->picView->setImage(image);
}

void AppWindow::joystickXchanged(float x)
{
//    qDebug() << "x" << x;
    m_joystick_x = x;
}

void AppWindow::joystickYchanged(float y)
{
//    qDebug() << "y" << y;
    m_joystick_y = y;
}

void AppWindow::sendControlFrame()
{
    // TODO: SEND joystick (X, Y)
    qDebug() << "(" << m_joystick_x <<", "<<m_joystick_y<<")";

    QByteArray control_frame_data = ControlFrameData(m_joystick_x, m_joystick_y).toBytes();

    QByteArray frame = ServerFrame<Client::MAX_TCP_MSG_SIZE>(ServerFrameId::Control, control_frame_data.size(), control_frame_data).toBytes();

    m_client->sendUdpFrame(frame);
}

void AppWindow::connexionButtonPushed()
{
    if (m_client != nullptr)
    {
        if (!m_client->isConnected())
        {
            m_client->tryToConnect(ui->ipValIn->text(), ui->portValIn->value());
        }
        else
        {
            m_client->disconnect();
        }
    }
}

void AppWindow::wifiConfigChanged(int index)
{
    WifiConfig wifi_config = (WifiConfig)index;

    if ((wifi_config != WifiConfig::AP) && (wifi_config != WifiConfig::STA))
    {
        ui->connexionButton->setEnabled(false);
        ui->ipValIn->setEnabled(false);
        ui->portValIn->setEnabled(false);
    }
    else
    {
        ui->connexionButton->setEnabled(true);
        ui->ipValIn->setEnabled(true);
        ui->portValIn->setEnabled(true);
    }

    WifiSetting wifi_setting = m_wifi_settings.m_map.value(wifi_config);

    ui->ipValIn->setText(wifi_setting.m_ip);
    ui->portValIn->setValue(wifi_setting.m_port);
}
