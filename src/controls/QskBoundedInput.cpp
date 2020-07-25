/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoundedInput.h"
#include "QskFunctions.h"
#include "QskIntervalF.h"

#include <cmath>

QSK_SYSTEM_STATE( QskBoundedInput, ReadOnly, ( QskAspect::FirstSystemState << 1 ) )

class QskBoundedInput::PrivateData
{
  public:
    PrivateData()
        : minimum( 0.0 )
        , maximum( 1.0 )
        , stepSize( 0.1 )
        , pageSize( 1 )
        , snap( false )
    {
    }

    qreal minimum;
    qreal maximum;
    qreal stepSize;
    int pageSize;

    bool snap : 1;
};

QskBoundedInput::QskBoundedInput( QQuickItem* parent )
    : QskControl( parent )
    , m_data( new PrivateData() )
{
    setFocusPolicy( Qt::StrongFocus );
    setAcceptedMouseButtons( Qt::LeftButton );
    setWheelEnabled( true );
}

QskBoundedInput::~QskBoundedInput()
{
}

void QskBoundedInput::setMinimum( qreal minimum )
{
    if ( m_data->minimum == minimum )
        return;

    m_data->minimum = minimum;
    Q_EMIT minimumChanged( minimum );

    if ( isComponentComplete() )
        adjustBoundaries( false );

    Q_EMIT boundariesChanged( boundaries() );
    update();
}

qreal QskBoundedInput::minimum() const
{
    return m_data->minimum;
}

void QskBoundedInput::setMaximum( qreal maximum )
{
    if ( m_data->maximum == maximum )
        return;

    m_data->maximum = maximum;
    Q_EMIT maximumChanged( maximum );

    if ( isComponentComplete() )
        adjustBoundaries( true );

    Q_EMIT boundariesChanged( boundaries() );
    update();
}

qreal QskBoundedInput::maximum() const
{
    return m_data->maximum;
}

void QskBoundedInput::setBoundaries( qreal min, qreal max )
{
    if ( max < min )
        max = min;

    const auto oldMin = m_data->minimum;
    const auto oldMax = m_data->maximum;

    if ( min == oldMin && max == oldMax )
        return;

    m_data->minimum = min;
    m_data->maximum = max;

    if ( isComponentComplete() )
        adjustBoundaries( false );

    if ( m_data->minimum != oldMin )
        Q_EMIT minimumChanged( m_data->minimum );

    if ( m_data->maximum != oldMax )
        Q_EMIT maximumChanged( m_data->maximum );

    Q_EMIT boundariesChanged( boundaries() );
    update();
}

void QskBoundedInput::setBoundaries( const QskIntervalF& boundaries )
{
    setBoundaries( boundaries.lowerBound(), boundaries.upperBound() );
}

QskIntervalF QskBoundedInput::boundaries() const
{
    return QskIntervalF( m_data->minimum, m_data->maximum );
}

void QskBoundedInput::adjustBoundaries( bool increasing )
{
    if ( m_data->maximum < m_data->minimum )
    {
        if ( increasing )
        {
            m_data->minimum = m_data->maximum;
            Q_EMIT minimumChanged( m_data->minimum );
        }
        else
        {
            m_data->maximum = m_data->minimum;
            Q_EMIT maximumChanged( m_data->maximum );
        }

        alignInput();
    }
}

qreal QskBoundedInput::boundaryLength() const
{
    return m_data->maximum - m_data->minimum;
}

void QskBoundedInput::setStepSize( qreal stepSize )
{
    if ( qFuzzyIsNull( stepSize ) )
        stepSize = 0.0;

    if ( qFuzzyCompare( m_data->stepSize, stepSize ) )
        return;

    m_data->stepSize = stepSize;
    Q_EMIT stepSizeChanged( stepSize );

    if ( isComponentComplete() )
    {
        if ( m_data->snap && stepSize )
            alignInput();
    }
}

qreal QskBoundedInput::stepSize() const
{
    return m_data->stepSize;
}

void QskBoundedInput::setPageSize( int pageSize )
{
    if ( m_data->pageSize == pageSize )
        return;

    m_data->pageSize = pageSize;
    Q_EMIT pageSizeChanged( pageSize );
}

int QskBoundedInput::pageSize() const
{
    return m_data->pageSize;
}

void QskBoundedInput::stepUp()
{
    increment( m_data->stepSize );
}   

void QskBoundedInput::stepDown()
{
    increment( -m_data->stepSize );
}   
    
void QskBoundedInput::pageUp()
{
    increment( m_data->pageSize * m_data->stepSize );
}   
    
void QskBoundedInput::pageDown()
{
    increment( -m_data->pageSize * m_data->stepSize );
}

void QskBoundedInput::setSnap( bool snap )
{
    if ( m_data->snap == snap )
        return;

    m_data->snap = snap;
    Q_EMIT snapChanged( snap );

    if ( snap )
        alignInput();
}

bool QskBoundedInput::snap() const
{
    return m_data->snap;
}

qreal QskBoundedInput::alignedValue( qreal value ) const
{
    if ( m_data->snap )
    {
        if ( const auto step = m_data->stepSize )
            value = qRound( value / step ) * step;
    }
    
    return qBound( minimum(), value, maximum() );
}

QskIntervalF QskBoundedInput::alignedInterval( const QskIntervalF& interval ) const
{
    if ( m_data->snap )
    {
        if ( const auto step = m_data->stepSize )
        {
            const qreal lower = std::floor( interval.lowerBound() / step ) * step;
            const qreal upper = std::ceil( interval.upperBound() / step ) * step;

            return QskIntervalF( lower, upper );
        }
    }

    return interval;
}

void QskBoundedInput::setReadOnly( bool readOnly )
{
    if ( readOnly == isReadOnly() )
        return;

    setSkinStateFlag( ReadOnly, readOnly );

    // we are killing user settings here !!
    setFocusPolicy( readOnly ? Qt::NoFocus : Qt::StrongFocus );
    setAcceptedMouseButtons( readOnly ? Qt::NoButton : Qt::LeftButton );
    setWheelEnabled( !readOnly );

    Q_EMIT readOnlyChanged( readOnly );
}

bool QskBoundedInput::isReadOnly() const
{
    return skinState() & ReadOnly;
}

void QskBoundedInput::keyPressEvent( QKeyEvent* event )
{
    if ( !isReadOnly() )
    {
        if ( event->key() == Qt::Key_Up || event->matches( QKeySequence::MoveToNextChar ) )
        {
            increment( m_data->stepSize );
            return;
        }

        if ( event->key() == Qt::Key_Down || event->matches( QKeySequence::MoveToPreviousChar ) )
        {
            increment( -m_data->stepSize );
            return;
        }
    }

    Inherited::keyPressEvent( event );
}

#ifndef QT_NO_WHEELEVENT

void QskBoundedInput::wheelEvent( QWheelEvent* event )
{
    if ( isReadOnly() )
        return;

#if QT_VERSION < 0x050e00
    const int wheelDelta = event->delta();
#else
    const auto delta = event->angleDelta();
    const int wheelDelta = ( qAbs( delta.x() ) > qAbs( delta.y() ) )
        ? delta.x() : delta.y();
#endif

    const int steps = wheelDelta / QWheelEvent::DefaultDeltasPerStep;
    increment( steps );
}

#endif

void QskBoundedInput::componentComplete()
{
    Inherited::componentComplete();
    adjustBoundaries( true );
}

#include "moc_QskBoundedInput.cpp"
