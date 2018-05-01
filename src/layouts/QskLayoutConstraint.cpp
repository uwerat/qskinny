/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLayoutConstraint.h"
#include "QskControl.h"
#include "QskSizePolicy.h"

#include <limits>

static inline qreal qskHintFor( const QQuickItem* item,
    const char* method, qreal widthOrHeight )
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

bool QskLayoutConstraint::hasDynamicConstraint( const QQuickItem* item )
{
    if ( const QskControl* control = qobject_cast< const QskControl* >( item ) )
    {
        const QskSizePolicy& policy = control->sizePolicy();
        return ( policy.horizontalPolicy() == QskSizePolicy::Constrained ) ||
            ( policy.verticalPolicy() == QskSizePolicy::Constrained );
    }

    return qskHasHintFor( item, "hasHeightForWidth" ) ||
        qskHasHintFor( item, "hasWidthForHeight" );
}

qreal QskLayoutConstraint::heightForWidth( const QQuickItem* item, qreal width )
{
    if ( const QskControl* control = qobject_cast< const QskControl* >( item ) )
        return control->heightForWidth( width );

    return qskHintFor( item, "heightForWidth", width );
}

qreal QskLayoutConstraint::widthForHeight( const QQuickItem* item, qreal height )
{
    if ( const QskControl* control = qobject_cast< const QskControl* >( item ) )
        return control->widthForHeight( height );

    return qskHintFor( item, "widthForHeight", height );
}

QSizeF QskLayoutConstraint::effectiveConstraint(
    const QQuickItem* item, Qt::SizeHint whichHint )
{
    if ( const QskControl* control = qobject_cast< const QskControl* >( item ) )
        return control->effectiveSizeHint( whichHint );

    QSizeF constraint( -1.0, -1.0 ); // no hint

    const char* properties[] =
    {
        "minimumSize",
        "preferredSize",
        "maximumSize"
    };

    const QVariant v = item->property( properties[whichHint] );
    if ( v.canConvert( QMetaType::QSizeF ) )
        constraint = v.toSizeF();

    switch( whichHint )
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
        if ( auto control = qobject_cast< const QskControl* >( item ) )
            return control->sizePolicy();

        const QVariant v = item->property( "sizePolicy" );
        if ( v.canConvert< QskSizePolicy >() )
            return qvariant_cast< QskSizePolicy >( v );
    }

    return QskSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Preferred );
}


