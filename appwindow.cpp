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

    connect(m_client, SIGNAL(appendLogSig(QString)), this, SLOT(appendLog(QString)));
    connect(m_client, SIGNAL(connected()), this, SLOT(clientConnected()));
    connect(m_client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));

    appendLog("Client Created");
}

AppWindow::~AppWindow()
{
    delete ui;
}

void AppWindow::appendLog(QString txt)
{
    ui->logText->append(txt);
}

void AppWindow::clientConnected()
{
    ui->connexionButton->setText("Disconnect");
    m_client_connected = true;

    // TODO: Move to a function
    ui->loginValIn->clear();
    ui->passwordValIn->clear();

    ui->stackedWidget->setCurrentWidget(ui->authPage);
}

void AppWindow::clientDisconnected()
{
    ui->connexionButton->setText("Connect");
    m_client_connected = false;

    ui->stackedWidget->setCurrentWidget(ui->connexionPage);
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
