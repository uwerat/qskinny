/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLayoutConstraint.h"
#include "QskControl.h"
#include "QskSizePolicy.h"

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

bool QskLayoutConstraint::hasDynamicConstraint( const QQuickItem* item )
{
    if ( auto control = qskControlCast( item ) )
    {
        const auto& policy = control->sizePolicy();
        return ( policy.horizontalPolicy() == QskSizePolicy::Constrained ) ||
            ( policy.verticalPolicy() == QskSizePolicy::Constrained );
    }

    return qskHasHintFor( item, "hasHeightForWidth" ) ||
        qskHasHintFor( item, "hasWidthForHeight" );
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
    Type type, const QskControl* control, qreal widthOrHeight )
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
                const auto v = constrainFunction( control, widthOrHeight );
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

QSizeF QskLayoutConstraint::adjustedSize( const QQuickItem* item, const QSizeF& targetSize )
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
