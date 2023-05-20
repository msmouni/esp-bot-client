#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QMap>

enum class WifiConfig : quint8
{
    None = 0,
    AP = 1,
    STA = 2,
    Invalid = 255,
};

struct WifiSetting
{
    QString m_config_name;
    QString m_ip;
    quint16 m_port;

    WifiSetting() : m_config_name(""), m_ip("xxx.xxx.xxx.xxx"), m_port(0){};
    WifiSetting(QString config_name, QString ip, quint16 port) : m_config_name(config_name), m_ip(ip), m_port(port){};
};

struct WifiSettingMap
{
    QMap<WifiConfig, WifiSetting> m_map;

    WifiSettingMap()
    {
        m_map.insert(WifiConfig::None, WifiSetting());
        m_map.insert(WifiConfig::AP, WifiSetting("Access Point", "192.168.1.56", 54321));
        m_map.insert(WifiConfig::STA, WifiSetting("Station", "192.168.1.55", 12345));
    }
};

#endif // SERVER_H
