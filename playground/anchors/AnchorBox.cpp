/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "AnchorBox.h"

#include "Solver.h"
#include "Constraint.h"
#include "Variable.h"
#include "Expression.h"

#include <QskEvent.h>
#include <QskQuick.h>
#include <QskLayoutHint.h>

#include <vector>

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
                    return Term( m_right );

                case Qt::AnchorTop:
                    return Term( m_top );

                case Qt::AnchorVerticalCenter:
                    return centerV();

                case Qt::AnchorBottom:
                    return Term( m_bottom );
            }

            return Expression();
        }

        inline Expression length( Qt::Orientation orientation )
        {
            return ( orientation == Qt::Horizontal ) ? width() : height();
        }

        inline QRectF rect() const
        {
            return QRectF( m_left.value(), m_top.value(),
                m_right.value() - m_left.value(), m_bottom.value() - m_top.value() );
        }

        inline QSizeF size() const
        {
            return QSizeF( m_right.value() - m_left.value(),
                m_bottom.value() - m_top.value() );
        }

        inline Expression width() const { return m_right - m_left; }
        inline Expression height() const { return m_bottom - m_top; }

        inline Expression centerH() const { return m_left + 0.5 * width(); }
        inline Expression centerV() const { return m_top + 0.5 * height(); }

        inline const Variable& left() const { return m_left; }
        inline const Variable& right() const { return m_right; }
        inline const Variable& top() const { return m_top; }
        inline const Variable& bottom() const { return m_bottom; }

      private:
        Variable m_left, m_right, m_top, m_bottom;
    };

    class Anchor
    {
      public:
        QQuickItem* item1 = nullptr;
        Qt::AnchorPoint edge1;

        QQuickItem* item2 = nullptr;
        Qt::AnchorPoint edge2;
    };
}

class AnchorBox::PrivateData
{
  public:
    void setItemGeometries( const AnchorBox*, const QRectF& );

    void setupAnchorConstraints( Solver& );
    void setupSizeConstraints( const AnchorBox*, bool, Solver& );

  public:
    QMap< QQuickItem*, Geometry > geometries;
    QVector< Anchor > anchors;
};

void AnchorBox::PrivateData::setupAnchorConstraints( Solver& solver )
{
    for ( const auto& anchor : anchors )
    {
        auto& r1 = geometries[ anchor.item1 ];
        auto& r2 = geometries[ anchor.item2 ];

        solver.addConstraint( r1.expressionAt( anchor.edge1 )
            == r2.expressionAt( anchor.edge2 ) );

#if 1
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
            solver.addConstraint( c );
        }
#endif
    }
}

void AnchorBox::PrivateData::setupSizeConstraints(
    const AnchorBox* box, bool preferred, Solver& solver )
{
    for ( auto it = geometries.begin(); it != geometries.end(); ++it )
    {
        const auto item = it.key();
        if ( item == box )
            continue;

        auto& r = it.value();

        {
            // minimum size
            const auto minSize = qskSizeConstraint( item, Qt::MinimumSize );

            if ( minSize.width() >= 0.0 )
                solver.addConstraint( r.right() >= r.left() + minSize.width() );

            if ( minSize.height() >= 0.0 )
                solver.addConstraint( r.bottom() >= r.top() + minSize.height() );
        }

        if ( preferred )
        {
            // preferred size
            const auto prefSize = qskSizeConstraint( item, Qt::PreferredSize );

            Constraint c1( r.right() == r.left() + prefSize.width(), Strength::strong );
            solver.addConstraint( c1 );

            Constraint c2( r.bottom() == r.top() + prefSize.height(), Strength::strong );
            solver.addConstraint( c2 );
        }

        {
            // maximum size
            const auto maxSize = qskSizeConstraint( item, Qt::MaximumSize );
            if ( maxSize.width() >= 0.0 )
                solver.addConstraint( r.right() <= r.left() + maxSize.width() );

            if ( maxSize.height() >= 0.0 )
                solver.addConstraint( r.bottom() <= r.top() + maxSize.height() );
        }
    }
}

void AnchorBox::PrivateData::setItemGeometries(
    const AnchorBox* box, const QRectF& rect )
{
    // Unefficient as we are always starting from scratch TODO ...
    Solver solver;
    setupAnchorConstraints( solver );
    setupSizeConstraints( box, true, solver );

    const auto& r0 = geometries[ const_cast< AnchorBox* >( box ) ];

    solver.addConstraint( r0.left() == rect.left() );
    solver.addConstraint( r0.right() == rect.right() );
    solver.addConstraint( r0.top() == rect.top() );
    solver.addConstraint( r0.bottom() == rect.bottom() );

    solver.updateVariables();

    for ( auto it = geometries.begin(); it != geometries.end(); ++it )
        qskSetItemGeometry( it.key(), it.value().rect() );

#if 0
    qDebug() << "=== Rect:" << rect;
    for ( auto it = geometries.begin(); it != geometries.end(); ++it )
        qDebug() << it.key()->objectName() << it.value().rect();
#endif
}

AnchorBox::AnchorBox( QQuickItem* parent )
    : QskControl( parent )
    , m_data( new PrivateData )
{
    (void)m_data->geometries[ this ];
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

    if ( item1 != this )
    {
        if ( item1->parent() == nullptr )
            item1->setParent( this );

        if ( item1->parentItem() != this )
            item1->setParentItem( this );
    }

    if ( item2 != this )
    {
        if ( item2->parent() == nullptr )
            item2->setParent( this );

        if ( item2->parentItem() != this )
            item2->setParentItem( this );
    }

    (void)m_data->geometries[ item1 ];
    (void)m_data->geometries[ item2 ];

    Anchor anchor;
    anchor.item1 = item1;
    anchor.edge1 = edge1;
    anchor.item2 = item2;
    anchor.edge2 = edge2;

    m_data->anchors += anchor;
}

QSizeF AnchorBox::layoutSizeHint( Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( constraint.width() >= 0.0 || constraint.height() >= 0.0 )
    {
        // TODO ...
        return QSizeF();
    }

    const auto& r0 = m_data->geometries[ const_cast< AnchorBox* >( this ) ];

    Solver solver;
    m_data->setupAnchorConstraints( solver );
    m_data->setupSizeConstraints( this, which == Qt::PreferredSize, solver );

    if ( which != Qt::PreferredSize )
    {
        const qreal b = ( which == Qt::MinimumSize ) ? 0.0 : QskLayoutHint::unlimited;

        // why do we need strong here ?
        solver.addConstraint( Constraint( r0.width() == b, Strength::strong ) );
        solver.addConstraint( Constraint( r0.height() == b, Strength::strong ) );
    }

    solver.updateVariables();

    return r0.size();
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
        m_data->setItemGeometries( this, layoutRect() );
}

#include "moc_AnchorBox.cpp"
