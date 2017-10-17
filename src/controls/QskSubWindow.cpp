/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSubWindow.h"
#include "QskAspect.h"
#include "QskSkin.h"
#include "QskFunctions.h"

#include <QQuickWindow>
#include <QtMath>

QSK_SUBCONTROL( QskSubWindow, Panel )
QSK_SUBCONTROL( QskSubWindow, TitleBar )

class QskSubWindow::PrivateData
{
public:
    PrivateData()
    {
        // should be available from the platform somehow. TODO ...

        windowButtons = QskSubWindow::WindowButtons( QskSubWindow::MinimizeButton |
            QskSubWindow::MaximizeButton | QskSubWindow::CloseButton );
    }

    QskSubWindow::WindowButtons windowButtons;
    QString title;
};

QskSubWindow::QskSubWindow( QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
    setMargins( 0 );
    initSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::MinimumExpanding );

    setAutoLayoutChildren( true );
}

QskSubWindow::~QskSubWindow()
{
}

bool QskSubWindow::isActive() const
{
    // not implemented yet
    return true;
}

void QskSubWindow::setDecorated( bool on )
{
    if ( on == isDecorated() )
        return;

    const auto subControl = effectiveSubcontrol( QskSubWindow::TitleBar );
    setFlagHint( subControl | QskAspect::Decoration, on );

    resetImplicitSize(); // in case some parent wants to layout

    polish();
    update();

    Q_EMIT decoratedChanged();
}

bool QskSubWindow::isDecorated() const
{
    return flagHint< bool >( TitleBar | QskAspect::Decoration, true );
}

void QskSubWindow::setTitle( const QString& title )
{
    if ( m_data->title != title )
    {
        m_data->title = title;
        Q_EMIT titleChanged();
    }
}

QString QskSubWindow::title() const
{
    return m_data->title;
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

QRectF QskSubWindow::layoutRect() const
{
    QRectF rect = contentsRect();

    const qreal top = rect.top() + subControlRect( TitleBar ).height();
    rect.setTop( qMin( top, rect.bottom() ) );

    // wrong: the Padding would be above the header: TODO
    return innerBox( Panel, rect );
}

void QskSubWindow::updateLayout()
{
    if ( !isInitiallyPainted() )
        setFocus( true );

    Inherited::updateLayout();
}

QSizeF QskSubWindow::contentsSizeHint() const
{
    qreal w = -1;
    qreal h = -1;

    for ( auto child : childItems() )
    {
        if ( isTransparentForPositioner( child ) )
            continue;

        const QskControl* control = qobject_cast< QskControl* >( child );
        if ( control )
        {
            const QSizeF sz = control->sizeHint();

            w = qMax( w, sz.width() );
            h = qMax( h, sz.height() );
        }
    }

#if 1
    // should be Minimum Width/Height from the hints
    if ( w < 0 )
        w = qskDpiScaled( 100 );

    if ( h < 0 )
        h = qskDpiScaled( 80 );
#endif

    QSizeF hint = outerBoxSize( Panel, QSizeF( w, h ) );
    hint.setHeight( hint.height() + titleBarRect().height() );

    return hint;
}

void QskSubWindow::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    switch( change )
    {
        case QQuickItem::ItemChildAddedChange:
        case QQuickItem::ItemChildRemovedChange:
        {
            if ( !isTransparentForPositioner( value.item ) )
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
