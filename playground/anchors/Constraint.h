#pragma once

#include "Strength.h"
#include <memory>

class Expression;
class Variable;
class Term;

enum RelationalOperator { OP_LE, OP_GE, OP_EQ };

class Constraint
{
  public:

    Constraint() = default;

    Constraint( const Expression&, RelationalOperator,
        double strength = Strength::required );

    Constraint( const Constraint&, double strength );

    ~Constraint();

    const Expression& expression() const;
    RelationalOperator oper() const;
    double strength() const;

    bool operator!() const { return !m_data; }

  private:
    class Data;
    std::shared_ptr< Data > m_data;

    friend bool operator<( const Constraint&, const Constraint& );
    friend bool operator==( const Constraint&, const Constraint& );
    friend bool operator!=( const Constraint&, const Constraint& );
};

inline bool operator<( const Constraint& lhs, const Constraint& rhs )
{
    return lhs.m_data < rhs.m_data;
}

inline bool operator==( const Constraint& lhs, const Constraint& rhs )
{
    return lhs.m_data == rhs.m_data;
}

inline bool operator!=( const Constraint& lhs, const Constraint& rhs )
{
    return lhs.m_data != rhs.m_data;
}

inline Constraint operator|( const Constraint& constraint, double strength )
{
    return Constraint( constraint, strength );
}

inline Constraint operator|( double strength, const Constraint& constraint )
{
    return constraint | strength;
}

extern Constraint operator==( const Expression&, const Expression& );
extern Constraint operator<=( const Expression&, const Expression& );
extern Constraint operator>=( const Expression&, const Expression& );

extern Constraint operator==( const Expression&, const Term& );
extern Constraint operator<=( const Expression&, const Term& );
extern Constraint operator>=( const Expression&, const Term& );
extern Constraint operator==( const Term&, const Expression& );
extern Constraint operator<=( const Term&, const Expression& );
extern Constraint operator>=( const Term&, const Expression& );

extern Constraint operator==( const Expression&, const Variable& );
extern Constraint operator<=( const Expression&, const Variable& );
extern Constraint operator>=( const Expression&, const Variable& );
extern Constraint operator==( const Variable&, const Expression& );
extern Constraint operator<=( const Variable&, const Expression& );
extern Constraint operator>=( const Variable&, const Expression& );

extern Constraint operator==( const Expression&, double );
extern Constraint operator<=( const Expression&, double );
extern Constraint operator>=( const Expression&, double );
extern Constraint operator==( double, const Expression& );
extern Constraint operator<=( double, const Expression& );
extern Constraint operator>=( double, const Expression& );

extern Constraint operator==( const Term&, const Term& );
extern Constraint operator<=( const Term&, const Term& );
extern Constraint operator>=( const Term&, const Term& );

extern Constraint operator==( const Term&, const Variable& );
extern Constraint operator<=( const Term&, const Variable& );
extern Constraint operator>=( const Term&, const Variable& );
extern Constraint operator==( const Variable&, const Term& );
extern Constraint operator<=( const Variable&, const Term& );
extern Constraint operator>=( const Variable&, const Term& );

extern Constraint operator==( const Term&, double );
extern Constraint operator<=( const Term&, double );
extern Constraint operator>=( const Term&, double );
extern Constraint operator==( double, const Term& );
extern Constraint operator<=( double, const Term& );
extern Constraint operator>=( double, const Term& );

extern Constraint operator==( const Variable&, const Variable& );
extern Constraint operator<=( const Variable&, const Variable& );
extern Constraint operator>=( const Variable&, const Variable& );

extern Constraint operator==( const Variable&, double );
extern Constraint operator<=( const Variable&, double );
extern Constraint operator>=( const Variable&, double );
extern Constraint operator==( double, const Variable& );
extern Constraint operator<=( double, const Variable& );
extern Constraint operator>=( double, const Variable& );
