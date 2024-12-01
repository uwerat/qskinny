/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskProgressIndicator.h"

#include "QskIntervalF.h"
#include "QskFunctions.h"
#include "QskAnimator.h"
#include "QskAspect.h"

QSK_SUBCONTROL( QskProgressIndicator, Groove )
QSK_SUBCONTROL( QskProgressIndicator, Fill )

namespace
{
    class PositionAnimator : public QskAnimator
    {
      public:
        PositionAnimator( QskProgressIndicator* indicator )
            : m_indicator( indicator )
        {
            setAutoRepeat( true );
            setDuration( 1300 );

            setWindow( indicator->window() );
        }

        void advance( qreal value ) override
        {
            if ( m_indicator->setPositionHint( QskProgressIndicator::Fill, value ) )
                m_indicator->update();
        }

      private:
        QskProgressIndicator* m_indicator;
    };
}

class QskProgressIndicator::PrivateData
{
  public:
    void updateIndeterminateAnimator( QskProgressIndicator* indicator )
    {
        if ( !isIndeterminate )
        {
            delete animator;
            animator = nullptr;

            return;
        }

        if ( indicator->window() && indicator->isVisible() )
        {
            if ( animator == nullptr )
                animator = new PositionAnimator( indicator );

            animator->start();
        }
        else
        {
            if ( animator )
                animator->stop();
        }
    }

    PositionAnimator* animator = nullptr;

    qreal value = 0.0;
    qreal origin = 0.0;

    bool hasOrigin = false;
    bool isIndeterminate = false;
};

QskProgressIndicator::QskProgressIndicator( qreal min, qreal max, QQuickItem* parent )
    : QskBoundedControl( min, max, parent )
    , m_data( new PrivateData )
{
    m_data->value = minimum();

    connect( this, &QskBoundedControl::boundariesChanged,
        this, &QskProgressIndicator::adjustValue );
}

QskProgressIndicator::QskProgressIndicator( QQuickItem* parent )
    : QskProgressIndicator( 0.0, 100.0, parent )
{
}

QskProgressIndicator::QskProgressIndicator( const QskIntervalF& boundaries, QQuickItem* parent )
    : QskProgressIndicator( boundaries.lowerBound(), boundaries.upperBound(), parent )
{
}

QskProgressIndicator::~QskProgressIndicator()
{
    delete m_data->animator;
}

bool QskProgressIndicator::isIndeterminate() const
{
    return m_data->isIndeterminate;
}

void QskProgressIndicator::setIndeterminate( bool on )
{
    if ( on == m_data->isIndeterminate )
        return;

    m_data->isIndeterminate = on;
    m_data->updateIndeterminateAnimator( this );

    update();
    Q_EMIT indeterminateChanged( on );
}

void QskProgressIndicator::setFillGradient( const QskGradient& gradient )
{
    setGradientHint( Fill, gradient );
}

void QskProgressIndicator::resetFillGradient()
{
    resetColor( Fill );
}

QskGradient QskProgressIndicator::fillGradient() const
{
    return gradientHint( Fill );
}

void QskProgressIndicator::setExtent( qreal extent )
{
    if ( extent < 0.0 )
        extent = 0.0;

    if ( setMetric( Groove | QskAspect::Size, extent ) )
        Q_EMIT extentChanged( extent );
}

void QskProgressIndicator::resetExtent()
{
    if ( resetMetric( Groove | QskAspect::Size ) )
        Q_EMIT extentChanged( extent() );
}

qreal QskProgressIndicator::extent() const
{
    return metric( Groove | QskAspect::Size );
}

void QskProgressIndicator::setOrigin( qreal origin )
{
    if ( isComponentComplete() )
        origin = boundedValue( origin );

    if( !m_data->hasOrigin || !qskFuzzyCompare( m_data->origin, origin ) )
    {
        m_data->hasOrigin = true;
        m_data->origin = origin;

        update();
        Q_EMIT originChanged( origin );
    }
}

void QskProgressIndicator::resetOrigin()
{
    if ( m_data->hasOrigin )
    {
        m_data->hasOrigin = false;

        update();
        Q_EMIT originChanged( origin() );
    }
}

qreal QskProgressIndicator::origin() const
{
    if ( m_data->hasOrigin )
    {
        return boundedValue( m_data->origin );
    }

    return minimum();
}

bool QskProgressIndicator::hasOrigin() const
{
    return m_data->hasOrigin;
}

void QskProgressIndicator::setValue( qreal value )
{
    if ( isComponentComplete() )
        value = boundedValue( value );

    setValueInternal( value );
}

qreal QskProgressIndicator::value() const
{
    return m_data->value;
}

void QskProgressIndicator::setValueAsRatio( qreal ratio )
{
    ratio = qBound( 0.0, ratio, 1.0 );
    setValue( minimum() + ratio * boundaryLength() );
}

qreal QskProgressIndicator::valueAsRatio() const
{
    return valueAsRatio( m_data->value );
}

void QskProgressIndicator::componentComplete()
{
    Inherited::componentComplete();
    adjustValue();
}

void QskProgressIndicator::adjustValue()
{
    if ( isComponentComplete() )
        setValueInternal( boundedValue( m_data->value ) );
}

void QskProgressIndicator::setValueInternal( qreal value )
{
    if ( !qskFuzzyCompare( value, m_data->value ) )
    {
        m_data->value = value;
        Q_EMIT valueChanged( value );

        update();
    }
}

void QskProgressIndicator::itemChange( QQuickItem::ItemChange change,
    const QQuickItem::ItemChangeData& value )
{
    switch( static_cast< int >( change ) )
    {
        case QQuickItem::ItemVisibleHasChanged:
        case QQuickItem::ItemSceneChange:
        {
            m_data->updateIndeterminateAnimator( this );
            break;
        }
    }

    Inherited::itemChange( change, value );
}

#include "moc_QskProgressIndicator.cpp"
