/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPushButton.h"
#include "QskAspect.h"
#include "QskBoxShapeMetrics.h"
#include "QskCorner.h"
#include "QskGraphic.h"
#include "QskGraphicProvider.h"
#include "QskSetup.h"
#include "QskSkin.h"
#include "QskSkinlet.h"
#include "QskTextOptions.h"

#include <qfontmetrics.h>

QSK_SUBCONTROL( QskPushButton, Panel )
QSK_SUBCONTROL( QskPushButton, Text )
QSK_SUBCONTROL( QskPushButton, Graphic )

class QskPushButton::PrivateData
{
  public:
    PrivateData( const QString& txt )
        : text( txt )
        , graphicSourceSize( -1, -1 )
        , isGraphicSourceDirty( false )
    {
        textOptions.setElideMode( Qt::ElideMiddle );
    }

    QString text;
    QskTextOptions textOptions;

    QUrl graphicSource;
    QskGraphic graphic;

    QSizeF graphicSourceSize;

    bool isGraphicSourceDirty : 1;
};

QskPushButton::QskPushButton( QQuickItem* parent )
    : QskPushButton( QString(), parent )
{
}

QskPushButton::QskPushButton( const QString& text, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( text ) )
{
    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Fixed );
}

QskPushButton::~QskPushButton()
{
}

void QskPushButton::setCorner( const QskCorner& corner )
{
    using namespace QskAspect;
    const Aspect aspect = Panel | Shape;

    if ( corner.metrics() != boxShapeHint( aspect ) )
    {
        setBoxShapeHint( aspect, corner.metrics() );

        update();
        Q_EMIT cornerChanged();
    }
}

QskCorner QskPushButton::corner() const
{
    using namespace QskAspect;
    const Aspect aspect = Panel | Shape;

    const auto shape = boxShapeHint( aspect );

#if 1
    QskCorner corner;
    corner.setRadius( shape.radius( Qt::TopLeftCorner ).width() );
    corner.setSizeMode( shape.sizeMode() );
    corner.setAspectRatioMode( shape.aspectRatioMode() );
#endif

    return corner;
}

void QskPushButton::setFlat( bool on )
{
    if ( on != isFlat() )
    {
        setSkinStateFlag( Flat, on );
        Q_EMIT flatChanged();
    }
}

bool QskPushButton::isFlat() const
{
    return skinState() & Flat;
}

void QskPushButton::setText( const QString& text )
{
    if ( text != m_data->text )
    {
        m_data->text = text;

        resetImplicitSize();
        update();

        Q_EMIT textChanged();
    }
}

QString QskPushButton::text() const
{
    return m_data->text;
}

void QskPushButton::setTextOptions( const QskTextOptions& options )
{
    if ( options != m_data->textOptions )
    {
        m_data->textOptions = options;

        resetImplicitSize();
        update();

        Q_EMIT textOptionsChanged();
    }
}

QskTextOptions QskPushButton::textOptions() const
{
    return m_data->textOptions;
}

QFont QskPushButton::font() const
{
    return effectiveFont( QskPushButton::Text );
}

void QskPushButton::resetGraphicSourceSize()
{
    setGraphicSourceSize( QSizeF( -1.0, -1.0 ) );
}

void QskPushButton::setGraphicSourceSize( const QSizeF& size )
{
    auto newSize = size;
    if ( newSize.width() < 0.0 )
        newSize.setWidth( -1.0 );

    if ( newSize.height() < 0.0 )
        newSize.setHeight( -1.0 );

    if ( size != m_data->graphicSourceSize )
    {
        m_data->graphicSourceSize = size;

        resetImplicitSize();
        polish();
        update();

        Q_EMIT graphicSourceSizeChanged();
    }
}

QSizeF QskPushButton::graphicSourceSize() const
{
    return m_data->graphicSourceSize;
}

void QskPushButton::setGraphicSource( const QUrl& url )
{
    if ( m_data->graphicSource == url )
        return;

    m_data->graphicSource = url;
    m_data->graphic.reset();

    m_data->isGraphicSourceDirty = true;

    resetImplicitSize();
    polish();
    update();

    Q_EMIT graphicSourceChanged();
}

void QskPushButton::setGraphicSource( const QString& source )
{
    setGraphicSource( QUrl( source ) );
}

QUrl QskPushButton::graphicSource() const
{
    return m_data->graphicSource;
}

void QskPushButton::setGraphic( const QskGraphic& graphic )
{
    if ( graphic != m_data->graphic )
    {
        m_data->graphic = graphic;

        if ( !m_data->graphicSource.isEmpty() )
        {
            m_data->graphicSource = QString();
            m_data->isGraphicSourceDirty = false;

            Q_EMIT graphicSourceChanged();
        }

        Q_EMIT graphicChanged();

        resetImplicitSize();
        polish();
        update();
    }
}

QskGraphic QskPushButton::graphic() const
{
    return m_data->graphic;
}

bool QskPushButton::hasGraphic() const
{
    return !( graphic().isEmpty() && graphicSource().isEmpty() );
}

void QskPushButton::updateLayout()
{
    if ( m_data->isGraphicSourceDirty )
    {
        if ( !m_data->graphicSource.isEmpty() )
            m_data->graphic = loadGraphic( m_data->graphicSource );

        m_data->isGraphicSourceDirty = false;
    }
}

QRectF QskPushButton::layoutRect() const
{
    return innerBox( Panel, effectiveSkinlet()->subControlRect( this, Panel ) );
}

QSizeF QskPushButton::contentsSizeHint() const
{
    QSizeF size( 0, 0 );

    const QFontMetricsF fm( font() );

    if ( !m_data->text.isEmpty() )
    {
        // in elide mode we might want to ignore the text width ???

        size += fm.size( Qt::TextShowMnemonic, m_data->text );
    }

    if ( m_data->isGraphicSourceDirty )
    {
        if ( !m_data->graphicSource.isEmpty() )
            m_data->graphic = loadGraphic( m_data->graphicSource );
        
        m_data->isGraphicSourceDirty = false;
    }   

    if ( !m_data->graphic.isEmpty() )
    {
        qreal w = m_data->graphicSourceSize.width();
        qreal h = m_data->graphicSourceSize.height();

        if ( ( w < 0.0 ) && ( h < 0.0 ) )
            h = 1.5 * fm.height();

        if ( w < 0 )
            w = m_data->graphic.widthForHeight( h );
        else if ( h < 0 )
            h = m_data->graphic.heightForWidth( w );

        const qreal padding = 2.0; // Graphic::Padding ???

        size.rheight() += 2 * padding + h;
        size.rwidth() = qMax( size.width(), w );
    }

    const QSizeF minSize( metric( Panel | QskAspect::MinimumWidth ),
        metric( Panel | QskAspect::MinimumHeight ) );

    size = size.expandedTo( minSize );
    size = outerBoxSize( Panel, size );

    return size;
}

void QskPushButton::changeEvent( QEvent* event )
{
    switch ( event->type() )
    {
        case QEvent::StyleChange:
        {
            if ( !m_data->graphicSource.isEmpty() &&
                qskSetup->skin()->hasGraphicProvider() )
            {
                // we might need to reload from a different skin
                m_data->isGraphicSourceDirty = true;
            }
            break;
        }
        case QEvent::LocaleChange:
        {
            if ( !m_data->text.isEmpty() )
            {
                // maybe QLocale::textDirection() has changed
                update();
            }

            break;
        }
        default:
            break;
    }

    Inherited::changeEvent( event );
}

void QskPushButton::hoverEnterEvent( QHoverEvent* event )
{
    Inherited::hoverEnterEvent( event );
    Q_EMIT hovered( true );
}

void QskPushButton::hoverLeaveEvent( QHoverEvent* event )
{
    Inherited::hoverLeaveEvent( event );
    Q_EMIT hovered( false );
}

QskGraphic QskPushButton::loadGraphic( const QUrl& url ) const
{
    return Qsk::loadGraphic( url );
}

#include "moc_QskPushButton.cpp"
