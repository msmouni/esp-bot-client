#ifndef FRAME_H
#define FRAME_H

#include <QObject>
#include <algorithm>
#include <QDebug>

// From ESP32-ROBOT project: To ADJUST LATER

// ServerFrame as uint16_t frame[Length]: uint8_t ID, uint8_t FRAME_LENGTH, uint8_t FRAME_NB, uint8_t DATA[DATA_LEN]]
// Frame definition
const uint8_t FRAME_ID_OFFSET = 0;
const uint8_t FRAME_LEN_OFFSET = 1;
const uint8_t FRAME_HEADER_LEN = 3;
const uint8_t FRAME_DATA_OFFSET = FRAME_HEADER_LEN;

enum class ServerFrameId : uint8_t
{
    NotDefined = 0x00,
    Authentification = 0x01,
    Status = 0x02,
    CamPic = 0x03,
    Debug = 0xFF,
    Unknown = UINT8_MAX,
};

// ServerFrame as uint8_t frame[Length]: [uint8_t ID[4], uint8_t FRAME_LENGTH, uint8_t DATA[DATA_LEN]]
template <uint8_t MaxFrameLen>
class ServerFrame
{
private:
    ServerFrameId m_id;
    uint8_t m_len;
    uint8_t m_num;
    QByteArray m_data;

public:
    ServerFrame(){};
    ServerFrame(ServerFrameId id, uint8_t len, QByteArray data) : m_id(id), m_len(len), m_data(data){};
    ~ServerFrame(){};

    void fromBytes(QByteArray bytes_frame)
    {
        //        qDebug()<<"size"<<bytes_frame.size()<<"bytes"<<bytes_frame;
        // Endianness ...
        m_id = static_cast<ServerFrameId>(bytes_frame[0]);

        uint8_t len = bytes_frame[1];
        m_len = std::min(len, MaxFrameLen);

        m_num=bytes_frame[2];

        m_data = bytes_frame.sliced(FRAME_DATA_OFFSET, m_len);
    }

    ServerFrame(QByteArray bytes_frame)
    {
        fromBytes(bytes_frame);
    };

    QByteArray toBytes()
    {
        QByteArray buffer;

        buffer.fill(0, MaxFrameLen);
        //        buffer.resize(MaxFrameLen);

//        uint32_t id_uint32 = static_cast<uint32_t>(m_id);
        // Endianness ...
        buffer[0] = static_cast<uint8_t>(m_id);
//        buffer[1] = static_cast<uint8_t>(id_uint32 >> 16);
//        buffer[2] = static_cast<uint8_t>(id_uint32 >> 8);
//        buffer[3] = static_cast<uint8_t>(id_uint32);

        buffer[1] = static_cast<uint8_t>(m_len);
        buffer[2] = static_cast<uint8_t>(m_num);

        buffer.remove(FRAME_DATA_OFFSET, m_data.length());
        buffer.insert(FRAME_DATA_OFFSET, m_data);

        return buffer;
    }

    void debug()
    {
        qDebug("ID: %ld\nLEN: %d\nData: [", static_cast<uint32_t>(m_id), m_len);

        for (uint8_t i = 0; i < m_len; i++)
        {
            if (i == m_len - 1)
            {
                qDebug("%d", m_data[i]);
            }
            else
            {
                qDebug("%d ,", m_data[i]);
            }
        }

        qDebug("]\n");
    }

    ServerFrameId &getId()
    {
        return m_id;
    }

    uint8_t getLen()
    {
        return m_len;
    }

    uint8_t getNum()
    {
        return m_num;
    }

    QByteArray getData()
    {
        return m_data;
    }
};

//////////////////////////////////////////////////////////////////////
// TODO: Move later to a shared submodule or use xml file parsing
enum class AuthentificationRequest : uint8_t
{
    LogIn = 1,
    LogOut = 2,
};

struct AuthFrameData
{
    static const uint8_t MAX_LOGIN_PASS_LEN = 126;
    AuthentificationRequest m_auth_type;
    char m_login_password[MAX_LOGIN_PASS_LEN];

    AuthFrameData(QByteArray bytes)
    {
        m_auth_type = static_cast<AuthentificationRequest>(bytes[0]);

        // Note: https://stackoverflow.com/questions/26456813/will-a-char-array-differ-in-ordering-in-a-little-endian-or-big-endian-system
        // Copy
        uint8_t login_pass_len = std::min(bytes.size() + 1,
                                          (qsizetype)MAX_LOGIN_PASS_LEN); // +1 for '\0'
        memcpy(m_login_password, bytes.sliced(1, login_pass_len), login_pass_len);
    }

    AuthFrameData(AuthentificationRequest auth_type, QByteArray login_password)
    {
        m_auth_type = auth_type;

        // Copy
        uint8_t login_pass_len = std::min(login_password.size() + 1,
                                          (qsizetype)MAX_LOGIN_PASS_LEN); // +1 for '\0'
        memcpy(m_login_password, login_password, login_pass_len);         // MAX_LOGIN_PASS_LEN);//
    }

    QByteArray toBytes()
    {
        QByteArray bytes;

        bytes.clear();
        bytes.fill(0, MAX_LOGIN_PASS_LEN + 1);

        bytes[0] = (uint8_t)m_auth_type;

        bytes.remove(1, MAX_LOGIN_PASS_LEN);
        bytes.insert(1, m_login_password);

        return bytes;
    }
};

///////////////////////////////:

enum class AuthentificationType : uint8_t
{
    Undefined,
    AsClient,
    AsSuperClient,
};

// To complete later
struct StatusFrameData
{
    AuthentificationType m_auth_type;

    StatusFrameData()
    {
        m_auth_type = AuthentificationType::Undefined;
    }

    StatusFrameData(QByteArray bytes)
    {
        m_auth_type = static_cast<AuthentificationType>(bytes[0]);

        // bytes++;
    }

    StatusFrameData(AuthentificationType auth_type)
    {
        m_auth_type = auth_type;
    }

    QByteArray toBytes()
    {
        QByteArray bytes;
        bytes.append((uint8_t)m_auth_type);

        return bytes;
    }
};

#endif // FRAME_H
