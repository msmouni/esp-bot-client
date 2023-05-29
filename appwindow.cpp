#include "appwindow.h"
#include "./ui_appwindow.h"

AppWindow::AppWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::AppWindow)
{
    m_client = new Client(this);
    m_client_connected = false;

    ui->setupUi(this);

    for (auto iter = m_wifi_settings.m_map.begin(); iter != m_wifi_settings.m_map.end(); ++iter)
    {
        ui->wifiConfigComboBox->addItem(iter.value().m_config_name);
    }

    ui->connexionButton->setEnabled(false);
    ui->ipValIn->setEnabled(false);
    ui->portValIn->setEnabled(false);

    ui->stackedWidget->setCurrentWidget(ui->connexionPage);

    connect(ui->connexionButton, SIGNAL(clicked()), this, SLOT(connexionButtonPushed()));
    connect(ui->wifiConfigComboBox, SIGNAL(activated(int)), this, SLOT(wifiConfigChanged(int)));
    connect(ui->hidePasswordCheckBox, SIGNAL(stateChanged(int)), this, SLOT(hidePassword(int)));
    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(authButtonPushed()));

    connect(m_client, SIGNAL(appendLogSig(QString)), this, SLOT(appendLog(QString)));
    connect(m_client, SIGNAL(socketState(QAbstractSocket::SocketState)), this, SLOT(clientSocketSate(QAbstractSocket::SocketState)));
    connect(m_client, SIGNAL(hasError()), this, SLOT(clientError()));
    connect(m_client, SIGNAL(updateStatus(Status)), this, SLOT(updateStatus(Status)));

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
    m_client_connected = false;

    ui->stackedWidget->setCurrentWidget(ui->connexionPage);
}

void AppWindow::updateState(Status status)
{
    if (status.m_client_state == ClientState::AuthAsCLient || status.m_client_state == ClientState::AuthAsSuperCLient)
    {
        m_client_authentificated = true;

        ui->loginValIn->setEnabled(false);
        ui->passwordValIn->setEnabled(false);
        ui->hidePasswordCheckBox->setEnabled(false);
        ui->loginButton->setText("Log Out");
    }
    else
    {
        m_client_authentificated = false;

        ui->loginValIn->setEnabled(true);
        ui->passwordValIn->setEnabled(true);
        ui->hidePasswordCheckBox->setEnabled(true);
        ui->loginButton->setText("Log In");
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
        m_client_connected = true;

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

    //    ui->hidePasswordCheckBox->stateChanged()
}

void AppWindow::authButtonPushed()
{
    if (m_client != nullptr)
    {
        if (m_client_authentificated)
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

void AppWindow::connexionButtonPushed()
{
    if (m_client != nullptr)
    {
        if (!m_client_connected)
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
