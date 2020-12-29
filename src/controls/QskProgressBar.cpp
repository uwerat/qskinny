/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskProgressBar.h"

#include "QskIntervalF.h"
#include "QskGradient.h"
#include "QskFunctions.h"
#include "QskAnimator.h"
#include "QskAspect.h"

QSK_SUBCONTROL( QskProgressBar, Groove )
QSK_SUBCONTROL( QskProgressBar, Bar )

namespace
{
    class PositionAnimator : public QskAnimator
    {
      public:
        PositionAnimator( QskProgressBar* progressBar )
            : m_progressBar( progressBar )
        {
            setAutoRepeat( true );
            setDuration( 1300 );

            setWindow( progressBar->window() );
        }

        void advance( qreal value ) override
        {
            const auto aspect = QskProgressBar::Bar | QskAspect::Position;

            m_progressBar->setMetric( aspect, value );
            m_progressBar->update();
        }

      private:
        QskProgressBar* m_progressBar;
    };
}

class QskProgressBar::PrivateData
{
  public:
    void updateIndeterminateAnimator( QskProgressBar* progressBar )
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

    Qt::Orientation orientation;
};

QskProgressBar::QskProgressBar( Qt::Orientation orientation,
        qreal min, qreal max, QQuickItem* parent )
    : QskBoundedControl( min, max, parent )
    , m_data( new PrivateData )
{
    m_data->orientation = orientation;
    m_data->value = minimum();

    if ( orientation == Qt::Horizontal )
        initSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::Fixed );
    else
        initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::MinimumExpanding );

    connect( this, &QskBoundedControl::boundariesChanged,
        this, &QskProgressBar::adjustValue );
}

QskProgressBar::QskProgressBar( Qt::Orientation orientation, QQuickItem* parent )
    : QskProgressBar( orientation, 0.0, 100.0, parent )
{
}

QskProgressBar::QskProgressBar( const QskIntervalF& boundaries, QQuickItem* parent )
    : QskProgressBar( boundaries.lowerBound(), boundaries.upperBound(), parent )
{
}

QskProgressBar::QskProgressBar( qreal min, qreal max, QQuickItem* parent )
    : QskProgressBar( Qt::Horizontal, min, max, parent )
{
}

QskProgressBar::QskProgressBar( QQuickItem* parent )
    : QskProgressBar( Qt::Horizontal, parent )
{
}

QskProgressBar::~QskProgressBar()
{
    delete m_data->animator;
}

Qt::Orientation QskProgressBar::orientation() const
{
    return m_data->orientation;
}

void QskProgressBar::setOrientation( Qt::Orientation orientation )
{
    if ( orientation != m_data->orientation )
    {
        m_data->orientation = orientation;
        setSizePolicy( sizePolicy( Qt::Vertical ), sizePolicy( Qt::Horizontal ) );

        resetImplicitSize();
        update();

        Q_EMIT orientationChanged( m_data->orientation );
    }
}

bool QskProgressBar::isIndeterminate() const
{
    return m_data->isIndeterminate;
}

void QskProgressBar::setIndeterminate( bool on )
{
    if ( on == m_data->isIndeterminate )
        return;

    m_data->isIndeterminate = on;
    m_data->updateIndeterminateAnimator( this );

    update();
    Q_EMIT indeterminateChanged( on );
}

QskAspect::Placement QskProgressBar::effectivePlacement() const
{
    // so you can define different hints depending on the orientation
    return static_cast< QskAspect::Placement >( m_data->orientation );
}

void QskProgressBar::setBarGradient( const QskGradient& gradient )
{
    // An API where we set the stops only would be more accurate TODO ...
    auto g = gradient;

    g.setOrientation( Qt::Horizontal );
    setGradientHint( Bar | QskAspect::Horizontal, g );

    g.setOrientation( Qt::Vertical );
    setGradientHint( Bar | QskAspect::Vertical, g );
}

void QskProgressBar::resetBarGradient()
{
    resetColor( Bar | QskAspect::Vertical );
    resetColor( Bar | QskAspect::Horizontal );
}

QskGradient QskProgressBar::barGradient() const
{
    return gradientHint( QskProgressBar::Bar );
}

void QskProgressBar::setExtent( qreal extent )
{
    if ( extent < 0.0 )
        extent = 0.0;

    if ( setMetric( Groove | QskAspect::Size, extent ) )
        Q_EMIT extentChanged( extent );
}

void QskProgressBar::resetExtent()
{
    if ( resetMetric( Groove | QskAspect::Size ) )
        Q_EMIT extentChanged( extent() );
}

qreal QskProgressBar::extent() const
{
    return metric( Groove | QskAspect::Size );
}

void QskProgressBar::setOrigin( qreal origin )
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

void QskProgressBar::resetOrigin()
{
    if ( m_data->hasOrigin )
    {
        m_data->hasOrigin = false;

        update();
        Q_EMIT originChanged( origin() );
    }
}

qreal QskProgressBar::origin() const
{
    if ( m_data->hasOrigin )
    {
        return boundedValue( m_data->origin );
    }

    return minimum();
}

void QskProgressBar::setValue( qreal value )
{
    if ( isComponentComplete() )
        value = boundedValue( value );

    setValueInternal( value );
}

qreal QskProgressBar::value() const
{
    return m_data->value;
}

void QskProgressBar::setValueAsRatio( qreal ratio )
{
    ratio = qBound( 0.0, ratio, 1.0 );
    setValue( minimum() + ratio * boundaryLength() );
}

qreal QskProgressBar::valueAsRatio() const
{
    return valueAsRatio( m_data->value );
}

void QskProgressBar::componentComplete()
{
    Inherited::componentComplete();
    adjustValue();
}

void QskProgressBar::adjustValue()
{
    if ( isComponentComplete() )
        setValueInternal( boundedValue( m_data->value ) );
}

void QskProgressBar::setValueInternal( qreal value )
{
    if ( !qskFuzzyCompare( value, m_data->value ) )
    {
        m_data->value = value;
        Q_EMIT valueChanged( value );

        update();
    }
}

void QskProgressBar::itemChange( QQuickItem::ItemChange change,
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

#include "moc_QskProgressBar.cpp"
