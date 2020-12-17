/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPageIndicator.h"
#include "QskAspect.h"

QSK_SUBCONTROL( QskPageIndicator, Panel )
QSK_SUBCONTROL( QskPageIndicator, Bullet )
QSK_SUBCONTROL( QskPageIndicator, Highlighted )

class QskPageIndicator::PrivateData
{
  public:
    PrivateData( int count )
        : count( count )
        , currentIndex( -1 )
        , orientation( Qt::Horizontal )
    {
    }

    int count;
    qreal currentIndex;
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

QSizeF QskPageIndicator::contentsSizeHint(
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto bulletSize = strutSizeHint( Bullet );
    const auto maxSize = bulletSize.expandedTo( strutSizeHint( Highlighted ) );

    const qreal spacing = spacingHint( Panel );

    const int n = m_data->count;

    qreal w = 0;
    qreal h = 0;

    if ( m_data->orientation == Qt::Horizontal )
    {
        if ( n > 0 )
        {
            w += maxSize.width();

            if ( n > 1 )
                w += ( n - 1 ) * ( bulletSize.width() + spacing );
        }

        h = maxSize.height();
    }
    else
    {
        if ( n > 0 )
        {
            h += maxSize.height();

            if ( n > 1 )
                h += ( n - 1 ) * ( bulletSize.height() + spacing );
        }

        w = maxSize.width();
    }

    const auto hint = outerBoxSize( Panel, QSizeF( w, h ) );
    return hint.expandedTo( strutSizeHint( Panel ) );
}

#include "moc_QskPageIndicator.cpp"
