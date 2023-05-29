#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <QTime>
#include <QMutex>
#include "option.h"

template <class T>
class WatchDog
{
private:
    Option<T> m_value;
    // The mutex is locked when QMutexLocker is created. If locked, the mutex will be unlocked when the QMutexLocker is destroyed.
    QMutex m_mutex;
    int m_timeout_ms;
    QTime m_t_set;

public:
    WatchDog(int);
    void set(T);
    Option<T> get();
};

template <class T>
WatchDog<T>::WatchDog(int timeout_ms)
{
    m_value = Option<T>();
    m_timeout_ms = timeout_ms;
    m_t_set = QTime();
}

template <class T>
void WatchDog<T>::set(T value)
{
    QMutexLocker ml(&m_mutex);
    m_value = value;
    m_t_set = QTime::currentTime();
}

template <class T>
Option<T> WatchDog<T>::get()
{
    QMutexLocker ml(&m_mutex);
    if (m_value.isSome() && m_t_set.isValid())
    {
        if (m_t_set.msecsTo(QTime::currentTime()) <= m_timeout_ms)
        {
            return m_value;
        }
        else
        {
            return Option<T>();
        }
    }
    else
    {
        return Option<T>();
    }
}

#endif // WATCHDOG_H
