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

static inline QSizeF qskEffectiveSizeHint(
    const QQuickItem* item, Qt::SizeHint whichHint )
{
    if ( auto control = qskControlCast( item ) )
        return control->effectiveSizeHint( whichHint );

    QSizeF hint( -1.0, -1.0 ); // no hint

    const char* properties[] =
    {
        "minimumSize",
        "preferredSize",
        "maximumSize"
    };

    const QVariant v = item->property( properties[ whichHint ] );
    if ( v.canConvert( QMetaType::QSizeF ) )
        hint = v.toSizeF();

    switch ( whichHint )
    {
        case Qt::MinimumSize:
        {
            if ( hint.width() < 0 )
                hint.setWidth( 0.0 );

            if ( hint.height() < 0 )
                hint.setHeight( 0.0 );

            break;
        }

        case Qt::PreferredSize:
        {
            if ( hint.width() < 0 )
                hint.setWidth( item->implicitWidth() );

            if ( hint.height() < 0 )
                hint.setHeight( item->implicitHeight() );

            break;
        }
        case Qt::MaximumSize:
        {
            if ( hint.width() < 0 )
                hint.setWidth( QskLayoutConstraint::unlimited );

            if ( hint.height() < 0 )
                hint.setHeight( QskLayoutConstraint::unlimited );

            break;
        }
        default:
            break;
    }

    return hint;
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
        In case of having a corner radius of Qt::RelativeSize
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
    qreal width, height;

    switch( constraintType( item ) )
    {
        case WidthForHeight:
        {
            const auto hintV = layoutHint( item, Qt::Vertical, -1 );
            height = qBound( hintV.minimum(), size.height(), hintV.maximum() );

            const auto hintH = layoutHint( item, Qt::Horizontal, height );
            width = qBound( hintH.minimum(), size.width(), hintH.maximum() );

            break;
        }
        case HeightForWidth:
        {
            const auto hintH = layoutHint( item, Qt::Horizontal, -1 );
            width = qBound( hintH.minimum(), size.width(), hintH.maximum() );

            const auto hintV = layoutHint( item, Qt::Vertical, width );
            height = qBound( hintV.minimum(), size.height(), hintV.maximum() );

            break;
        }
        default:
        {
            const auto hintH = layoutHint( item, Qt::Horizontal, -1 );
            const auto hintV = layoutHint( item, Qt::Vertical, -1 );

            width = qBound( hintH.minimum(), size.width(), hintH.maximum() );
            height = qBound( hintV.minimum(), size.height(), hintV.maximum() );
        }
    }

    return QSizeF( width, height );

}

qreal QskLayoutConstraint::sizeHint( const QQuickItem* item,
    Qt::SizeHint whichHint, Qt::Orientation orientation, qreal constraint )
{
    if ( orientation == Qt::Horizontal )
        return sizeHint( item, whichHint, QSizeF( -1.0, constraint ) ).width();
    else
        return sizeHint( item, whichHint, QSizeF( constraint, -1.0 ) ).height();
}

QSizeF QskLayoutConstraint::sizeHint( const QQuickItem* item,
    Qt::SizeHint whichHint, const QSizeF& constraint )
{
    if ( item == nullptr || whichHint < Qt::MinimumSize || whichHint > Qt::MaximumSize )
        return QSizeF( 0, 0 );

    if ( constraint.isValid() )
        return constraint;

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
                const auto maxW = qskEffectiveSizeHint( item, Qt::PreferredSize ).width();

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
                const auto maxH = qskEffectiveSizeHint( item, Qt::PreferredSize ).height();

                if ( maxH >= 0.0 )
                    h = qMin( h, maxH );
            }

            hint.setWidth( widthForHeight( item, h ) );
            hint.setHeight( h );
        }
        else
        {
            hint = qskEffectiveSizeHint( item, Qt::PreferredSize );

            if ( constraintType == WidthForHeight )
                hint.setWidth( widthForHeight( item, hint.height() ) );
            else
                hint.setHeight( heightForWidth( item, hint.width() ) );
        }
    }
    else
    {
        hint = qskEffectiveSizeHint( item, whichHint );
    }

    hint = hint.expandedTo( QSizeF( 0.0, 0.0 ) );

    return hint;
}

QRectF QskLayoutConstraint::boundedRect( const QQuickItem* item,
    const QRectF& rect, Qt::Alignment alignment )
{
    auto size = boundedSize( item, rect.size() );
#if 0
    size = size.boundedTo( rect.size() ); // ignoring minimumSize
#endif

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
