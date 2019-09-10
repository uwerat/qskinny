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

#include <vector>

static inline Qt::Orientation qskOrientation( int edge )
{
    return ( edge <= Qt::AnchorRight ) ? Qt::Horizontal : Qt::Vertical;
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
                    return Term( left );

                case Qt::AnchorHorizontalCenter:
                    return centerH();

                case Qt::AnchorRight:
                    return Term( right );

                case Qt::AnchorTop:
                    return Term( top );

                case Qt::AnchorVerticalCenter:
                    return centerV();

                case Qt::AnchorBottom:
                    return Term( bottom );
            }

            return Expression();
        }

        Expression length( Qt::Orientation orientation )
        {
            return ( orientation == Qt::Horizontal ) ? width() : height();
        }

        QRectF rect() const
        {
            return QRectF( left.value(), top.value(),
                right.value() - left.value(), bottom.value() - top.value() );
        }

        Expression width() const { return right - left; }
        Expression height() const { return bottom - top; }

        Expression centerH() const { return left + 0.5 * width(); }
        Expression centerV() const { return top + 0.5 * height(); }

        Variable left, right, top, bottom;
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
    void setupSolver( int type, Solver& );

    QMap< QQuickItem*, Geometry > geometries;
    QVector< Anchor > anchors;
};

void AnchorBox::PrivateData::setupSolver( int type, Solver& solver )
{
    for ( const auto& anchor : anchors )
    {
        auto& r1 = geometries[ anchor.item1 ];
        auto& r2 = geometries[ anchor.item2 ];

        solver.addConstraint( r1.expressionAt( anchor.edge1 )
            == r2.expressionAt( anchor.edge2 ) );

#if 1
        if ( type < 0 )
        {
            const auto s1 = 1.0;
            const auto s2 = 1.0;
            const auto o = qskOrientation( anchor.edge1 );

            Constraint c( r1.length( o ) * s1 == r2.length( o ) * s2, Strength::medium );
            solver.addConstraint( c );
        }
#endif
    }

    for ( auto it = geometries.begin(); it != geometries.end(); ++it )
    {
        const auto item = it.key();
        auto& r = it.value();

        if ( type < 0 || type == Qt::MinimumSize )
        {
            const auto minSize = qskSizeConstraint( item, Qt::MinimumSize );

            if ( minSize.width() >= 0.0 )
                solver.addConstraint( r.right >= r.left + minSize.width() );

            if ( minSize.height() >= 0.0 )
                solver.addConstraint( r.bottom >= r.top + minSize.height() );
        }

        if ( type < 0 || type == Qt::PreferredSize )
        {
            const auto prefSize = qskSizeConstraint( item, Qt::PreferredSize );

            Constraint c1( r.right == r.left + prefSize.width(), Strength::strong );
            solver.addConstraint( c1 );

            Constraint c2( r.bottom == r.top + prefSize.height(), Strength::strong );
            solver.addConstraint( c2 );
        }

        if ( type < 0 || type == Qt::MaximumSize )
        {
            const auto maxSize = qskSizeConstraint( item, Qt::MaximumSize );
            if ( maxSize.width() >= 0.0 )
                solver.addConstraint( r.right <= r.left + maxSize.width() );

            if ( maxSize.height() >= 0.0 )
                solver.addConstraint( r.bottom <= r.top + maxSize.height() );
        }
    }
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

QSizeF AnchorBox::boxHint( Qt::SizeHint which )
{
    Solver solver;
    m_data->setupSolver( which, solver );
    solver.updateVariables();

    auto& r0 = m_data->geometries[ this ];

    return QSizeF( r0.rect().size() );
}

void AnchorBox::setItemGeometries( const QRectF& rect )
{
    Solver solver;
    m_data->setupSolver( -1, solver );

    auto& r0 = m_data->geometries[ this ];

    solver.addConstraint( r0.left == rect.left() );
    solver.addConstraint( r0.right == rect.right() );
    solver.addConstraint( r0.top == rect.top() );
    solver.addConstraint( r0.bottom == rect.bottom() );

    solver.updateVariables();

    const auto& geometries = m_data->geometries;
    for ( auto it = geometries.begin(); it != geometries.end(); ++it )
        qskSetItemGeometry( it.key(), it.value().rect() );

#if 0
    qDebug() << "=== Rect:" << rect;
    for ( auto it = geometries.begin(); it != geometries.end(); ++it )
        qDebug() << it.key()->objectName() << it.value().rect();
#endif
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
        setItemGeometries( layoutRect() );
}

#include "moc_AnchorBox.cpp"
