/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPageIndicator.h"
#include "QskSkinlet.h"
#include "QskEvent.h"

QSK_SUBCONTROL( QskPageIndicator, Panel )
QSK_SUBCONTROL( QskPageIndicator, Bullet )

QSK_SYSTEM_STATE( QskPageIndicator, Selected, QskAspect::FirstSystemState << 1 )

static int qskKeyIncrement(
    const QskPageIndicator* indicator, const QKeyEvent* event )
{
    if ( qskIsStandardKeyInput( event, QKeySequence::MoveToNextChar ) )
        return 1;

    if ( qskIsStandardKeyInput( event, QKeySequence::MoveToPreviousChar ) )
        return -1;

    const auto key = event->key();

    if ( indicator->orientation() == Qt::Horizontal )
    {
        const bool mirrored = indicator->layoutMirroring();

        if ( key == Qt::Key_Left )
            return mirrored ? 1 : -1;

        if ( key == Qt::Key_Right )
            return mirrored ? -1 : 1;
    }
    else
    {
        if ( key == Qt::Key_Up )
            return -1;

        if ( key == Qt::Key_Down )
            return 1;
    }

    return 0;
}


class QskPageIndicator::PrivateData
{
  public:
    PrivateData( int count )
        : count( count )
        , orientation( Qt::Horizontal )
    {
    }

    qreal currentIndex = -1;
    int pressedIndex = -1;

    int count;
    Qt::Orientation orientation : 2;
};

QskPageIndicator::QskPageIndicator( int count, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( count ) )
{
    // as we don't stretch the bullets
    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
}

QskPageIndicator::QskPageIndicator( QQuickItem* parent )
    : QskPageIndicator( 0, parent )
{
}

QskPageIndicator::~QskPageIndicator()
{
}

int QskPageIndicator::count() const
{
    return m_data->count;
}

qreal QskPageIndicator::currentIndex() const
{
    return m_data->currentIndex;
}

Qt::Orientation QskPageIndicator::orientation() const
{
    return m_data->orientation;
}

void QskPageIndicator::setOrientation( Qt::Orientation orientation )
{
    if ( orientation != m_data->orientation )
    {
        m_data->orientation = orientation;

        resetImplicitSize();
        update();

        Q_EMIT orientationChanged( orientation );
    }
}

void QskPageIndicator::setCount( int count )
{
    if ( count != m_data->count )
    {
        m_data->count = count;

        resetImplicitSize();
        update();

        Q_EMIT countChanged( count );
    }
}

void QskPageIndicator::setCurrentIndex( qreal index )
{
    if ( index < 0 || index >= m_data->count )
        index = -1;

    if ( index != m_data->currentIndex )
    {
        m_data->currentIndex = index;
        update();

        Q_EMIT currentIndexChanged( index );
    }
}

qreal QskPageIndicator::valueRatioAt( int index ) const
{
    if ( m_data->currentIndex >= 0.0 && index >= 0 )
    {
        qreal pos = m_data->currentIndex;

        if ( index == 0 && pos > m_data->count - 1 )
            pos -= m_data->count;

        const qreal diff = 1.0 - std::abs( pos - index );
        return std::max( diff, 0.0 );
    }

    return 0.0;
}

QRectF QskPageIndicator::bulletRect( int index ) const
{
    return effectiveSkinlet()->sampleRect(
        this, contentsRect(), QskPageIndicator::Bullet, index );
}

int QskPageIndicator::indexAtPosition( const QPointF& pos ) const
{
    return effectiveSkinlet()->sampleIndexAt(
        this, contentsRect(), QskPageIndicator::Bullet, pos );
}

QskAspect::Placement QskPageIndicator::effectivePlacement() const
{
    return static_cast< QskAspect::Placement >( m_data->orientation );
}

void QskPageIndicator::mousePressEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton )
    {
        /*
            The bullets are usually small and therefore hard to click - with
            touch input almost impossible. It might be better to
            increment in direction of the mouse position ?

            A swipe gesture might make more sense, TODO ...
         */
        const auto pos = qskMousePosition( event );
        m_data->pressedIndex = indexAtPosition( pos );

        return;
    }

    Inherited::mousePressEvent( event );
}

void QskPageIndicator::mouseUngrabEvent()
{
    m_data->pressedIndex = -1;
}

void QskPageIndicator::mouseReleaseEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton )
    {
        const auto index = m_data->pressedIndex;
        m_data->pressedIndex = -1;

        if ( index >= 0 )
        {
            const auto pos = qskMousePosition( event );
            if ( indexAtPosition( pos ) == index )
                Q_EMIT pageRequested( index );
        }

        return;
    }

    Inherited::mouseReleaseEvent( event );
}

void QskPageIndicator::keyPressEvent( QKeyEvent* event )
{
    if ( const int increment = qskKeyIncrement( this, event ) )
    {
        incrementRequested( increment );
        return;
    }

    Inherited::keyPressEvent( event );
}

#ifndef QT_NO_WHEELEVENT

void QskPageIndicator::wheelEvent( QWheelEvent* event )
{
    incrementRequested( qskWheelSteps( event ) );
}

#endif

void QskPageIndicator::incrementRequested( int offset )
{
    const auto n = m_data->count;

    if ( offset == 0 || n == 0 )
        return;

    int index = m_data->currentIndex;
    if ( index < 0 && offset < 0 )
        index = n;

    // do we need a cycling on/off attribute, TODO ...

    index = ( index + offset ) % n;
    if ( index < 0 )
        index += n;

    if ( index != m_data->currentIndex )
        Q_EMIT pageRequested( index );
}

#include "moc_QskPageIndicator.cpp"
