/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskControlPrivate.h"
#include "QskSetup.h"
#include "QskLayoutConstraint.h"

static inline void qskSendEventTo( QObject* object, QEvent::Type type )
{
    QEvent event( type );
    QCoreApplication::sendEvent( object, &event );
}

/*
    Qt 5.12:
        sizeof( QQuickItemPrivate::ExtraData ) -> 184
        sizeof( QQuickItemPrivate ) -> 320

        ( these numbers include pointers to optional extra data, but not
          the size for the extra data. So the effective memory footprint,
          is often even worse ).

        sizeof( QskControlPrivate ) -> sizeof( QQuickItemPrivate ) + 32
        sizeof( QskSkinnable::PrivateData ) -> 40

    It might be possible to save some bytes, but in the end QskControl
    is heavy simply because of deriving from QQuickItem. So without
    patching Qt the only way to limit the memory footprint of an application
    substantially is to limit the number of QQuickItems.

    That's why QSkinny builds more complex controls from scene graph nodes
    instead of doing QQuickItem composition. As this can only be done
    in C++ it is kind of obvious, why it is often a bad idea to build
    custom controls in QML.
 */

QskControlPrivate::QskControlPrivate()
    : explicitSizeHints( nullptr )
    , sizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Preferred )
    , explicitLocale( false )
    , autoFillBackground( false )
    , autoLayoutChildren( false )
    , focusPolicy( Qt::NoFocus )
    , isWheelEnabled( false )
    , blockLayoutRequestEvents( true )
{
}

QskControlPrivate::~QskControlPrivate()
{
    delete [] explicitSizeHints;
}

void QskControlPrivate::layoutConstraintChanged()
{
    if ( !blockLayoutRequestEvents )
    {
        Inherited::layoutConstraintChanged();

        /*
            We don't send further LayoutRequest events until someone
            actively requests a layout relevant information
         */
        blockLayoutRequestEvents = true;
    }
}

void QskControlPrivate::implicitSizeChanged()
{
    if ( !q_func()->explicitSizeHint( Qt::PreferredSize ).isValid() )
    {
        // when we have no explit size, the implicit size matters
        layoutConstraintChanged();
    }
}

QSizeF QskControlPrivate::implicitSizeHint() const
{
    Q_Q( const QskControl );

    const auto m = q->margins();
    const auto dw = m.left() + m.right();
    const auto dh = m.top() + m.bottom();

    const auto hint = q->contentsSizeHint();

    const qreal w = ( hint.width() >= 0 ) ? dw + hint.width() : -1.0;
    const qreal h = ( hint.height() >= 0 ) ? dh + hint.height() : -1.0;

    return QSizeF( w, h );
}

void QskControlPrivate::setExplicitSizeHint(
    Qt::SizeHint whichHint, const QSizeF& size )
{
    if ( explicitSizeHints == nullptr )
    {
        using namespace QskLayoutConstraint;

        explicitSizeHints = new QSizeF[3];
        explicitSizeHints[0] = defaultSizeHints[0];
        explicitSizeHints[1] = defaultSizeHints[1];
        explicitSizeHints[2] = defaultSizeHints[2];
    }

    explicitSizeHints[ whichHint ] = size;
}

void QskControlPrivate::resetExplicitSizeHint( Qt::SizeHint whichHint )
{
    if ( explicitSizeHints )
    {
        using namespace QskLayoutConstraint;
        explicitSizeHints[ whichHint ] = defaultSizeHints[ whichHint ];
    }
}

QSizeF QskControlPrivate::explicitSizeHint( Qt::SizeHint whichHint ) const
{
    if ( explicitSizeHints )
        return explicitSizeHints[ whichHint ];

    return QskLayoutConstraint::defaultSizeHints[ whichHint ];
}

bool QskControlPrivate::maybeGesture( QQuickItem* child, QEvent* event )
{
    Q_Q( QskControl );

    switch ( event->type() )
    {
        case QEvent::MouseButtonPress:
        {
            const auto mouseEvent = static_cast< const QMouseEvent* >( event );
            const QPointF pos = q->mapFromScene( mouseEvent->windowPos() );

            if ( !q->gestureRect().contains( pos ) )
                return false;

            break;
        }

        case QEvent::MouseMove:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::UngrabMouse:
        case QEvent::TouchBegin:
        case QEvent::TouchCancel:
        case QEvent::TouchUpdate:
            break;

        default:
            return false;
    }

    return q->gestureFilter( child, event );
}

QSGTransformNode* QskControlPrivate::createTransformNode()
{
    return Inherited::createTransformNode();
}

void QskControlPrivate::transformChanged()
{
    Inherited::transformChanged();
}

bool QskControlPrivate::inheritLocale( QskControl* control, const QLocale& locale )
{
    auto d = static_cast< QskControlPrivate* >( QQuickItemPrivate::get( control ) );

    if ( d->explicitLocale || d->locale == locale )
        return true;

    d->locale = locale;
    qskSendEventTo( control, QEvent::LocaleChange );

    return false;
}

void QskControlPrivate::resolveLocale( QskControl* control )
{
    const auto locale = qskSetup->inheritedLocale( control );

    auto d = static_cast< QskControlPrivate* >( QQuickItemPrivate::get( control ) );
    if ( d->locale != locale )
    {
        d->locale = locale;

        qskSendEventTo( control, QEvent::LocaleChange );
        qskSetup->inheritLocale( control, locale );
    }
}
