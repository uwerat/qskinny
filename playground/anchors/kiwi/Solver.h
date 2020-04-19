#pragma once

#include <qglobal.h>
#include <memory>

class Variable;
class Constraint;
class SimplexSolver;

class Solver
{
  public:

    Solver();
    ~Solver();

    bool hasConstraints() const;

    void addConstraint( const Constraint& );
    void removeConstraint( const Constraint& );
    bool hasConstraint( const Constraint& ) const;

    void addEditVariable( const Variable&, double strength );
    void removeEditVariable( const Variable& );

    bool hasEditVariable( const Variable& ) const;
    void suggestValue( const Variable&, double value );

    void updateVariables();
    void reset();

  private:
    Q_DISABLE_COPY( Solver )
    std::unique_ptr< SimplexSolver > m_solver;
};
