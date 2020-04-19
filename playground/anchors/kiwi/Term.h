#pragma once

#include <utility>
#include "Variable.h"

class Term
{
  public:

    Term( const Variable& variable, double coefficient = 1.0 )
        : m_variable( variable )
        , m_coefficient( coefficient )
    {
    }

    // to facilitate efficient map -> vector copies
    Term( const std::pair< const Variable, double >& pair )
        : m_variable( pair.first )
        , m_coefficient( pair.second )
    {
    }

    const Variable& variable() const
    {
        return m_variable;
    }

    double coefficient() const
    {
        return m_coefficient;
    }

    double value() const
    {
        return m_coefficient * m_variable.value();
    }

  private:
    Variable m_variable;
    double m_coefficient;
};

inline Term operator*( const Variable& variable, double coefficient )
{
    return Term( variable, coefficient );
}

inline Term operator/( const Variable& variable, double denominator )
{
    return variable * ( 1.0 / denominator );
}

inline Term operator-( const Variable& variable )
{
    return variable * -1.0;
}

inline Term operator*( const Term& term, double coefficient )
{
    return Term( term.variable(), term.coefficient() * coefficient );
}

inline Term operator/( const Term& term, double denominator )
{
    return term * ( 1.0 / denominator );
}

inline Term operator-( const Term& term )
{
    return term * -1.0;
}

inline Term operator*( double coefficient, const Term& term )
{
    return term * coefficient;
}

inline Term operator*( double coefficient, const Variable& variable )
{
    return variable * coefficient;
}
