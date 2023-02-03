/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPushButton.h"
#include "QskAnimationHint.h"
#include "QskAspect.h"
#include "QskBoxShapeMetrics.h"
#include "QskGraphic.h"
#include "QskGraphicProvider.h"
#include "QskSetup.h"
#include "QskSkin.h"
#include "QskSkinlet.h"
#include "QskTextOptions.h"

QSK_SUBCONTROL( QskPushButton, Panel )
QSK_SUBCONTROL( QskPushButton, Ripple )
QSK_SUBCONTROL( QskPushButton, Text )
QSK_SUBCONTROL( QskPushButton, Graphic )

class QskPushButton::PrivateData
{
  public:
    PrivateData( const QString& txt )
        : text( txt )
        , isCheckable( false )
        , isGraphicSourceDirty( false )
        , emphasis( NoEmphasis )
    {
    }

    void ensureGraphic( const QskPushButton* button )
    {
        if ( isGraphicSourceDirty )
        {
            if ( !graphicSource.isEmpty() )
                graphic = button->loadGraphic( graphicSource );

            isGraphicSourceDirty = false;
        }
    }

    QString text;

    QUrl graphicSource;
    QskGraphic graphic;

    bool isCheckable : 1;
    bool isGraphicSourceDirty : 1;
    int emphasis : 4;
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

void QskPushButton::setCheckable( bool on )
{
    if ( on != m_data->isCheckable )
    {
        m_data->isCheckable = on;
        Q_EMIT checkableChanged( on );
    }
}

bool QskPushButton::isCheckable() const
{
    return m_data->isCheckable;
}

void QskPushButton::setEmphasis( Emphasis emphasis )
{
    if ( emphasis != m_data->emphasis )
    {
        m_data->emphasis = emphasis;
        Q_EMIT emphasisChanged( emphasis );
    }
}

QskPushButton::Emphasis QskPushButton::emphasis() const
{
    return static_cast< Emphasis >( m_data->emphasis );
}

void QskPushButton::setShape( const QskBoxShapeMetrics& shape )
{
    if ( setBoxShapeHint( Panel, shape ) )
        Q_EMIT shapeChanged();
}

void QskPushButton::resetShape()
{
    if ( resetBoxShapeHint( Panel ) )
        Q_EMIT shapeChanged();
}

QskBoxShapeMetrics QskPushButton::shape() const
{
    return boxShapeHint( Panel );
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

void QskPushButton::setTextOptions( const QskTextOptions& textOptions )
{
    if ( setTextOptionsHint( Text, textOptions ) )
        Q_EMIT textOptionsChanged();
}

QskTextOptions QskPushButton::textOptions() const
{
    return textOptionsHint( Text );
}

void QskPushButton::resetTextOptions()
{
    if ( resetTextOptionsHint( Text ) )
        Q_EMIT textOptionsChanged();
}

QFont QskPushButton::font() const
{
    return effectiveFont( Text );
}

void QskPushButton::resetGraphicStrutSize()
{
    if ( resetStrutSizeHint( Graphic ) )
        Q_EMIT graphicStrutSizeChanged();
}

void QskPushButton::setGraphicStrutSize( const QSizeF& size )
{
    auto newSize = size;
    if ( newSize.width() < 0.0 )
        newSize.setWidth( -1.0 );

    if ( newSize.height() < 0.0 )
        newSize.setHeight( -1.0 );

    if ( setStrutSizeHint( Graphic, newSize ) )
        Q_EMIT graphicStrutSizeChanged();
}

QSizeF QskPushButton::graphicStrutSize() const
{
    return strutSizeHint( Graphic );
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
    m_data->ensureGraphic( this );
    return m_data->graphic;
}

bool QskPushButton::hasGraphic() const
{
    return !( graphic().isEmpty() && graphicSource().isEmpty() );
}

void QskPushButton::updateResources()
{
    m_data->ensureGraphic( this );
}

QskAspect::Variation QskPushButton::effectiveVariation() const
{
    switch( m_data->emphasis )
    {
        case VeryLowEmphasis:
            return QskAspect::Tiny;

        case LowEmphasis:
            return QskAspect::Small;

        case HighEmphasis:
            return QskAspect::Large;

        case VeryHighEmphasis:
            return QskAspect::Huge;

        default:
            return QskAspect::NoVariation;
    }
}

QRectF QskPushButton::layoutRectForSize( const QSizeF& size ) const
{
    return subControlContentsRect( size, Panel );
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

void QskPushButton::mousePressEvent( QMouseEvent* event )
{
    Inherited::mousePressEvent( event );

    using A = QskAspect;

    const auto hint = animationHint( Ripple | A::Color );
    if( hint.isValid() )
    {
        setSkinHint( Ripple | A::Metric | A::Position, event->pos() );
        startTransition( Ripple | A::Metric | A::Size, hint, 0.0, 1.0 );
    }
}

QskGraphic QskPushButton::loadGraphic( const QUrl& url ) const
{
    return Qsk::loadGraphic( url );
}

#include "moc_QskPushButton.cpp"
