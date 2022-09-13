/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTabButton.h"
#include "QskSkinlet.h"
#include "QskTabBar.h"
#include "QskTextOptions.h"
#include "QskQuick.h"

#include <qfontmetrics.h>
#include <qpointer.h>

QSK_SUBCONTROL( QskTabButton, Panel )
QSK_SUBCONTROL( QskTabButton, Text )

static inline QskTabBar* qskFindTabBar( const QskTabButton* button )
{
    return qskFindAncestorOf< QskTabBar* >( button->parentItem() );
}

class QskTabButton::PrivateData
{
  public:
    PrivateData( const QString& txt )
        : text( txt )
    {
    }

    QString text;
    QPointer< QskTabBar > tabBar;
};

QskTabButton::QskTabButton( QQuickItem* parent )
    : QskTabButton( QString(), parent )
{
}

QskTabButton::QskTabButton( const QString& text, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( text ) )
{
    if ( parent )
        m_data->tabBar = qskFindTabBar( this );

    initSizePolicy( QskSizePolicy::MinimumExpanding,
        QskSizePolicy::QskSizePolicy::MinimumExpanding );

    setExclusive( true );
}

QskTabButton::~QskTabButton()
{
}

bool QskTabButton::isCheckable() const
{
    return true;
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

void QskTabButton::setTextOptions( const QskTextOptions& textOptions )
{
    if ( setTextOptionsHint( Text, textOptions ) )
        Q_EMIT textOptionsChanged();
}

QskTextOptions QskTabButton::textOptions() const
{
    return textOptionsHint( Text );
}

void QskTabButton::resetTextOptions()
{
    if ( resetTextOptionsHint( Text ) )
        Q_EMIT textOptionsChanged();
}

QRectF QskTabButton::layoutRectForSize( const QSizeF& size ) const
{
    return subControlContentsRect( size, Panel );
}

QskAspect::Placement QskTabButton::effectivePlacement() const
{
    if ( m_data->tabBar )
        return m_data->tabBar->effectivePlacement();

    return QskAspect::NoPlacement;
}

const QskTabBar* QskTabButton::tabBar() const
{
    return m_data->tabBar;
}

QskTabBar* QskTabButton::tabBar()
{
    return m_data->tabBar;
}

void QskTabButton::changeEvent( QEvent* event )
{
    switch ( event->type() )
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
            m_data->tabBar = qskFindTabBar( this );
            break;
        }
        default:
            break;
    }

    Inherited::changeEvent( event );
}

#include "moc_QskTabButton.cpp"
