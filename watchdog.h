#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <QTime>
#include "option.h"

template <class T>
class WatchDog
{
private:
    Option<T> m_value;
    int m_timeout_ms;
    QTime m_t_set;
public:
    WatchDog(int);
    void set(T);
    Option<T> get();
};

template<class T>
WatchDog<T>::WatchDog(int timeout_ms)
{
    m_value=Option<T>();
    m_timeout_ms=timeout_ms;
    m_t_set=QTime();
}

template<class T>
void WatchDog<T>::set(T value)
{
    m_value=value;
    m_t_set=QTime::currentTime();
}

template<class T>
Option<T> WatchDog<T>::get()
{
    if (m_value.isSome() && m_t_set.isValid()){
        if (QTime::currentTime().msecsTo(m_t_set)<=m_timeout_ms){
            return m_value;
        }else {
            return Option<T>();
        }

    }else {
        return Option<T>();
    }

}


#endif // WATCHDOG_H
