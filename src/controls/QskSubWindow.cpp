/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSubWindow.h"
#include "QskAspect.h"
#include "QskFunctions.h"
#include "QskGraphic.h"
#include "QskGraphicProvider.h"
#include "QskTextOptions.h"
#include "QskQuick.h"

#include <qurl.h>

QSK_SUBCONTROL( QskSubWindow, Panel )
QSK_SUBCONTROL( QskSubWindow, TitleBarPanel )
QSK_SUBCONTROL( QskSubWindow, TitleBarSymbol )
QSK_SUBCONTROL( QskSubWindow, TitleBarText )

namespace
{
    inline QskAspect aspectDecoration()
    {
        return QskSubWindow::TitleBarPanel | QskAspect::Flag | QskAspect::Style;
    }
}

class QskSubWindow::PrivateData
{
  public:
    PrivateData()
        : isWindowIconSourceDirty( false )
    {
        windowTitleTextOptions.setElideMode( Qt::ElideRight );
    }

    QString windowTitle;
    QskTextOptions windowTitleTextOptions;

    QUrl windowIconSource;
    QskGraphic windowIcon;

    bool isWindowIconSourceDirty : 1;
};

QskSubWindow::QskSubWindow( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setMargins( 0 );
    initSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::MinimumExpanding );

    setAutoLayoutChildren( true );
}

QskSubWindow::~QskSubWindow()
{
}

void QskSubWindow::setDecorations( Decorations decorations )
{
    if ( setFlagHint( aspectDecoration(), decorations ) )
        Q_EMIT decorationsChanged( decorations );
}

QskSubWindow::Decorations QskSubWindow::decorations() const
{
    return flagHint< QskSubWindow::Decorations >( aspectDecoration() );
}

void QskSubWindow::setDecoration( Decoration decoration, bool on )
{
    auto d = decorations();

    if ( on )
        d |= decoration;
    else
        d &= ~decoration;

    setDecorations( d );
}

void QskSubWindow::resetDecorations()
{
    if ( resetFlagHint( aspectDecoration() ) )
        Q_EMIT decorationsChanged( decorations() );
}

bool QskSubWindow::hasDecoration( Decoration decoration ) const
{
    return decorations() & decoration;
}

void QskSubWindow::setWindowTitle( const QString& title )
{
    if ( m_data->windowTitle != title )
    {
        m_data->windowTitle = title;

        update();
        Q_EMIT windowTitleChanged();
    }
}

QString QskSubWindow::windowTitle() const
{
    return m_data->windowTitle;
}

void QskSubWindow::setWindowTitleTextOptions( const QskTextOptions& options )
{
    if ( options != m_data->windowTitleTextOptions )
    {
        m_data->windowTitleTextOptions = options;

        update();
        Q_EMIT windowTitleTextOptionsChanged();
    }
}

QskTextOptions QskSubWindow::windowTitleTextOptions() const
{
    return m_data->windowTitleTextOptions;
}

void QskSubWindow::setWindowIconSource( const QString& url )
{
    setWindowIconSource( QUrl( url ) );
}

void QskSubWindow::setWindowIconSource( const QUrl& url )
{
    if ( m_data->windowIconSource == url )
        return;

    m_data->windowIconSource = url;
    m_data->windowIcon.reset();

    m_data->isWindowIconSourceDirty = true;

    polish();
    update();

    Q_EMIT windowIconSourceChanged();
}

QUrl QskSubWindow::windowIconSource() const
{
    return m_data->windowIconSource;
}

void QskSubWindow::setWindowIcon( const QskGraphic& graphic )
{
    if ( graphic != m_data->windowIcon )
    {
        m_data->windowIcon = graphic;

        if ( !m_data->windowIconSource.isEmpty() )
        {
            m_data->windowIconSource = QString();
            m_data->isWindowIconSourceDirty = false;

            Q_EMIT windowIconSourceChanged();
        }

        polish();
        update();

        Q_EMIT windowIconChanged();
    }
}

QskGraphic QskSubWindow::windowIcon() const
{
    return m_data->windowIcon;
}

bool QskSubWindow::hasWindowIcon() const
{
    return !( windowIcon().isEmpty() && windowIconSource().isEmpty() );
}

QRectF QskSubWindow::titleBarRect() const
{
    return subControlRect( TitleBarPanel );
}

bool QskSubWindow::event( QEvent* event )
{
    if ( event->type() == QEvent::LayoutRequest )
        resetImplicitSize();

    return Inherited::event( event );
}

void QskSubWindow::updateLayout()
{
    if ( m_data->isWindowIconSourceDirty )
    {
        if ( !m_data->windowIconSource.isEmpty() )
        {
            m_data->windowIcon = Qsk::loadGraphic( m_data->windowIconSource );
            Q_EMIT windowIconChanged();
        }

        m_data->isWindowIconSourceDirty = false;
    }

    Inherited::updateLayout();
}

QRectF QskSubWindow::layoutRectForSize( const QSizeF& size ) const
{
    QRectF rect;
    rect.setSize( size );
    rect.setTop( subControlRect( size, TitleBarPanel ).bottom() );

    return innerBox( Panel, rect );
}

QSizeF QskSubWindow::layoutSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    // the size we get from the children
    auto hint = Inherited::layoutSizeHint( which, constraint );

    if ( which == Qt::PreferredSize )
    {
        // should be Minimum Width/Height from the skin hints
        if ( hint.width() < 0.0 )
            hint.setWidth( qskDpiScaled( 100 ) );

        if ( hint.height() < 0.0 )
            hint.setHeight( qskDpiScaled( 80 ) );
    }

    return hint;
}

void QskSubWindow::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    switch ( change )
    {
        case QQuickItem::ItemChildAddedChange:
        case QQuickItem::ItemChildRemovedChange:
        {
            if ( !qskIsTransparentForPositioner( value.item ) )
            {
                resetImplicitSize();
                polish();
            }
            break;
        }
        default:
            ;
    }
}

#include "moc_QskSubWindow.cpp"
