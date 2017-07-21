/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPageIndicator.h"
#include "QskPageIndicatorSkinlet.h"
#include "QskAspect.h"

QSK_SUBCONTROL( QskPageIndicator, Panel )
QSK_SUBCONTROL( QskPageIndicator, Bullet )
QSK_SUBCONTROL( QskPageIndicator, Highlighted )

class QskPageIndicator::PrivateData
{
public:
    PrivateData( int count ):
        count ( count ),
        currentIndex( -1 ),
        orientation( Qt::Horizontal )
    {
    }

    int count;
    qreal currentIndex;
    Qt::Orientation orientation : 2;
};

QskPageIndicator::QskPageIndicator( int count, QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData( count ) )
{
    // as we don't stretch the bullets
    setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
}

QskPageIndicator::QskPageIndicator( QQuickItem* parent ):
    QskPageIndicator( 0, parent )
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
    if (orientation != m_data->orientation )
    {
        m_data->orientation = orientation;

        resetImplicitSize();
        update();

        Q_EMIT orientationChanged();
    }
}

void QskPageIndicator::setCount( int count )
{
    if ( count != m_data->count )
    {
        m_data->count = count;

        resetImplicitSize();
        update();

        Q_EMIT countChanged();
    }
}

void QskPageIndicator::setCurrentIndex( qreal index )
{
    if ( index < 0 || index >= m_data->count )
        index = -1;

    if (index != m_data->currentIndex )
    {
        m_data->currentIndex = index;
        update();

        Q_EMIT currentIndexChanged();
    }
}

QSizeF QskPageIndicator::contentsSizeHint() const
{
    using namespace QskAspect;

    const QSizeF sizeBullet = bulletSize( Bullet );
    const QSizeF sizeCurrent = bulletSize( Highlighted );
    const qreal spacing = metric( Panel | Spacing );

    const int n = m_data->count;

    qreal w = 0;
    qreal h = 0;

    if ( m_data->orientation == Qt::Horizontal )
    {
        if ( n > 0 )
        {
            w += qMax( sizeCurrent.width(), sizeBullet.width() );

            if ( n > 1 )
                w += ( n - 1 ) * ( sizeBullet.width() + spacing );

        }

        h = qMax( sizeCurrent.height(), sizeBullet.height() );
    }
    else
    {
        if ( n > 0 )
        {
            h += qMax( sizeCurrent.height(), sizeBullet.height() );

            if ( n > 1 )
                h += ( n - 1 ) * ( sizeBullet.height() + spacing );
        }

        w = qMax( sizeCurrent.width(), sizeBullet.width() );
    }

    const QSizeF minSize(
        metric( Panel | QskAspect::MinimumWidth ),
        metric( Panel | QskAspect::MinimumHeight ) );

    return outerBoxSize( Panel, QSizeF( w, h ) ).expandedTo( minSize );
}

QSizeF QskPageIndicator::bulletSize( QskAspect::Subcontrol subControl ) const
{
    const qreal dim = metric( subControl | QskAspect::Size );
    return QSizeF( dim, dim );
}

#include "moc_QskPageIndicator.cpp"
