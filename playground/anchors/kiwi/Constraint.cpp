#include "Constraint.h"
#include "Expression.h"

#include <map>

static Expression reduce( const Expression& expr )
{
    std::map< Variable, double > vars;

    for ( auto term : expr.terms() )
        vars[ term.variable() ] += term.coefficient();

    const std::vector< Term > terms( vars.begin(), vars.end() );
    return Expression( terms, expr.constant() );
}

class Constraint::Data
{
  public:
    Data( const Expression& expr, RelationalOperator op, double strength )
        : expression( reduce( expr ) )
        , strength( Strength::clip( strength ) )
        , op( op )
    {
    }

    Expression expression;
    double strength;
    RelationalOperator op;
};


Constraint::Constraint( const Expression& expr, RelationalOperator op, double strength )
    : m_data( std::make_shared< Data >( expr, op, strength ) )
{
}

Constraint::Constraint( const Constraint& other, double strength )
    : Constraint( other.expression(), other.oper(), strength )
{
}

Constraint::~Constraint()
{
}

const Expression& Constraint::expression() const
{
    return m_data->expression;
}

RelationalOperator Constraint::oper() const
{
    return m_data->op;
}

double Constraint::strength() const
{
    return m_data->strength;
}

Constraint operator==( const Expression& first, const Expression& second )
{
    return Constraint( first - second, OP_EQ );
}

Constraint operator==( const Expression& expression, const Term& term )
{
    return expression == Expression( term );
}

Constraint operator==( const Expression& expression, const Variable& variable )
{
    return expression == Term( variable );
}

Constraint operator==( const Expression& expression, double constant )
{
    return expression == Expression( constant );
}

Constraint operator<=( const Expression& first, const Expression& second )
{
    return Constraint( first - second, OP_LE );
}

Constraint operator<=( const Expression& expression, const Term& term )
{
    return expression <= Expression( term );
}

Constraint operator<=( const Expression& expression, const Variable& variable )
{
    return expression <= Term( variable );
}

Constraint operator<=( const Expression& expression, double constant )
{
    return expression <= Expression( constant );
}

Constraint operator>=( const Expression& first, const Expression& second )
{
    return Constraint( first - second, OP_GE );
}

Constraint operator>=( const Expression& expression, const Term& term )
{
    return expression >= Expression( term );
}

Constraint operator>=( const Expression& expression, const Variable& variable )
{
    return expression >= Term( variable );
}

Constraint operator>=( const Expression& expression, double constant )
{
    return expression >= Expression( constant );
}

Constraint operator==( const Term& term, const Expression& expression )
{
    return expression == term;
}

Constraint operator==( const Term& first, const Term& second )
{
    return Expression( first ) == second;
}

Constraint operator==( const Term& term, const Variable& variable )
{
    return Expression( term ) == variable;
}

Constraint operator==( const Term& term, double constant )
{
    return Expression( term ) == constant;
}

Constraint operator<=( const Term& term, const Expression& expression )
{
    return expression >= term;
}

Constraint operator<=( const Term& first, const Term& second )
{
    return Expression( first ) <= second;
}

Constraint operator<=( const Term& term, const Variable& variable )
{
    return Expression( term ) <= variable;
}

Constraint operator<=( const Term& term, double constant )
{
    return Expression( term ) <= constant;
}

Constraint operator>=( const Term& term, const Expression& expression )
{
    return expression <= term;
}

Constraint operator>=( const Term& first, const Term& second )
{
    return Expression( first ) >= second;
}

Constraint operator>=( const Term& term, const Variable& variable )
{
    return Expression( term ) >= variable;
}

Constraint operator>=( const Term& term, double constant )
{
    return Expression( term ) >= constant;
}

Constraint operator==( const Variable& variable, const Expression& expression )
{
    return expression == variable;
}

Constraint operator==( const Variable& variable, const Term& term )
{
    return term == variable;
}

Constraint operator==( const Variable& first, const Variable& second )
{
    return Term( first ) == second;
}

Constraint operator==( const Variable& variable, double constant )
{
    return Term( variable ) == constant;
}

Constraint operator<=( const Variable& variable, const Expression& expression )
{
    return expression >= variable;
}

Constraint operator<=( const Variable& variable, const Term& term )
{
    return term >= variable;
}

Constraint operator<=( const Variable& first, const Variable& second )
{
    return Term( first ) <= second;
}

Constraint operator<=( const Variable& variable, double constant )
{
    return Term( variable ) <= constant;
}

Constraint operator>=( const Variable& variable, const Expression& expression )
{
    return expression <= variable;
}

Constraint operator>=( const Variable& variable, const Term& term )
{
    return term <= variable;
}

Constraint operator>=( const Variable& first, const Variable& second )
{
    return Term( first ) >= second;
}

Constraint operator>=( const Variable& variable, double constant )
{
    return Term( variable ) >= constant;
}

Constraint operator==( double constant, const Expression& expression )
{
    return expression == constant;
}

Constraint operator==( double constant, const Term& term )
{
    return term == constant;
}

Constraint operator==( double constant, const Variable& variable )
{
    return variable == constant;
}

Constraint operator<=( double constant, const Expression& expression )
{
    return expression >= constant;
}

Constraint operator<=( double constant, const Term& term )
{
    return term >= constant;
}

Constraint operator<=( double constant, const Variable& variable )
{
    return variable >= constant;
}

Constraint operator>=( double constant, const Expression& expression )
{
    return expression <= constant;
}

Constraint operator>=( double constant, const Term& term )
{
    return term <= constant;
}

Constraint operator>=( double constant, const Variable& variable )
{
    return variable <= constant;
}
