#pragma once

#include <string>

class SensorEvent
{
public:

    SensorEvent() = default;
    SensorEvent(const SensorEvent& event) = default;
    ~SensorEvent() = default;
    SensorEvent& operator=(const SensorEvent&) = default;


    SensorEvent(const char* type, float x, float y, float z = 0.0)
        : m_type(type),
          m_x(x),
          m_y(y),
          m_z(z)
    {
    }

    std::string type() const
    {
        return m_type;
    }

    float x() const
    {
        return m_x;
    }

    float y() const
    {
        return m_x;
    }

    float z() const
    {
        return m_x;
    }

    bool operator<(const SensorEvent& other)
    {
        return std::make_tuple(m_type, m_x, m_y, m_z) < std::make_tuple(other.type(), other.x(), other.y(), other.z());
    }

    private:
        const char* m_type;
        float m_x;
        float m_y;
        float m_z;

};


