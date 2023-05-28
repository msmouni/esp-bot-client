#ifndef OPTION_H
#define OPTION_H

enum DataType
{
    None = 0,
    Some = 1,
};

template <class T>
struct Option
{
private:
    DataType m_type;
    T m_data;

public:
    // T(): default constructed value for a given type.
    Option() : m_type(DataType::None), m_data(T()) {}

    Option(T data) : m_type(DataType::Some), m_data(data) {}

    bool isSome(void) { return (m_type == DataType::Some); }
    bool isNone(void) { return (m_type == DataType::None); }

    // Note: make sure to check if isSome() before trying to getData()
    T &getData(void) { return m_data; }

    void setData(T data)
    {
        m_data = data;
        m_type = DataType::Some;
    }

    void removeData()
    {
        m_data = {};
        m_type = DataType::None;
    }
};

#endif // OPTION_H
