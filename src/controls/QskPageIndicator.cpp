/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPageIndicator.h"
#include "QskAspect.h"

QSK_SUBCONTROL( QskPageIndicator, Panel )
QSK_SUBCONTROL( QskPageIndicator, Bullet )

QSK_SYSTEM_STATE( QskPageIndicator, Selected, QskAspect::FirstSystemState << 1 )

class QskPageIndicator::PrivateData
{
  public:
    PrivateData( int count )
        : count( count )
        , interactive( false )
        , orientation( Qt::Horizontal )
    {
    }

    qreal currentIndex = -1;

    int count;

    bool interactive : 1;
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

bool QskPageIndicator::isInteractive() const
{
    return m_data->interactive;
}

void QskPageIndicator::setInteractive( bool on )
{
    if ( on != m_data->interactive )
    {
        m_data->interactive = on;

        // this flag might have an impact on its representation
        resetImplicitSize();
        update();

        Q_EMIT interactiveChanged( on );
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

QskAspect::Placement QskPageIndicator::effectivePlacement() const
{
    return static_cast< QskAspect::Placement >( m_data->orientation );
}

#include "moc_QskPageIndicator.cpp"
