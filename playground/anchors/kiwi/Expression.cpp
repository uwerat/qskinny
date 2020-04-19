#include "Expression.h"
#include "Term.h"

Expression::Expression( double constant )
    : m_constant( constant )
{
}

Expression::Expression( const Term& term, double constant )
    : m_terms( 1, term )
    , m_constant( constant )
{
}

Expression::Expression( const std::vector< Term >&& terms, double constant )
    : m_terms( std::move( terms ) )
    , m_constant( constant )
{
}

Expression::Expression( const std::vector< Term >& terms, double constant )
    : m_terms( terms )
    , m_constant( constant )
{
}

double Expression::value() const
{
    double result = m_constant;

    for ( const auto& term : m_terms )
        result += term.value();

    return result;
}

Expression operator*( const Expression& expression, double coefficient )
{
    std::vector< Term > terms;
    terms.reserve( expression.terms().size() );

    for ( const auto& term : expression.terms() )
        terms.push_back( term * coefficient );

    return Expression( std::move( terms ), expression.constant() * coefficient );
}

Expression operator/( const Expression& expression, double denominator )
{
    return expression * ( 1.0 / denominator );
}

Expression operator-( const Expression& expression )
{
    return expression * -1.0;
}

Expression operator*( double coefficient, const Expression& expression )
{
    return expression * coefficient;
}

Expression operator+( const Expression& first, const Expression& second )
{
    std::vector< Term > terms;
    terms.reserve( first.terms().size() + second.terms().size() );
    terms.insert( terms.begin(), first.terms().begin(), first.terms().end() );
    terms.insert( terms.end(), second.terms().begin(), second.terms().end() );

    return Expression( std::move( terms ), first.constant() + second.constant() );
}

Expression operator+( const Expression& expression, const Term& term )
{
    std::vector< Term > terms;
    terms.reserve( expression.terms().size() + 1 );
    terms.insert( terms.begin(), expression.terms().begin(), expression.terms().end() );
    terms.push_back( term );

    return Expression( std::move( terms ), expression.constant() );
}

Expression operator+( const Expression& expression, const Variable& variable )
{
    return expression + Term( variable );
}

Expression operator+( const Expression& expression, double constant )
{
    return Expression( expression.terms(), expression.constant() + constant );
}

Expression operator-( const Expression& first, const Expression& second )
{
    return first + -second;
}

Expression operator-( const Expression& expression, const Term& term )
{
    return expression + -term;
}

Expression operator-( const Expression& expression, const Variable& variable )
{
    return expression + -variable;
}

Expression operator-( const Expression& expression, double constant )
{
    return expression + -constant;
}

Expression operator+( const Term& term, const Expression& expression )
{
    return expression + term;
}

Expression operator+( const Term& first, const Term& second )
{
    std::vector< Term > terms;
    terms.reserve( 2 );
    terms.push_back( first );
    terms.push_back( second );

    return Expression( std::move( terms ) );
}

Expression operator+( const Term& term, const Variable& variable )
{
    return term + Term( variable );
}

Expression operator+( const Term& term, double constant )
{
    return Expression( term, constant );
}

Expression operator-( const Term& term, const Expression& expression )
{
    return -expression + term;
}

Expression operator-( const Term& first, const Term& second )
{
    return first + -second;
}

Expression operator-( const Term& term, const Variable& variable )
{
    return term + -variable;
}

Expression operator-( const Term& term, double constant )
{
    return term + -constant;
}

Expression operator+( const Variable& variable, const Expression& expression )
{
    return expression + variable;
}

Expression operator+( const Variable& variable, const Term& term )
{
    return term + variable;
}

Expression operator+( const Variable& first, const Variable& second )
{
    return Term( first ) + second;
}

Expression operator+( const Variable& variable, double constant )
{
    return Term( variable ) + constant;
}

Expression operator-( const Variable& variable, const Expression& expression )
{
    return variable + -expression;
}

Expression operator-( const Variable& variable, const Term& term )
{
    return variable + -term;
}

Expression operator-( const Variable& first, const Variable& second )
{
    return first + -second;
}

Expression operator-( const Variable& variable, double constant )
{
    return variable + -constant;
}

Expression operator+( double constant, const Expression& expression )
{
    return expression + constant;
}

Expression operator+( double constant, const Term& term )
{
    return term + constant;
}

Expression operator+( double constant, const Variable& variable )
{
    return variable + constant;
}

Expression operator-( double constant, const Expression& expression )
{
    return -expression + constant;
}

Expression operator-( double constant, const Term& term )
{
    return -term + constant;
}

Expression operator-( double constant, const Variable& variable )
{
    return -variable + constant;
}
