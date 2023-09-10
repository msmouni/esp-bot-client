#ifndef STATUS_H
#define STATUS_H

#include "client_state.h"

struct Status
{
    ClientState m_client_state;

    Status(ClientState client_state) : m_client_state(client_state){};
};

#endif // STATUS_H
