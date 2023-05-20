#include "appwindow.h"
#include "./ui_appwindow.h"

AppWindow::AppWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::AppWindow)
{
    m_client = new Client(this);

    ui->setupUi(this);

    for (auto iter = m_wifi_settings.m_map.begin(); iter != m_wifi_settings.m_map.end(); ++iter)
    {
        ui->wifiConfigComboBox->addItem(iter.value().m_config_name);
    }

    ui->connexionButton->setEnabled(false);
    ui->ipValIn->setEnabled(false);
    ui->portValIn->setEnabled(false);

    connect(ui->connexionButton, SIGNAL(clicked()), this, SLOT(tryToConnect()));
    connect(ui->wifiConfigComboBox, SIGNAL(activated(int)), this, SLOT(wifiConfigChanged(int)));
    connect(m_client, SIGNAL(appendLogSig(QString)), this, SLOT(appendLog(QString)));

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

void AppWindow::tryToConnect()
{
    if (m_client != nullptr)
    {
        m_client->tryToConnect(ui->ipValIn->text(), ui->portValIn->value());
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
