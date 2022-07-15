/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPushButton.h"
#include "QskAnimationHint.h"
#include "QskAnimator.h"
#include "QskAspect.h"
#include "QskBoxShapeMetrics.h"
#include "QskGraphic.h"
#include "QskGraphicProvider.h"
#include "QskSetup.h"
#include "QskSkin.h"
#include "QskSkinlet.h"
#include "QskTextOptions.h"

#include <qfontmetrics.h>

QSK_SUBCONTROL( QskPushButton, Panel )
QSK_SUBCONTROL( QskPushButton, Ripple )
QSK_SUBCONTROL( QskPushButton, Text )
QSK_SUBCONTROL( QskPushButton, Graphic )

namespace
{
    class ClickAnimator : public QskAnimator
    {
      public:
        ClickAnimator()
            : QskAnimator()
        {
        }

        void setButton( QskPushButton* button )
        {
            m_button = button;
        }

      protected:
        void advance( qreal value ) override
        {
            m_button->setMetric( QskPushButton::Ripple | QskAspect::Size, value );
        }

        void done() override
        {
            m_button->setMetric( QskPushButton::Ripple | QskAspect::Size, 0.0 );
        }

      private:
        QskPushButton* m_button;
    };
}

class QskPushButton::PrivateData
{
  public:
    PrivateData( const QString& txt )
        : text( txt )
        , isCheckable( false )
        , isGraphicSourceDirty( false )
    {
        textOptions.setElideMode( Qt::ElideMiddle );
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
    QskTextOptions textOptions;

    QUrl graphicSource;
    QskGraphic graphic;

    QSizeF graphicSourceSize;

    ClickAnimator clickAnimator;
    qreal rippleSize = 0.0;

    bool isCheckable : 1;
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

    const auto hint = animationHint( Ripple | QskAspect::Color );

    if( hint.isValid() )
    {
        setSkinHint( Ripple | QskAspect::Position, event->pos() );

        m_data->clickAnimator.setWindow( window() );
        m_data->clickAnimator.setButton( this );
        m_data->clickAnimator.setDuration( hint.duration );
        m_data->clickAnimator.setEasingCurve( hint.type );
        m_data->clickAnimator.start();
    }
}

QskGraphic QskPushButton::loadGraphic( const QUrl& url ) const
{
    return Qsk::loadGraphic( url );
}

#include "moc_QskPushButton.cpp"
