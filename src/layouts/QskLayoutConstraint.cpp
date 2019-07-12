/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLayoutConstraint.h"
#include "QskControl.h"
#include "QskSizePolicy.h"
#include "QskLayoutHint.h"
#include "QskFunctions.h"

#include <functional>

static inline qreal qskHintFor(
    const QQuickItem* item, const char* method, qreal widthOrHeight )
{
    if ( item->metaObject()->indexOfMethod( method ) >= 0 )
    {
        qreal value;

        ( void ) QMetaObject::invokeMethod(
            const_cast< QQuickItem* >( item ), method, Qt::DirectConnection,
            Q_RETURN_ARG( qreal, value ), Q_ARG( qreal, widthOrHeight ) );

        return value;
    }

    return -1;
}

static inline bool qskHasHintFor( const QQuickItem* item, const char* method )
{
    if ( item->metaObject()->indexOfMethod( method ) >= 0 )
    {
        bool enabled;

        ( void ) QMetaObject::invokeMethod( const_cast< QQuickItem* >( item ),
            method, Qt::DirectConnection, Q_RETURN_ARG( bool, enabled ) );

        return enabled;
    }

    return false;
}

static inline qreal qskAdjustedValue(
    QskSizePolicy::Policy policy, qreal value, qreal targetValue )
{
    if ( targetValue > value )
    {
        if ( policy & QskSizePolicy::GrowFlag )
            return targetValue;
    }
    else if ( targetValue < value )
    {
        if ( policy & QskSizePolicy::ShrinkFlag )
            return targetValue;
    }

    return value;
}

static inline QSizeF qskExpandedSize( const QQuickItem* item, const QSizeF& constraint )
{
    using namespace QskLayoutConstraint;

    const auto policy = sizePolicy( item );

    QSizeF size = constraint;

    const bool vGrow = ( policy.policy( Qt::Vertical ) & QskSizePolicy::GrowFlag );
    const bool hGrow = ( policy.policy( Qt::Horizontal ) & QskSizePolicy::GrowFlag );

    if ( !vGrow || !hGrow )
    {
        const auto hint = sizeHint( item, Qt::PreferredSize, constraint );

        if ( !vGrow )
            size.setHeight( hint.height() );

        if ( !hGrow )
            size.setWidth( hint.width() );
    }

    if ( !size.isValid() )
    {
        const auto hint = sizeHint( item, Qt::MaximumSize, size );

        if ( size.width() == -1 )
            size.setWidth( hint.width() );

        if ( size.height() == -1 )
            size.setHeight( hint.height() );
    }

    return size;
}

QskLayoutConstraint::Type QskLayoutConstraint::constraintType( const QQuickItem* item )
{
    if ( item == nullptr )
        return Unconstrained;

    Type constraintType = Unconstrained;

    if ( auto control = qskControlCast( item ) )
    {
        const auto policy = control->sizePolicy();
        if ( policy.horizontalPolicy() == QskSizePolicy::Constrained )
        {
            constraintType = WidthForHeight;
        }
        else if ( policy.verticalPolicy() == QskSizePolicy::Constrained )
        {
            constraintType = HeightForWidth;
        }
    }
    else
    {
        if ( qskHasHintFor( item, "hasWidthForHeight" ) )
        {
            constraintType = WidthForHeight;
        }
        else if ( qskHasHintFor( item, "hasHeightForWidth" ) )
        {
            constraintType = HeightForWidth;
        }
    }

    return constraintType;
}

bool QskLayoutConstraint::isConstrained(
    const QQuickItem* item, Qt::Orientation orientation )
{
    switch( constraintType( item ) )
    {
        case QskLayoutConstraint::WidthForHeight:
            return orientation == Qt::Horizontal;

        case QskLayoutConstraint::HeightForWidth:
            return orientation == Qt::Vertical;

        default:
            return false;
    }
}

qreal QskLayoutConstraint::heightForWidth( const QQuickItem* item, qreal width )
{
    if ( auto control = qskControlCast( item ) )
        return control->heightForWidth( width );

    return qskHintFor( item, "heightForWidth", width );
}

qreal QskLayoutConstraint::widthForHeight( const QQuickItem* item, qreal height )
{
    if ( auto control = qskControlCast( item ) )
        return control->widthForHeight( height );

    return qskHintFor( item, "widthForHeight", height );
}

qreal QskLayoutConstraint::constrainedMetric(
    Type type, const QskControl* control, qreal widthOrHeight,
    std::function< qreal( Type, const QskControl*, qreal ) > constrainFunction )
{
#if 1
    /*
        In case of having a corner radius of Qt::RelativeSize the margins
        we might have a wrong result when using QskLayoutConstraint::unlimited.
        No idea how to solve this in a generic way: TODO ...
     */
#endif

    const qreal upperLimit = 1e6;

    if ( type == WidthForHeight )
    {
        const QSizeF outer( upperLimit, widthOrHeight );
        const QSizeF inner = control->layoutRectForSize( outer ).size();

        qreal width = constrainFunction( type, control, inner.height() );

        if ( width >= 0.0 )
            width += outer.width() - inner.width();

        return width;
    }
    else
    {
        const QSizeF outer( widthOrHeight, upperLimit );
        const QSizeF inner = control->layoutRectForSize( outer ).size();

        qreal height = constrainFunction( type, control, inner.width() );

        if ( height >= 0.0 )
            height += outer.height() - inner.height();

        return height;
    }
}

qreal QskLayoutConstraint::constrainedChildrenMetric(
    Type type, const QskControl* control, qreal constraint )
{
    auto constrainFunction =
        ( type == WidthForHeight ) ? widthForHeight : heightForWidth;

    qreal constrainedValue = -1.0;

    const auto children = control->childItems();
    for ( auto child : children )
    {
        if ( auto control = qskControlCast( child ) )
        {
            if ( !control->isTransparentForPositioner() )
            {
                const auto v = constrainFunction( control, constraint );
                if ( v > constrainedValue )
                    constrainedValue = v;
            }
        }
    }

    return constrainedValue;
}

QSizeF QskLayoutConstraint::effectiveConstraint(
    const QQuickItem* item, Qt::SizeHint whichHint )
{
    if ( auto control = qskControlCast( item ) )
        return control->effectiveSizeHint( whichHint );

    QSizeF constraint( -1.0, -1.0 ); // no hint

    const char* properties[] =
    {
        "minimumSize",
        "preferredSize",
        "maximumSize"
    };

    const QVariant v = item->property( properties[ whichHint ] );
    if ( v.canConvert( QMetaType::QSizeF ) )
        constraint = v.toSizeF();

    switch ( whichHint )
    {
        case Qt::MinimumSize:
        {
            if ( constraint.width() < 0 )
                constraint.setWidth( 0.0 );

            if ( constraint.height() < 0 )
                constraint.setHeight( 0.0 );

            break;
        }

        case Qt::PreferredSize:
        {
            if ( constraint.width() < 0 )
                constraint.setWidth( item->implicitWidth() );

            if ( constraint.height() < 0 )
                constraint.setHeight( item->implicitHeight() );

            break;
        }
        case Qt::MaximumSize:
        {
            if ( constraint.width() < 0 )
                constraint.setWidth( unlimited );

            if ( constraint.height() < 0 )
                constraint.setHeight( unlimited );

            break;
        }
        default:
            break;
    }

    return constraint;
}

QskSizePolicy QskLayoutConstraint::sizePolicy( const QQuickItem* item )
{
    if ( item )
    {
        if ( auto control = qskControlCast( item ) )
            return control->sizePolicy();

        const QVariant v = item->property( "sizePolicy" );
        if ( v.canConvert< QskSizePolicy >() )
            return qvariant_cast< QskSizePolicy >( v );
    }

    return QskSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Preferred );
}

QSizeF QskLayoutConstraint::boundedSize( const QQuickItem* item, const QSizeF& size )
{
    const auto minSize = effectiveConstraint( item, Qt::MinimumSize );
    const auto maxSize = effectiveConstraint( item, Qt::MaximumSize );

    qreal width = size.width();
    qreal height = size.height();

    if ( ( minSize.width() >= 0 ) && ( minSize.width() > width ) )
        width = minSize.width();

    if ( ( minSize.height() >= 0 ) && ( minSize.height() > height ) )
        height = minSize.height();

    if ( ( maxSize.width() >= 0 ) && ( maxSize.width() < width ) )
        width = maxSize.width();

    if ( ( maxSize.height() >= 0 ) && ( maxSize.height() < height ) )
        height = maxSize.height();

    return QSizeF( width, height );
}

QSizeF QskLayoutConstraint::adjustedSize(
    const QQuickItem* item, const QSizeF& targetSize )
{
    const auto policy = sizePolicy( item );

    const auto boundedSize = QskLayoutConstraint::boundedSize( item, targetSize );
    const auto preferredSize = effectiveConstraint( item, Qt::PreferredSize );

    qreal w;
    qreal h;

    if ( policy.horizontalPolicy() == QskSizePolicy::Constrained )
    {
        h = qskAdjustedValue( policy.verticalPolicy(),
            preferredSize.height(), boundedSize.height() );

        w = widthForHeight( item, h );

        if ( w < boundedSize.height() )
            w = boundedSize.height();
    }
    else if ( policy.verticalPolicy() == QskSizePolicy::Constrained )
    {
        w = qskAdjustedValue( policy.horizontalPolicy(),
            preferredSize.width(), boundedSize.width() );

        h = heightForWidth( item, w );

        if ( h < boundedSize.height() )
            h = boundedSize.height();
    }
    else
    {
        w = qskAdjustedValue( policy.horizontalPolicy(),
            preferredSize.width(), boundedSize.width() );

        h = qskAdjustedValue( policy.verticalPolicy(),
            preferredSize.height(), boundedSize.height() );
    }

    return QSizeF( w, h );
}

qreal QskLayoutConstraint::sizeHint( const QQuickItem* item,
    Qt::SizeHint whichHint, Qt::Orientation orientation, qreal constraint )
{
    if ( orientation == Qt::Horizontal )
    {
        return sizeHint( item, whichHint, QSizeF( -1.0, constraint ) ).width();
    }
    else
    {
        return sizeHint( item, whichHint, QSizeF( constraint, -1.0 ) ).height();
    }
}

QSizeF QskLayoutConstraint::sizeHint( const QQuickItem* item,
    Qt::SizeHint whichHint, const QSizeF& constraint )
{
    if ( item == nullptr || whichHint < Qt::MinimumSize || whichHint > Qt::MaximumSize )
        return QSizeF( 0, 0 );

    QSizeF hint( 0, 0 );

    Type constraintType = Unconstrained;

    if ( whichHint == Qt::PreferredSize )
        constraintType = QskLayoutConstraint::constraintType( item );

    if ( constraintType != Unconstrained )
    {
        const quint32 growFlags = QskSizePolicy::GrowFlag | QskSizePolicy::ExpandFlag;

        if ( constraint.width() > 0 ) // && constrainedType == HeightForWidth ??
        {
            qreal w = constraint.width();

            if ( !( sizePolicy( item ).policy( Qt::Horizontal ) & growFlags ) )
            {
                const auto maxW = effectiveConstraint( item, Qt::PreferredSize ).width();

                if ( maxW >= 0.0 )
                    w = qMin( w, maxW );
            }

            hint.setWidth( w );
            hint.setHeight( heightForWidth( item, w ) );
        }
        else if ( constraint.height() > 0 ) // && constrainedType == WidthForHeight ??
        {
            qreal h = constraint.height();

            if ( !( sizePolicy( item ).policy( Qt::Vertical ) & growFlags ) )
            {
                const auto maxH = effectiveConstraint( item, Qt::PreferredSize ).height();

                if ( maxH >= 0.0 )
                    h = qMin( h, maxH );
            }

            hint.setWidth( widthForHeight( item, h ) );
            hint.setHeight( h );
        }
        else
        {
            hint = effectiveConstraint( item, Qt::PreferredSize );

            if ( constraintType == WidthForHeight )
                hint.setWidth( widthForHeight( item, hint.height() ) );
            else
                hint.setHeight( heightForWidth( item, hint.width() ) );
        }
    }
    else
    {
        hint = effectiveConstraint( item, whichHint );
    }

    hint = hint.expandedTo( QSizeF( 0.0, 0.0 ) );

    return hint;
}

QRectF QskLayoutConstraint::itemRect( const QQuickItem* item,
    const QRectF& rect, Qt::Alignment alignment )
{
    QSizeF size = qskExpandedSize( item, QSizeF( -1, -1 ) );

    switch( constraintType( item ) )
    {
        case HeightForWidth:
        {
            if ( size.width() > rect.width() )
                size = qskExpandedSize( item, QSizeF( rect.width(), -1 ) );

            break;
        }
        case WidthForHeight:
        {
            if ( size.height() > rect.height() )
                size = qskExpandedSize( item, QSizeF( -1, rect.height() ) );

            break;
        }
        default:
            break;
    }

    size = size.boundedTo( rect.size() );

    return qskAlignedRectF( rect, size.width(), size.height(), alignment );
}

QskLayoutHint QskLayoutConstraint::layoutHint(
    const QQuickItem* item, Qt::Orientation orientation, qreal constraint )
{
    if ( item == nullptr )
        return QskLayoutHint();

    const auto policy = sizePolicy( item ).policy( orientation );

    if ( constraint >= 0.0 )
    {
        if ( !isConstrained( item, orientation ) )
            constraint = -1.0;
    }

    qreal minimum, preferred, maximum;

    const auto expandFlags = QskSizePolicy::GrowFlag | QskSizePolicy::ExpandFlag;

    if ( ( policy & QskSizePolicy::ShrinkFlag ) &&
        ( policy & expandFlags ) && ( policy & QskSizePolicy::IgnoreFlag ) )
    {
        // we don't need to calculate the preferred size

        minimum = sizeHint( item, Qt::MinimumSize, orientation, constraint );
        maximum = sizeHint( item, Qt::MaximumSize, orientation, constraint );
        preferred = minimum;
    }
    else
    {
        preferred = sizeHint( item, Qt::PreferredSize, orientation, constraint );

        if ( policy & QskSizePolicy::ShrinkFlag )
            minimum = sizeHint( item, Qt::MinimumSize, orientation, constraint );
        else
            minimum = preferred;

        if ( policy & expandFlags )
            maximum = sizeHint( item, Qt::MaximumSize, orientation, constraint );
        else
            maximum = preferred;

        if ( policy & QskSizePolicy::IgnoreFlag )
            preferred = minimum;
    }

    return QskLayoutHint( minimum, preferred, maximum );
}
