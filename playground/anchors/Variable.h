#pragma once

#include <memory>

class Variable
{
  public:
    Variable( double value = 0.0 )
        : m_value( std::make_shared< double >(value) )
    {
    }

    Variable( const Variable& v )
        : m_value( v.m_value )
    {
    }

    Variable& operator=( const Variable& v )
    {
        m_value = v.m_value;
        return *this;
    }

    double value() const
    {
        return m_value ? *m_value : 0.0;
    }

    void setValue(double x)
    {
        *m_value = x;
    }

    bool equals( const Variable& other )
    {
        return m_value == other.m_value;
    }

  private:
    std::shared_ptr< double > m_value;

    friend bool operator<( const Variable& lhs, const Variable& rhs )
    {
        return lhs.m_value < rhs.m_value;
    }
};
