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
#include "QskEvent.h"

QSK_SUBCONTROL( QskPushButton, Panel )
QSK_SUBCONTROL( QskPushButton, Splash )
QSK_SUBCONTROL( QskPushButton, Text )
QSK_SUBCONTROL( QskPushButton, Icon )

class QskPushButton::PrivateData
{
  public:
    PrivateData( const QString& txt )
        : text( txt )
        , isCheckable( false )
        , isIconSourceDirty( false )
        , emphasis( NoEmphasis )
    {
    }

    void ensureIcon( const QskPushButton* button )
    {
        if ( isIconSourceDirty )
        {
            if ( !iconSource.isEmpty() )
                icon = button->loadIcon( iconSource );

            isIconSourceDirty = false;
        }
    }

    QString text;

    QUrl iconSource;
    QskGraphic icon;

    bool isCheckable : 1;
    bool isIconSourceDirty : 1;
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

void QskPushButton::resetIconStrutSize()
{
    if ( resetStrutSizeHint( Icon ) )
        Q_EMIT iconStrutSizeChanged();
}

void QskPushButton::setIconStrutSize( const QSizeF& size )
{
    auto newSize = size;
    if ( newSize.width() < 0.0 )
        newSize.setWidth( -1.0 );

    if ( newSize.height() < 0.0 )
        newSize.setHeight( -1.0 );

    if ( setStrutSizeHint( Icon, newSize ) )
        Q_EMIT iconStrutSizeChanged();
}

QSizeF QskPushButton::iconStrutSize() const
{
    return strutSizeHint( Icon );
}

void QskPushButton::setIconSource( const QUrl& url )
{
    if ( m_data->iconSource == url )
        return;

    m_data->iconSource = url;
    m_data->icon.reset();

    m_data->isIconSourceDirty = true;

    resetImplicitSize();
    polish();
    update();

    Q_EMIT iconSourceChanged();
}

void QskPushButton::setIconSource( const QString& source )
{
    setIconSource( QUrl( source ) );
}

QUrl QskPushButton::iconSource() const
{
    return m_data->iconSource;
}

void QskPushButton::setIcon( const QskGraphic& icon )
{
    if ( icon != m_data->icon )
    {
        m_data->icon = icon;

        if ( !m_data->iconSource.isEmpty() )
        {
            m_data->iconSource = QString();
            m_data->isIconSourceDirty = false;

            Q_EMIT iconSourceChanged();
        }

        Q_EMIT iconChanged();

        resetImplicitSize();
        polish();
        update();
    }
}

QskGraphic QskPushButton::icon() const
{
    m_data->ensureIcon( this );
    return m_data->icon;
}

bool QskPushButton::hasIcon() const
{
    return !( icon().isEmpty() && iconSource().isEmpty() );
}

void QskPushButton::updateResources()
{
    m_data->ensureIcon( this );
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
            if ( !m_data->iconSource.isEmpty() &&
                qskSetup->skin()->hasGraphicProvider() )
            {
                // we might need to reload from a different skin
                m_data->isIconSourceDirty = true;
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

    const auto hint = animationHint( Splash | A::Color );
    if( hint.isValid() )
    {
        setPositionHint( Splash, qskMousePosition( event ).x() );
        startTransition( Splash | A::Metric | A::Size, hint, 0.0, 1.0 );
    }
}

QskGraphic QskPushButton::loadIcon( const QUrl& url ) const
{
    return Qsk::loadGraphic( url );
}

#include "moc_QskPushButton.cpp"
