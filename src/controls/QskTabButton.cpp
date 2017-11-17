/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTabButton.h"
#include "QskTabBar.h"
#include "QskTextOptions.h"
#include "QskSkinlet.h"

#include <QFontMetricsF>
#include <QPointer>

QSK_SUBCONTROL( QskTabButton, Panel )
QSK_SUBCONTROL( QskTabButton, Text )

class QskTabButton::PrivateData
{
public:
    PrivateData( const QString& txt ):
        text( txt )
    {
    }

    QString text;
    QskTextOptions textOptions;
    QPointer< QskTabBar > tabBar;
};

QskTabButton::QskTabButton( QQuickItem* parent ):
    QskTabButton( QString(), parent )
{
}

QskTabButton::QskTabButton( const QString& text, QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData( text ) )
{
    resolveTabBar();

    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Fixed );

    setCheckable( true );
    setExclusive( true );
}

QskTabButton::~QskTabButton()
{
}

void QskTabButton::setText( const QString& text )
{
    if ( m_data->text == text )
        return;

    m_data->text = text;
    Q_EMIT textChanged( text );

    resetImplicitSize();
    update();
}

QString QskTabButton::text() const
{
    return m_data->text;
}

void QskTabButton::setTextOptions( const QskTextOptions& options )
{
    if ( options != m_data->textOptions )
    {
        m_data->textOptions = options;
        Q_EMIT textOptionsChanged();
    }
}

QskTextOptions QskTabButton::textOptions() const
{
    return m_data->textOptions;
}

QSizeF QskTabButton::contentsSizeHint() const
{
    QSizeF size( metric( Panel | QskAspect::MinimumWidth ),
        metric( Panel | QskAspect::MinimumHeight ) );

    if ( !m_data->text.isEmpty() )
    {
        const QFontMetricsF fm( effectiveFont( Text ) );
        const auto textSize = fm.size( Qt::TextShowMnemonic, m_data->text );
        size += textSize;
    }

    return size;
}

QRectF QskTabButton::layoutRect() const
{
    return innerBox( Panel, effectiveSkinlet()->subControlRect( this, Panel ) );
}

QskAspect::Placement QskTabButton::effectivePlacement() const
{
    using namespace QskAspect;

    if ( m_data->tabBar )
    {
        const auto o = m_data->tabBar->orientation();
        return ( o == Qt::Horizontal ) ? Preserved : Transposed;
    }

    return Preserved;
}

QskTabBar* QskTabButton::tabBar() const
{
    return m_data->tabBar;
}

void QskTabButton::changeEvent( QEvent* event )
{
    switch( event->type() )
    {
        case QEvent::LocaleChange:
        {
            if ( !m_data->text.isEmpty() )
            {
                // maybe QLocale::textDirection() has changed
                update();
            }

            break;
        }
        case QEvent::ParentChange:
        {
            resolveTabBar();
            break;
        }
        default:
            break;
    }

    Inherited::changeEvent( event );
}

void QskTabButton::resolveTabBar()
{
    auto p = parent();
    while ( p )
    {
        if ( const auto tabBar = qobject_cast< QskTabBar* >( p ) )
        {
            m_data->tabBar = tabBar;
            break;
        }
        p = p->parent();
    }
}

#include "moc_QskTabButton.cpp"
