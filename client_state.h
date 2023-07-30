#ifndef CLIENT_STATE_H
#define CLIENT_STATE_H

#include <QMutex>

enum class ClientState
{
    Init,
    Connected,
    AuthAsSuperCLient,
    AuthAsCLient,
    Disconneted,
};

struct ClientStateHandler
{
    ClientState m_state = ClientState::Init;
    // The mutex is locked when QMutexLocker is created. If locked, the mutex will be unlocked when the QMutexLocker is destroyed.
    QMutex m_mutex;

    void set(ClientState state)
    {
        QMutexLocker ml(&m_mutex);

        m_state = state;
    };

    ClientState get()
    {
        QMutexLocker ml(&m_mutex);

        return m_state;
    };

    bool isConnected(){
        return (m_state==ClientState::Connected || m_state==ClientState::AuthAsCLient || m_state==ClientState::AuthAsSuperCLient);
    }

    bool isAuthentificated(){
        return (m_state==ClientState::AuthAsCLient || m_state==ClientState::AuthAsSuperCLient);
    }
};

#endif // CLIENT_STATE_H
