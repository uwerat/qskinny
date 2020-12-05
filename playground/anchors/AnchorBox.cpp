/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "AnchorBox.h"

#include "kiwi/Solver.h"
#include "kiwi/Constraint.h"
#include "kiwi/Variable.h"
#include "kiwi/Expression.h"

#include <QskEvent.h>
#include <QskQuick.h>

#include <vector>
#include <map>

static inline Qt::Orientation qskOrientation( int edge )
{
    return ( edge <= Qt::AnchorRight ) ? Qt::Horizontal : Qt::Vertical;
}

static inline Qt::AnchorPoint qskAnchorPoint(
    Qt::Corner corner, Qt::Orientation orientation )
{
    if ( orientation == Qt::Horizontal )
        return ( corner & 0x1 ) ? Qt::AnchorRight : Qt::AnchorLeft;
    else
        return ( corner >= 0x2 ) ? Qt::AnchorBottom : Qt::AnchorTop;
}

namespace
{
    class Geometry
    {
      public:
        Expression expressionAt( int anchorPoint )
        {
            switch( anchorPoint )
            {
                case Qt::AnchorLeft:
                    return Term( m_left );

                case Qt::AnchorHorizontalCenter:
                    return centerH();

                case Qt::AnchorRight:
                    return right();

                case Qt::AnchorTop:
                    return Term( m_top );

                case Qt::AnchorVerticalCenter:
                    return centerV();

                case Qt::AnchorBottom:
                    return bottom();
            }

            return Expression();
        }

        inline const Variable& length( Qt::Orientation orientation )
        {
            return ( orientation == Qt::Horizontal ) ? m_width : m_height;
        }

        inline QRectF rect() const
        {
            return QRectF( m_left.value(), m_top.value(),
                m_width.value(), m_height.value() );
        }

        inline QSizeF size() const
        {
            return QSizeF( m_width.value(), m_height.value() );
        }

        inline Expression centerH() const { return m_left + 0.5 * m_width; }
        inline Expression centerV() const { return m_top + 0.5 * m_height; }
        inline Expression right() const { return m_left + m_width; }
        inline Expression bottom() const { return m_top + m_height; }

        inline const Variable& left() const { return m_left; }
        inline const Variable& top() const { return m_top; }
        inline const Variable& width() const { return m_width; }
        inline const Variable& height() const { return m_height; }

      private:
        Variable m_left, m_top, m_width, m_height;
    };

    class Anchor
    {
      public:
        QQuickItem* item1 = nullptr;
        Qt::AnchorPoint edge1;

        QQuickItem* item2 = nullptr;
        Qt::AnchorPoint edge2;
    };

    class LayoutSolver : public Solver
    {
      public:
        void setup( bool layoutChildren,
            const QVector< Anchor >&, std::map< QQuickItem*, Geometry >& );

        void addSizeConstraints();

        QSizeF resolvedSize();
        QSizeF resolvedSize( qreal width, qreal height );

        void resolve( qreal width, qreal height );

      private:
        void addSizeConstraints( Geometry& rect, const QSizeF& size,
            RelationalOperator op, double strength );

        Variable m_width, m_height;
    };
}

void LayoutSolver::setup( bool layoutChildren,
    const QVector< Anchor >& anchors,
    std::map< QQuickItem*, Geometry >& geometries )
{
    for ( const auto& anchor : anchors )
    {
        auto& r1 = geometries[ anchor.item1 ];

        const auto expr1 = r1.expressionAt( anchor.edge1 );

        Expression expr2;

        if ( anchor.item2 == nullptr )
        {
            switch( anchor.edge2 )
            {
                case Qt::AnchorLeft:
                case Qt::AnchorTop:
                    expr2 = 0;
                    break;

                case Qt::AnchorHorizontalCenter:
                    expr2 = Term( 0.5 * m_width );
                    break;

                case Qt::AnchorRight:
                    expr2 = Term( m_width );
                    break;

                case Qt::AnchorVerticalCenter:
                    expr2 = Term( 0.5 * m_height );
                    break;

                case Qt::AnchorBottom:
                    expr2 = Term( m_height );
                    break;
            }

            addConstraint( expr1 == expr2 );
        }
        else
        {
            auto& r2 = geometries[ anchor.item2 ];
            const auto expr2 = r2.expressionAt( anchor.edge2 );

            addConstraint( expr1 == expr2 );

            if ( layoutChildren )
            {
                const auto o = qskOrientation( anchor.edge1 );

                /*
                    A constraint with medium strength to make anchored item
                    being stretched according to their stretch factors s1, s2.
                    ( For the moment we don't support having specific factors. )
                 */
                const auto s1 = 1.0;
                const auto s2 = 1.0;

                Constraint c( r1.length( o ) * s1 == r2.length( o ) * s2, Strength::medium );
                addConstraint( c );
            }
        }
    }

    for ( auto it = geometries.begin(); it != geometries.end(); ++it )
    {
        const auto minSize = qskSizeConstraint( it->first, Qt::MinimumSize );
        addSizeConstraints( it->second, minSize, OP_GE, Strength::required );

        const auto maxSize = qskSizeConstraint( it->first, Qt::MaximumSize );
        addSizeConstraints( it->second, maxSize, OP_LE, Strength::required );

        const auto prefSize = qskSizeConstraint( it->first, Qt::PreferredSize );
        addSizeConstraints( it->second, prefSize, OP_EQ, Strength::strong );
    }
}

void LayoutSolver::addSizeConstraints()
{
    const double strength = 0.9 * Strength::required;

    addEditVariable( m_width, strength );
    addEditVariable( m_height, strength );
}

void LayoutSolver::resolve( qreal width, qreal height )
{
    suggestValue( m_width, width );
    suggestValue( m_height, height );
    updateVariables();
}

QSizeF LayoutSolver::resolvedSize()
{
    updateVariables();
    return QSizeF( m_width.value(), m_height.value() );
}

QSizeF LayoutSolver::resolvedSize( qreal width, qreal height )
{
    resolve( width, height );
    return QSizeF( m_width.value(), m_height.value() );
}

void LayoutSolver::addSizeConstraints( Geometry& rect, const QSizeF& size,
    RelationalOperator op, double strength )
{
    if ( size.width() >= 0.0 )
    {
        const Constraint c( rect.width() - size.width(), op, strength );
        addConstraint( c );
    }

    if ( size.height() >= 0.0 )
    {
        const Constraint c( rect.height() - size.height(), op, strength );
        addConstraint( c );
    }
}

class AnchorBox::PrivateData
{
  public:
    ~PrivateData() { delete solver; }

    std::map< QQuickItem*, Geometry > geometries;

    QVector< Anchor > anchors;
    LayoutSolver* solver = nullptr;

    QSizeF hints[3];
    bool hasValidHints = false;
};

AnchorBox::AnchorBox( QQuickItem* parent )
    : QskControl( parent )
    , m_data( new PrivateData )
{
}

AnchorBox::~AnchorBox()
{
}

void AnchorBox::addAnchors( QQuickItem* item, Qt::Orientations orientations )
{
    addAnchors( item, this, orientations );
}

void AnchorBox::addAnchors( QQuickItem* item1,
    QQuickItem* item2, Qt::Orientations orientations )
{
    if ( orientations & Qt::Horizontal )
    {
        addAnchor( item1, Qt::AnchorLeft, item2, Qt::AnchorLeft );
        addAnchor( item1, Qt::AnchorRight, item2, Qt::AnchorRight );
    }

    if ( orientations & Qt::Vertical )
    {
        addAnchor( item1, Qt::AnchorTop, item2, Qt::AnchorTop );
        addAnchor( item1, Qt::AnchorBottom, item2, Qt::AnchorBottom );
    }
}

void AnchorBox::addAnchors( QQuickItem* item, Qt::Corner corner )
{
    addAnchors( item, corner, this, corner );
}

void AnchorBox::addAnchors( QQuickItem* item1,
    Qt::Corner corner1, QQuickItem* item2, Qt::Corner corner2 )
{
    addAnchor( item1, qskAnchorPoint( corner1, Qt::Horizontal ),
        item2, qskAnchorPoint( corner2, Qt::Horizontal ) );

    addAnchor( item1, qskAnchorPoint( corner1, Qt::Vertical ),
        item2, qskAnchorPoint( corner2, Qt::Vertical ) );
}

void AnchorBox::addAnchor( QQuickItem* item,
    Qt::AnchorPoint edge1, Qt::AnchorPoint edge2 )
{
    addAnchor( item, edge1, this, edge2 );
}

void AnchorBox::addAnchor( QQuickItem* item1, Qt::AnchorPoint edge1,
    QQuickItem* item2, Qt::AnchorPoint edge2 )
{
    if ( item1 == item2 || item1 == nullptr || item2 == nullptr )
        return;

    if ( item1 == this )
        std::swap( item1, item2 );

    if ( item2 == this )
        item2 = nullptr;

    if ( item1->parent() == nullptr )
        item1->setParent( this );

    if ( item1->parentItem() != this )
        item1->setParentItem( this );

    (void)m_data->geometries[ item1 ];

    if ( item2 )
    {
        if ( item2->parent() == nullptr )
            item2->setParent( this );

        if ( item2->parentItem() != this )
            item2->setParentItem( this );

        (void)m_data->geometries[ item2 ];
    }

    Anchor anchor;
    anchor.item1 = item1;
    anchor.edge1 = edge1;
    anchor.item2 = item2;
    anchor.edge2 = edge2;

    m_data->anchors += anchor;
}

void AnchorBox::geometryChangeEvent( QskGeometryChangeEvent* event )
{
    Inherited::geometryChangeEvent( event );

    if ( event->isResized() )
        polish();
}

void AnchorBox::updateLayout()
{
    if ( !maybeUnresized() )
        updateGeometries( layoutRect() );
}

QSizeF AnchorBox::layoutSizeHint( Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( constraint.width() >= 0.0 || constraint.height() >= 0.0 )
    {
        // TODO ...
        return QSizeF();
    }

    if ( !m_data->hasValidHints )
    {
        auto that = const_cast< AnchorBox* >( this );

        that->updateHints();
        m_data->hasValidHints = true;
    }

    return m_data->hints[ which ];
}

void AnchorBox::updateHints()
{
    /*
         The solver seems to run into overflows with
         std::numeric_limits< unsigned float >::max()
     */
    const qreal max = std::numeric_limits< unsigned int >::max();

    LayoutSolver solver;
    solver.setup( false, m_data->anchors, m_data->geometries );

    m_data->hints[ Qt::PreferredSize ] = solver.resolvedSize();

    solver.addSizeConstraints();

    m_data->hints[ Qt::MinimumSize ] = solver.resolvedSize( 0.0, 0.0 );
    m_data->hints[ Qt::MaximumSize ] = solver.resolvedSize( max, max );
}

void AnchorBox::updateGeometries( const QRectF& rect )
{
    auto& solver = m_data->solver;

    if ( solver == nullptr )
    {
        solver = new LayoutSolver();
        solver->setup( true, m_data->anchors, m_data->geometries );
        solver->addSizeConstraints();
    }

    solver->resolve( rect.width(), rect.height() );

    const auto& geometries = m_data->geometries;
    for ( auto it = geometries.begin(); it != geometries.end(); ++it )
    {
        auto r = it->second.rect();
        r.translate( rect.left(), rect.top() );

        qskSetItemGeometry( it->first, r );
    }
}

#include "moc_AnchorBox.cpp"
