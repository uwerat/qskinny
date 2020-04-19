/******************************************************************************
 * This code is a stripped down version of the Cassowary constraint solving
 * algorithm. The implementation has been taken from the Kiwi project:
 *
 * Copyright (c) 2013-2017, Nucleic Development Team.
 * Distributed under the terms of the Modified BSD License.
 * https://github.com/nucleic/kiwi/blob/master/LICENSE
 *****************************************************************************/

#include "Solver.h"
#include "Constraint.h"
#include "Variable.h"
#include "Expression.h"
#include "Constraint.h"

#include <limits>
#include <memory>
#include <vector>
#include <cstdint>

template< typename T >
class FlatMap
{
  public:
    using iterator = typename std::vector< T >::iterator;
    using const_iterator = typename std::vector< T >::const_iterator;
    using Key = typename std::remove_reference< decltype( std::declval< T >().key() ) >::type;

    iterator begin() { return m_entries.begin(); }
    const_iterator begin() const { return m_entries.begin(); }

    iterator end() { return m_entries.end(); }
    const_iterator end() const { return m_entries.end(); }

    iterator find( const Key& key )
    {
        auto it = lowerBound( key );
        if ( ( it == m_entries.end() ) || key < it->key() )
            return m_entries.end();

        return it;
    }

    const_iterator find( const Key& key ) const
    {
        auto it = lowerBound( key );
        if ( ( it == m_entries.end() ) || key < it->key() )
            return m_entries.end();

        return it;
    }

    T& operator[]( const Key& key )
    {
        auto it = lowerBound( key );
        if ( ( it == m_entries.end() ) || key < it->key() )
            it = m_entries.insert( it, T( key ) );

        return *it;
    }

    void erase( iterator it )
    {
        m_entries.erase( it );
    }

    void erase( const Key& key )
    {
        auto it = find( key );
        if ( it != m_entries.end() )
            m_entries.erase( it );
    }

    void insert( iterator it, const T& entry )
    {
        m_entries.insert( it, entry );
    }

    void clear()
    {
        m_entries.clear();
    }

    bool empty() const
    {
        return m_entries.empty();
    }

    inline typename std::vector< T >::iterator lowerBound( const Key& key ) const
    {
        auto cmp = []( const T& entry, const Key& key )
            { return entry.key() < key; };

        auto& entries = const_cast< std::vector< T >& >( m_entries );
        return std::lower_bound( entries.begin(), entries.end(), key, cmp );
    }

  private:
    std::vector< T > m_entries;
};

namespace
{
    // qFuzzyIsNull ??
    inline bool nearZero( double value )
    {
        const double eps = 1.0e-8;
        return value < 0.0 ? -value < eps : value < eps;
    }

    class Symbol
    {
      public:
        enum Type
        {
            Invalid,
            External,
            Slack,
            Error,
            Dummy
        };

        Symbol()
            : m_id( 0 )
            , m_type( Invalid )
        {
        }

        uint32_t id() const { return m_id; }
        Type type() const { return m_type; }

        static Symbol external() { return Symbol( Type::External ); }
        static Symbol slack() { return Symbol( Type::Slack ); }
        static Symbol error() { return Symbol( Type::Error ); }
        static Symbol dummy() { return Symbol( Type::Dummy ); }

      private:
        Symbol( Type t )
            : m_id( nextId() )
            , m_type( t )
        {
        }

        static inline uint32_t nextId()
        {
            static uint32_t id = 0;
            return ++id;
        }

        friend bool operator<( const Symbol& lhs, const Symbol& rhs )
        {
            return lhs.m_id < rhs.m_id;
        }

        uint32_t m_id;
        Type m_type;
    };
}

namespace
{
    class Cell
    {
      public:
        Cell( const Symbol& symbol, double coefficient = 0.0 )
            : symbol( symbol )
            , coefficient( coefficient )
        {
        }

        inline const Symbol& key() const { return symbol; }

        Symbol symbol;
        double coefficient;
    };

    class Row
    {
      public:

        Row( double constant = 0.0 )
            : m_constant( constant )
        {
        }

        const FlatMap< Cell >& cells() const
        {
            return m_cells;
        }

        double constant() const
        {
            return m_constant;
        }

        double add( double value )
        {
            return m_constant += value;
        }

        void insert( const Symbol& symbol, double coefficient = 1.0 )
        {
            auto it = m_cells.lowerBound( symbol );
            if ( ( it == m_cells.end() ) || symbol < it->symbol )
            {
                if ( !nearZero( coefficient ) )
                    m_cells.insert( it, Cell( symbol, coefficient ) );
            }
            else
            {
                if( nearZero( it->coefficient += coefficient ) )
                    m_cells.erase( it );
            }
        }

        void insert( const Row& other, double coefficient )
        {
            m_constant += other.m_constant * coefficient;

            for ( auto& cell : other.m_cells )
            {
                const double coeff = cell.coefficient * coefficient;
                insert( cell.symbol, coeff );
            }
        }

        void remove( const Symbol& symbol )
        {
            auto it = m_cells.find( symbol );
            if( it != m_cells.end() )
                m_cells.erase( it );
        }

        void reverseSign()
        {
            m_constant = -m_constant;

            for ( auto& cell : m_cells )
                cell.coefficient = -cell.coefficient;
        }

        void solveFor( const Symbol& symbol )
        {
            /*
                This method assumes the row is of the form a * x + b * y + c = 0
                and (assuming solve for x) will modify the row to represent the
                right hand side of x = -b/a * y - c / a. The target symbol will
                be removed from the row, and the constant and other cells will
                be multiplied by the negative inverse of the target coefficient.

                The given symbol *must* exist in the row.
             */
            auto it = m_cells.find( symbol );

            const double coeff = -1.0 / it->coefficient;
            m_cells.erase( it );

            m_constant *= coeff;

            for ( auto& cell : m_cells )
                cell.coefficient *= coeff;
        }

        void solveFor( const Symbol& lhs, const Symbol& rhs )
        {
            /*
                This method assumes the row is of the form x = b * y + c and will
                solve the row such that y = x / b - c / b. The rhs symbol will be
                removed from the row, the lhs added, and the result divided by the
                negative inverse of the rhs coefficient.

                The lhs symbol *must not* exist in the row, and the rhs symbol
                must exist in the row.
             */
            insert( lhs, -1.0 );
            solveFor( rhs );
        }

        double coefficientFor( const Symbol& symbol ) const
        {
            const auto it = m_cells.find( symbol );
            return ( it == m_cells.end() ) ? 0.0 : it->coefficient;
        }

        void substitute( const Symbol& symbol, const Row& row )
        {
            /*
                Given a row of the form a * x + b and a substitution of the
                form x = 3 * y + c the row will be updated to reflect the
                expression 3 * a * y + a * c + b.
             */
            const auto it = m_cells.find( symbol );
            if( it != m_cells.end() )
            {
                const double coefficient = it->coefficient;
                m_cells.erase( it );
                insert( row, coefficient );
            }
        }

        bool isDummyRow() const
        {
            for ( const auto& cell : m_cells )
            {
                if( cell.symbol.type() != Symbol::Dummy )
                    return false;
            }
            return true;
        }
      private:
        FlatMap< Cell > m_cells;
        double m_constant;
    };
}

namespace
{
    struct Tag
    {
        Symbol marker;
        Symbol other;
    };

    struct EditInfo
    {
        EditInfo( const Variable& variable )
            : variable( variable )
            , constant( 0.0 )
        {
        }

        inline const Variable& key() const { return variable; }

        Variable variable;
        Tag tag;
        Constraint constraint;
        double constant;
    };

    struct ConstraintInfo
    {
        ConstraintInfo( const Constraint& constraint )
            : constraint( constraint )
        {
        }

        inline const Constraint& key() const { return constraint; }

        Constraint constraint;
        Tag tag;
    };

    struct VariableInfo
    {
        VariableInfo( const Variable& variable )
            : variable( variable )
        {
        }

        inline const Variable& key() const { return variable; }

        Variable variable;
        Symbol symbol;
    };

    struct RowInfo
    {
        RowInfo( const Symbol& symbol )
            : symbol( symbol )
            , row( nullptr )
        {
        }

        inline const Symbol& key() const { return symbol; }

        Symbol symbol;
        Row* row;
    };
}

class SimplexSolver
{
  public:
    SimplexSolver();
    ~SimplexSolver();

    void addConstraint( const Constraint& );
    void removeConstraint( const Constraint& );
    bool hasConstraint( const Constraint& ) const;

    bool hasConstraints() const;

    void addEditVariable( const Variable&, double strength );
    void removeEditVariable( const Variable& );

    bool hasEditVariable( const Variable& ) const;
    void suggestValue( const Variable&, double );

    void updateVariables();
    void reset();

  private:
    void clearRows();

    Symbol getVarSymbol( const Variable& );
    Row* createRow( const Constraint& constraint, Tag& );
    Symbol chooseSubject( const Row&, const Tag& );
    bool addWithArtificialVariable( const Row& );

    void substitute( const Symbol&, const Row& );
    bool optimize( const Row& );
    void dualOptimize();

    Symbol getEnteringSymbol( const Row& );
    Symbol getDualEnteringSymbol( const Row& );
    Symbol anyPivotableSymbol( const Row& );

    FlatMap< RowInfo >::iterator getLeavingRow( const Symbol& );
    FlatMap< RowInfo >::iterator getMarkerLeavingRow( const Symbol& );

    void removeMarkerEffects( const Symbol&, double strength );

    FlatMap< ConstraintInfo > m_constraints;

    FlatMap< RowInfo > m_rows;
    FlatMap< VariableInfo > m_variables;
    FlatMap< EditInfo > m_editVariables;

    std::vector< Symbol > m_infeasibleRows;
    std::unique_ptr< Row > m_objective;
    std::unique_ptr< Row > m_artificial;
};

SimplexSolver::SimplexSolver()
    : m_objective( new Row() )
{
}

SimplexSolver::~SimplexSolver()
{
    clearRows();
}

bool SimplexSolver::hasConstraints() const
{
    return !m_constraints.empty();
}

void SimplexSolver::addConstraint( const Constraint& constraint )
{
    if( m_constraints.find( constraint ) != m_constraints.end() )
    {
        qWarning( "The constraint has already been added to the solver." );
        return;
    }

    // Creating a row causes symbols to be reserved for the variables
    // in the constraint. If this method exits with an exception,
    // then its possible those variables will linger in the var map.
    // Since its likely that those variables will be used in other
    // constraints and since exceptional conditions are uncommon,
    // i'm not too worried about aggressive cleanup of the var map.
    Tag tag;

    std::unique_ptr< Row > rowptr( createRow( constraint, tag ) );
    auto subject = chooseSubject( *rowptr, tag );

    // If chooseSubject could not find a valid entering symbol, one
    // last option is available if the entire row is composed of
    // dummy variables. If the constant of the row is zero, then
    // this represents redundant constraints and the new dummy
    // marker can enter the basis. If the constant is non-zero,
    // then it represents an unsatisfiable constraint.
    if( subject.type() == Symbol::Invalid && rowptr->isDummyRow() )
    {
        if( !nearZero( rowptr->constant() ) )
        {
            qWarning( "The constraint can not be satisfied." );
            return;
        }

        subject = tag.marker;
    }

    // If an entering symbol still isn't found, then the row must
    // be added using an artificial variable. If that fails, then
    // the row represents an unsatisfiable constraint.
    if( subject.type() == Symbol::Invalid )
    {
        if( !addWithArtificialVariable( *rowptr ) )
        {
            qWarning( "The constraint can not be satisfied." );
            return;
        }
    }
    else
    {
        rowptr->solveFor( subject );
        substitute( subject, *rowptr );
        m_rows[ subject ].row = rowptr.release();
    }

    m_constraints[ constraint ].tag = tag;

    // Optimizing after each constraint is added performs less
    // aggregate work due to a smaller average system size. It
    // also ensures the solver remains in a consistent state.
    optimize( *m_objective );
}

void SimplexSolver::removeConstraint( const Constraint& constraint )
{
    auto cn_it = m_constraints.find( constraint );
    if( cn_it == m_constraints.end() )
        return;

    const Tag tag = cn_it->tag;
    m_constraints.erase( cn_it );

    // Remove the error effects from the objective function
    // *before* pivoting, or substitutions into the objective
    // will lead to incorrect solver results.
    if( tag.marker.type() == Symbol::Error )
        removeMarkerEffects( tag.marker, constraint.strength() );

    if( tag.other.type() == Symbol::Error )
        removeMarkerEffects( tag.other, constraint.strength() );

    // If the marker is basic, simply drop the row. Otherwise,
    // pivot the marker into the basis and then drop the row.
    auto row_it = m_rows.find( tag.marker );
    if( row_it != m_rows.end() )
    {
        std::unique_ptr< Row > rowptr( row_it->row );
        m_rows.erase( row_it );
    }
    else
    {
        row_it = getMarkerLeavingRow( tag.marker );
        if( row_it == m_rows.end() )
        {
            qWarning( "failed to find leaving row" );
            return;
        }

        const auto leaving = row_it->symbol;

        std::unique_ptr< Row > rowptr( row_it->row );
        m_rows.erase( row_it );

        rowptr->solveFor( leaving, tag.marker );
        substitute( tag.marker, *rowptr );
    }

    // Optimizing after each constraint is removed ensures that the
    // solver remains consistent. It makes the solver api easier to
    // use at a small tradeoff for speed.
    optimize( *m_objective );
}

bool SimplexSolver::hasConstraint( const Constraint& constraint ) const
{
    return m_constraints.find( constraint ) != m_constraints.end();
}

void SimplexSolver::addEditVariable( const Variable& variable, double strength )
{
    if( m_editVariables.find( variable ) != m_editVariables.end() )
        return;

    strength = Strength::clip( strength );
    if( strength == Strength::required )
    {
        qWarning( "A required strength cannot be used in this context." );
        return;
    }

    Constraint cn( Expression( variable ), OP_EQ, strength );
    addConstraint( cn );

    EditInfo info( variable );
    info.tag = m_constraints[ cn ].tag;
    info.constraint = cn;

    m_editVariables[ variable ] = info;
}

void SimplexSolver::removeEditVariable( const Variable& variable )
{
    auto it = m_editVariables.find( variable );
    if( it == m_editVariables.end() )
        return;

    removeConstraint( it->constraint );
    m_editVariables.erase( it );
}

bool SimplexSolver::hasEditVariable( const Variable& variable ) const
{
    return m_editVariables.find( variable ) != m_editVariables.end();
}

void SimplexSolver::suggestValue( const Variable& variable, double value )
{
    auto it = m_editVariables.find( variable );
    if( it == m_editVariables.end() )
    {
        qWarning( "The edit variable has not been added to the solver." );
        return;
    }

    auto& editInfo = *it;

    const double delta = value - editInfo.constant;
    editInfo.constant = value;

    /*
        Check first if the positive error variable is basic.
        Check next if the negative error variable is basic.
        Otherwise update each row where the error variables exist.
     */
    auto row_it = m_rows.find( editInfo.tag.marker );
    if( row_it != m_rows.end() )
    {
        if( row_it->row->add( -delta ) < 0.0 )
            m_infeasibleRows.push_back( row_it->symbol );
    }
    else
    {
        row_it = m_rows.find( editInfo.tag.other );
        if( row_it != m_rows.end() )
        {
            if( row_it->row->add( delta ) < 0.0 )
                m_infeasibleRows.push_back( row_it->symbol );
        }
        else
        {
            for ( const auto& row : m_rows )
            {
                const double coeff = row.row->coefficientFor( editInfo.tag.marker );

                if( coeff != 0.0 && row.row->add( delta * coeff ) < 0.0 &&
                    row.symbol.type() != Symbol::External )
                {
                    m_infeasibleRows.push_back( row.symbol );
                }
            }
        }
    }

    dualOptimize();
}

void SimplexSolver::updateVariables()
{
    for ( auto& info : m_variables )
    {
        const auto it = m_rows.find( info.symbol );

        if( it == m_rows.end() )
            info.variable.setValue( 0.0 );
        else
            info.variable.setValue( it->row->constant() );
    }
}

void SimplexSolver::reset()
{
    clearRows();

    m_constraints.clear();
    m_variables.clear();
    m_editVariables.clear();
    m_infeasibleRows.clear();
    m_objective.reset( new Row() );
    m_artificial.reset();
    // nextId !
}

void SimplexSolver::clearRows()
{
    for ( auto& row : m_rows )
        delete row.row;

    m_rows.clear();
}

Symbol SimplexSolver::getVarSymbol( const Variable& variable )
{
    auto it = m_variables.find( variable );
    if( it != m_variables.end() )
        return it->symbol;

    return m_variables[ variable ].symbol = Symbol::external();
}

Row* SimplexSolver::createRow( const Constraint& constraint, Tag& tag )
{
    const auto& expr = constraint.expression();

    auto row = new Row( expr.constant() );

    // Substitute the current basic variables into the row.
    for ( auto& term : expr.terms() )
    {
        if( !nearZero( term.coefficient() ) )
        {
            const auto symbol = getVarSymbol( term.variable() );

            const auto it = m_rows.find( symbol );
            if( it != m_rows.end() )
                row->insert( *it->row, term.coefficient() );
            else
                row->insert( symbol, term.coefficient() );
        }
    }

    // Add the necessary slack, error, and dummy variables.
    switch( constraint.oper() )
    {
        case OP_LE:
        case OP_GE:
        {
            double coeff = constraint.oper() == OP_LE ? 1.0 : -1.0;
            auto slack = Symbol::slack();
            tag.marker = slack;
            row->insert( slack, coeff );

            if( constraint.strength() < Strength::required )
            {
                const auto error = Symbol::error();
                tag.other = error;
                row->insert( error, -coeff );
                m_objective->insert( error, constraint.strength() );
            }
            break;
        }
        case OP_EQ:
        {
            if( constraint.strength() < Strength::required )
            {
                const auto errplus = Symbol::error();
                const auto errminus = Symbol::error();

                tag.marker = errplus;
                tag.other = errminus;
                row->insert( errplus, -1.0 ); // v = eplus - eminus
                row->insert( errminus, 1.0 ); // v - eplus + eminus = 0
                m_objective->insert( errplus, constraint.strength() );
                m_objective->insert( errminus, constraint.strength() );
            }
            else
            {
                const auto dummy = Symbol::dummy();
                tag.marker = dummy;
                row->insert( dummy );
            }
            break;
        }
    }

    // Ensure the row as a positive constant.
    if( row->constant() < 0.0 )
        row->reverseSign();

    return row;
}

Symbol SimplexSolver::chooseSubject( const Row& row, const Tag& tag )
{
    for ( const auto& cell : row.cells() )
    {
        if( cell.symbol.type() == Symbol::External )
            return cell.symbol;
    }

    if( tag.marker.type() == Symbol::Slack || tag.marker.type() == Symbol::Error )
    {
        if( row.coefficientFor( tag.marker ) < 0.0 )
            return tag.marker;
    }

    if( tag.other.type() == Symbol::Slack || tag.other.type() == Symbol::Error )
    {
        if( row.coefficientFor( tag.other ) < 0.0 )
            return tag.other;
    }

    return Symbol();
}

bool SimplexSolver::addWithArtificialVariable( const Row& row )
{
    // Create and add the artificial variable to the tableau
    auto art = Symbol::slack();
    m_rows[ art ].row = new Row( row );
    m_artificial.reset( new Row( row ) );

    // Optimize the artificial objective. This is successful
    // only if the artificial objective is optimized to zero.
    bool success = optimize( *m_artificial );
    if ( !success )
        return false;

    success = nearZero( m_artificial->constant() );
    m_artificial.reset();

    // If the artificial variable is not basic, pivot the row so that
    // it becomes basic. If the row is constant, exit early.
    auto it = m_rows.find( art );
    if( it != m_rows.end() )
    {
        std::unique_ptr< Row > rowptr( it->row );
        m_rows.erase( it );
        if( rowptr->cells().empty() )
            return success;

        const auto entering = anyPivotableSymbol( *rowptr );
        if( entering.type() == Symbol::Invalid )
            return false;  // unsatisfiable (will this ever happen?)

        rowptr->solveFor( art, entering );
        substitute( entering, *rowptr );
        m_rows[ entering ].row = rowptr.release();
    }

    // Remove the artificial variable from the tableau.

    for ( auto& rowInfo : m_rows )
        rowInfo.row->remove( art );

    m_objective->remove( art );

    return success;
}

void SimplexSolver::substitute( const Symbol& symbol, const Row& row )
{
    for ( auto& r : m_rows )
    {
        r.row->substitute( symbol, row );
        if( r.symbol.type() != Symbol::External && r.row->constant() < 0.0 )
            m_infeasibleRows.push_back( r.symbol );
    }

    m_objective->substitute( symbol, row );

    if( m_artificial.get() )
        m_artificial->substitute( symbol, row );
}

bool SimplexSolver::optimize( const Row& objective )
{
    while( true )
    {
        const auto entering = getEnteringSymbol( objective );
        if( entering.type() == Symbol::Invalid )
            return true;

        const auto it = getLeavingRow( entering );
        if( it == m_rows.end() )
        {
            qWarning( "The objective is unbounded." );
            return false;
        }

        // pivot the entering symbol into the basis
        const auto leaving = it->symbol;
        auto row = it->row;

        m_rows.erase( it );
        row->solveFor( leaving, entering );
        substitute( entering, *row );

        m_rows[ entering ].row = row;
    }
}

void SimplexSolver::dualOptimize()
{
    while( !m_infeasibleRows.empty() )
    {
        auto leaving = m_infeasibleRows.back();
        m_infeasibleRows.pop_back();

        auto it = m_rows.find( leaving );
        if( it != m_rows.end() && !nearZero( it->row->constant() ) &&
            it->row->constant() < 0.0 )
        {
            auto entering = getDualEnteringSymbol( *it->row );
            if( entering.type() == Symbol::Invalid )
            {
                qWarning( "Dual optimize failed." );
                return;
            }

            // Pivot the entering symbol into the basis
            auto row = it->row;
            m_rows.erase( it );

            row->solveFor( leaving, entering );
            substitute( entering, *row );

            m_rows[ entering ].row = row;
        }
    }
}

Symbol SimplexSolver::getEnteringSymbol( const Row& objective )
{
    for ( const auto& cell : objective.cells() )
    {
        if( cell.symbol.type() != Symbol::Dummy && cell.coefficient < 0.0 )
            return cell.symbol;
    }
    return Symbol();
}

Symbol SimplexSolver::getDualEnteringSymbol( const Row& row )
{
    Symbol entering;
    double ratio = std::numeric_limits< double >::max();

    for ( const auto& cell : row.cells() )
    {
        if( cell.coefficient > 0.0 && cell.symbol.type() != Symbol::Dummy )
        {
            const double coeff = m_objective->coefficientFor( cell.symbol );
            const double r = coeff / cell.coefficient;

            if( r < ratio )
            {
                ratio = r;
                entering = cell.symbol;
            }
        }
    }
    return entering;
}

Symbol SimplexSolver::anyPivotableSymbol( const Row& row )
{
    for ( const auto& cell : row.cells() )
    {
        const auto& symbol = cell.symbol;
        if( symbol.type() == Symbol::Slack || symbol.type() == Symbol::Error )
            return symbol;
    }
    return Symbol();
}

FlatMap< RowInfo >::iterator SimplexSolver::getLeavingRow( const Symbol& entering )
{
    double ratio = std::numeric_limits< double >::max();
    auto found = m_rows.end();

    for( auto it = m_rows.begin(); it != m_rows.end(); ++it )
    {
        if( it->symbol.type() != Symbol::External )
        {
            const double coeff = it->row->coefficientFor( entering );
            if( coeff < 0.0 )
            {
                const double temp_ratio = -it->row->constant() / coeff;
                if( temp_ratio < ratio )
                {
                    ratio = temp_ratio;
                    found = it;
                }
            }
        }
    }
    return found;
}

FlatMap< RowInfo >::iterator SimplexSolver::getMarkerLeavingRow( const Symbol& marker )
{
    const double dmax = std::numeric_limits< double >::max();

    double r1 = dmax;
    double r2 = dmax;

    auto end = m_rows.end();
    auto first = end;
    auto second = end;
    auto third = end;

    for( auto it = m_rows.begin(); it != m_rows.end(); ++it )
    {
        double coeff = it->row->coefficientFor( marker );
        if( coeff == 0.0 )
            continue;

        if( it->symbol.type() == Symbol::External )
        {
            third = it;
        }
        else if( coeff < 0.0 )
        {
            const double r = -it->row->constant() / coeff;
            if( r < r1 )
            {
                r1 = r;
                first = it;
            }
        }
        else
        {
            double r = it->row->constant() / coeff;
            if( r < r2 )
            {
                r2 = r;
                second = it;
            }
        }
    }
    if( first != end )
        return first;

    if( second != end )
        return second;

    return third;
}

void SimplexSolver::removeMarkerEffects( const Symbol& marker, double strength )
{
    auto row_it = m_rows.find( marker );
    if( row_it != m_rows.end() )
        m_objective->insert( *row_it->row, -strength );
    else
        m_objective->insert( marker, -strength );
}

Solver::Solver()
    : m_solver( new SimplexSolver() )
{
}

Solver::~Solver()
{
}

bool Solver::hasConstraints() const
{
    return m_solver->hasConstraints();
}

void Solver::addConstraint( const Constraint& constraint )
{
    m_solver->addConstraint( constraint );
}

void Solver::removeConstraint( const Constraint& constraint )
{
    m_solver->removeConstraint( constraint );
}

bool Solver::hasConstraint( const Constraint& constraint ) const
{
    return m_solver->hasConstraint( constraint );
}

void Solver::addEditVariable( const Variable& variable, double strength )
{
    m_solver->addEditVariable( variable, strength );
}

void Solver::removeEditVariable( const Variable& variable )
{
    m_solver->removeEditVariable( variable );
}

bool Solver::hasEditVariable( const Variable& variable ) const
{
    return m_solver->hasEditVariable( variable );
}

void Solver::suggestValue( const Variable& variable, double value )
{
    m_solver->suggestValue( variable, value );
}

void Solver::updateVariables()
{
    m_solver->updateVariables();
}

void Solver::reset()
{
    m_solver->reset();
}
