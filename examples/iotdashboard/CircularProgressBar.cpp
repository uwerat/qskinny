/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "CircularProgressBar.h"

#include <QskAnimator.h>
#include <QskFunctions.h>

QSK_SUBCONTROL( CircularProgressBar, Groove )
QSK_SUBCONTROL( CircularProgressBar, Bar )

namespace
{
    class PositionAnimator : public QskAnimator
    {
      public:
        PositionAnimator( CircularProgressBar* progressBar )
            : m_progressBar( progressBar )
        {
            setAutoRepeat( true );
            setDuration( 1300 );

            setWindow( progressBar->window() );
        }

        void advance( qreal value ) override
        {
            const auto aspect = CircularProgressBar::Bar | QskAspect::Position;

            m_progressBar->setMetric( aspect, value );
            m_progressBar->update();
        }

      private:
        CircularProgressBar* m_progressBar;
    };
}

class CircularProgressBar::PrivateData
{
  public:
    void updateIndeterminateAnimator( CircularProgressBar* progressBar )
    {
        if ( !isIndeterminate )
        {
            delete animator;
            animator = nullptr;

            return;
        }

        if ( progressBar->window() && progressBar->isVisible() )
        {
            if ( animator == nullptr )
                animator = new PositionAnimator( progressBar );

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

CircularProgressBar::~CircularProgressBar() = default;

CircularProgressBar::CircularProgressBar( qreal min, qreal max, QQuickItem* parent )
    : QskBoundedControl( min, max, parent )
    , m_data( new PrivateData )
{
    m_data->value = minimum();

    initSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::MinimumExpanding );

    connect( this, &QskBoundedControl::boundariesChanged,
        this, &CircularProgressBar::adjustValue );
}

CircularProgressBar::CircularProgressBar( QQuickItem* parent )
    : CircularProgressBar( 0.0, 100.0, parent )
{
}

bool CircularProgressBar::isIndeterminate() const
{
    return m_data->isIndeterminate;
}

void CircularProgressBar::setIndeterminate( bool on )
{
    if ( on == m_data->isIndeterminate )
        return;

    m_data->isIndeterminate = on;
    m_data->updateIndeterminateAnimator( this );

    update();
    Q_EMIT indeterminateChanged( on );
}

void CircularProgressBar::resetOrigin()
{
    if ( m_data->hasOrigin )
    {
        m_data->hasOrigin = false;

        update();
        Q_EMIT originChanged( origin() );
    }
}

qreal CircularProgressBar::origin() const
{
    if ( m_data->hasOrigin )
    {
        return boundedValue( m_data->origin );
    }

    return minimum();
}

qreal CircularProgressBar::value() const
{
    return m_data->value;
}

qreal CircularProgressBar::valueAsRatio() const
{
    return QskBoundedControl::valueAsRatio( m_data->value );
}

void CircularProgressBar::setValue( qreal value )
{
    if ( isComponentComplete() )
        value = boundedValue( value );

    setValueInternal( value );
}

void CircularProgressBar::setValueAsRatio( qreal ratio )
{
    ratio = qBound( 0.0, ratio, 1.0 );
    setValue( minimum() + ratio * boundaryLength() );
}

void CircularProgressBar::setOrigin( qreal origin )
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

void CircularProgressBar::componentComplete()
{
    Inherited::componentComplete();
    adjustValue();
}

void CircularProgressBar::setValueInternal( qreal value )
{
    if ( !qskFuzzyCompare( value, m_data->value ) )
    {
        m_data->value = value;

        Q_EMIT valueChanged( value );

        update();
    }
}

void CircularProgressBar::adjustValue()
{
    if ( isComponentComplete() )
        setValueInternal( boundedValue( m_data->value ) );
}

#include "moc_CircularProgressBar.cpp"
