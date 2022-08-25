/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMenu.h"

#include "QskGraphicProvider.h"
#include "QskTextOptions.h"
#include "QskGraphic.h"
#include "QskColorFilter.h"
#include "QskSkinlet.h"
#include "QskEvent.h"
#include "QskPlatform.h"

#include <qvector.h>
#include <qvariant.h>
#include <qeventloop.h>

QSK_SUBCONTROL( QskMenu, Panel )
QSK_SUBCONTROL( QskMenu, Segment )
QSK_SUBCONTROL( QskMenu, Cursor )
QSK_SUBCONTROL( QskMenu, Text )
QSK_SUBCONTROL( QskMenu, Graphic )
QSK_SUBCONTROL( QskMenu, Separator )

QSK_SYSTEM_STATE( QskMenu, Selected, QskAspect::FirstSystemState << 2 )

namespace
{
    class Option
    {
      public:
        Option( const QUrl& graphicSource, const QString& text )
            : graphicSource( graphicSource )
            , text( text )
        {
#if 1
            // lazy loading TODO ...
            if( !graphicSource.isEmpty() )
                graphic = Qsk::loadGraphic( graphicSource );
#endif
        }

        QUrl graphicSource;
        QString text;

        QskGraphic graphic;

#if 0
        // TODO ...
        bool isEnabled = true;
#endif
    };
}

class QskMenu::PrivateData
{
  public:
    QVector< Option > options;
    QVector< int > separators;

    QPointF origin;

    // current/selected are not well defined yet, TODO ...
    int currentIndex = -1;
    int selectedIndex = -1;

    bool isPressed = false;
};

QskMenu::QskMenu( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    setModal( true );
    setOverlay( true );

    setFaderAspect( QskMenu::Panel | QskAspect::Position | QskAspect::Metric );

    setPopupFlag( QskPopup::CloseOnPressOutside, true );
    setPopupFlag( QskPopup::DeleteOnClose, true );

    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
}

QskMenu::~QskMenu()
{
}

#if 1

// has no effect as we do not offer submenus yet. TODO ...
bool QskMenu::isCascading() const
{
    return flagHint( QskMenu::Panel | QskAspect::Style, qskMaybeDesktopPlatform() );
}

void QskMenu::setCascading( bool on )
{
    if ( setFlagHint( QskMenu::Panel | QskAspect::Style, on ) )
        Q_EMIT cascadingChanged( on );
}

void QskMenu::resetCascading()
{
    if ( resetFlagHint( QskMenu::Panel | QskAspect::Style ) )
        Q_EMIT cascadingChanged( isCascading() );
}

#endif

void QskMenu::setOrigin( const QPointF& origin )
{
    if ( origin != m_data->origin )
    {
        m_data->origin = origin;
        Q_EMIT originChanged( origin );
    }
}

QPointF QskMenu::origin() const
{
    return m_data->origin;
}

void QskMenu::addOption( const QUrl& graphicSource, const QString& text )
{
    m_data->options += Option( graphicSource, text );

    resetImplicitSize();
    update();

    if ( isComponentComplete() )
        Q_EMIT countChanged( count() );
}

void QskMenu::addOption( const QString& graphicSource, const QString& text )
{
    addOption( QUrl( graphicSource ), text );
}

int QskMenu::count() const
{
    return m_data->options.count();
}

void QskMenu::addSeparator()
{
    m_data->separators += m_data->options.count();

    resetImplicitSize();
    update();
}

int QskMenu::separatorPosition( int index ) const
{
    return m_data->separators.value( index, -1 );
}

int QskMenu::separatorCount() const
{
    return m_data->separators.count();
}

void QskMenu::clear()
{
    m_data->options.clear();
    m_data->separators.clear();
}

QVariantList QskMenu::optionAt( int index ) const
{
    const auto& options = m_data->options;

    if( index < 0 || index >= options.count() )
        return QVariantList();

    const auto& option = options[ index ];

    QVariantList list;
    list += QVariant::fromValue( option.graphic );
    list += QVariant::fromValue( option.text );

    return list;
}

void QskMenu::setTextOptions( const QskTextOptions& textOptions )
{
    setTextOptionsHint( Text, textOptions );
}

QskTextOptions QskMenu::textOptions() const
{
    return textOptionsHint( Text );
}

void QskMenu::setCurrentIndex( int index )
{
    if( index < 0 || index >= count() )
        index = -1;

    if( index != m_data->currentIndex )
    {
        setPositionHint( Cursor, index );

        m_data->currentIndex = index;
        update();

        Q_EMIT currentIndexChanged( index );
    }
}

int QskMenu::currentIndex() const
{
    return m_data->currentIndex;
}

void QskMenu::keyPressEvent( QKeyEvent* event )
{
    if( m_data->currentIndex < 0 )
    {
        return;
    }

    int key = event->key();

    switch( key )
    {
        case Qt::Key_Up:
        {
            traverse( -1 );
            break;
        }

        case Qt::Key_Down:
        {
            traverse( 1 );
            break;
        }

        case Qt::Key_Select:
        case Qt::Key_Space:
        {
            m_data->isPressed = true;
            return;
        }

        case Qt::Key_Escape:
        case Qt::Key_Cancel:
        {
            setSelectedIndex( -1 );
            return;
        }

        default:
        {
            return;
        }
    }
}

void QskMenu::keyReleaseEvent( QKeyEvent* )
{
    if( m_data->isPressed )
    {
        m_data->isPressed = false;
        setSelectedIndex( m_data->currentIndex );
    }
}

#ifndef QT_NO_WHEELEVENT

void QskMenu::wheelEvent( QWheelEvent* event )
{
    const auto steps = qskWheelSteps( event );
    traverse( -steps );
}

#endif

void QskMenu::traverse( int steps )
{
    if ( count() == 0 || ( steps % count() == 0 ) )
        return;

    auto index = m_data->currentIndex + steps;

    auto newIndex = index % count();
    if ( newIndex < 0 )
        newIndex += count();

    // when cycling we want slide in

    int startIndex = m_data->currentIndex;

    if ( index < 0 )
        startIndex = count();
    else if ( index >= count() )
        startIndex = -1;

    movePositionHint( Cursor, startIndex, newIndex );
    setCurrentIndex( newIndex );
}

void QskMenu::mousePressEvent( QMouseEvent* event )
{
    // QGuiApplication::styleHints()->setFocusOnTouchRelease ??

    if ( event->button() == Qt::LeftButton )
    {
        const auto index = indexAtPosition( qskMousePosition( event ) );
        if ( index >= 0 )
        {
            setCurrentIndex( index );
            m_data->isPressed = true;
        }

        return;
    }

    Inherited::mousePressEvent( event );
}

void QskMenu::mouseReleaseEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton )
    {
        if( m_data->isPressed )
        {
            m_data->isPressed = false;

            const auto index = indexAtPosition( qskMousePosition( event ) );
            if ( index == m_data->currentIndex )
                setSelectedIndex( index );
        }

        return;
    }

    Inherited::mouseReleaseEvent( event );
}

void QskMenu::aboutToShow()
{
    setGeometry( QRectF( m_data->origin, sizeConstraint() ) );

    if ( m_data->currentIndex < 0 )
        setCurrentIndex( 0 );

    Inherited::aboutToShow();
}

QRectF QskMenu::focusIndicatorRect() const
{
    // highlighting the item is good enough
    return QRectF();
}

void QskMenu::setSelectedIndex( int index )
{
    if ( !isOpen() )
        return;

    if ( index >= 0 )
        setCurrentIndex( index );

    m_data->selectedIndex = index;
    Q_EMIT triggered( index );

    close();
}

QRectF QskMenu::cellRect( int index ) const
{
    return effectiveSkinlet()->sampleRect(
        this, contentsRect(), QskMenu::Segment, index );
}

int QskMenu::indexAtPosition( const QPointF& pos ) const
{
    return effectiveSkinlet()->sampleIndexAt(
        this, contentsRect(), QskMenu::Segment, pos );
}

int QskMenu::exec()
{
    m_data->selectedIndex = -1;
    (void) execPopup();
    return m_data->selectedIndex;
}

#include "moc_QskMenu.cpp"
