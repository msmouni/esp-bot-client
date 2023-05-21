#ifndef FRAME_H
#define FRAME_H

#include <QObject>
#include <algorithm>

// From ESP32-ROBOT project: To ADJUST LATER

// To Maybe adjust later : 32bits ...
enum class ServerFrameId : uint32_t
{
    Authentification = 0x01,
    Status = 0x02,
    Debug = 0xFF,
};

// ServerFrame as uint8_t frame[Length]: [uint8_t ID[4], uint8_t FRAME_LENGTH, uint8_t DATA[DATA_LEN]]
template <uint8_t MaxFrameLen>
class ServerFrame
{
private:
    ServerFrameId m_id;
    uint8_t m_len;
    QByteArray m_data;

public:
    ServerFrame(){};
    ServerFrame(ServerFrameId id, uint8_t len, QByteArray data) : m_id(id), m_len(len), m_data(data){};
    ~ServerFrame(){};

    void fromBytes(QByteArray bytes_frame)
    {
        // Endianness ...
        m_id = static_cast<ServerFrameId>((uint32_t(bytes_frame[0]) << 24) | (uint32_t(bytes_frame[1]) << 16) | (uint32_t(bytes_frame[2]) << 8) | (uint32_t(bytes_frame[3])));

        m_len = std::min((uint8_t) bytes_frame[4], MaxFrameLen);

        m_data = bytes_frame.sliced(5, m_len);
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

        uint32_t id_uint32 = static_cast<uint32_t>(m_id);
        // Endianness ...
        buffer[0] = static_cast<uint8_t>(id_uint32 >> 24);
        buffer[1] = static_cast<uint8_t>(id_uint32 >> 16);
        buffer[2] = static_cast<uint8_t>(id_uint32 >> 8);
        buffer[3] = static_cast<uint8_t>(id_uint32);

        buffer[4] = m_len;

        buffer.remove(5, m_data.length());
        buffer.insert(5, m_data);

        return buffer;
    }

    void debug()
    {
        printf("ID: %ld\nLEN: %d\nData: [", static_cast<uint32_t>(m_id), m_len);

        for (uint8_t i = 0; i < m_len; i++)
        {
            if (i == m_len - 1)
            {
                printf("%d", m_data[i]);
            }
            else
            {
                printf("%d ,", m_data[i]);
            }
        }

        printf("]\n");
    }

    ServerFrameId &getId()
    {
        return m_id;
    }

    uint8_t getLen()
    {
        return m_len;
    }

    QByteArray getData()
    {
        return m_data;
    }
};

#endif // FRAME_H
