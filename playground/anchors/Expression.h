#pragma once

#include <vector>
#include "Term.h"

class Expression
{
  public:
    Expression( double constant = 0.0 );
    Expression( const Term&, double constant = 0.0 );

    Expression( const std::vector< Term >&, double constant = 0.0 );
    Expression( const std::vector< Term >&&, double constant = 0.0 );

    const std::vector< Term >& terms() const { return m_terms; }
    double constant() const { return m_constant; }

    double value() const;

  private:
    std::vector< Term > m_terms;
    double m_constant;
};

extern Expression operator-( const Expression& );

extern Expression operator+( const Expression&, const Expression& );
extern Expression operator-( const Expression&, const Expression& );

extern Expression operator+( const Expression&, const Term& );
extern Expression operator-( const Expression&, const Term& );
extern Expression operator+( const Term&, const Expression&);
extern Expression operator-( const Term&, const Expression&);

extern Expression operator+( const Expression&, const Variable& );
extern Expression operator-( const Expression&, const Variable& );
extern Expression operator-( const Variable&, const Expression&);
extern Expression operator+( const Variable&, const Expression&);

extern Expression operator*( const Expression&, double );
extern Expression operator/( const Expression&, double );
extern Expression operator+( const Expression&, double );
extern Expression operator-( const Expression&, double );
extern Expression operator*( double, const Expression&);
extern Expression operator+( double, const Expression&);
extern Expression operator-( double, const Expression&);

extern Expression operator+( const Term&, const Term& );
extern Expression operator-( const Term&, const Term& );

extern Expression operator+( const Term&, const Variable& );
extern Expression operator-( const Term&, const Variable& );
extern Expression operator-( const Variable&, const Term&);
extern Expression operator+( const Variable&, const Term&);

extern Expression operator+( const Term&, double );
extern Expression operator-( const Term&, double );
extern Expression operator+( double, const Term&);
extern Expression operator-( double, const Term&);

extern Expression operator+( const Variable&, const Variable& );
extern Expression operator-( const Variable&, const Variable& );

extern Expression operator+( const Variable&, double );
extern Expression operator-( const Variable&, double );
extern Expression operator+( double, const Variable& );
extern Expression operator-( double, const Variable& );

