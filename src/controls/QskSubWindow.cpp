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
QSK_SUBCONTROL( QskSubWindow, TitleBar )
QSK_SUBCONTROL( QskSubWindow, TitleBarSymbol )
QSK_SUBCONTROL( QskSubWindow, TitleBarText )

class QskSubWindow::PrivateData
{
  public:
    PrivateData()
        : isWindowIconSourceDirty( false )
    {
        // should be available from the platform somehow. TODO ...

        windowButtons = QskSubWindow::WindowButtons( QskSubWindow::MinimizeButton |
            QskSubWindow::MaximizeButton | QskSubWindow::CloseButton );

        windowTitleTextOptions.setElideMode( Qt::ElideRight );
    }

    QskSubWindow::WindowButtons windowButtons;

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

void QskSubWindow::setDecorated( bool on )
{
    if ( on == isDecorated() )
        return;

    const auto subControl = effectiveSubcontrol( QskSubWindow::Panel );
    setFlagHint( subControl | QskAspect::Decoration, on );

    resetImplicitSize(); // in case some parent wants to layout

    polish();
    update();

    Q_EMIT decoratedChanged();
}

bool QskSubWindow::isDecorated() const
{
    return flagHint< bool >( Panel | QskAspect::Decoration, true );
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


void QskSubWindow::setWindowButtons( WindowButtons buttons )
{
    if ( buttons != m_data->windowButtons )
    {
        m_data->windowButtons = buttons;
        update();

        Q_EMIT windowButtonsChanged();
    }
}

QskSubWindow::WindowButtons QskSubWindow::windowButtons() const
{
    return m_data->windowButtons;
}

void QskSubWindow::setWindowButton( WindowButton button, bool on )
{
    if ( on == bool( button & m_data->windowButtons ) )
        return;

    if ( on )
        m_data->windowButtons |= button;
    else
        m_data->windowButtons &= ~button;

    Q_EMIT windowButtonsChanged();
}

bool QskSubWindow::testWindowButton( WindowButton button ) const
{
    return m_data->windowButtons & button;
}

QRectF QskSubWindow::titleBarRect() const
{
    return subControlRect( TitleBar );
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
    QRectF rect = contentsRect();

    const qreal top = rect.top() + subControlRect( size, TitleBar ).height();
    rect.setTop( top );

    return innerBox( Panel, rect );
}

QSizeF QskSubWindow::contentsSizeHint() const
{
    // the size we get from the children
    auto hint = Inherited::contentsSizeHint();

#if 1
    // should be Minimum Width/Height from the hints
    if ( hint.width() < 0 )
        hint.setWidth( qskDpiScaled( 100 ) );

    if ( hint.height() < 0 )
        hint.setHeight( qskDpiScaled( 80 ) );
#endif

    hint = outerBoxSize( Panel, hint );
    hint.setHeight( hint.height() + subControlRect( TitleBar ).height() );

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
