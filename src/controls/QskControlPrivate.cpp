/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskControlPrivate.h"
#include "QskSetup.h"
#include "QskLayoutHint.h"

static inline void qskSendEventTo( QObject* object, QEvent::Type type )
{
    QEvent event( type );
    QCoreApplication::sendEvent( object, &event );
}

static inline QPointF qskScenePosition( const QMouseEvent* event )
{
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    return event->scenePosition();
#else
    return event->windowPos();
#endif
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
    , layoutHints( 0 )
    , layoutAlignmentHint( 0 )
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
    if ( !( explicitSizeHints && explicitSizeHints[ Qt::PreferredSize ].isValid() ) )
    {
        // when we have no explit size, the implicit size matters
        layoutConstraintChanged();
    }
}

QSizeF QskControlPrivate::implicitSizeHint() const
{
    return implicitSizeHint( Qt::PreferredSize, QSizeF() );
}

QSizeF QskControlPrivate::implicitSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    Q_Q( const QskControl );

    /*
        The hint is calculated from the contents ( usually scene graph nodes )
        and - when being a container - the children.
     */
    QSizeF contentsHint;

    {
        const auto m = q->margins();
        const auto dw = m.left() + m.right();
        const auto dh = m.top() + m.bottom();

        if ( constraint.width() >= 0.0 )
        {
            contentsHint.setWidth( qMax( constraint.width() - dw, 0.0 ) );
        }
        else if ( constraint.height() >= 0.0 )
        {
            contentsHint.setHeight( qMax( constraint.height() - dh, 0.0 ) );
        }

        contentsHint = q->contentsSizeHint( which, contentsHint );

        if ( contentsHint.rwidth() >= 0 )
            contentsHint.rwidth() += dw;

        if ( contentsHint.rheight() >= 0 )
            contentsHint.rheight() += dh;
    }

    QSizeF layoutHint;
    {
        if ( constraint.width() >= 0.0 )
        {
            const QSizeF boundingSize( constraint.width(), 1e6 );
            const QSizeF layoutSize = q->layoutRectForSize( boundingSize ).size();

            layoutHint = q->layoutSizeHint( which, QSizeF( layoutSize.width(), -1 ) );

            if ( layoutHint.height() >= 0 )
                layoutHint.rheight() += boundingSize.height() - layoutSize.height();
        }
        else if ( constraint.height() >= 0.0 )
        {
            const QSizeF boundingSize( 1e6, constraint.height() );
            const QSizeF layoutSize = q->layoutRectForSize( boundingSize ).size();

            layoutHint = q->layoutSizeHint( which, QSizeF( -1, layoutSize.height() ) );

            if ( layoutHint.width() >= 0 )
                layoutHint.rwidth() += boundingSize.width() - layoutSize.width();
        }
        else
        {
            /*
                In situations, where layoutRectForSize depends on
                the size ( f.e when using a corner radius of Qt::RelativeSize )
                we will have wrong results. TODO ...
             */
            const QSizeF boundingSize( 1000.0, 1000.0 );
            const QSizeF layoutSize = q->layoutRectForSize( boundingSize ).size();

            layoutHint = q->layoutSizeHint( which, QSizeF() );

            if ( layoutHint.width() >= 0 )
                layoutHint.rwidth() += boundingSize.width() - layoutSize.width();

            if ( layoutHint.height() >= 0 )
                layoutHint.rheight() += boundingSize.height() - layoutSize.height();
        }
    }

    // Combining both hints
    qreal w = constraint.width();
    qreal h = constraint.height();

    if ( w < 0.0 )
        w = QskLayoutHint::combined( which, contentsHint.width(), layoutHint.width() );

    if ( h < 0.0 )
        h = QskLayoutHint::combined( which, contentsHint.height(), layoutHint.height() );

    return QSizeF( w, h );
}

void QskControlPrivate::setExplicitSizeHint(
    Qt::SizeHint whichHint, const QSizeF& size )
{
    if ( explicitSizeHints == nullptr )
        explicitSizeHints = new QSizeF[3];

    explicitSizeHints[ whichHint ] = size;
}

void QskControlPrivate::resetExplicitSizeHint( Qt::SizeHint whichHint )
{
    if ( explicitSizeHints )
        explicitSizeHints[ whichHint ] = QSizeF();
}

QSizeF QskControlPrivate::explicitSizeHint( Qt::SizeHint whichHint ) const
{
    if ( explicitSizeHints )
        return explicitSizeHints[ whichHint ];

    return QSizeF();
}

bool QskControlPrivate::maybeGesture( QQuickItem* child, QEvent* event )
{
    Q_Q( QskControl );

    switch ( event->type() )
    {
        case QEvent::MouseButtonPress:
        {
            const auto mouseEvent = static_cast< const QMouseEvent* >( event );

            auto pos = qskScenePosition( mouseEvent );
            pos = q->mapFromScene( pos );

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
